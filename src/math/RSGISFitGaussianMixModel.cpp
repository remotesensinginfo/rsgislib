/*
 *  RSGISFitGaussianMixModel.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 12/03/2015.
 *  Copyright 2015 RSGISLib.
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

#include "RSGISFitGaussianMixModel.h"

namespace rsgis{namespace math{
    
    RSGISFitGaussianMixModel::RSGISFitGaussianMixModel()
    {
        
    }
    
    std::vector<GaussianModelParams> RSGISFitGaussianMixModel::performFit(std::vector<std::pair<double, double> > *hist, float binWidth, double peakThres, double ampVar, unsigned int peakLocVar, unsigned int initWidth, double minWidth, double maxWidth, bool debug_info)
    {
        std::vector<GaussianModelParams> outGauParams;
        try
        {
            if(hist->size() < 4)
            {
                throw RSGISMathException("RSGISFitGaussianMixModel: There must be at least 4 histogram bins.");
            }
            
            
            // Count the number of peaks...
            std::vector<unsigned int> peakIdxs = std::vector<unsigned int>();
            
            unsigned int histLenLess1 = hist->size()-1;
            double forGrad = 0;
            double backGrad = 0;

            for(unsigned int i = 1; i < histLenLess1; ++i)
            {
                forGrad = hist->at(i).second - hist->at(i-1).second;
                backGrad = hist->at(i+1).second - hist->at(i).second;
                if((forGrad > 0) & (backGrad < 0))
                {
                    if(hist->at(i).second > peakThres)
                    {
                        peakIdxs.push_back(i);
                    }
                }
            }
            
            if(debug_info)
            {
                std::cout << "Number of Peaks: " << peakIdxs.size() << std::endl;
                for(std::vector<unsigned int>::iterator iterPeaks = peakIdxs.begin(); iterPeaks != peakIdxs.end(); ++iterPeaks)
                {
                    std::cout << *iterPeaks << ": " << hist->at(*iterPeaks).first << ", " << hist->at(*iterPeaks).second << std::endl;
                }
            }
            
            if(peakIdxs.size() > 0)
            {
                rsgis_mp_config *mpConfigValues = new rsgis_mp_config();
                mpConfigValues->ftol = 1e-10;
                mpConfigValues->xtol = 1e-10;
                mpConfigValues->gtol = 1e-10;
                mpConfigValues->epsfcn = RSGIS_MP_MACHEP0;
                mpConfigValues->stepfactor = 100.0;
                mpConfigValues->covtol = 1e-14;
                mpConfigValues->maxiter = 10;
                mpConfigValues->maxfev = 0;
                mpConfigValues->nprint = 1;
                mpConfigValues->douserscale = 0;
                mpConfigValues->nofinitecheck = 0;
                mpConfigValues->iterproc = 0;
                
                rsgis_mp_result *mpResultsValues = new rsgis_mp_result();
                mpResultsValues->bestnorm = 0;
                mpResultsValues->orignorm = 0;
                mpResultsValues->niter = 0;
                mpResultsValues->nfev = 0;
                mpResultsValues->status = 0;
                mpResultsValues->npar = 0;
                mpResultsValues->nfree = 0;
                mpResultsValues->npegged = 0;
                mpResultsValues->nfunc = 0;
                mpResultsValues->resid = 0;
                mpResultsValues->xerror = 0;
                mpResultsValues->covar = 0; // Not being retrieved
                
                int numOfParams = (peakIdxs.size() * 3) + 1;
                /*
                 * p[0] = noise
                 * p[1] = amplitude
                 * p[2] = time offset
                 * p[3] = width
                 */
                double *parameters = new double[numOfParams];
                mp_par *paramConstraints = new mp_par[numOfParams];
                
                parameters[0] = peakThres/3;
                paramConstraints[0].fixed = false;
                paramConstraints[0].limited[0] = true;
                paramConstraints[0].limited[1] = true;
                paramConstraints[0].limits[0] = 0.0;
                paramConstraints[0].limits[1] = peakThres; // We can't detect peaks above the noise threshold
                paramConstraints[0].parname = const_cast<char*>(std::string("Noise").c_str());;
                paramConstraints[0].step = 0.01;
                paramConstraints[0].relstep = 0;
                paramConstraints[0].side = 0;
                paramConstraints[0].deriv_debug = 0;
                
                int idx = 0;
                for(unsigned int i = 0; i < peakIdxs.size(); ++i)
                {
                    idx = (i*3)+1;
                    parameters[idx] = hist->at(peakIdxs.at(i)).second; // Amplitude
                    paramConstraints[idx].fixed = false;
                    paramConstraints[idx].limited[0] = true;
                    paramConstraints[idx].limited[1] = true;
                    paramConstraints[idx].limits[0] = parameters[idx] - ampVar;
                    if(paramConstraints[idx].limits[0] < 0.001)
                    {
                        paramConstraints[idx].limits[0] = 0.001;
                    }
                    paramConstraints[idx].limits[1] = parameters[idx] + ampVar;
                    paramConstraints[idx].parname = const_cast<char*>(std::string("Amplitude").c_str());;
                    paramConstraints[idx].step = 0;
                    paramConstraints[idx].relstep = 0;
                    paramConstraints[idx].side = 0;
                    paramConstraints[idx].deriv_debug = 0;
                    
                    parameters[idx+1] = hist->at(peakIdxs.at(i)).first; // X Value
                    paramConstraints[idx+1].fixed = false;
                    paramConstraints[idx+1].limited[0] = true;
                    paramConstraints[idx+1].limited[1] = true;
                    paramConstraints[idx+1].limits[0] = parameters[idx+1] - (peakLocVar*binWidth);
                    paramConstraints[idx+1].limits[1] = parameters[idx+1] + (peakLocVar*binWidth);
                    paramConstraints[idx+1].parname = const_cast<char*>(std::string("XVal").c_str());;
                    paramConstraints[idx+1].step = 0;
                    paramConstraints[idx+1].relstep = 0;
                    paramConstraints[idx+1].side = 0;
                    paramConstraints[idx+1].deriv_debug = 0;
                    
                    parameters[idx+2] = initWidth * binWidth;
                    paramConstraints[idx+2].fixed = false;
                    paramConstraints[idx+2].limited[0] = true;
                    paramConstraints[idx+2].limited[1] = true;
                    paramConstraints[idx+2].limits[0] = minWidth;
                    paramConstraints[idx+2].limits[1] = maxWidth;
                    paramConstraints[idx+2].parname = const_cast<char*>(std::string("Width").c_str());;
                    paramConstraints[idx+2].step = 0.01;
                    paramConstraints[idx+2].relstep = 0;
                    paramConstraints[idx+2].side = 0;
                    paramConstraints[idx+2].deriv_debug = 0;
                    
                }
                
                if(debug_info)
                {
                    std::cout << "Noise = " << parameters[0] << std::endl << std::endl;
                    for(unsigned int i = 0; i < peakIdxs.size(); ++i)
                    {
                        idx = (i*3)+1;
                        std::cout << "Point " << i+1 << " Amplitude = " << parameters[idx] << std::endl;
                        std::cout << "Point " << i+1 << " XVal = " << parameters[idx+1] << std::endl;
                        std::cout << "Point " << i+1 << " Width = " << parameters[idx+2] << std::endl << std::endl;
                    }
                }

                
                // Contruct data for decomposition
                GaussianModelData *decompData = new GaussianModelData();
                decompData->xVal = new double[hist->size()];
                decompData->amplitude = new double[hist->size()];
                decompData->error = new double[hist->size()];
                for(unsigned int i = 0; i < hist->size(); ++i)
                {
                    decompData->xVal[i] = hist->at(i).first;
                    decompData->amplitude[i] = hist->at(i).second;
                    decompData->error[i] = 1.0;
                }
                
                
                
                /*
                 * int m     - number of data points
                 * int npar  - number of parameters
                 * double *xall - parameters values (initial values and then best fit values)
                 * mp_par *pars - Constrains
                 * mp_config *config - Configuration parameters
                 * void *private_data - Waveform data structure
                 * mp_result *result - diagnostic info from function
                 */
                int returnCode = rsgis_mpfit(gaussianSum, hist->size(), numOfParams, parameters, paramConstraints, mpConfigValues, decompData, mpResultsValues);
                
                if(debug_info)
                {
                    if(returnCode == RSGIS_MP_OK_CHI)
                    {
                        std::cout << "mpfit - Convergence in chi-square value.\n";
                    }
                    else if(returnCode == RSGIS_MP_OK_PAR)
                    {
                        std::cout << "mpfit - Convergence in parameter value.\n";
                    }
                    else if(returnCode == RSGIS_MP_OK_BOTH)
                    {
                        std::cout << "mpfit - Convergence in chi-square and parameter value.\n";
                    }
                    else if(returnCode == RSGIS_MP_OK_DIR)
                    {
                        std::cout << "mpfit - Convergence in orthogonality.\n";
                    }
                    else if(returnCode == RSGIS_MP_MAXITER)
                    {
                        std::cout << "mpfit - Maximum number of iterations reached.\n";
                    }
                    else if(returnCode == RSGIS_MP_FTOL)
                    {
                        std::cout << "mpfit - ftol is too small; cannot make further improvements.\n";
                    }
                    else if(returnCode == RSGIS_MP_XTOL)
                    {
                        std::cout << "mpfit - xtol is too small; cannot make further improvements.\n";
                    }
                    else if(returnCode == RSGIS_MP_XTOL)
                    {
                        std::cout << "mpfit - gtol is too small; cannot make further improvements.\n";
                    }
                    else
                    {
                        std::cout << "An error has probably occurred - wait for exception...\n";
                    }
                    
                    
                    std::cout << "Run Results (MPFIT version: " << mpResultsValues->version << "):\n";
                    std::cout << "Final Chi-Squaured = " << mpResultsValues->bestnorm << std::endl;
                    std::cout << "Start Chi-Squaured = " << mpResultsValues->orignorm << std::endl;
                    std::cout << "Num Iterations = " << mpResultsValues->niter << std::endl;
                    std::cout << "Num Func Evals = " << mpResultsValues->nfev << std::endl;
                    std::cout << "Status Fit Code = " << mpResultsValues->status << std::endl;
                    std::cout << "Num Params = " << mpResultsValues->npar << std::endl;
                    std::cout << "Num Free Params = " << mpResultsValues->nfree << std::endl;
                    std::cout << "Num Pegged Params = " << mpResultsValues->npegged << std::endl;
                    std::cout << "Num Residuals Params = " << mpResultsValues->nfunc << std::endl << std::endl;
                }
                
                if((returnCode == RSGIS_MP_OK_CHI) | (returnCode == RSGIS_MP_OK_PAR) |
                   (returnCode == RSGIS_MP_OK_BOTH) | (returnCode == RSGIS_MP_OK_DIR) |
                   (returnCode == RSGIS_MP_MAXITER) | (returnCode == RSGIS_MP_FTOL)
                   | (returnCode == RSGIS_MP_XTOL) | (returnCode == RSGIS_MP_XTOL))
                {
                    // MP Fit completed.. On on debug_info for more information.
                }
                else if(returnCode == RSGIS_MP_ERR_INPUT)
                {
                    throw RSGISMathException("mpfit - Check inputs.");
                }
                else if(returnCode == RSGIS_MP_ERR_NAN)
                {
                    throw RSGISMathException("mpfit - Sum of Gaussians function produced NaN value.");
                }
                else if(returnCode == RSGIS_MP_ERR_FUNC)
                {
                    throw RSGISMathException("mpfit - No Sum of Gaussians function was supplied.");
                }
                else if(returnCode == RSGIS_MP_ERR_NPOINTS)
                {
                    throw RSGISMathException("mpfit - No data points were supplied.");
                }
                else if(returnCode == RSGIS_MP_ERR_NFREE)
                {
                    throw RSGISMathException("mpfit - No parameters are free - i.e., nothing to optimise!");
                }
                else if(returnCode == RSGIS_MP_ERR_MEMORY)
                {
                    throw RSGISMathException("mpfit - memory allocation error - may have run out!");
                }
                else if(returnCode == RSGIS_MP_ERR_INITBOUNDS)
                {
                    throw RSGISMathException("mpfit - Initial parameter values inconsistant with constraints.");
                }
                else if(returnCode == RSGIS_MP_ERR_PARAM)
                {
                    throw RSGISMathException("mpfit - An error has occur with an input parameter.");
                }
                else if(returnCode == RSGIS_MP_ERR_DOF)
                {
                    throw RSGISMathException("mpfit - Not enough degrees of freedom.");
                }
                else
                {
                    std::cout << "Return code is :" << returnCode << " - this can not been defined!\n";
                }
                
                if(debug_info)
                {
                    std::cout << "Noise = " << parameters[0] << std::endl << std::endl;
                    for(unsigned int i = 0; i < peakIdxs.size(); ++i)
                    {
                        idx = (i*3)+1;
                        std::cout << "Gaussian " << i+1 << " Amplitude = " << parameters[idx] << std::endl;
                        std::cout << "Gaussian " << i+1 << " XVal = " << parameters[idx+1] << std::endl;
                        std::cout << "Gaussian " << i+1 << " Width = " << parameters[idx+2] << std::endl << std::endl;
                    }
                }
                
                
                outGauParams.reserve(peakIdxs.size());
                for(unsigned int i = 0; i < peakIdxs.size(); ++i)
                {
                    GaussianModelParams params = GaussianModelParams();
                    params.noise = parameters[0];
                    idx = (i*3)+1;
                    params.amplitude = parameters[idx];
                    params.offset = parameters[idx+1];
                    params.fwhm = parameters[idx+2];
                    outGauParams.push_back(params);
                }
                
                
                delete[] decompData->xVal;
                delete[] decompData->amplitude;
                delete[] decompData->error;
                delete decompData;
                
                delete[] parameters;
                delete[] paramConstraints;
                
                delete mpConfigValues;
                delete mpResultsValues;
            }
        }
        catch (RSGISMathException &e)
        {
            throw e;
        }
        catch(RSGISException &e)
        {
            throw RSGISMathException(e.what());
        }
        catch(std::exception &e)
        {
            throw RSGISMathException(e.what());
        }
        
        return outGauParams;
    }
    
    RSGISFitGaussianMixModel::~RSGISFitGaussianMixModel()
    {
        
    }
    
    
}}
