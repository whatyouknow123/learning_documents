Example:
编译：mpicc connect.c
运行：本实例中使用3个处理器。
　　　mpirun –np 3 a.out
运行结果：
Input the vertex num:
8
Input the adjacent matrix:
0 0 0 0 0 0 0 1
0 0 0 0 0 1 1 0
0 0 0 0 0 0 0 0
0 0 0 0 0 1 0 1
0 0 0 0 0 0 1 1
0 1 0 1 0 0 0 0
0 1 0 0 1 0 0 0
1 0 0 1 1 0 0 0
最终输出结果：
Result:
0 0 2 0 0 0 0 0
说明：使用了和上一小节传递闭包时相同的输入矩阵，得到的结果也相同。除顶点2外，其余顶点构成另一个连通分

