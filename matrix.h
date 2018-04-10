#include <stdio.h>
#include <stdlib.h>
#include <math.h>
double Determinant(double **a,int n);
void CoFactor(double **a,int n,double **b);
void Transpose(double **a,int n);

void Shifted(double **echelon, int n, double **shifted);