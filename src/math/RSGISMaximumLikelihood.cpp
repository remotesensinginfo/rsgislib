/*
 *  RSGISMaximumLikelihood.cpp
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

#include "RSGISMaximumLikelihood.h"

#include <sstream>

namespace rsgis {namespace math{

    std::string NumberToString(int Number)
    {
        std::ostringstream ss;
        ss << Number;
        return ss.str();
    }
    
    RSGISMaximumLikelihood::RSGISMaximumLikelihood()
    {
        
    }
    
    int RSGISMaximumLikelihood::compute_ml(MaximumLikelihood *ml,int n,int d,double *x[],int y[])
    {
        double ***tmpMat;
        int *index;
        int i,j,k;
        
        
        ml->nclasses=this->iunique(y,n, &(ml->classes));
        
        if(ml->nclasses<=0)
        {
            throw RSGISMaximumLikelihoodException("compute_ml: iunique error");
        }
        
        if(ml->nclasses==1)
        {
            throw RSGISMaximumLikelihoodException("compute_ml: only 1 class recognized");
        }
        
        if(ml->nclasses==2)
        {
            if(ml->classes[0] != -1 || ml->classes[1] != 1)
            {
                throw RSGISMaximumLikelihoodException("compute_ml: for binary classification classes must be -1,1");
            }
        }
        
        if(ml->nclasses > 2)
        {
            for(i=0; i<ml->nclasses; i++)
            {
                if(ml->classes[i] != i+1)
                {
                    std::string msg = std::string("compute_ml: for ") + NumberToString(ml->nclasses) + "-class classification classes must be 1,...," +
                            NumberToString(ml->nclasses);
                    throw RSGISMaximumLikelihoodException(msg);
                }
            }
        }
        
        if(!(ml->npoints_for_class=ivector(ml->nclasses)))
        {
            throw RSGISMaximumLikelihoodException("compute_ml: out of memory");
        }
        
        for(i=0;i<n;i++)
        {
            for(j=0;j<ml->nclasses;j++)
            {
                if(y[i] == ml->classes[j])
                {
                    ml->npoints_for_class[j]+=1;
                }
            }
        }
        
        ml->d = d;
        if(!(ml->priors = dvector(ml->nclasses)))
        {
            throw RSGISMaximumLikelihoodException("compute_ml: out of memory");
        }
        if(!(ml->mean=dmatrix(ml->nclasses,ml->d)))
        {
            throw RSGISMaximumLikelihoodException("compute_ml: out of memory");
        }
        if(!(ml->det = dvector(ml->nclasses)))
        {
            throw RSGISMaximumLikelihoodException("compute_ml: out of memory");
        }
        if(!(ml->covar = (double ***) calloc(ml->nclasses, sizeof(double **))))
        {
            throw RSGISMaximumLikelihoodException("compute_ml: out of memory");
        }
        for(i=0;i<ml->nclasses;i++)
        {
            if(!(ml->covar[i] = dmatrix(ml->d,ml->d)))
            {
                throw RSGISMaximumLikelihoodException("compute_ml: out of memory");
            }
        }
        
        if(!(tmpMat = (double ***) calloc(ml->nclasses, sizeof(double **))))
        {
            throw RSGISMaximumLikelihoodException("compute_ml: out of memory");
        }
        for(i=0;i<ml->nclasses;i++)
        {
            if(!(tmpMat[i] = dmatrix(ml->npoints_for_class[i],ml->d)))
            {
                throw RSGISMaximumLikelihoodException("compute_ml: out of memory");
            }
        }
        
        
        if(!(index = ivector(ml->nclasses)))
        {
            throw RSGISMaximumLikelihoodException("compute_ml: out of memory");
        }
        for(i=0;i<n;i++)
        {
            for(j=0;j<ml->nclasses;j++)
            {
                if(y[i]==ml->classes[j])
                {
                    for(k=0;k<ml->d;k++)
                    {
                        tmpMat[j][index[j]][k] = x[i][k];
                    }
                    index[j] += 1;
                }
            }
        }
        
        for(i=0;i<ml->nclasses;i++)
        {
            compute_mean(tmpMat,ml,i);
        }
        
        for(i=0;i<ml->nclasses;i++)
        {
            compute_covar(tmpMat,ml,i);
        }
        
        for(i=0;i<ml->nclasses;i++)
        {
            ml->priors[i] = (double)ml->npoints_for_class[i] / (double)n;
        }
        
        for(i=0;i<ml->nclasses;i++)
        {
            free_dmatrix(tmpMat[i],ml->npoints_for_class[i],ml->d);
        }
        
        if(!(ml->det = dvector(ml->nclasses)))
        {
            throw RSGISMaximumLikelihoodException("compute_ml: out of memory");
        }
        if(!(ml->inv_covar = (double ***) calloc(ml->nclasses, sizeof(double **))))
        {
            throw RSGISMaximumLikelihoodException("compute_ml: out of memory");
        }
        for(i=0;i<ml->nclasses;i++)
        {
            if(!(ml->inv_covar[i] = dmatrix(ml->d,ml->d)))
            {
                throw RSGISMaximumLikelihoodException("compute_ml: out of memory");
            }
        }
        
        for(j=0;j<ml->nclasses;j++)
        {
            ml->det[j] = determinant(ml->covar[j],ml->d);
            if(inverse(ml->covar[j],ml->inv_covar[j],ml->d)!=0)
            {
                std::string msg = std::string("compute_ml: error computing inverse covariance matrix of class ") + NumberToString(ml->classes[j]);
            }
        }
        
        free(tmpMat);
        free_ivector(index);
        
        return 0;
    }
    
    
    int RSGISMaximumLikelihood::predict_ml(MaximumLikelihood *ml, double x[], double **margin)
    {
        int i,j,c;
        double *tmpVect;
        double *distmean;
        double delta;
        double max_posterior;
        int max_posterior_index;
        double sum_posteriors;
        
        if(!(tmpVect = dvector(ml->d)))
        {
            throw RSGISMaximumLikelihoodException("predict_ml: out of memory");
        }
        if(!(distmean= dvector(ml->d)))
        {
            throw RSGISMaximumLikelihoodException("predict_ml: out of memory");
        }
        if(!((*margin)= dvector(ml->nclasses)))
        {
            throw RSGISMaximumLikelihoodException("predict_ml: out of memory");
        }
        
        for(c=0;c<ml->nclasses;c++)
        {
            for(i=0;i<ml->d;i++)
            {
                distmean[i] = x[i] - ml->mean[c][i];
            }
            
            for(i=0;i<ml->d;i++)
            {
                tmpVect[i] = 0.0;
            }
            
            for(i=0;i<ml->d;i++)
            {
                for(j=0;j<ml->d;j++)
                {
                    tmpVect[i] += distmean[j] * ml->inv_covar[c][j][i];
                }
            }
            
            delta=0.0;
            for(i=0;i<ml->d;i++)
            {
                delta += tmpVect[i] * distmean[i];
            }
            
            if(ml->det[c] > 0.0)
            {
                (*margin)[c] = exp(-0.5 * delta)/ sqrt(ml->det[c]);
            }
            else
            {
                std::string msg = std::string("predict_ml:  det. of cov. matrix of class ") + NumberToString(c) + " = 0";
                throw RSGISMaximumLikelihoodException(msg);
            }
            (*margin)[c] = (*margin)[c] * ml->priors[c];
        }
        
        max_posterior = 0.0;
        max_posterior_index =0;
        sum_posteriors = 0.0;
        for(c=0;c<ml->nclasses;c++)
        {
            sum_posteriors += (*margin)[c];
            if((*margin)[c] > max_posterior)
            {
                max_posterior = (*margin)[c];
                max_posterior_index = c;
            }
        }
        for(c=0;c<ml->nclasses;c++)
        {
            (*margin)[c] /= sum_posteriors;
        }
        
        free_dvector(tmpVect);
        free_dvector(distmean);
        
        return ml->classes[max_posterior_index];
    }
    
    int RSGISMaximumLikelihood::iunique(int y[], int n, int **values)
    {
        int nvalues=1;
        int i,j;
        int addclass;
        int *indx;
        
        if(!(*values=ivector(1)))
        {
            throw RSGISMaximumLikelihoodException("iunique: out of memory");
        }
        
        (*values)[0]=y[0];
        for(i=1; i<n; i++)
        {
            addclass=1;
            for(j=0;j<nvalues;j++)
            {
                if((*values)[j]==y[i])
                {
                    addclass=0;
                }
            }
            if(addclass)
            {
                if(!(*values=(int*)realloc(*values,(nvalues+1)*sizeof(int))))
                {
                    throw RSGISMaximumLikelihoodException("iunique: out of memory");
                }
                (*values)[nvalues++]=y[i];
            }
        }
        
        if(!(indx=ivector(nvalues)))
        {
            throw RSGISMaximumLikelihoodException("iunique: out of memory");
        }
        
        this->isort(*values, indx, nvalues, rsgis_ascending);
        
        if(free_ivector(indx)!=0)
        {
            throw RSGISMaximumLikelihoodException("iunique: free_ivector error");
        }
        
        return nvalues;
    }
    
    void RSGISMaximumLikelihood::isort(int a[], int ib[], int n, rsgissort action)
    {
        int l, j, ir, i;
        int ra;
        int ii;
        
        
        if (n <= 1)
        {
            return;
        }
        
        a--; ib--;
        
        l = (n >> 1) + 1;
        ir = n;
        
        for (;;)
        {
            if (l > 1)
            {
                l = l - 1;
                ra = a[l];
                ii = ib[l];
            }
            else
            {
                ra = a[ir];
                ii = ib[ir];
                a[ir] = a[1];
                ib[ir] = ib[1];
                if (--ir == 1)
                {
                    a[1] = ra;
                    ib[1] = ii;
                    return;
                }
            }
            i = l;
            j = l << 1;
            switch(action)
            {
                case rsgis_descending:
                    while (j <= ir)
                    {
                        if (j < ir && a[j] > a[j + 1])
                        {
                            ++j;
                        }
                        if (ra > a[j])
                        {
                            a[i] = a[j];
                            ib[i] = ib[j];
                            j += (i = j);
                        }
                        else
                        {
                            j = ir + 1;
                        }
                    }
                    break;
                case rsgis_ascending:
                    while (j <= ir)
                    {
                        if (j < ir && a[j] < a[j + 1])
                        {
                            ++j;
                        }
                        if (ra < a[j])
                        {
                            a[i] = a[j];
                            ib[i] = ib[j];
                            j += (i = j);
                        }
                        else
                        {
                            j = ir + 1;
                        }
                    }
                    break;
            }
            a[i] = ra;
            ib[i] = ii;
        }
    }
    
    void RSGISMaximumLikelihood::compute_covar(double ***mat, MaximumLikelihood *ml, int classID)
    {        
        for(int i = 0; i < ml->d; i++)
        {
            for(int j = i; j < ml->d; j++)
            {
                for(int k = 0; k < ml->npoints_for_class[classID]; k++)
                {
                    ml->covar[classID][i][j] += (mat[classID][k][i] - ml->mean[classID][i]) * (mat[classID][k][j] - ml->mean[classID][j]);
                }
                ml->covar[classID][j][i] = ml->covar[classID][i][j];
            }
        }
        for(int i = 0; i < ml->d; i++)
        {
            for(int j = 0; j < ml->d; j++)
            {
                ml->covar[classID][i][j] /= ((double)ml->npoints_for_class[classID] - 1.);
            }
        }
    }
    
    void RSGISMaximumLikelihood::compute_mean(double ***mat, MaximumLikelihood *ml, int classID)
    {        
        for(int i=0;i < ml->d;i++)
        {
            for(int j=0;j< ml->npoints_for_class[classID];j++)
            {
                ml->mean[classID][i] += mat[classID][j][i];
            }
        }
        
        for(int i=0;i < ml->d;i++)
        {
            ml->mean[classID][i] /= ml->npoints_for_class[classID];
        }
    }
    
    RSGISMaximumLikelihood::~RSGISMaximumLikelihood()
    {
        
    }
    
    
}}

