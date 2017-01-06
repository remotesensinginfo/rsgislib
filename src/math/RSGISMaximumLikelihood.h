/*
 *  RSGISMaximumLikelihood.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 05/11/2012.
 *  
 *  This code has been taken and updated from http://mlpy.sourceforge.net
 *  and updated to C++ to allow it to be called within RSGISLib more 
 *  easily...
 *
 *  Copyright 2012 RSGISLib.
 *
 *  RSGISLib is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  RSGISLib is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with RSGISLib.  If not, see <http://www.gnu.org/licenses/>.
 *
 */



#ifndef RSGISMaximumLikelihood_H
#define RSGISMaximumLikelihood_H

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "RSGISMathsUtils.h"
#include "math/RSGISMathException.h"
#include "math/RSGISMaximumLikelihoodException.h"

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#ifdef _MSC_VER
    #ifdef rsgis_maths_EXPORTS
        #define DllExport   __declspec( dllexport )
    #else
        #define DllExport   __declspec( dllimport )
    #endif
#else
    #define DllExport
#endif

namespace rsgis {namespace math{
    
    #define CTINY 1.0e-32
    typedef struct DllExport MaximumLikelihood
    {
        /**
         * number of classes
         */
        int nclasses;
        /**
         * array of the class names
         */
        int *classes;
        /** 
         * number of examples contained in each class
         */
        int *npoints_for_class; 
        /** 
         * number of predictor variables
         */
        int d;
        /** 
         * for each class, the mean value of the examples stored
         * in an array of length nvars
         */
        double **mean;
        /** 
         * for each class, the covariance matrix of the esamples
         * stored in a matrix of dimension nvars x nvars
         */
        double ***covar;
        /** 
         * for each class, the inverse of the covariance matrix
         * stored in a matrix of dimension nvars x nvars
         */
        double ***inv_covar;
        /** 
         * prior probabilities of each class
         */
        double *priors;
        /**
         * for each class, the determinant of the inverse of the
         * covariance matrix
         */
        double *det;
    } MaximumLikelihood;


    class DllExport RSGISMaximumLikelihood
    {
    public:
        RSGISMaximumLikelihood();
        /**
         * Compute ml model, given a matrix of examples x of dimension
         * n x d. Classes of each example are contained in y.
         *
         * Return value: 0 on success, 1 otherwise.
         */
        int compute_ml(MaximumLikelihood *ml, int n, int d, double *x[], int y[]) throw(RSGISMaximumLikelihoodException);
        /**
         * predicts ml model on a test point x. Posteriors probability
         * for each class will be stored within the array margin
         * (an array of length ml->nclasses).
         *
         * Return value: the predicted value on success (-1 or 1 for
         * binary classification; 1,...,nclasses in the multiclass case),
         * 0 on succes with non unique classification, -2 otherwise.
         */
        int predict_ml(MaximumLikelihood *ml, double x[], double **margin) throw(RSGISMaximumLikelihoodException);
        ~RSGISMaximumLikelihood();
    protected:
        /**
         * Extract unique values from a vector y of n integers.
         *
         * Return value: the number of unique values on success, 0 otherwise.
         */
        int iunique(int y[], int n, int **values) throw(RSGISMaximumLikelihoodException);
        /**
         * Sort a[] (an array of n integers) by "heapsort" according to action
         * action=SORT_ASCENDING (=1) --> sorting in ascending order
         * action=SORT_DESCENDING (=2) --> sorting in descending order
         * sort ib[] alongside;
         * if initially, ib[] = 0...n-1, it will contain the permutation finally
         */
        void isort(int a[], int ib[], int n, rsgissort action);
        void compute_covar(double ***mat, MaximumLikelihood *ml, int classID);
        void compute_mean(double ***mat, MaximumLikelihood *ml, int classID);
        /**
         * Allocates memory for an array of n integers.
         *
         * Return value: a pointer to the allocated  memory or
         * NULL if the request fails
         */
        inline int *ivector(long n)
        {
            int *v;
            
            if(n<1)
            {
                fprintf(stderr,"ivector: parameter n must be > 0\n");
                return NULL;
            }
            
            if(!(v=(int *)calloc(n,sizeof(int))))
            {
                fprintf(stderr,"ivector: out of memory\n");
                return NULL;
            }
            
            return v;
        };
        /**
         * Allocates memory for an array of n doubles
         *
         * Return value: a pointer to the allocated  memory or
         * NULL if the request fails
         */
        inline double *dvector(long n)
        {
            double *v;
            
            if(n<1)
            {
                fprintf(stderr,"dvector: parameter n must be > 0\n");
                return NULL;
            }
            
            if (!(v=(double *)calloc(n,sizeof(double))))
            {
                fprintf(stderr,"dvector: out of memory\n");
                return NULL;
            }
            
            return v;
        };
        /**
         * Allocates memory for a matrix of n x m doubles
         *
         * Return value: a pointer to the allocated  memory or
         * NULL if the request fails
         */
        inline double **dmatrix(long n, long m)
        {
            double **M;
            int i;
            
            if(n<1 || m<1)
            {
                fprintf(stderr,"dmatrix: parameters n and m must be > 0\n");
                return NULL;
            }
            
            if(!(M=(double **)calloc(n,sizeof(double*))))
            {
                fprintf(stderr,"dmatrix: out of memory");
                return NULL;
            }
            
            for(i=0;i<n;i++)
            {
                if(!(M[i]=(double*)dvector(m)))
                {
                    fprintf(stderr,"dmatrix: error allocating memory for M[%d]\n",i);
                    return NULL;
                }
            }
            
            if (!M)
            {
                fprintf(stderr,"dmatrix: out of memory\n");
                return NULL;
            }
            
            return M;
        };
        /**
         * Allocates memory for a matrix of n x m integers
         *
         * Return value: a pointer to the allocated  memory or
         * NULL if the request fails
         */
        inline int **imatrix(long n, long m)
        {
            int **M;
            int i;
            
            if(n<1 || m<1)
            {
                fprintf(stderr,"imatrix: parameters n and m must be > 0\n");
                return NULL;
            }
            
            if(!(M=(int **)calloc(n,sizeof(int*))))
            {
                fprintf(stderr,"imatrix: out of memory\n");
                return NULL;
            }
            
            for(i=0;i<n;i++)
            {
                if(!(M[i]=(int*)ivector(m)))
                {
                    fprintf(stderr,"imatrix: error allocating memory for M[%d]\n",i);
                    return NULL;
                }
            }
            
            if (!M)
            {
                fprintf(stderr,"imatrix: out of memory\n");
                return NULL;
            }
            
            return M;
        }
        /**
         * Frees the memory space pointed to by v
         *
         * Return value: 1 if v is NULL, 0 otherwise
         */
        inline int free_ivector(int *v)
        {
            if(!v)
            {
                fprintf(stderr,"free_ivector: pointer v empty\n");
                return 1;
            }
            
            free(v);
            return 0;
        };
        /**
         * Frees the memory space pointed to by v
         *
         * Return value: 1 if v is NULL, 0 otherwise
         */
        inline int free_dvector(double *v)
        {
            if(!v)
            {
                fprintf(stderr,"free_dvector: pointer v empty\n");
                return 1;
            }
            
            free(v);
            return 0;
        };
        /**
         * Frees the memory space pointed to by a n xm matrix of doubles
         *
         * Return value: a positive integer if an error occurred, 0 otherwise
         */
        inline int free_dmatrix(double **M, long n, long m)
        {
            int i;
            
            if(n<1 || m<1)
            {
                fprintf(stderr,"free_dmatrix: parameters n and m must be > 0\n");
                return 1;
            }
            
            if(!M)
            {
                fprintf(stderr,"free_dmatrix: pointer M empty\n");
                return 2;
            }
            
            for(i=0;i<n;i++){
                if(!M[i]){
                    fprintf(stderr,"free_dmatrix: pointer M[%d] empty\n",i);
                    return 3;
                }
                free(M[i]);
            }
            
            free(M);
            
            return 0;
        };
        /**
         * Frees the memory space pointed to by a n x m matrix of integers
         *
         * Return value: a positive integer if an error occurred, 0 otherwise
         */
        inline int free_imatrix(int **M, long n, long m)
        {
            int i;
            
            if(n<1 || m<1)
            {
                fprintf(stderr,"free_imatrix: parameters n and m must be > 0\n");
                return 1;
            }
            
            if(!M)
            {
                fprintf(stderr,"free_imatrix: pointer M empty\n");
                return 2;
            }
            
            for(i=0;i<n;i++)
            {
                if(!M[i])
                {
                    fprintf(stderr,"free_imatrix: pointer M[%d] empty\n",i);
                    return 3;
                }
                free(M[i]);
            }
            
            free(M);
            
            return 0;
        };
        /**
         * Compute inverse matrix of a n xn matrix A.
         *
         * Return value: 0 on success, 1 otherwise.
         */
        inline int inverse(double *A[],double *inv_A[],int n)
        {
            double d,*col, **tmpA;
            int i,j,*indx;
            
            tmpA=dmatrix(n,n);
            
            for (j=0;j<n;j++)
            {
                for (i=0;i<n;i++)
                {
                    tmpA[j][i]=A[j][i];
                }
            }
            
            col=dvector(n);
            indx=ivector(n);
            
            if(ludcmp(tmpA,n,indx,&d) !=0)
            {
                fprintf(stderr,"inverse: ludcmp error\n");
                return 1;
            }
            for (j=0;j<n;j++)
            {
                for (i=0;i<n;i++)
                {
                    col[i]=0;
                }
                col[j]=1;
                lubksb(tmpA,n,indx,col);
                for (i=0;i<n;i++)
                {
                    inv_A[i][j]=col[i];
                }
            }
            
            free_dvector(col);
            free_ivector(indx);
            free_dmatrix(tmpA,n,n);
            
            return 0;
        };
        
        /**
         * Compute determinant of a n x n matrix A.
         *
         * Return value: the determinant
         */
        inline double determinant(double *A[],int n)
        {
            double d, **tmpA;
            int i,j,*indx;
            
            tmpA=dmatrix(n,n);
            
            for (j=0;j<n;j++)
            {
                for (i=0;i<n;i++)
                {
                    tmpA[j][i]=A[j][i];
                }
            }
            
            indx=ivector(n);
            
            ludcmp(tmpA,n,indx,&d);
            
            for (j=0;j<n;j++)
            {
                d *= tmpA[j][j];
            }
            
            free_ivector(indx);
            free_dmatrix(tmpA,n,n);
            
            return(d);
        };
        inline int ludcmp(double *a[],int n,int indx[],double *d)
        {
            int i,imax=0,j,k;
            double big,dum,sum,temp;
            double *vv;
            
            vv=dvector(n);
            *d=1.0;
            for (i=0;i<n;i++)
            {
                big=0;
                for (j=0;j<n;j++)
                {
                    if ((temp=fabs(a[i][j]))>big)
                    {
                        big=temp;
                    }
                }
                if (big==0.0)
                {
                    fprintf(stderr,"ludcmp: singular matrix\n");
                    return 1;
                }
                vv[i]=1.0/big;
            }
            for (j=0;j<n;j++)
            {
                for (i=0;i<j;i++)
                {
                    sum=a[i][j];
                    for (k=0;k<i;k++) sum -= a[i][k]*a[k][j];
                    a[i][j]=sum;
                }
                big=0.0;
                for (i=j;i<n;i++)
                {
                    sum=a[i][j];
                    for (k=0;k<j;k++)
                    {
                        sum -= a[i][k]*a[k][j];
                    }
                    a[i][j]=sum;
                    if ((dum=vv[i]*fabs(sum))>=big)
                    {
                        big=dum;
                        imax=i;
                    }
                }
                if (j!=imax)
                {
                    for (k=0;k<n;k++)
                    {
                        dum=a[imax][k];
                        a[imax][k]=a[j][k];
                        a[j][k]=dum;
                    }
                    *d= -(*d);
                    vv[imax]=vv[j];
                }
                indx[j]=imax;
                if (a[j][j]==0.0)
                {
                    a[j][j]=CTINY;
                }
                if (j!=n)
                {
                    dum=1.0/a[j][j];
                    for (i=j+1;i<n;i++)
                    {
                        a[i][j]*=dum;
                    }
                }
            }
            free_dvector(vv);
            return 0;
        };
        /**
         * Solve linear equation Ax=B
         * a has to be a LU decomposed n x n matrix, and indx
         * is usually the output of ludcmp.
         * On output, b contains the solution
         */
        inline void lubksb(double *a[],int n,int indx[],double b[])
        {
            int i,ii= -1,ip,j;
            double sum;
            
            for (i=0;i<n;i++)
            {
                ip=indx[i];
                sum=b[ip];
                b[ip]=b[i];
                if (ii>=0)
                {
                    for (j=ii;j<=i-1;j++)
                    {
                        sum -=a[i][j]*b[j];
                    }
                }
                else if (sum!=0.0)
                {
                    ii=i;
                }
                b[i]=sum;
            }
            for(i=n-1;i>=0;i--)
            {
                sum=b[i];
                for (j=i+1;j<n;j++)
                {
                    sum -= a[i][j]*b[j];
                }
                b[i]=sum/a[i][i];
            }
        };
    };
    
}}

#endif

