1. compile:
mpicc seidel.c -o seidel

2. run:
mpirun -np 4 seidel

3. result:
Input of file "dataIn.txt"
3       4
9.000000        -1.000000       -1.000000       7.000000
-1.000000       8.000000        0.000000        7.000000
-1.000000       0.000000        9.000000        8.000000

0.000000        0.000000        1.000000


Output of result
in the 1 times total vaule = 0
in the 2 times total vaule = 0
in the 3 times total vaule = 0
in the 4 times total vaule = 3
x[0] = 0.999998
x[1] = 1.000000
x[2] = 1.000000

Iteration num = 4
