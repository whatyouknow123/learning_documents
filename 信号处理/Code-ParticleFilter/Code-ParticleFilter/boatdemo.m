% demonstration of particle filtering

clear,clc,close all

% let x be the position of the boat
% create a model for the movement of the boat
xmin=-10;
xmax=10;
xpmin=-.3;
xpmax=.3;
N=10000;
m=1; % kg
kk=1; % N/m resistance coefficient
c=.5; % N/s yank=jerk*kg; jerk: m/s^3
F0=25; % N
dT=.05; % s
M=400; % timesteps

% first calculate the true sequence of disturbance force
wk=(rand(M,1)-.5)*2*F0; % N

af=[1 -2 1].*(dT^-2)+[0 kk/m 0]+[1 0 -1]./2/dT*c/m;
bf=1/m; % bf=1;
xtrue=filter(bf,af,wk); % position
% (1/dT^2+c/(2*dT*m))*xtrue[n]+(-2/dT^2+kk/m)*xtrue[n-1]+...
% (1/dT^2-c/(2*dT*m))*xtrue[n-2]=wk[n]/m % acceleration

xptrue=filter([1 -1]/dT,1,xtrue); % velocity
% xptrue[n]=(1/dT)*xtrue[n]-(1/dT)xtrue[n-1] 

% plot(xtrue)
% title('true x')
% xlabel('time step')
% ylabel('x (m)')
% pause

% calculate the measurement
sigma=.3;   % measurent noise std deviation.
a=.2;   % constant for the average slope of the bottom surface
b=0;
z = sin(xtrue)+a*xtrue+b*xtrue.*xtrue + randn(M,1)*sigma;
% first part is the bottom surface, last term is the measurement noise


% initialization
xk = rand(N,1)*(xmax-xmin)+xmin; % particle initial position
xpk = zeros(N,1);

pik=repmat(1/N,N,1); % the propability that we are in state xk, xpk.

figure(3)
set(3,'doublebuffer','on','position',[239   100   681   343])
t=linspace(xmin,xmax,N)';
h1=line(t,sin(t)+a*t+b*t.*t,'marker','none');   
% t is just a temporary variable to draw the bottom surface
h3=line(xtrue(1),0,'marker','o','linestyle','none');

% plot the boat
boat.x=[-2.4 -2  -1   0   1 2 2 -2.4]';
boat.y=[   1  0 -.2 -.3 -.3 -.3 .8 1]'+5;
hboat=line(boat.x+xtrue(1),boat.y,'color','r');
% plot the depth measurement
hecho=line(xtrue(1)*[1 1]',[z(1) boat.y(4)]','color','r','linestyle','--');
% plot the water surface
hwater=line([xmin xmax],boat.y(2)*[1 1],'color','b','linestyle','--');

% histogram is plotted also to show p(x) (the propability density function)
h_hist=line(1,1,'color','k');
xlabel('Position x (m)')
xlim([xmin,xmax])
ylim([-3 ceil(max(boat.y))+2])

% print some text
htext(1)=text(xmin+.5,boat.y(2),'Sea surface','verticalalignment','bottom');
htext(2)=text(boat.x(7),boat.y(2),'S/Y OptFilt','verticalalignment','bottom',...
    'horizontalalignment','right');
htext(3)=text(xmin+.5,0,'p(x)','verticalalignment','bottom');
htext(4)=text(xmin+.5,sin(xmin+.5)+a*(xmin+.5)+b*(xmin+.5).*(xmin+.5),...
    'Sea bottom','verticalalignment','bottom','color','b');


resample=.5;
% to use resampling or not: 0 is no resampling, 1 is always resampling. Based on Neff/N.

Neff=zeros(M,1); % number of efficient paritcle

% precalculate some constants to save time
sigma_sqrt_2_pi= sigma*sqrt(2*pi);
two_sigma_square= 2*sigma*sigma;


for k=1:M    % loop over timesteps
    % time update step
    % predict particle i from the previous step, using random disturbance for each particle
    wk=randn(N,1)*F0;
    % predict every sample (using matrices instead of for loops to get faster execution)
    
    % sampling (generating particle postition from timestep k-1 to k)
    xk=xk+xpk*dT; % position update
    % (PRML 13.119: generate particle x_k from particle x_{k-1})
    xpk=xpk+(wk-xk*kk-xpk*(c-dT*kk))/m*dT; % velocity update
    % acceleration=(wk-xk*kk-xpk*(c-dT*kk))/m
    % ok, c-dT*kk is used instead of c to reduce the need for temporary variable 
    % on the line above
    
    % measurement update
    % (let the measurement be z=f(x)+v where v is normal distributed with sigma std dev.)
    % z~normal(z|f(x),sigma^2);
    
    pik=pik.*exp(-(sin(xk)+a*xk+b*xk.*xk - z(k)).^2/two_sigma_square)/sigma_sqrt_2_pi; 
    % ok, this is made to all particles at once. (PRML P.646 z(k) is the new observation.)
    % p(z(k)|xk)=exp(-(sin(xk)+a*xk+b*xk.*xk - z(k)).^2/two_sigma_square)/sigma_sqrt_2_pi

    % normalize the weights
    pik=pik/sum(pik); % (PRML 13.118)
    
    % resample if necessary
    if resample>0
        % only resample if a criterion is fulfilled
        % resample based on the quality of the distribution
        Neff(k)=1/sum(pik.^2);
        if Neff(k)<(resample*N)
            % setting resample to 1 makes this condition always true, 
            % because 1<Neff<N always!
            Inew=rsmp(pik,N);
            xk=xk(Inew);
            xpk=xpk(Inew);
            pik=repmat(1/N,N,1);
        end
    end
    
    
    % update the plots
    
    set(h3,'xdata',xtrue(k),'ydata',z(k))
    set(hboat,'xdata',boat.x+xtrue(k) );
    set(hecho,'xdata',xtrue(k)*[1 1]','ydata',[z(k) boat.y(4)]');
    

    [plotx,ploty]=histweight(xk,pik,200,[xmin xmax]);
    
    set(h_hist,'xdata',plotx,'ydata',ploty)
    set(htext(2),'position',[boat.x(7)+xtrue(k) boat.y(2)]);
    drawnow;
end

figure(4)
plot(Neff/N)
xlabel('time step')
ylabel('Particle efficiency')
title('Efficient number of particles')
