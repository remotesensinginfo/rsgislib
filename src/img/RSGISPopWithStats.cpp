/*
 *  RSGISPopWithStats.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 26/04/2012.
 *  Copyright 2012 RSGISLib. All rights reserved.
 *  This file is part of RSGISLib.
 *
 *  This code is edited from code provided by 
 *  Sam Gillingham
 *
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

#include "RSGISPopWithStats.h"

namespace rsgis { namespace img {

    void RSGISPopWithStats::addpyramid( GDALDataset *handle )
    {
        /* These are the levels Imagine seems to use */

        /* The levels used depend on the size of the image */
        int nLevels[] = { 4, 8, 16, 32, 64, 128, 256, 512 };
        int nOverviews = 0;
        int mindim = 0;
        GDALRasterBand *hBand = NULL;
        const char *pszType = "";

        /* first we work out how many overviews to build based on the size */
        mindim = handle->GetRasterXSize() < handle->GetRasterYSize() ? handle->GetRasterXSize() : handle->GetRasterYSize();

        nOverviews = 0;
        for(int i = 0; i < 8; i++) 
        {
            if( (mindim/nLevels[i]) > MINOVERVIEWDIM )
            {
                ++nOverviews;
            }
        }

        /* Need to find out if we are thematic or continuous */
        hBand = handle->GetRasterBand( 1 );

        if( strcmp( hBand->GetMetadataItem( "LAYER_TYPE", "" ), "athematic" ) == 0 )
        {
            pszType = "AVERAGE";
        }
        else
        {
            pszType = "NEAREST";
        }
        
        int nLastProgress = -1;
        handle->BuildOverviews( pszType, nOverviews, nLevels, 0, NULL,  (GDALProgressFunc)StatsTextProgress, &nLastProgress );
    }

    void RSGISPopWithStats::getRangeMean(float *pData,int size,float &min,float &max,float &mean, bool ignore, float ignoreVal)
    {
        bool bFirst = true;
        double dTot = 0;
        int total_used = 0;
        float val = 0.0;
        for(int count = 0; count < size; count++)
        {
            val = pData[count];
            if( !ignore || val != ignoreVal )
            {
                dTot += val;
                total_used++;
                if( bFirst )
                {
                    max = val;
                    min = val;
                    bFirst = false;
                }
                else
                {
                    if( val > max )
                    {
                        max = val;
                    }
                    else if( val < min )
                    {
                        min = val;
                    }
                }
            }
        }
        mean = dTot / double(total_used);
    }

    float RSGISPopWithStats::getStdDev(float *pData, int size, float fmean, bool ignore, float ignoreVal)
    {
        double dVariance = 0;
        double dTot = 0;
        float val;
        int total_used = 0;
        for(int count = 0; count < size; count++)
        {
            val = pData[count];
            if( !ignore || val != ignoreVal )
            {
                double diff = val - fmean;
                dTot += diff;
                dVariance += diff * diff;
                total_used++;
            }
        }
        if( total_used > 1 )
        {
            dVariance = (dVariance - dTot*dTot/total_used)/(total_used - 1);
            return sqrt(dVariance);
        }
        else
        {
            return 0;
        }
    }
      
    float* RSGISPopWithStats::getSubSampledImage( GDALRasterBand *hBand, int nLevel, int *pnSize )
    {
        /*
         * Our own "pyramid" layer code that does a nearest neighbour resample.
         * Can't use the GDAL pyramid layers as they are generally AVERAGE
         * and can't seem to trick GDAL into making them in a seperate 
         * file for us using GDALRegenerateOverviews.
         * Creates a single dimension array which must be free()ed by the caller.
         */
        int inxsize, inysize, outxsize, outysize, y;
        float *pData;

        inxsize = hBand->GetXSize();
        inysize = hBand->GetYSize();

        outxsize = inxsize / nLevel;
        outysize = inysize / nLevel;

        pData = (float*)malloc( outxsize * outysize * sizeof(float));
        *pnSize = outxsize * outysize;

        for( y = 0; y < outysize; y++ )
        {
            hBand->RasterIO( GF_Read, 0, y*nLevel, inxsize, 1, &pData[y*outxsize], outxsize, 1, GDT_Float32,0, 0 );
        }

        return pData;
    }
      
    void RSGISPopWithStats::getHistogramIgnore( GDALRasterBand *pBand, double dfMin, double dfMax, int nBuckets, int *panHistogram, int bIncludeOutOfRange, bool bIgnore, float fIgnore)
    {        
        int nBlockXSize, nBlockYSize;
        pBand->GetBlockSize( &nBlockXSize, &nBlockYSize );
        int nBlocksPerRow = (pBand->GetXSize()+nBlockXSize-1) / nBlockXSize;
        int nBlocksPerColumn = (pBand->GetYSize()+nBlockYSize-1) / nBlockYSize;

        /* -------------------------------------------------------------------- */
        /*      Figure out the ratio of blocks we will read to get an           */
        /*      approximate value.                                              */
        /* -------------------------------------------------------------------- */
        int         nSampleRate;
        double      dfScale;

        nSampleRate = 1;
        
        if(dfMin == 0)
        {
            dfScale = nBuckets / (dfMax + 1);
        }
        else
        {
            dfScale = nBuckets / (dfMax - dfMin);
        }
        
        /* -------------------------------------------------------------------- */
        /*      Read the blocks, and add to histogram.                          */
        /* -------------------------------------------------------------------- */
        memset( panHistogram, 0, sizeof(int) * nBuckets );
        for(int iSampleBlock = 0; iSampleBlock < nBlocksPerRow * nBlocksPerColumn; iSampleBlock += nSampleRate )
        {
            double dfValue = 0.0, dfReal, dfImag;
            int  iXBlock, iYBlock, nXCheck, nYCheck;
            GDALRasterBlock *poBlock;

            iYBlock = iSampleBlock / nBlocksPerRow;
            iXBlock = iSampleBlock - nBlocksPerRow * iYBlock;
            
            poBlock = pBand->GetLockedBlockRef( iXBlock, iYBlock );
            if( poBlock == NULL )
            {
                return;
            }
            
            if( (iXBlock+1) * nBlockXSize > pBand->GetXSize() )
            {
                nXCheck = pBand->GetXSize() - iXBlock * nBlockXSize;
            }
            else
            {
                nXCheck = nBlockXSize;
            }

            if( (iYBlock+1) * nBlockYSize > pBand->GetYSize() )
            {
                nYCheck = pBand->GetYSize() - iYBlock * nBlockYSize;
            }
            else
            {
                nYCheck = nBlockYSize;
            }

            /* this is a special case for a common situation */
            if( poBlock->GetDataType() == GDT_Byte
                && dfScale == 1.0 && (dfMin >= -0.5 && dfMin <= 0.5)
                && nYCheck == nBlockYSize && nXCheck == nBlockXSize
                && nBuckets == 256 )
            {
                int nPixels = nXCheck * nYCheck;
                GByte  *pabyData = (GByte *) poBlock->GetDataRef();
                GByte val;
                
                for( int i = 0; i < nPixels; i++ )
                {
                    val = pabyData[i];
                    if( !bIgnore || ( val != fIgnore ) )
                    {
                      panHistogram[val]++;
                    }
                }

                poBlock->DropLock();
                continue; /* to next sample block */
            }

            /* this isn't the fastest way to do this, but is easier for now */
            for(int iY = 0; iY < nYCheck; iY++ )
            {
                for( int iX = 0; iX < nXCheck; iX++ )
                {
                    int iOffset = iX + iY * nBlockXSize;
                    int nIndex;

                    switch( poBlock->GetDataType() )
                    {
                      case GDT_Byte:
                        dfValue = ((GByte *) poBlock->GetDataRef())[iOffset];
                        break;
                      case GDT_UInt16:
                        dfValue = ((GUInt16 *) poBlock->GetDataRef())[iOffset];
                        break;
                      case GDT_Int16:
                        dfValue = ((GInt16 *) poBlock->GetDataRef())[iOffset];
                        break;
                      case GDT_UInt32:
                        dfValue = ((GUInt32 *) poBlock->GetDataRef())[iOffset];
                        break;
                      case GDT_Int32:
                        dfValue = ((GInt32 *) poBlock->GetDataRef())[iOffset];
                        break;
                      case GDT_Float32:
                        dfValue = ((float *) poBlock->GetDataRef())[iOffset];
                        break;
                      case GDT_Float64:
                        dfValue = ((double *) poBlock->GetDataRef())[iOffset];
                        break;
                      case GDT_CInt16:
                        dfReal = ((GInt16 *) poBlock->GetDataRef())[iOffset*2];
                        dfImag = ((GInt16 *) poBlock->GetDataRef())[iOffset*2+1];
                        dfValue = sqrt( dfReal * dfReal + dfImag * dfImag );
                        break;
                      case GDT_CInt32:
                        dfReal = ((GInt32 *) poBlock->GetDataRef())[iOffset*2];
                        dfImag = ((GInt32 *) poBlock->GetDataRef())[iOffset*2+1];
                        dfValue = sqrt( dfReal * dfReal + dfImag * dfImag );
                        break;
                      case GDT_CFloat32:
                        dfReal = ((float *) poBlock->GetDataRef())[iOffset*2];
                        dfImag = ((float *) poBlock->GetDataRef())[iOffset*2+1];
                        dfValue = sqrt( dfReal * dfReal + dfImag * dfImag );
                        break;
                      case GDT_CFloat64:
                        dfReal = ((double *) poBlock->GetDataRef())[iOffset*2];
                        dfImag = ((double *) poBlock->GetDataRef())[iOffset*2+1];
                        dfValue = sqrt( dfReal * dfReal + dfImag * dfImag );
                        break;
                      default:
                        CPLAssert( FALSE );
                        return;
                    }
                    
                    if( !bIgnore || ( dfValue != fIgnore ) )
                    {
                        nIndex = (int) floor((dfValue - dfMin) * dfScale);

                        if( nIndex < 0 )
                        {
                            if( bIncludeOutOfRange )
                            {
                                panHistogram[0]++;
                            }
                        }
                        else if( nIndex >= nBuckets )
                        {
                            if( bIncludeOutOfRange )
                            {
                                panHistogram[nBuckets-1]++;
                            }
                        }
                        else
                        {
                            panHistogram[nIndex]++;
                        }
                    }
                }

            }
            poBlock->DropLock();
        }
    }

    void RSGISPopWithStats::calcPopStats( GDALDataset *hHandle, bool bIgnore, float fIgnoreVal, bool bPyramid ) throw(rsgis::RSGISImageException)
    {
        int band;//, xsize, ysize, osize,nlevel;
        GDALRasterBand *hBand;
        char szTemp[64];
        std::string histoType;
        std::string histoTypeDirect = "direct";
        std::string histoTypeLinear = "linear";

        /* we calculate a single overview to speed up the calculation of stats */

        int nBands = hHandle->GetRasterCount();
        for( band = 0; band < nBands; band++ )
        {
            std::cout << "Processing band " << band+1 << " of " << nBands << std::endl;
            
            hBand = hHandle->GetRasterBand( band + 1 );
            
            if(strcmp( hBand->GetMetadataItem( "LAYER_TYPE", "" ), "thematic" ) == 0)
            {
                throw rsgis::RSGISImageException("Use the rastergis function populateImageWithRasterGISStats (In python rsgislib.rastergis.populateStats) function for thermatic layers.");
            }
            
            if( bIgnore )
            {
                hBand->SetNoDataValue( fIgnoreVal );
                sprintf( szTemp, "%f", fIgnoreVal );
                GDALSetMetadataItem( hBand, "STATISTICS_EXCLUDEDVALUES", szTemp, NULL );
            }
            
            /* Find min, Max and mean */ 
            double fmin=0, fmax=0, fMean=0, fStdDev=0;
            int nLastProgress = -1;
            hBand->ComputeStatistics(false, &fmin, &fmax, &fMean, &fStdDev,  (GDALProgressFunc)StatsTextProgress, &nLastProgress);

            /* Write Statistics */
            sprintf( szTemp, "%f", fmin );
            hBand->SetMetadataItem( "STATISTICS_MINIMUM", szTemp, NULL );

            sprintf( szTemp, "%f", fmax );
            hBand->SetMetadataItem( "STATISTICS_MAXIMUM", szTemp, NULL );

            sprintf( szTemp, "%f", fMean );
            hBand->SetMetadataItem( "STATISTICS_MEAN", szTemp, NULL );

            sprintf( szTemp, "%f", fStdDev );
            hBand->SetMetadataItem( "STATISTICS_STDDEV", szTemp, NULL );

            /* make sure that the histogram will work even if there */
            /* is only one value in it                              */
            fmin = fmin == fmax ? fmin - 1e-05 : fmin;

            /* Calc the histogram */
            int *pHisto;
            int nHistBuckets;
            float histmin = 0, histmax = 0, histminTmp = 0, histmaxTmp = 0;
            if( hBand->GetRasterDataType() == GDT_Byte )
            {
                /* if it is 8 bit just do a histo on the lot so we don't get rounding errors */ 
                nHistBuckets = 256;
                histmin = 0;
                histmax = 255;
                histminTmp = -0.5;
                histmaxTmp = 255.5;
                sprintf( szTemp, "%f", fStdDev );
                histoType = histoTypeDirect;
            }
            else if(strcmp( hBand->GetMetadataItem( "LAYER_TYPE", "" ), "thematic" ) == 0)
            {
                nHistBuckets = ceil(fmax)+1;
                histmin = 0;
                histmax = ceil(fmax);
                histminTmp = -0.5;
                histmaxTmp = histmax + 0.5;
                histoType = histoTypeDirect;
            }
            else
            {
                int range = (ceil(fmax) - floor(fmin));
                histmin = fmin;
                histmax = fmax;
                if(range <= HISTO_NBINS)
                {
                    nHistBuckets = range;
                    histoType = histoTypeDirect;
                    
                    histminTmp = histmin - 0.5;
                    histmaxTmp = histmax + 0.5;
                }
                else
                {
                    nHistBuckets = HISTO_NBINS;
                    histoType = histoTypeLinear;
                    histminTmp = histmin;
                    histmaxTmp = histmax;
                }
            }
            pHisto = (int*)calloc(nHistBuckets, sizeof(int));
            if(bIgnore)
            {
                this->getHistogramIgnore(hBand, histminTmp, histmaxTmp, nHistBuckets, pHisto, false, bIgnore, fIgnoreVal);
            }
            else
            {
                int nLastProgress = -1;
                hBand->GetHistogram(histminTmp, histmaxTmp, nHistBuckets, pHisto, true, false,  (GDALProgressFunc)StatsTextProgress, &nLastProgress);
            }
            
            //int histoStrLen = nHistBuckets * 8;
            //char *szHistoString = new char[histoStrLen];
            //szHistoString[0] = '\0';

            /* Mode is the bin with the highest count */
            int maxcount = 0;
            int maxbin = 0;
            long totalvalues = 0;
            for(int count = 0; count < nHistBuckets; count++)
            {
                if( pHisto[count] > maxcount )
                {
                    maxcount = pHisto[count];
                    maxbin = count;
                }
                totalvalues += pHisto[count];
            }
            float fMode = maxbin * ((histmax-histmin) / nHistBuckets);

            if( ( hBand->GetRasterDataType( ) == GDT_Float32 ) || ( hBand->GetRasterDataType( ) == GDT_Float64 ) )
            {
                sprintf( szTemp, "%f", fMode );
            }
            else
            {
                sprintf( szTemp, "%f", floor(fMode+0.5) );
            }  
            GDALSetMetadataItem( hBand, "STATISTICS_MODE", szTemp, NULL );

            int nWhichMedian = -1;
            int nCumSum = 0;
            for(int count = 0; count < nHistBuckets; count++)
            {
                /* we also estimate the median based on the histogram */
                if(nWhichMedian == -1) 
                {
                    /* then haven't found the median yet */
                    nCumSum += pHisto[count];
                    if(nCumSum > (totalvalues/2.0)) 
                    {
                        nWhichMedian = count;
                    }
                }
                //sprintf( szTemp, "%d|", pHisto[count] );
                //strcat( szHistoString, szTemp );
            }

            float fMedian = nWhichMedian * ((histmax-histmin) / nHistBuckets);
            if( nWhichMedian < nHistBuckets - 1 )
            {
                fMedian = ( fMedian + ((nWhichMedian+1) * ((histmax-histmin) / nHistBuckets)) ) / 2;
            }

            if( ( GDALGetRasterDataType( hBand ) == GDT_Float32 ) || 
            ( GDALGetRasterDataType( hBand ) == GDT_Float64 ) )
            {
                sprintf( szTemp, "%f", fMedian );
            }
            else
            {
                sprintf( szTemp, "%f", floor(fMedian+0.5) );
            }
            GDALSetMetadataItem( hBand, "STATISTICS_MEDIAN", szTemp, NULL );

            if( GDALGetRasterDataType( hBand ) == GDT_Byte )
            {
                sprintf( szTemp, "0" );
            }
            else
            {
                sprintf( szTemp, "%f", histmin );
            }
            GDALSetMetadataItem( hBand, "STATISTICS_HISTOMIN", szTemp, NULL );

            if( GDALGetRasterDataType( hBand ) == GDT_Byte )
            {
                sprintf( szTemp, "255" );
            }
            else
            {
                sprintf( szTemp, "%f", histmax );
            }
            GDALSetMetadataItem( hBand, "STATISTICS_HISTOMAX", szTemp, NULL );

            sprintf( szTemp, "%d", nHistBuckets );
            GDALSetMetadataItem( hBand, "STATISTICS_HISTONUMBINS", szTemp, NULL );

            //GDALSetMetadataItem( hBand, "STATISTICS_HISTOBINVALUES", szHistoString, NULL );
            //delete[] szHistoString;

            /* set histogram bin function the same as Imagine does
            ie Linear for floats, and direct for integer types
            This means Raster Attribute Dialog gets displayed
            correctly in Imagine */
            GDALSetMetadataItem( hBand, "STATISTICS_HISTOBINFUNCTION", histoType.c_str(), NULL );
            
            
            // Write the histogram to the RAT
            GDALRasterAttributeTable *attTable = hBand->GetDefaultRAT();
            attTable->SetRowCount(nHistBuckets);
            
            int numColumns = attTable->GetColumnCount();
            bool foundCol = false;
            unsigned int histoColIdx = 0;
            for(int i = 0; i < numColumns; ++i)
            {
                if(std::string(attTable->GetNameOfCol(i)) == "Histogram")
                {
                    foundCol = true;
                    histoColIdx = i;
                    break;
                }
            }
            
            if(!foundCol)
            {
                attTable->CreateColumn("Histogram", GFT_Real, GFU_PixelCount);
                histoColIdx = numColumns;
            }
            
            attTable->ValuesIO(GF_Write, histoColIdx, 0, nHistBuckets, pHisto);
            
            free( pHisto );
        }
        if( bPyramid )
        {
            std::cout << "Building Pyramids:\n";
            this->addpyramid(hHandle);
        }
    }
    
    
}}
 

