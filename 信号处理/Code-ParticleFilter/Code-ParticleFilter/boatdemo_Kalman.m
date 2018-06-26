clear
clc
close all
addpath('./Kalman');
addpath('./KPMstats');
% addpath('./KPMtools');

% Make a point move in the 2D plane
% State = (x y xdot ydot). We only observe (x y).

% This code was used to generate Figure 15.9 of "Artificial Intelligence: a Modern Approach",
% Russell and Norvig, 2nd edition, Prentice Hall, 2003.

% X(t+1) = F X(t) + noise(Q)
% Y(t) = H X(t) + noise(R)

ss = 1; % state size
os = 1; % observation size
F = 5; %100;
H = 108;
Q = 0.25; % alpha in the problem
R = 0.3;
M = 400;
m=1; % kg
kk=1; % N/m
c=.5; % N/s
F0=25; % N
dT=.05; % s
M=400; % timesteps

seed = 9;
rand('state', seed);
randn('state', seed);
% T = 15;
% [x,y] = sample_lds(F, H, Q, R, initx, T);
% wk = Q*randn(M,1);
wk = normrnd(0,Q,[M,1])*F0;
af = [1 -2 1].*(dT^-2)+[0 kk/m 0]+[1 0 -1]./2/dT*c/m;
bf = 1/m;
x = filter(bf,af,wk);
x = x';
% vk = R*randn(M,1);
vk = normrnd(0,R,[M,1]);
y = H*x+vk';
% initx = x(1);
% initV = Q;
initx = 1;
initV = 1;
[xfilt, Vfilt, VVfilt, loglik] = kalman_filter(y, F, H, Q, R, initx, initV);
% [xsmooth, Vsmooth] = kalman_smoother(y, F, H, Q, R, initx, initV);

dfilt = x(:) - xfilt(:);
mse_filt = sqrt(sum(sum(dfilt.^2)))

figure
timesteps=(1:400)';
plot(timesteps,x,'-.b',timesteps,xfilt,'-r');
legend('true', 'predict')
title('true x and predict x');
xlabel('time step');
ylabel('x (m)');
