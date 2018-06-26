#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

/*
  * ������: main
  * ���ܣ�   ��������ʵ��ö������
  * ���룺argcΪ�����в���������
  *            argvΪÿ�������в�����ɵ��ַ�������
  * ���������1���������������
*/
int main(int argc,char *argv[])
{
	int DataSize, MyLength;              /*DataSize:���鳤�ȣ�MyLength�����������䵽�����ݳ���*/
	int *data_in, *data_out;             /*������������ָ��*/
	int *rank;                           /*������*/
	int MyID, SumID;
	int i, j;                                     
        double starttime, endtime, diff;
	MPI_Status status;                   

	MPI_Init(&argc,&argv);                /*MPI ��ʼ��*/
	MPI_Comm_rank(MPI_COMM_WORLD,&MyID);  /*ÿ��������ȷ������ID*/
        MPI_Comm_size(MPI_COMM_WORLD,&SumID); /*ÿ��������ȷ���ܴ���������*/
	
	//record the excute time
        if(MyID == 0){
           starttime = MPI_Wtime();
        }

	if(MyID==0)                           /*��������*/
		DataSize=GetDataSize();       /*������������еĳ���*/
	
	MPI_Bcast(&DataSize, 1, MPI_INT, 0, MPI_COMM_WORLD);
                                              /*���������㲥���������еĳ���*/
	/*�ڸ����������仮������*/
	MyLength=DataSize/SumID;              
	if(MyID==SumID-1)                     /*ÿ��������ȷ������Ҫ��������г���*/
		MyLength=DataSize-MyLength*(SumID-1);

	data_in=(int *)malloc(DataSize*sizeof(int)); /*������������еĿռ�*/
	if(data_in==0) ErrMsg("Malloc memory error!");


	if(MyID==0){                     
		data_out=(int *)malloc(DataSize*sizeof(int)); /*���������������������Ŀռ�*/
		if(data_out==0) ErrMsg("Malloc memory error!");

		rank=(int *)malloc(DataSize*sizeof(int));     /*�����������Ŀռ�*/
		if(rank==0) ErrMsg("Malloc memory error!");
	}
	else{
		rank=(int *)malloc(MyLength*sizeof(int));     /*�����������Ŀռ�*/
		if(rank==0) ErrMsg("Malloc memory error!");
	}

	if(MyID==0){
	        int seed;
                printf("please input the seed:\n");
	        scanf("%d",&seed);                       /*��������������*/
		srand(seed);                          
		//printf("Random Numbers:\n"); 
		for(i=0;i<DataSize;i++){
			data_in[i]=((int)rand())%10000;  /*����������������*/
			printf("%10d ",data_in[i]);
		}
		printf("\nOutput:");
		printf("\n");
	}

	/*��������������ʹ��������У���Ӧ���㷨13.2���裨1��*/
	MPI_Bcast(data_in, DataSize, MPI_INT, 0, MPI_COMM_WORLD);

	/*�����������ֱ��������Ԫ�ص��ȣ���Ӧ���㷨13.2���裨2��*/
	CountRank(data_in,DataSize,MyLength,rank,SumID,MyID);

	/*�Ӹ����������ռ�������õ����ݣ���Ӧ���㷨13.2���裨3��*/
	if(MyID==0){
		for(i=1;i<SumID;i++){
			if(i==SumID-1)
				MPI_Recv(rank+MyLength*i,DataSize-MyLength*(SumID-1),MPI_INT,i,0,MPI_COMM_WORLD,&status);
			else
				MPI_Recv(rank+MyLength*i,MyLength,MPI_INT,i,0,MPI_COMM_WORLD,&status);
		}
	}
	else
		MPI_Send(rank,MyLength,MPI_INT,0,0,MPI_COMM_WORLD);

	/*��������õ������¶�λ�������ݣ���Ӧ���㷨13.2���裨4��*/
	if(MyID==0){
		for(i=0;i<DataSize;i++)
			data_out[rank[i]]=data_in[i];

		for(i=0;i<DataSize;i++){
			printf("%10d ",data_out[i]);
		}
		printf("\n");
	}
        if(MyID == 0){
           endtime = MPI_Wtime();
           diff = endtime - starttime;
           printf("the excute time is %f \n", diff);
        }
	MPI_Finalize();   
        return 1;
}
/*
 * ������: CountRank
 * ���ܣ� ���������������ݵ���
 * ���룺 data��ָ����������е�ָ��
 *        DataSizeΪ���������еĳ���
          MyLengthΪ�ô�����Ҫ��������еĳ���
          rank��ָ���������ָ��
          SumID���ܴ���������
          MyID��������ID
 * ���������1���������������
 */
int CountRank(int *data,int DataSize,int MyLength,int *rank,int SumID,int MyID)
{
	int i, j;
	int start, end;

	start=DataSize/SumID*MyID;      /*��������������ʼλ��*/
	end=start+MyLength;             /*�����������ݽ���λ��*/

	for(j=start;j<end;j++){         /*���������������ݵ�rank*/
		rank[j-start]=0;
		for(i=0;i<DataSize;i++){
			if((data[j]>data[i]) || ((data[j]==data[i]) && (j>i)))
				rank[j-start]++;
		}
	}
     return 1;
}

/*
 * ������: ErrMsg
 * ���ܣ� ������������еĳ���
 * ���룺 ��
 * ����� ���ش��������еĳ���
 */
int GetDataSize()
{
	int i;

	while(1){
		printf("please input the data length:\n");
		scanf("%d",&i);
		if((i>0) && (i<=65535))
			break;
		ErrMsg("Wrong Data Size, must between [1..65535]");
	}
	return i;
}
/*
 * ������: ErrMsg
 * ���ܣ� ���������Ϣ
 * ���룺 msg:������Ϣ�ַ���
 * ���������1���������������
 */
int ErrMsg(char *msg)
{
	printf("Error: %s \n",msg);
    return 1;
}
