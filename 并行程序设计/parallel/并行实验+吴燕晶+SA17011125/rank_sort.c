#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

/*
  * 函数名: main
  * 功能：   主函数，实现枚举排序
  * 输入：argc为命令行参数个数；
  *            argv为每个命令行参数组成的字符串数组
  * 输出：返回1代表程序正常结束
*/
int main(int argc,char *argv[])
{
	int DataSize, MyLength;              /*DataSize:数组长度；MyLength：处理器分配到的数据长度*/
	int *data_in, *data_out;             /*输入和输出数组指针*/
	int *rank;                           /*秩数组*/
	int MyID, SumID;
	int i, j;                                     
        double starttime, endtime, diff;
	MPI_Status status;                   

	MPI_Init(&argc,&argv);                /*MPI 初始化*/
	MPI_Comm_rank(MPI_COMM_WORLD,&MyID);  /*每个处理器确定各自ID*/
        MPI_Comm_size(MPI_COMM_WORLD,&SumID); /*每个处理器确定总处理器个数*/
	
	//record the excute time
        if(MyID == 0){
           starttime = MPI_Wtime();
        }

	if(MyID==0)                           /*主处理器*/
		DataSize=GetDataSize();       /*读入待排序序列的长度*/
	
	MPI_Bcast(&DataSize, 1, MPI_INT, 0, MPI_COMM_WORLD);
                                              /*主处理器广播待排序序列的长度*/
	/*在各个处理器间划分任务*/
	MyLength=DataSize/SumID;              
	if(MyID==SumID-1)                     /*每个处理器确定各自要排序的序列长度*/
		MyLength=DataSize-MyLength*(SumID-1);

	data_in=(int *)malloc(DataSize*sizeof(int)); /*分配待排序序列的空间*/
	if(data_in==0) ErrMsg("Malloc memory error!");


	if(MyID==0){                     
		data_out=(int *)malloc(DataSize*sizeof(int)); /*主处理器分配排序后数组的空间*/
		if(data_out==0) ErrMsg("Malloc memory error!");

		rank=(int *)malloc(DataSize*sizeof(int));     /*分配序号数组的空间*/
		if(rank==0) ErrMsg("Malloc memory error!");
	}
	else{
		rank=(int *)malloc(MyLength*sizeof(int));     /*分配序号数组的空间*/
		if(rank==0) ErrMsg("Malloc memory error!");
	}

	if(MyID==0){
	        int seed;
                printf("please input the seed:\n");
	        scanf("%d",&seed);                       /*获得随机数的种子*/
		srand(seed);                          
		//printf("Random Numbers:\n"); 
		for(i=0;i<DataSize;i++){
			data_in[i]=((int)rand())%10000;  /*生成随机数，并输出*/
			printf("%10d ",data_in[i]);
		}
		printf("\nOutput:");
		printf("\n");
	}

	/*向各个处理器播送待排序序列，对应于算法13.2步骤（1）*/
	MPI_Bcast(data_in, DataSize, MPI_INT, 0, MPI_COMM_WORLD);

	/*各个处理器分别计算所属元素的秩，对应于算法13.2步骤（2）*/
	CountRank(data_in,DataSize,MyLength,rank,SumID,MyID);

	/*从各个处理器收集已排序好的数据，对应于算法13.2步骤（3）*/
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

	/*根据所获得的秩重新定位各个数据，对应于算法13.2步骤（4）*/
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
 * 函数名: CountRank
 * 功能： 计算所属部分数据的秩
 * 输入： data：指向待排序序列的指针
 *        DataSize为待排序序列的长度
          MyLength为该处理器要排序的序列的长度
          rank：指向秩数组的指针
          SumID：总处理器个数
          MyID：处理器ID
 * 输出：返回1代表程序正常结束
 */
int CountRank(int *data,int DataSize,int MyLength,int *rank,int SumID,int MyID)
{
	int i, j;
	int start, end;

	start=DataSize/SumID*MyID;      /*所属部分数据起始位置*/
	end=start+MyLength;             /*所属部分数据结束位置*/

	for(j=start;j<end;j++){         /*计算所属部分数据的rank*/
		rank[j-start]=0;
		for(i=0;i<DataSize;i++){
			if((data[j]>data[i]) || ((data[j]==data[i]) && (j>i)))
				rank[j-start]++;
		}
	}
     return 1;
}

/*
 * 函数名: ErrMsg
 * 功能： 读入待排序序列的长度
 * 输入： 无
 * 输出： 返回待排序序列的长度
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
 * 函数名: ErrMsg
 * 功能： 输出错误信息
 * 输入： msg:出错信息字符串
 * 输出：返回1代表程序正常结束
 */
int ErrMsg(char *msg)
{
	printf("Error: %s \n",msg);
    return 1;
}
