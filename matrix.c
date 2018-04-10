#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "matrix.h"
double Determinant(double **a,int n)
{
   int i,j,j1,j2;
   double det = 0;
   double **m = NULL;

   if (n < 1) { /* Error */

   } else if (n == 1) { /* Shouldn't get used */
      det = a[0][0];
   } else if (n == 2) {
      det = a[0][0] * a[1][1] - a[1][0] * a[0][1];
   } else {
      det = 0;
      for (j1=0;j1<n;j1++) {
         m = malloc((n-1)*sizeof(double *));
         for (i=0;i<n-1;i++)
            m[i] = malloc((n-1)*sizeof(double));
         for (i=1;i<n;i++) {
            j2 = 0;
            for (j=0;j<n;j++) {
               if (j == j1)
                  continue;
               m[i-1][j2] = a[i][j];
               j2++;
            }
         }
         det += pow(-1.0,j1+2.0) * a[0][j1] * Determinant(m,n-1);
         for (i=0;i<n-1;i++)
            free(m[i]);
         free(m);
      }
   }
   return(det);
}

/*
   Find the cofactor matrix of a square matrix
*/
void CoFactor(double **a,int n,double **b)
{
   int i,j,ii,jj,i1,j1;
   double det;
   double **c;

   c = malloc((n-1)*sizeof(double *));
   for (i=0;i<n-1;i++)
     c[i] = malloc((n-1)*sizeof(double));

   for (j=0;j<n;j++) {
      for (i=0;i<n;i++) {

         /* Form the adjoint a_ij */
         i1 = 0;
         for (ii=0;ii<n;ii++) {
            if (ii == i)
               continue;
            j1 = 0;
            for (jj=0;jj<n;jj++) {
               if (jj == j)
                  continue;
               c[i1][j1] = a[ii][jj];
               j1++;
            }
            i1++;
         }

         /* Calculate the determinate */
         det = Determinant(c,n-1);

         /* Fill in the elements of the cofactor */
         b[i][j] = pow(-1.0,i+j+2.0) * det;
      }
   }
   for (i=0;i<n-1;i++)
      free(c[i]);
   free(c);
}

/*
   Transpose of a square matrix, do it in place
*/
void Transpose(double **a,int n)
{
   int i,j;
   double tmp;

   for (i=1;i<n;i++) {
      for (j=0;j<i;j++) {
         tmp = a[i][j];
         a[i][j] = a[j][i];
         a[j][i] = tmp;
      }
   }
}
void inverse(double **a, int n, double **b)
{
	double det;
	int i, j;
	CoFactor(a, n, b);
    Transpose(b, n);
     det=Determinant(a, n);
    for(i = 0; i < n; i++){
        for(j = 0; j < n; j++){
            b[i][j]=b[i][j]/det;
        }
    }
}
void Shifted(double **echelon, int n, double **shifted)
{
   double **first;
   double **second;
   double **third;
   double **fourth;
   double **firstINVERSE;
   double **secondINVERSE;
   double **thirdINVERSE;
   double **fourthINVERSE;
   int SIZE=n;
   int i, j, m , mat;
   int fi, fj, sj, temp;
   double det;
    first = (double**) malloc(SIZE/2*sizeof(double*));
    for (i = 0; i < SIZE/2; i++){
            first[i] = (double*)malloc(SIZE/2*sizeof(double));
    }
    second = (double**) malloc(SIZE/2*sizeof(double*));
    for (i = 0; i < SIZE/2; i++){
            second[i] = (double*)malloc(SIZE/2*sizeof(double));
    }
    third = (double**) malloc(SIZE/2*sizeof(double*));
    for (i = 0; i < SIZE/2; i++){
            third[i] = (double*)malloc(SIZE/2*sizeof(double));
    }
    fourth = (double**) malloc(SIZE/2*sizeof(double*));
    for (i = 0; i < SIZE/2; i++){
            fourth[i] = (double*)malloc(SIZE/2*sizeof(double));
    }
    firstINVERSE = (double**) malloc(SIZE/2*sizeof(double*));
    for (i = 0; i < SIZE/2; i++){
            firstINVERSE[i] = (double*)malloc(SIZE/2*sizeof(double));
    }
    secondINVERSE = (double**) malloc(SIZE/2*sizeof(double*));
    for (i = 0; i < SIZE/2; i++){
            secondINVERSE[i] = (double*)malloc(SIZE/2*sizeof(double));
    }
    thirdINVERSE = (double**) malloc(SIZE/2*sizeof(double*));
    for (i = 0; i < SIZE/2; i++){
            thirdINVERSE[i] = (double*)malloc(SIZE/2*sizeof(double));
    }
    fourthINVERSE = (double**) malloc(SIZE/2*sizeof(double*));
    for (i = 0; i < SIZE/2; i++){
            fourthINVERSE[i] = (double*)malloc(SIZE/2*sizeof(double));
    }
    fj=0;
    sj=0;
    temp=sj;
   for(m=0; m<SIZE/2; m++)
   {
      for (j = 0; j < SIZE/2; ++j)
      {
         first[m][j]=echelon[fj][sj];
         sj++;
      }
      fj++;
      sj=temp;

   }
   inverse(first, n/2, firstINVERSE);

   fj=0;
    sj=SIZE/2;
    temp=sj;
   for(m=0; m<SIZE/2; m++)
   {
      for (j = 0; j < SIZE/2; ++j)
      {
         second[m][j]=echelon[fj][sj];
         sj++;
      }
      fj++;
      sj=temp;

   }
   inverse(second, n/2, secondINVERSE);
   fj=SIZE/2;
    sj=0;
    temp=sj;
   for(m=0; m<SIZE/2; m++)
   {
      for (j = 0; j < SIZE/2; ++j)
      {
         third[m][j]=echelon[fj][sj];
         sj++;
      }
      fj++;
      sj=temp;
   }
   inverse(third, n/2, thirdINVERSE);
   fj=SIZE/2;
    sj=SIZE/2;
    temp=sj;
   for(m=0; m<SIZE/2; m++)
   {
      for (j = 0; j < SIZE/2; ++j)
      {
         fourth[m][j]=echelon[fj][sj];
         sj++;
      }
      fj++;
      sj=temp;
   }

    inverse(fourth, n/2, fourthINVERSE);

    /*******************************************************************************/
    fj=0;
    sj=0;
    temp=sj;
   for(m=0; m<SIZE/2; m++)
   {
      for (j = 0; j < SIZE/2; ++j)
      {
         shifted[fj][sj]=firstINVERSE[m][j];
         sj++;
      }
      fj++;
      sj=temp;

   }

   fj=0;
    sj=SIZE/2;
    temp=sj;
   for(m=0; m<SIZE/2; m++)
   {
      for (j = 0; j < SIZE/2; ++j)
      {
         shifted[fj][sj]=secondINVERSE[m][j];
         sj++;
      }
      fj++;
      sj=temp;
   }
   fj=SIZE/2;
    sj=0;
    temp=sj;
   for(m=0; m<SIZE/2; m++)
   {
      for (j = 0; j < SIZE/2; ++j)
      {
         shifted[fj][sj]=thirdINVERSE[m][j];
         sj++;
      }
      fj++;
      sj=temp;
   }
   fj=SIZE/2;
    sj=SIZE/2;
    temp=sj;
   for(m=0; m<SIZE/2; m++)
   {
      for (j = 0; j < SIZE/2; ++j)
      {
         shifted[fj][sj]=fourthINVERSE[m][j];
         sj++;
      }
      fj++;
      sj=temp;
   }
  	/****************************************************************************/
     /*for(i = 0; i < n; i++){
        for(j = 0; j < n; j++){
            printf("%f ", shifted[i][j]);
        }
    }*/

    /*freeeee*/
    for (i = 0; i < SIZE/2; i++){
	   free(fourthINVERSE[i]);
	}
	free(fourthINVERSE);
	for (i = 0; i < SIZE/2; i++){
	   free(thirdINVERSE[i]);
	}
	free(thirdINVERSE);
	for (i = 0; i < SIZE/2; i++){
	   free(secondINVERSE[i]);
	}
	free(secondINVERSE);
	for (i = 0; i < SIZE/2; i++){
	   free(firstINVERSE[i]);
	}
	free(firstINVERSE);
	for (i = 0; i < SIZE/2; i++){
	   free(fourth[i]);
	}
	free(fourth);
	for (i = 0; i < SIZE/2; i++){
	   free(third[i]);
	}
	free(third);
	for (i = 0; i < SIZE/2; i++){
	   free(second[i]);
	}
	free(second);
	for (i = 0; i < SIZE/2; i++){
	   free(first[i]);
	}
	free(first);
}