/*
 *  RSGISImageMosaic.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 07/11/2008.
 *  Copyright 2008 RSGISLib.
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
 *
 *  Modified by Dan Clewley on 21/11/2010
 *  Added 'mosaicSkipVals' and 'mosaicSkipThresh'
 *  to skip values in input image
 *
 *  Modified by Dan Clewley on 27/05/2013
 *  Changed to block read / write
 *  Added ability to take minimum / maximum pixel in overlapping regions
 */

#include "RSGISImageMosaic.h"

namespace rsgis{namespace img{

	RSGISImageMosaic::RSGISImageMosaic()
	{

	}

	void RSGISImageMosaic::mosaic(std::string *inputImages, int numDS, std::string outputImage, float background, bool projFromImage, std::string proj, std::string format, GDALDataType imgDataType)
	{
		RSGISImageUtils imgUtils;
        rsgis::math::RSGISMathsUtils mathsUtils;
        GDALAllRegister();
        GDALDataset *dataset = NULL;
        GDALRasterBand *imgBand = NULL;
		int width;
		int height;
		double *transformation = new double[6];
		double *imgTransform = new double[6];
		int numberBands = 0;
		int tileXSize = 0;
		int tileYSize = 0;
		double xDiff = 0;
		double yDiff = 0;
		int xStart = 0;
		int yStart = 0;
		std::string projection = proj;
		GDALDataset *outputDataset = NULL;
		GDALRasterBand **inputRasterBands = NULL;
		GDALRasterBand **outputRasterBands = NULL;
		float **inputData = NULL;

        std::vector<std::string> bandnames;

		try
		{
			for(int i = 0; i < numDS; i++)
			{
                dataset = (GDALDataset *) GDALOpenShared(inputImages[i].c_str(), GA_ReadOnly);
                if(dataset == NULL)
                {
                    std::string message = std::string("Could not open image ") + inputImages[i];
                    throw RSGISImageException(message.c_str());
                }

				if(i == 0)
				{
					numberBands = dataset->GetRasterCount();
                    for(int j = 0; j < numberBands; ++j)
                    {
                        imgBand = dataset->GetRasterBand(j+1);
                        bandnames.push_back(std::string(imgBand->GetDescription()));
                    }
                    if(projFromImage)
                    {
                        projection = std::string(dataset->GetProjectionRef());
                    }
				}
				else
				{
					if(dataset->GetRasterCount() != numberBands)
					{
						throw RSGISImageBandException("All input images need to have the same number of bands (" + mathsUtils.doubletostring(numberBands) + ").\n"
                                    + inputImages[i] + " has " + mathsUtils.doubletostring(dataset->GetRasterCount()) );
					}
				}
                GDALClose(dataset);
			}

			imgUtils.getImagesExtent(inputImages, numDS, &width, &height, transformation);

            // Create blank image
			std::cout << "Create new image [" << width << "," << height << "] with projection: \n" << projection << std::endl;

			outputDataset = imgUtils.createBlankImage(outputImage, transformation, width, height, numberBands, projection, background, bandnames, format, imgDataType);

			// COPY IMAGE DATA INTO THE BLANK IMAGE

            //Get Image Output Bands
            outputRasterBands = new GDALRasterBand*[numberBands];
            for(int i = 0; i < numberBands; i++)
            {
                outputRasterBands[i] = outputDataset->GetRasterBand(i+1);
            }

            int xBlockSize = 0;
            int yBlockSize = 0;
            outputRasterBands[0]->GetBlockSize (&xBlockSize, &yBlockSize);

			std::cout << "Started (total " << numDS << ") ." << std::flush;

            for(int ds = 0; ds < numDS; ds++)
			{
				std::cout << "." << ds+1 << "." << std::flush;

                dataset = (GDALDataset *) GDALOpenShared(inputImages[ds].c_str(), GA_ReadOnly);
                if(dataset == NULL)
                {
                    std::string message = std::string("Could not open image ") + inputImages[ds];
                    throw RSGISImageException(message.c_str());
                }

                inputRasterBands = new GDALRasterBand*[numberBands];
                for(int i = 0; i < numberBands; i++)
                {
                    inputRasterBands[i] = dataset->GetRasterBand(i+1);
                }

				dataset->GetGeoTransform(imgTransform);
				tileXSize = dataset->GetRasterXSize();
				tileYSize = dataset->GetRasterYSize();

				xDiff = imgTransform[0] - transformation[0];
				yDiff = transformation[3] - imgTransform[3];

				xStart = floor((xDiff/transformation[1])+0.5);
				yStart = floor((yDiff/transformation[1])+0.5);

                // Allocate memory
                inputData = new float*[numberBands];
                for(int i = 0; i < numberBands; i++)
                {
                    inputData[i] = (float *) CPLMalloc(sizeof(float)*(tileXSize*yBlockSize));
                }

                int nYBlocks = tileYSize / yBlockSize;
                int remainRows = tileYSize - (nYBlocks * yBlockSize);
                int rowOffset = 0;

                for(int i = 0; i < nYBlocks; i++)
                {
                    for(int n = 0; n < numberBands; n++)
                    {
                        rowOffset = yBlockSize * i;
                        inputRasterBands[n]->RasterIO(GF_Read, 0, rowOffset, tileXSize, yBlockSize, inputData[n], tileXSize, yBlockSize, GDT_Float32, 0, 0);
                        outputRasterBands[n]->RasterIO(GF_Write, xStart, (yStart + rowOffset), tileXSize, yBlockSize, inputData[n], tileXSize, yBlockSize, GDT_Float32, 0, 0);
                    }

                }

                if(remainRows > 0)
                {
                    for(int n = 0; n < numberBands; n++)
                    {
                        rowOffset = yBlockSize * nYBlocks;
                        inputRasterBands[n]->RasterIO(GF_Read, 0, rowOffset, tileXSize, remainRows, inputData[n], tileXSize, remainRows, GDT_Float32, 0, 0);
                        outputRasterBands[n]->RasterIO(GF_Write, xStart, (yStart + rowOffset), tileXSize, remainRows, inputData[n], tileXSize, remainRows, GDT_Float32, 0, 0);
                    }
                }

                // Tidy up
                if(inputData != NULL)
                {
                    for(int i = 0; i < numberBands; i++)
                    {
                        if(inputData[i] != NULL)
                        {
                            delete[] inputData[i];
                        }
                    }
                    delete[] inputData;
                }
                if(inputRasterBands != NULL)
                {
                    delete[] inputRasterBands;
                }

                GDALClose(dataset);
			}
			std::cout << ".complete\n";

		}
		catch(RSGISImageBandException &e)
		{
            if(inputData != NULL)
            {
                for(int i = 0; i < numberBands; i++)
                {
                    if(inputData[i] != NULL)
                    {
                        delete[] inputData[i];
                    }
                }
                delete[] inputData;
            }
            if(inputRasterBands != NULL)
            {
                delete[] inputRasterBands;
            }
            if(outputRasterBands != NULL)
            {
                delete[] outputRasterBands;
            }
			if(transformation != NULL)
			{
				delete[] transformation;
			}
			if(imgTransform != NULL)
			{
				delete[] imgTransform;
			}
			throw e;
		}

        // Tidy
        if(outputRasterBands != NULL)
        {
            delete[] outputRasterBands;
        }

		if(transformation != NULL)
		{
			delete[] transformation;
		}
		if(imgTransform != NULL)
		{
			delete[] imgTransform;
		}
		GDALClose(outputDataset);
	}

	void RSGISImageMosaic::mosaicSkipVals(std::string *inputImages, int numDS, std::string outputImage, float background, float skipVal, bool projFromImage, std::string proj, unsigned int skipBand, unsigned int overlapBehaviour, std::string format, GDALDataType imgDataType)
	{
		RSGISImageUtils imgUtils;
		rsgis::math::RSGISMathsUtils mathsUtils;

        GDALAllRegister();
        GDALDataset *dataset = NULL;
        GDALRasterBand *imgBand = NULL;
		int width;
		int height;
		double *transformation = new double[6];
		double *imgTransform = new double[6];
		int numberBands = 0;
		int tileXSize = 0;
		int tileYSize = 0;
		double xDiff = 0;
		double yDiff = 0;
		int xStart = 0;
		int yStart = 0;
		std::string projection = proj;
		GDALDataset *outputDataset = NULL;
		GDALRasterBand **inputRasterBands = NULL;
		GDALRasterBand **outputRasterBands = NULL;
		float **inputData = NULL;
		float **outputData = NULL;
		bool pixelsChanged;

        std::vector<std::string> bandnames;

		try
		{
			for(int i = 0; i < numDS; i++)
			{
                dataset = (GDALDataset *) GDALOpenShared(inputImages[i].c_str(), GA_ReadOnly);
                if(dataset == NULL)
                {
                    std::string message = std::string("Could not open image ") + inputImages[i];
                    throw RSGISImageException(message.c_str());
                }

				if(i == 0)
				{
					numberBands = dataset->GetRasterCount();
                    for(int j = 0; j < numberBands; ++j)
                    {
                        imgBand = dataset->GetRasterBand(j+1);
                        bandnames.push_back(std::string(imgBand->GetDescription()));
                    }
                    if(projFromImage)
                    {
                        projection = std::string(dataset->GetProjectionRef());
                    }
				}
				else
				{
					if(dataset->GetRasterCount() != numberBands)
					{
						throw RSGISImageBandException("All input images need to have the same number of bands (" + mathsUtils.doubletostring(numberBands) + ").\n"
                                    + inputImages[i] + " has " + mathsUtils.doubletostring(dataset->GetRasterCount()) );
					}
				}
                GDALClose(dataset);
			}

			imgUtils.getImagesExtent(inputImages, numDS, &width, &height, transformation);

            // Create blank image
			std::cout << "Create new image [" << width << "," << height << "] with projection: \n" << projection << std::endl;

			outputDataset = imgUtils.createBlankImage(outputImage, transformation, width, height, numberBands, projection, background, bandnames, format, imgDataType);

			// COPY IMAGE DATA INTO THE BLANK IMAGE

            //Get Image Output Bands
            outputRasterBands = new GDALRasterBand*[numberBands];
            for(int i = 0; i < numberBands; i++)
            {
                outputRasterBands[i] = outputDataset->GetRasterBand(i+1);
            }

            int xBlockSize = 0;
            int yBlockSize = 0;
            outputRasterBands[0]->GetBlockSize (&xBlockSize, &yBlockSize);

			std::cout << "Started (total " << numDS << ") ." << std::flush;

            for(int ds = 0; ds < numDS; ds++)
			{
				std::cout << "." << ds+1 << "." << std::flush;

                dataset = (GDALDataset *) GDALOpenShared(inputImages[ds].c_str(), GA_ReadOnly);
                if(dataset == NULL)
                {
                    std::string message = std::string("Could not open image ") + inputImages[ds];
                    throw RSGISImageException(message.c_str());
                }

                inputRasterBands = new GDALRasterBand*[numberBands];
                for(int i = 0; i < numberBands; i++)
                {
                    inputRasterBands[i] = dataset->GetRasterBand(i+1);
                }

				dataset->GetGeoTransform(imgTransform);
				tileXSize = dataset->GetRasterXSize();
				tileYSize = dataset->GetRasterYSize();

				xDiff = imgTransform[0] - transformation[0];
				yDiff = transformation[3] - imgTransform[3];

				xStart = floor((xDiff/transformation[1])+0.5);
				yStart = floor((yDiff/transformation[1])+0.5);

                // Allocate memory
                inputData = new float*[numberBands];
                outputData = new float*[numberBands];
                for(int i = 0; i < numberBands; i++)
                {
                    inputData[i] = (float *) CPLMalloc(sizeof(float)*(tileXSize*yBlockSize));
                    outputData[i] = (float *) CPLMalloc(sizeof(float)*(tileXSize*yBlockSize));
                }

                int nYBlocks = tileYSize / yBlockSize;
                int remainRows = tileYSize - (nYBlocks * yBlockSize);
                int rowOffset = 0;

                for(int i = 0; i < nYBlocks; i++)
                {
                    // Read input and output data
                    for(int n = 0; n < numberBands; n++)
                    {
                        rowOffset = yBlockSize * i;
                        // Read input data
                        inputRasterBands[n]->RasterIO(GF_Read, 0, rowOffset, tileXSize, yBlockSize, inputData[n], tileXSize, yBlockSize, GDT_Float32, 0, 0);
                        // Read output data
                        outputRasterBands[n]->RasterIO(GF_Read, xStart, (yStart + rowOffset), tileXSize, yBlockSize, outputData[n], tileXSize, yBlockSize, GDT_Float32, 0, 0);
                    }

                    pixelsChanged = false; // Keep track if there are changes that need writing out.

                    for(int m = 0; m < yBlockSize; ++m)
                    {
                        for(int j = 0; j < tileXSize; j++)
                        {
                            // Check for skip value
                            if(inputData[skipBand][(m*tileXSize)+j] != skipVal)
                            {
                                // Check if behaviour is defined for overlap and not the first image
                                if( (overlapBehaviour > 0) && (ds > 0) )
                                {
                                    // Check if pixel is background value (no data has been written yet)
                                    if( outputData[skipBand][(m*tileXSize)+j] == background )
                                    {
                                        for(int n = 0; n < numberBands; n++)
                                        {
                                            outputData[n][(m*tileXSize)+j] = inputData[n][(m*tileXSize)+j];
                                        }
                                        pixelsChanged = true;
                                    }
                                    // If data has been written - check if new value is less (min overlap behaviour)
                                    else if( (overlapBehaviour == 1) &&  (inputData[skipBand][(m*tileXSize)+j] < outputData[skipBand][(m*tileXSize)+j]) )
                                    {
                                        for(int n = 0; n < numberBands; n++)
                                        {
                                            outputData[n][(m*tileXSize)+j] = inputData[n][(m*tileXSize)+j];
                                        }
                                        pixelsChanged = true;
                                    }
                                    // If data has been written - check if new value is greater (max overlap behaviour)
                                    else if( (overlapBehaviour == 2) &&  (inputData[skipBand][(m*tileXSize)+j] > outputData[skipBand][(m*tileXSize)+j]) )
                                    {
                                        for(int n = 0; n < numberBands; n++)
                                        {
                                            outputData[n][(m*tileXSize)+j] = inputData[n][(m*tileXSize)+j];
                                        }
                                        pixelsChanged = true;
                                    }
                                }
                                // If no overlap behaviour defined, write out,
                                else
                                {
                                    for(int n = 0; n < numberBands; n++)
                                    {
                                        outputData[n][(m*tileXSize)+j] = inputData[n][(m*tileXSize)+j];
                                    }
                                    pixelsChanged = true;
                                }


                            }
                        }
                    }

                    // Write out data to mosaic
                    if(pixelsChanged)
                    {
                        for(int n = 0; n < numberBands; n++)
                        {
                            outputRasterBands[n]->RasterIO(GF_Write, xStart, (yStart + rowOffset), tileXSize, yBlockSize, outputData[n], tileXSize, yBlockSize, GDT_Float32, 0, 0);
                        }
                    }

                }

                if(remainRows > 0)
                {
                    for(int n = 0; n < numberBands; n++)
                    {
                        rowOffset = yBlockSize * nYBlocks;
                        // Read input data
                        inputRasterBands[n]->RasterIO(GF_Read, 0, rowOffset, tileXSize, remainRows, inputData[n], tileXSize, remainRows, GDT_Float32, 0, 0);
                        // Read output data
                        outputRasterBands[n]->RasterIO(GF_Read, xStart, (yStart + rowOffset), tileXSize, remainRows, outputData[n], tileXSize, remainRows, GDT_Float32, 0, 0);
                    }

                    pixelsChanged = false; // Keep track if there are changes that need writing out.

                    for(int m = 0; m < remainRows; ++m)
                    {
                        for(int j = 0; j < tileXSize; j++)
                        {
                            // Check for skip value
                            if(inputData[skipBand][(m*tileXSize)+j] != skipVal)
                            {
                                // Check if behaviour is defined for overlap and not the first image
                                if( (overlapBehaviour > 0) && (ds > 0) )
                                {
                                    // Check if pixel is background value (no data has been written yet)
                                    if( outputData[skipBand][(m*tileXSize)+j] == background )
                                    {
                                        for(int n = 0; n < numberBands; n++)
                                        {
                                            outputData[n][(m*tileXSize)+j] = inputData[n][(m*tileXSize)+j];
                                        }
                                        pixelsChanged = true;
                                    }
                                    // If data has been written - check if new value is less (min overlap behaviour)
                                    else if( (overlapBehaviour == 1) &&  (inputData[skipBand][(m*tileXSize)+j] < outputData[skipBand][(m*tileXSize)+j]) )
                                    {
                                        for(int n = 0; n < numberBands; n++)
                                        {
                                            outputData[n][(m*tileXSize)+j] = inputData[n][(m*tileXSize)+j];
                                        }
                                        pixelsChanged = true;
                                    }
                                    // If data has been written - check if new value is greater (max overlap behaviour)
                                    else if( (overlapBehaviour == 2) &&  (inputData[skipBand][(m*tileXSize)+j] > outputData[skipBand][(m*tileXSize)+j]) )
                                    {
                                        for(int n = 0; n < numberBands; n++)
                                        {
                                            outputData[n][(m*tileXSize)+j] = inputData[n][(m*tileXSize)+j];
                                        }
                                        pixelsChanged = true;
                                    }
                                }
                                // If no overlap behaviour defined, write out,
                                else
                                {
                                    for(int n = 0; n < numberBands; n++)
                                    {
                                        outputData[n][(m*tileXSize)+j] = inputData[n][(m*tileXSize)+j];
                                    }
                                    pixelsChanged = true;
                                }


                            }
                        }
                    }

                    // Write out data
                    if(pixelsChanged)
                    {
                        for(int n = 0; n < numberBands; n++)
                        {
                            outputRasterBands[n]->RasterIO(GF_Write, xStart, (yStart + rowOffset), tileXSize, remainRows, outputData[n], tileXSize, remainRows, GDT_Float32, 0, 0);
                        }
                    }

                }

                // Tidy up
                if(inputData != NULL)
                {
                    for(int i = 0; i < numberBands; i++)
                    {
                        if(inputData[i] != NULL)
                        {
                            delete[] inputData[i];
                        }
                    }
                    delete[] inputData;
                }
                if(outputData != NULL)
                {
                    for(int i = 0; i < numberBands; i++)
                    {
                        if(outputData[i] != NULL)
                        {
                            delete[] outputData[i];
                        }
                    }
                    delete[] outputData;
                }
                if(inputRasterBands != NULL)
                {
                    delete[] inputRasterBands;
                }

                GDALClose(dataset);
			}
			std::cout << ".complete\n";

		}
		catch(RSGISImageBandException &e)
		{
            if(inputData != NULL)
            {
                for(int i = 0; i < numberBands; i++)
                {
                    if(inputData[i] != NULL)
                    {
                        delete[] inputData[i];
                    }
                }
                delete[] inputData;
            }
            if(outputData != NULL)
            {
                for(int i = 0; i < numberBands; i++)
                {
                    if(outputData[i] != NULL)
                    {
                        delete[] outputData[i];
                    }
                }
                delete[] outputData;
            }
            if(inputRasterBands != NULL)
            {
                delete[] inputRasterBands;
            }
            if(outputRasterBands != NULL)
            {
                delete[] outputRasterBands;
            }
			if(transformation != NULL)
			{
				delete[] transformation;
			}
			if(imgTransform != NULL)
			{
				delete[] imgTransform;
			}
			throw e;
		}

        // Tidy
        if(outputRasterBands != NULL)
        {
            delete[] outputRasterBands;
        }

		if(transformation != NULL)
		{
			delete[] transformation;
		}
		if(imgTransform != NULL)
		{
			delete[] imgTransform;
		}
		GDALClose(outputDataset);
	}

	void RSGISImageMosaic::mosaicSkipThresh(std::string *inputImages, int numDS, std::string outputImage, float background, float skipLowerThresh, float skipUpperThresh, bool projFromImage, std::string proj, unsigned int threshBand, unsigned int overlapBehaviour, std::string format, GDALDataType imgDataType)
	{
		RSGISImageUtils imgUtils;
        rsgis::math::RSGISMathsUtils mathsUtils;
        GDALAllRegister();
        GDALDataset *dataset = NULL;
        GDALRasterBand *imgBand = NULL;
		int width;
		int height;
		double *transformation = new double[6];
		double *imgTransform = new double[6];
		int numberBands = 0;
		int tileXSize = 0;
		int tileYSize = 0;
		double xDiff = 0;
		double yDiff = 0;
		int xStart = 0;
		int yStart = 0;
		std::string projection = proj;
		GDALDataset *outputDataset = NULL;
		GDALRasterBand **inputRasterBands = NULL;
		GDALRasterBand **outputRasterBands = NULL;
		float **inputData = NULL;
		float **outputData = NULL;
		bool pixelsChanged;

        std::vector<std::string> bandnames;

		try
		{
			for(int i = 0; i < numDS; i++)
			{
                dataset = (GDALDataset *) GDALOpenShared(inputImages[i].c_str(), GA_ReadOnly);
                if(dataset == NULL)
                {
                    std::string message = std::string("Could not open image ") + inputImages[i];
                    throw RSGISImageException(message.c_str());
                }

				if(i == 0)
				{
					numberBands = dataset->GetRasterCount();
                    for(int j = 0; j < numberBands; ++j)
                    {
                        imgBand = dataset->GetRasterBand(j+1);
                        bandnames.push_back(std::string(imgBand->GetDescription()));
                    }
                    if(projFromImage)
                    {
                        projection = std::string(dataset->GetProjectionRef());
                    }
				}
				else
				{
					if(dataset->GetRasterCount() != numberBands)
					{
						throw RSGISImageBandException("All input images need to have the same number of bands (" + mathsUtils.doubletostring(numberBands) + ").\n"
                                    + inputImages[i] + " has " + mathsUtils.doubletostring(dataset->GetRasterCount()) );
					}
				}
                GDALClose(dataset);
			}

			imgUtils.getImagesExtent(inputImages, numDS, &width, &height, transformation);

            // Create blank image
			std::cout << "Create new image [" << width << "," << height << "] with projection: \n" << projection << std::endl;

			outputDataset = imgUtils.createBlankImage(outputImage, transformation, width, height, numberBands, projection, background, bandnames, format, imgDataType);

			// COPY IMAGE DATA INTO THE BLANK IMAGE

            //Get Image Output Bands
            outputRasterBands = new GDALRasterBand*[numberBands];
            for(int i = 0; i < numberBands; i++)
            {
                outputRasterBands[i] = outputDataset->GetRasterBand(i+1);
            }

            int xBlockSize = 0;
            int yBlockSize = 0;
            outputRasterBands[0]->GetBlockSize (&xBlockSize, &yBlockSize);

			std::cout << "Started (total " << numDS << ") ." << std::flush;

            for(int ds = 0; ds < numDS; ds++)
			{
				std::cout << "." << ds+1 << "." << std::flush;

                dataset = (GDALDataset *) GDALOpenShared(inputImages[ds].c_str(), GA_ReadOnly);
                if(dataset == NULL)
                {
                    std::string message = std::string("Could not open image ") + inputImages[ds];
                    throw RSGISImageException(message.c_str());
                }

                inputRasterBands = new GDALRasterBand*[numberBands];
                for(int i = 0; i < numberBands; i++)
                {
                    inputRasterBands[i] = dataset->GetRasterBand(i+1);
                }

				dataset->GetGeoTransform(imgTransform);
				tileXSize = dataset->GetRasterXSize();
				tileYSize = dataset->GetRasterYSize();

				xDiff = imgTransform[0] - transformation[0];
				yDiff = transformation[3] - imgTransform[3];

				xStart = floor((xDiff/transformation[1])+0.5);
				yStart = floor((yDiff/transformation[1])+0.5);

                // Allocate memory
                inputData = new float*[numberBands];
                outputData = new float*[numberBands];
                for(int i = 0; i < numberBands; i++)
                {
                    inputData[i] = (float *) CPLMalloc(sizeof(float)*(tileXSize*yBlockSize));
                    outputData[i] = (float *) CPLMalloc(sizeof(float)*(tileXSize*yBlockSize));
                }

                int nYBlocks = tileYSize / yBlockSize;
                int remainRows = tileYSize - (nYBlocks * yBlockSize);
                int rowOffset = 0;

                for(int i = 0; i < nYBlocks; i++)
                {
                    // Read input and output data
                    for(int n = 0; n < numberBands; n++)
                    {
                        rowOffset = yBlockSize * i;
                        // Read input data
                        inputRasterBands[n]->RasterIO(GF_Read, 0, rowOffset, tileXSize, yBlockSize, inputData[n], tileXSize, yBlockSize, GDT_Float32, 0, 0);
                        // Read output data
                        outputRasterBands[n]->RasterIO(GF_Read, xStart, (yStart + rowOffset), tileXSize, yBlockSize, outputData[n], tileXSize, yBlockSize, GDT_Float32, 0, 0);
                    }

                    pixelsChanged = false; // Keep track if there are changes that need writing out.

                    for(int m = 0; m < yBlockSize; ++m)
                    {
                        for(int j = 0; j < tileXSize; j++)
                        {
                            // Check value is between upper and lower limits
                            if((inputData[threshBand][(m*tileXSize)+j] > skipLowerThresh) && (inputData[threshBand][(m*tileXSize)+j] < skipUpperThresh))
                            {
                                // Check if behaviour is defined for overlap and not the first image
                                if( (overlapBehaviour > 0) && (ds > 0) )
                                {
                                    // Check if pixel is background value (no data has been written yet)
                                    if( outputData[threshBand][(m*tileXSize)+j] == background )
                                    {
                                        for(int n = 0; n < numberBands; n++)
                                        {
                                            outputData[n][(m*tileXSize)+j] = inputData[n][(m*tileXSize)+j];
                                        }
                                        pixelsChanged = true;
                                    }
                                    // If data has been written - check if new value is less (min overlap behaviour)
                                    else if( (overlapBehaviour == 1) &&  (inputData[threshBand][(m*tileXSize)+j] < outputData[threshBand][(m*tileXSize)+j]) )
                                    {
                                        for(int n = 0; n < numberBands; n++)
                                        {
                                            outputData[n][(m*tileXSize)+j] = inputData[n][(m*tileXSize)+j];
                                        }
                                        pixelsChanged = true;
                                    }
                                    // If data has been written - check if new value is greater (max overlap behaviour)
                                    else if( (overlapBehaviour == 2) &&  (inputData[threshBand][(m*tileXSize)+j] > outputData[threshBand][(m*tileXSize)+j]) )
                                    {
                                        for(int n = 0; n < numberBands; n++)
                                        {
                                            outputData[n][(m*tileXSize)+j] = inputData[n][(m*tileXSize)+j];
                                        }
                                        pixelsChanged = true;
                                    }
                                }
                                // If no overlap behaviour defined, write out,
                                else
                                {
                                    for(int n = 0; n < numberBands; n++)
                                    {
                                        outputData[n][(m*tileXSize)+j] = inputData[n][(m*tileXSize)+j];
                                    }
                                    pixelsChanged = true;
                                }


                            }
                        }
                    }

                    // Write out data to mosaic
                    if(pixelsChanged)
                    {
                        for(int n = 0; n < numberBands; n++)
                        {
                            outputRasterBands[n]->RasterIO(GF_Write, xStart, (yStart + rowOffset), tileXSize, yBlockSize, outputData[n], tileXSize, yBlockSize, GDT_Float32, 0, 0);
                        }
                    }

                }

                if(remainRows > 0)
                {
                    for(int n = 0; n < numberBands; n++)
                    {
                        rowOffset = yBlockSize * nYBlocks;
                        // Read input data
                        inputRasterBands[n]->RasterIO(GF_Read, 0, rowOffset, tileXSize, remainRows, inputData[n], tileXSize, remainRows, GDT_Float32, 0, 0);
                        // Read output data
                        outputRasterBands[n]->RasterIO(GF_Read, xStart, (yStart + rowOffset), tileXSize, remainRows, outputData[n], tileXSize, remainRows, GDT_Float32, 0, 0);
                    }

                    pixelsChanged = false; // Keep track if there are changes that need writing out.

                    for(int m = 0; m < remainRows; ++m)
                    {
                        for(int j = 0; j < tileXSize; j++)
                        {
                            // Check value is between upper and lower limits
                            if((inputData[threshBand][(m*tileXSize)+j] > skipLowerThresh) && (inputData[threshBand][(m*tileXSize)+j] < skipUpperThresh))
                            {
                                // Check if behaviour is defined for overlap and not the first image
                                if( (overlapBehaviour > 0) && (ds > 0) )
                                {
                                    // Check if pixel is background value (no data has been written yet)
                                    if( outputData[threshBand][(m*tileXSize)+j] == background )
                                    {
                                        for(int n = 0; n < numberBands; n++)
                                        {
                                            outputData[n][(m*tileXSize)+j] = inputData[n][(m*tileXSize)+j];
                                        }
                                        pixelsChanged = true;
                                    }
                                    // If data has been written - check if new value is less (min overlap behaviour)
                                    else if( (overlapBehaviour == 1) &&  (inputData[threshBand][(m*tileXSize)+j] < outputData[threshBand][(m*tileXSize)+j]) )
                                    {
                                        for(int n = 0; n < numberBands; n++)
                                        {
                                            outputData[n][(m*tileXSize)+j] = inputData[n][(m*tileXSize)+j];
                                        }
                                        pixelsChanged = true;
                                    }
                                    // If data has been written - check if new value is greater (max overlap behaviour)
                                    else if( (overlapBehaviour == 2) &&  (inputData[threshBand][(m*tileXSize)+j] > outputData[threshBand][(m*tileXSize)+j]) )
                                    {
                                        for(int n = 0; n < numberBands; n++)
                                        {
                                            outputData[n][(m*tileXSize)+j] = inputData[n][(m*tileXSize)+j];
                                        }
                                        pixelsChanged = true;
                                    }
                                }
                                // If no overlap behaviour defined, write out,
                                else
                                {
                                    for(int n = 0; n < numberBands; n++)
                                    {
                                        outputData[n][(m*tileXSize)+j] = inputData[n][(m*tileXSize)+j];
                                    }
                                    pixelsChanged = true;
                                }


                            }
                        }
                    }

                    // Write out data
                    if(pixelsChanged)
                    {
                        for(int n = 0; n < numberBands; n++)
                        {
                            outputRasterBands[n]->RasterIO(GF_Write, xStart, (yStart + rowOffset), tileXSize, remainRows, outputData[n], tileXSize, remainRows, GDT_Float32, 0, 0);
                        }
                    }

                }

                // Tidy up
                if(inputData != NULL)
                {
                    for(int i = 0; i < numberBands; i++)
                    {
                        if(inputData[i] != NULL)
                        {
                            delete[] inputData[i];
                        }
                    }
                    delete[] inputData;
                }
                if(outputData != NULL)
                {
                    for(int i = 0; i < numberBands; i++)
                    {
                        if(outputData[i] != NULL)
                        {
                            delete[] outputData[i];
                        }
                    }
                    delete[] outputData;
                }
                if(inputRasterBands != NULL)
                {
                    delete[] inputRasterBands;
                }

                GDALClose(dataset);
			}
			std::cout << ".complete\n";

		}
		catch(RSGISImageBandException &e)
		{
            if(inputData != NULL)
            {
                for(int i = 0; i < numberBands; i++)
                {
                    if(inputData[i] != NULL)
                    {
                        delete[] inputData[i];
                    }
                }
                delete[] inputData;
            }
            if(outputData != NULL)
            {
                for(int i = 0; i < numberBands; i++)
                {
                    if(outputData[i] != NULL)
                    {
                        delete[] outputData[i];
                    }
                }
                delete[] outputData;
            }
            if(inputRasterBands != NULL)
            {
                delete[] inputRasterBands;
            }
            if(outputRasterBands != NULL)
            {
                delete[] outputRasterBands;
            }
			if(transformation != NULL)
			{
				delete[] transformation;
			}
			if(imgTransform != NULL)
			{
				delete[] imgTransform;
			}
			throw e;
		}

        // Tidy
        if(outputRasterBands != NULL)
        {
            delete[] outputRasterBands;
        }

		if(transformation != NULL)
		{
			delete[] transformation;
		}
		if(imgTransform != NULL)
		{
			delete[] imgTransform;
		}
		GDALClose(outputDataset);
	}

	void RSGISImageMosaic::includeDatasets(GDALDataset *baseImage, std::string *inputImages, int numDS, std::vector<int> bands, bool bandsDefined)
	{
		RSGISImageUtils imgUtils;
        GDALDataset *dataset = NULL;
		int width;
		int height;

		double *transformation = new double[6];
		double *imgTransform = new double[6];
		double *baseTransform = new double[6];
		int numberBands = 0;
		int tileXSize = 0;
		int tileYSize = 0;
		double xDiff = 0;
		double yDiff = 0;
		int xStart = 0;
		int yStart = 0;
		std::string projection;

		GDALRasterBand *inputBand = NULL;
		GDALRasterBand **outputBand = NULL;
		float *imgData = NULL;

		try
		{
			numberBands = baseImage->GetRasterCount();
            for(int i = 0; i < numDS; i++)
			{
                dataset = (GDALDataset *) GDALOpenShared(inputImages[i].c_str(), GA_ReadOnly);
                if(dataset == NULL)
                {
                    std::string message = std::string("Could not open image ") + inputImages[i];
                    throw RSGISImageException(message.c_str());
                }

                if(!bandsDefined)
                {
                    if(dataset->GetRasterCount() != numberBands)
                    {
                        throw RSGISImageBandException("All input images need to have the same number of bands.");
                    }
                }
                else
                {
                    for(std::vector<int>::iterator iterBands = bands.begin(); iterBands != bands.end(); ++iterBands)
                    {
                        if(((*iterBands) <= 0) | ((*iterBands) > dataset->GetRasterCount()))
                        {
                            std::cerr << "Band = " << *iterBands << std::endl;
                            std::string message = std::string("Band is not within the input dataset ") + inputImages[i];
                            throw RSGISImageException(message.c_str());
                        }
                    }
                }
                GDALClose(dataset);
			}

            if(bandsDefined)
            {
                if(numberBands < bands.size())
                {
                    throw RSGISImageException("The base image does not have enough image bands for the output data specificed.");
                }
            }

            projection = std::string(baseImage->GetProjectionRef());
			imgUtils.getImagesExtent(inputImages, numDS, &width, &height, transformation);

			baseImage->GetGeoTransform(baseTransform);

			double baseExtentX = baseTransform[0] + (baseImage->GetRasterXSize() * baseTransform[1]);
			double baseExtentY = baseTransform[3] + (baseImage->GetRasterYSize() * baseTransform[5]);
			double imgExtentX = transformation[0] + (width * transformation[1]);
			double imgExtentY = transformation[3] + (height * transformation[5]);

			// Check datasets fit within the base image.
			if(transformation[0] < baseTransform[0])
			{
                std::cerr << "transformation[0] = " << transformation[0] << std::endl;
                std::cerr << "baseTransform[0] = " << baseTransform[0] << std::endl;
				throw RSGISImageException("Images do not fit within the base image (Eastings Min)");
			}
			if(transformation[3] > baseTransform[3])
			{
                std::cerr << "transformation[3] = " << transformation[3] << std::endl;
                std::cerr << "baseTransform[3] = " << baseTransform[3] << std::endl;
				throw RSGISImageException("Images do not fit within the base image (Northings Max)");
			}
			if(imgExtentX > baseExtentX)
			{
                std::cerr << "imgExtentX = " << imgExtentX << std::endl;
                std::cerr << "baseExtentX = " << baseExtentX << std::endl;
				throw RSGISImageException("Images do not fit within the base image (Eastings Max)");
			}
			if(imgExtentY < baseExtentY)
			{
                std::cerr << "imgExtentY = " << imgExtentY << std::endl;
                std::cerr << "baseExtentY = " << baseExtentY << std::endl;
				throw RSGISImageException("Images do not fit within the base image (Northings Min)");
			}

			height = baseImage->GetRasterYSize();
			width = baseImage->GetRasterXSize();

            outputBand = new GDALRasterBand*[numberBands];
            for(int i = 0; i < numberBands; i++)
            {
                outputBand[i] = baseImage->GetRasterBand(i+1);
            }

            int xBlockSize = 0;
            int yBlockSize = 0;
            outputBand[0]->GetBlockSize (&xBlockSize, &yBlockSize);

			std::cout << "Started (total " << numDS << ") ." << std::flush;

			for(int i = 0; i < numDS; i++)
			{
				std::cout << "." << i << "." << std::flush;
                dataset = (GDALDataset *) GDALOpenShared(inputImages[i].c_str(), GA_ReadOnly);
                if(dataset == NULL)
                {
                    std::string message = std::string("Could not open image ") + inputImages[i];
                    throw RSGISImageException(message.c_str());
                }

				dataset->GetGeoTransform(imgTransform);
				tileXSize = dataset->GetRasterXSize();
				tileYSize = dataset->GetRasterYSize();

				xDiff = imgTransform[0] - baseTransform[0];
				yDiff = baseTransform[3] - imgTransform[3];

				xStart = floor((xDiff/baseTransform[1])+0.5);
				yStart = floor((yDiff/baseTransform[1])+0.5);
				imgData = (float *) CPLMalloc(sizeof(float)*(tileXSize*yBlockSize));

                int nYBlocks = tileYSize / yBlockSize;
                int remainRows = tileYSize - (nYBlocks * yBlockSize);
                int rowOffset = 0;

                for(int i = 0; i < nYBlocks; i++)
                {
                    rowOffset = yBlockSize * i;
                    if(bandsDefined)
                    {
                        int nBand = 1;
                        for(std::vector<int>::iterator iterBands = bands.begin(); iterBands != bands.end(); ++iterBands)
                        {
                            if(((*iterBands) <= 0) | ((*iterBands) > dataset->GetRasterCount()))
                            {
                                throw RSGISImageException("Band is not within the input dataset.");
                            }
                            inputBand = dataset->GetRasterBand(*iterBands);

                            inputBand->RasterIO(GF_Read, 0, rowOffset, tileXSize, yBlockSize, imgData, tileXSize, yBlockSize, GDT_Float32, 0, 0);
                            outputBand[nBand-1]->RasterIO(GF_Write, xStart, (yStart + rowOffset), tileXSize, yBlockSize, imgData, tileXSize, yBlockSize, GDT_Float32, 0, 0);

                            ++nBand;
                        }
                    }
                    else
                    {
                        if(numberBands != dataset->GetRasterCount())
                        {
                            throw RSGISImageException("The number of bands in the input datasets and base dataset need to be the same.");
                        }

                        for(int n = 1; n <= numberBands; n++)
                        {
                            inputBand = dataset->GetRasterBand(n);

                            inputBand->RasterIO(GF_Read, 0, rowOffset, tileXSize, yBlockSize, imgData, tileXSize, yBlockSize, GDT_Float32, 0, 0);
                            outputBand[n-1]->RasterIO(GF_Write, xStart, (yStart + rowOffset), tileXSize, yBlockSize, imgData, tileXSize, yBlockSize, GDT_Float32, 0, 0);

                        }
                    }
                }
                if(remainRows > 0)
                {
                    rowOffset = yBlockSize * nYBlocks;
                    if(bandsDefined)
                    {
                        int nBand = 1;
                        for(std::vector<int>::iterator iterBands = bands.begin(); iterBands != bands.end(); ++iterBands)
                        {
                            if(((*iterBands) <= 0) | ((*iterBands) > dataset->GetRasterCount()))
                            {
                                throw RSGISImageException("Band is not within the input dataset.");
                            }
                            inputBand = dataset->GetRasterBand(*iterBands);

                            inputBand->RasterIO(GF_Read, 0, rowOffset, tileXSize, remainRows, imgData, tileXSize, remainRows, GDT_Float32, 0, 0);
                            outputBand[nBand-1]->RasterIO(GF_Write, xStart, (yStart + rowOffset), tileXSize, remainRows, imgData, tileXSize, remainRows, GDT_Float32, 0, 0);

                            ++nBand;
                        }
                    }
                    else
                    {
                        if(numberBands != dataset->GetRasterCount())
                        {
                            throw RSGISImageException("The number of bands in the input datasets and base dataset need to be the same.");
                        }

                        for(int n = 1; n <= numberBands; n++)
                        {
                            inputBand = dataset->GetRasterBand(n);

                            inputBand->RasterIO(GF_Read, 0, rowOffset, tileXSize, remainRows, imgData, tileXSize, remainRows, GDT_Float32, 0, 0);
                            outputBand[n-1]->RasterIO(GF_Write, xStart, (yStart + rowOffset), tileXSize, remainRows, imgData, tileXSize, remainRows, GDT_Float32, 0, 0);
                        }
                    }
                }
				CPLFree(imgData);
                GDALClose(dataset);
			}
			std::cout << ".complete\n";
		}
		catch(RSGISImageBandException &e)
		{
			if(imgData != NULL)
			{
				delete imgData;
			}
			if(transformation != NULL)
			{
				delete[] transformation;
			}
			if(imgTransform != NULL)
			{
				delete[] imgTransform;
			}
			throw e;
		}

		if(transformation != NULL)
		{
			delete[] transformation;
		}
		if(imgTransform != NULL)
		{
			delete[] imgTransform;
		}
	}

    void RSGISImageMosaic::includeDatasetsSkipVals(GDALDataset *baseImage, std::string *inputImages, int numDS, std::vector<int> bands, bool bandsDefined, float skipVal)
    {
        RSGISImageUtils imgUtils;
        GDALDataset *dataset = NULL;
        int width;
        int height;
        
        double *transformation = new double[6];
        double *imgTransform = new double[6];
        double *baseTransform = new double[6];
        int numberBands = 0;
        int tileXSize = 0;
        int tileYSize = 0;
        double xDiff = 0;
        double yDiff = 0;
        int xStart = 0;
        int yStart = 0;
        std::string projection;
        
        GDALRasterBand *inputBand = NULL;
        GDALRasterBand **outputBand = NULL;
        float *imgBaseData = NULL;
        float *imgInData = NULL;
        
        try
        {
            numberBands = baseImage->GetRasterCount();
            for(int i = 0; i < numDS; i++)
            {
                dataset = (GDALDataset *) GDALOpenShared(inputImages[i].c_str(), GA_ReadOnly);
                if(dataset == NULL)
                {
                    std::string message = std::string("Could not open image ") + inputImages[i];
                    throw RSGISImageException(message.c_str());
                }
                
                if(!bandsDefined)
                {
                    if(dataset->GetRasterCount() != numberBands)
                    {
                        throw RSGISImageBandException("All input images need to have the same number of bands.");
                    }
                }
                else
                {
                    for(std::vector<int>::iterator iterBands = bands.begin(); iterBands != bands.end(); ++iterBands)
                    {
                        if(((*iterBands) <= 0) | ((*iterBands) > dataset->GetRasterCount()))
                        {
                            std::cerr << "Band = " << *iterBands << std::endl;
                            std::string message = std::string("Band is not within the input dataset ") + inputImages[i];
                            throw RSGISImageException(message.c_str());
                        }
                    }
                }
                GDALClose(dataset);
            }
            
            if(bandsDefined)
            {
                if(numberBands < bands.size())
                {
                    throw RSGISImageException("The base image does not have enough image bands for the output data specificed.");
                }
            }
            
            projection = std::string(baseImage->GetProjectionRef());
            imgUtils.getImagesExtent(inputImages, numDS, &width, &height, transformation);
            
            baseImage->GetGeoTransform(baseTransform);
            
            double baseExtentX = baseTransform[0] + (baseImage->GetRasterXSize() * baseTransform[1]);
            double baseExtentY = baseTransform[3] + (baseImage->GetRasterYSize() * baseTransform[5]);
            double imgExtentX = transformation[0] + (width * transformation[1]);
            double imgExtentY = transformation[3] + (height * transformation[5]);
            
            // Check datasets fit within the base image.
            if(transformation[0] < baseTransform[0])
            {
                std::cerr << "transformation[0] = " << transformation[0] << std::endl;
                std::cerr << "baseTransform[0] = " << baseTransform[0] << std::endl;
                throw RSGISImageException("Images do not fit within the base image (Eastings Min)");
            }
            if(transformation[3] > baseTransform[3])
            {
                std::cerr << "transformation[3] = " << transformation[3] << std::endl;
                std::cerr << "baseTransform[3] = " << baseTransform[3] << std::endl;
                throw RSGISImageException("Images do not fit within the base image (Northings Max)");
            }
            if(imgExtentX > baseExtentX)
            {
                std::cerr << "imgExtentX = " << imgExtentX << std::endl;
                std::cerr << "baseExtentX = " << baseExtentX << std::endl;
                throw RSGISImageException("Images do not fit within the base image (Eastings Max)");
            }
            if(imgExtentY < baseExtentY)
            {
                std::cerr << "imgExtentY = " << imgExtentY << std::endl;
                std::cerr << "baseExtentY = " << baseExtentY << std::endl;
                throw RSGISImageException("Images do not fit within the base image (Northings Min)");
            }
            
            height = baseImage->GetRasterYSize();
            width = baseImage->GetRasterXSize();
            
            outputBand = new GDALRasterBand*[numberBands];
            for(int i = 0; i < numberBands; i++)
            {
                outputBand[i] = baseImage->GetRasterBand(i+1);
            }
            
            int xBlockSize = 0;
            int yBlockSize = 0;
            outputBand[0]->GetBlockSize (&xBlockSize, &yBlockSize);
                        
            std::cout << "Started (total " << numDS << ") ." << std::flush;
            
            for(int i = 0; i < numDS; i++)
            {
                std::cout << "." << i << "." << std::flush;
                dataset = (GDALDataset *) GDALOpenShared(inputImages[i].c_str(), GA_ReadOnly);
                if(dataset == NULL)
                {
                    std::string message = std::string("Could not open image ") + inputImages[i];
                    throw RSGISImageException(message.c_str());
                }
                
                dataset->GetGeoTransform(imgTransform);
                tileXSize = dataset->GetRasterXSize();
                tileYSize = dataset->GetRasterYSize();
                
                xDiff = imgTransform[0] - baseTransform[0];
                yDiff = baseTransform[3] - imgTransform[3];
                
                xStart = floor((xDiff/baseTransform[1])+0.5);
                yStart = floor((yDiff/baseTransform[1])+0.5);
                imgBaseData = (float *) CPLMalloc(sizeof(float)*(tileXSize*yBlockSize));
                imgInData = (float *) CPLMalloc(sizeof(float)*(tileXSize*yBlockSize));
                unsigned long numPxlsInBlock = tileXSize*yBlockSize;
                
                int nYBlocks = (tileYSize / yBlockSize);
                int remainRows = tileYSize - (nYBlocks * yBlockSize);
                int rowOffset = 0;
                
                for(int j = 0; j < nYBlocks; j++)
                {
                    rowOffset = yBlockSize * j;
                    if(bandsDefined)
                    {
                        int nBand = 1;
                        for(std::vector<int>::iterator iterBands = bands.begin(); iterBands != bands.end(); ++iterBands)
                        {
                            if(((*iterBands) <= 0) | ((*iterBands) > dataset->GetRasterCount()))
                            {
                                throw RSGISImageException("Band is not within the input dataset.");
                            }
                            inputBand = dataset->GetRasterBand(*iterBands);
                            
                            inputBand->RasterIO(GF_Read, 0, rowOffset, tileXSize, yBlockSize, imgInData, tileXSize, yBlockSize, GDT_Float32, 0, 0);
                            outputBand[nBand-1]->RasterIO(GF_Read, xStart, (yStart + rowOffset), tileXSize, yBlockSize, imgBaseData, tileXSize, yBlockSize, GDT_Float32, 0, 0);
                            
                            for(unsigned long m = 0; m < numPxlsInBlock; ++m)
                            {
                                if(imgInData[m] == skipVal)
                                {
                                    imgInData[m] = imgBaseData[m];
                                }
                            }
                            
                            outputBand[nBand-1]->RasterIO(GF_Write, xStart, (yStart + rowOffset), tileXSize, yBlockSize, imgInData, tileXSize, yBlockSize, GDT_Float32, 0, 0);
                            
                            ++nBand;
                        }
                    }
                    else
                    {
                        if(numberBands != dataset->GetRasterCount())
                        {
                            throw RSGISImageException("The number of bands in the input datasets and base dataset need to be the same.");
                        }
                        
                        for(int n = 1; n <= numberBands; n++)
                        {
                            inputBand = dataset->GetRasterBand(n);
                            
                            inputBand->RasterIO(GF_Read, 0, rowOffset, tileXSize, yBlockSize, imgInData, tileXSize, yBlockSize, GDT_Float32, 0, 0);
                            outputBand[n-1]->RasterIO(GF_Read, xStart, (yStart + rowOffset), tileXSize, yBlockSize, imgBaseData, tileXSize, yBlockSize, GDT_Float32, 0, 0);
                            
                            for(unsigned long m = 0; m < numPxlsInBlock; ++m)
                            {
                                if(imgInData[m] == skipVal)
                                {
                                    imgInData[m] = imgBaseData[m];
                                }
                            }
                            
                            outputBand[n-1]->RasterIO(GF_Write, xStart, (yStart + rowOffset), tileXSize, yBlockSize, imgInData, tileXSize, yBlockSize, GDT_Float32, 0, 0);
                            
                        }
                    }
                }
                
                if(remainRows > 0)
                {
                    for(unsigned long m = 0; m < numPxlsInBlock; ++m)
                    {
                        imgInData[m] = 0;
                        imgBaseData[m] = 0;
                    }
                 
                    rowOffset = yBlockSize * nYBlocks;
                    numPxlsInBlock = tileXSize * remainRows;
                    if(bandsDefined)
                    {
                        int nBand = 1;
                        for(std::vector<int>::iterator iterBands = bands.begin(); iterBands != bands.end(); ++iterBands)
                        {
                            if(((*iterBands) <= 0) | ((*iterBands) > dataset->GetRasterCount()))
                            {
                                throw RSGISImageException("Band is not within the input dataset.");
                            }
                            inputBand = dataset->GetRasterBand(*iterBands);
                            
                            inputBand->RasterIO(GF_Read, 0, rowOffset, tileXSize, remainRows, imgInData, tileXSize, remainRows, GDT_Float32, 0, 0);
                            outputBand[nBand-1]->RasterIO(GF_Read, xStart, (yStart + rowOffset), tileXSize, remainRows, imgBaseData, tileXSize, remainRows, GDT_Float32, 0, 0);
                            
                            for(unsigned long m = 0; m < numPxlsInBlock; ++m)
                            {
                                if(imgInData[m] == skipVal)
                                {
                                    imgInData[m] = imgBaseData[m];
                                }
                            }

                            outputBand[nBand-1]->RasterIO(GF_Write, xStart, (yStart + rowOffset), tileXSize, remainRows, imgInData, tileXSize, remainRows, GDT_Float32, 0, 0);
                            
                            ++nBand;
                        }
                    }
                    else
                    {
                        if(numberBands != dataset->GetRasterCount())
                        {
                            throw RSGISImageException("The number of bands in the input datasets and base dataset need to be the same.");
                        }
                        
                        for(int n = 1; n <= numberBands; n++)
                        {
                            inputBand = dataset->GetRasterBand(n);
                            
                            inputBand->RasterIO(GF_Read, 0, rowOffset, tileXSize, remainRows, imgInData, tileXSize, remainRows, GDT_Float32, 0, 0);
                            outputBand[n-1]->RasterIO(GF_Read, xStart, (yStart + rowOffset), tileXSize, remainRows, imgBaseData, tileXSize, remainRows, GDT_Float32, 0, 0);

                            for(unsigned long m = 0; m < numPxlsInBlock; ++m)
                            {
                                if(imgInData[m] == skipVal)
                                {
                                    imgInData[m] = imgBaseData[m];
                                }
                            }
                            
                            outputBand[n-1]->RasterIO(GF_Write, xStart, (yStart + rowOffset), tileXSize, remainRows, imgBaseData, tileXSize, remainRows, GDT_Float32, 0, 0);
                        }
                    }
                }
                
                CPLFree(imgInData);
                CPLFree(imgBaseData);
                GDALClose(dataset);
            }
            std::cout << ".complete\n";
        }
        catch(RSGISImageBandException &e)
        {
            if(imgInData != NULL)
            {
                CPLFree(imgInData);
            }
            if(imgBaseData != NULL)
            {
                CPLFree(imgBaseData);
            }
            if(transformation != NULL)
            {
                delete[] transformation;
            }
            if(imgTransform != NULL)
            {
                delete[] imgTransform;
            }
            throw e;
        }
        
        if(transformation != NULL)
        {
            delete[] transformation;
        }
        if(imgTransform != NULL)
        {
            delete[] imgTransform;
        }
    }
    
    void RSGISImageMosaic::includeDatasetsIgnoreOverlap(GDALDataset *baseImage, std::string *inputImages, int numDS, int numOverlapPxls)
    {
        RSGISImageUtils imgUtils;
        GDALDataset *dataset = NULL;
        int width;
        int height;
        
        double *transformation = new double[6];
        double *imgTransform = new double[6];
        double *baseTransform = new double[6];
        int numberBands = 0;
        int tileXSize = 0;
        int tileYSize = 0;
        double xDiff = 0;
        double yDiff = 0;
        int xStart = 0;
        int yStart = 0;
        std::string projection;
        
        GDALRasterBand *inputBand = NULL;
        GDALRasterBand **outputBand = NULL;
        float *imgData = NULL;
        
        try
        {
            numberBands = baseImage->GetRasterCount();
            for(int i = 0; i < numDS; i++)
            {
                dataset = (GDALDataset *) GDALOpenShared(inputImages[i].c_str(), GA_ReadOnly);
                if(dataset == NULL)
                {
                    std::string message = std::string("Could not open image ") + inputImages[i];
                    throw RSGISImageException(message.c_str());
                }
                
                if(dataset->GetRasterCount() != numberBands)
                {
                    throw RSGISImageBandException("All input images need to have the same number of bands.");
                }
                
                
                GDALClose(dataset);
            }
            
            projection = std::string(baseImage->GetProjectionRef());
            imgUtils.getImagesExtent(inputImages, numDS, &width, &height, transformation);
            
            baseImage->GetGeoTransform(baseTransform);
            
            double baseExtentX = baseTransform[0] + (baseImage->GetRasterXSize() * baseTransform[1]);
            double baseExtentY = baseTransform[3] + (baseImage->GetRasterYSize() * baseTransform[5]);
            double imgExtentX = transformation[0] + (width * transformation[1]);
            double imgExtentY = transformation[3] + (height * transformation[5]);
                        
            // Check datasets fit within the base image.
            if(transformation[0] < baseTransform[0])
            {
                std::cerr << "transformation[0] = " << transformation[0] << std::endl;
                std::cerr << "baseTransform[0] = " << baseTransform[0] << std::endl;
                throw RSGISImageException("Images do not fit within the base image (Eastings Min)");
            }
            if(transformation[3] > baseTransform[3])
            {
                std::cerr << "transformation[3] = " << transformation[3] << std::endl;
                std::cerr << "baseTransform[3] = " << baseTransform[3] << std::endl;
                throw RSGISImageException("Images do not fit within the base image (Northings Max)");
            }
            if(imgExtentX > baseExtentX)
            {
                std::cerr << "imgExtentX = " << imgExtentX << std::endl;
                std::cerr << "baseExtentX = " << baseExtentX << std::endl;
                throw RSGISImageException("Images do not fit within the base image (Eastings Max)");
            }
            if(imgExtentY < baseExtentY)
            {
                std::cerr << "imgExtentY = " << imgExtentY << std::endl;
                std::cerr << "baseExtentY = " << baseExtentY << std::endl;
                throw RSGISImageException("Images do not fit within the base image (Northings Min)");
            }
            
            height = baseImage->GetRasterYSize();
            width = baseImage->GetRasterXSize();
            
            outputBand = new GDALRasterBand*[numberBands];
            for(int i = 0; i < numberBands; i++)
            {
                outputBand[i] = baseImage->GetRasterBand(i+1);
            }
            
            int xBlockSize = 0;
            int yBlockSize = 0;
            outputBand[0]->GetBlockSize (&xBlockSize, &yBlockSize);
                        
            std::cout << "Started (total " << numDS << ") ." << std::flush;
            
            for(int i = 0; i < numDS; i++)
            {
                std::cout << "." << i << "." << std::flush;
                dataset = (GDALDataset *) GDALOpenShared(inputImages[i].c_str(), GA_ReadOnly);
                if(dataset == NULL)
                {
                    std::string message = std::string("Could not open image ") + inputImages[i];
                    throw RSGISImageException(message.c_str());
                }
                
                dataset->GetGeoTransform(imgTransform);
                tileXSize = dataset->GetRasterXSize();
                tileYSize = dataset->GetRasterYSize();
                
                // Apply offset to image size.
                tileXSize = tileXSize - (numOverlapPxls * 2);
                tileYSize = tileYSize - (numOverlapPxls * 2);
                
                imgTransform[0] = imgTransform[0] + (numOverlapPxls * baseTransform[1]);
                imgTransform[3] = imgTransform[3] - (numOverlapPxls * baseTransform[1]);
                
                xDiff = imgTransform[0] - baseTransform[0];
                yDiff = baseTransform[3] - imgTransform[3];
                
                xStart = floor((xDiff/baseTransform[1])+0.5);
                yStart = floor((yDiff/baseTransform[1])+0.5);
                imgData = (float *) CPLMalloc(sizeof(float)*(tileXSize*yBlockSize));
                
                int nYBlocks = tileYSize / yBlockSize;
                int remainRows = tileYSize - (nYBlocks * yBlockSize);
                int rowOffset = 0;
                
                for(int i = 0; i < nYBlocks; i++)
                {
                    rowOffset = (yBlockSize * i);// + numOverlapPxls;
                    
                    for(int n = 1; n <= numberBands; n++)
                    {
                        inputBand = dataset->GetRasterBand(n);
                        
                        inputBand->RasterIO(GF_Read, numOverlapPxls, (rowOffset + numOverlapPxls), tileXSize, yBlockSize, imgData, tileXSize, yBlockSize, GDT_Float32, 0, 0);
                        outputBand[n-1]->RasterIO(GF_Write, xStart, (yStart + rowOffset), tileXSize, yBlockSize, imgData, tileXSize, yBlockSize, GDT_Float32, 0, 0);
                        
                    }
                }
                if(remainRows > 0)
                {
                    rowOffset = (yBlockSize * nYBlocks);// + numOverlapPxls;
                    
                    for(int n = 1; n <= numberBands; n++)
                    {
                        inputBand = dataset->GetRasterBand(n);
                        
                        inputBand->RasterIO(GF_Read, numOverlapPxls, (rowOffset + numOverlapPxls), tileXSize, remainRows, imgData, tileXSize, remainRows, GDT_Float32, 0, 0);
                        outputBand[n-1]->RasterIO(GF_Write, xStart, (yStart + rowOffset), tileXSize, remainRows, imgData, tileXSize, remainRows, GDT_Float32, 0, 0);
                    }
                }
                CPLFree(imgData);
                GDALClose(dataset);
            }
            std::cout << ".complete\n";
        }
        catch(RSGISImageBandException &e)
        {
            if(imgData != NULL)
            {
                delete imgData;
            }
            if(transformation != NULL)
            {
                delete[] transformation;
            }
            if(imgTransform != NULL)
            {
                delete[] imgTransform;
            }
            throw e;
        }
        
        if(transformation != NULL)
        {
            delete[] transformation;
        }
        if(imgTransform != NULL)
        {
            delete[] imgTransform;
        }
    }
    
    void RSGISImageMosaic::orderInImagesValidData(std::vector<std::string> images, std::vector<std::string> *orderedImages, float noDataValue)
    {
        try
        {
            RSGISImageValidDataMetric *validPxlCountObj = new RSGISImageValidDataMetric();
            RSGISCountValidPixels *calcImageValidPxlCount = new RSGISCountValidPixels(validPxlCountObj, noDataValue);
			RSGISCalcImage calcImg = RSGISCalcImage(calcImageValidPxlCount, "", true);

            std::list<RSGISImageValidDataMetric> validDataImageMetrics;
            for(std::vector<std::string>::iterator iterImage = images.begin(); iterImage != images.end(); ++iterImage)
            {
                // Calculate Valid Pixel Count.
                validPxlCountObj->totalNumPxls = 0;
                validPxlCountObj->validPxlCount = 0;
                validPxlCountObj->noDataPxlCount = 0;
                calcImageValidPxlCount->resetValidPxlsObj();

                std::cout << "Processing Image: " << (*iterImage) << std::endl;
                GDALDataset *dataset = (GDALDataset *) GDALOpen((*iterImage).c_str(), GA_ReadOnly);
                if(dataset == NULL)
                {
                    std::string message = std::string("Could not open image ") + (*iterImage);
                    throw rsgis::RSGISImageException(message.c_str());
                }

                try
                {
                    calcImg.calcImage(&dataset, 1);

                    RSGISImageValidDataMetric imgDataMetric;
                    imgDataMetric.imageFile = (*iterImage);
                    imgDataMetric.totalNumPxls = validPxlCountObj->totalNumPxls;
                    imgDataMetric.validPxlCount = validPxlCountObj->validPxlCount;
                    imgDataMetric.noDataPxlCount = validPxlCountObj->noDataPxlCount;
                    imgDataMetric.validPxlFunc = ((double)validPxlCountObj->validPxlCount) / ((double)validPxlCountObj->totalNumPxls);

                    validDataImageMetrics.push_back(imgDataMetric);

                    GDALClose(dataset);
                }
                catch (rsgis::RSGISException &e)
                {
                    delete validPxlCountObj;
                    delete calcImageValidPxlCount;
                    GDALClose(dataset);
                    throw RSGISImageException(e.what());
                }
                catch (std::exception &e)
                {
                    delete validPxlCountObj;
                    delete calcImageValidPxlCount;
                    GDALClose(dataset);
                    throw RSGISImageException(e.what());
                }
            }

            validDataImageMetrics.sort(compare_ImageValidPxlCounts);

            orderedImages->clear();
            for(std::list<RSGISImageValidDataMetric>::iterator iterImage = validDataImageMetrics.begin(); iterImage != validDataImageMetrics.end(); ++iterImage)
            {
                orderedImages->push_back((*iterImage).imageFile);
            }

            delete validPxlCountObj;
            delete calcImageValidPxlCount;
        }
        catch (RSGISImageException &e)
        {
            throw e;
        }
        catch (rsgis::RSGISException &e)
        {
            throw RSGISImageException(e.what());
        }
        catch (std::exception &e)
        {
            throw RSGISImageException(e.what());
        }
    }

	RSGISImageMosaic::~RSGISImageMosaic()
	{

	}

    RSGISCountValidPixels::RSGISCountValidPixels(RSGISImageValidDataMetric *validPxlsObj, float noDataVal):RSGISCalcImageValue(0)
    {
        this->validPxlsObj = validPxlsObj;
        this->noDataVal = noDataVal;
    }

    void RSGISCountValidPixels::calcImageValue(float *bandValues, int numBands) 
    {
        bool pxlIsNoData = true;
        for(int i = 0; i < numBands; ++i)
        {
            if(bandValues[i] != noDataVal)
            {
                pxlIsNoData = false;
                break;
            }
        }

        if(pxlIsNoData)
        {
            ++this->validPxlsObj->noDataPxlCount;
        }
        else
        {
            ++this->validPxlsObj->validPxlCount;
        }

        ++this->validPxlsObj->totalNumPxls;
    }

    void RSGISCountValidPixels::resetValidPxlsObj()
    {
        this->validPxlsObj->noDataPxlCount = 0;
        this->validPxlsObj->validPxlCount = 0;
        this->validPxlsObj->totalNumPxls = 0;
    }

    RSGISCountValidPixels::~RSGISCountValidPixels()
    {

    }


    RSGISCombineImgTileOverview::RSGISCombineImgTileOverview()
    {
        
    }
    
    void RSGISCombineImgTileOverview::combineKEAImgTileOverviews(GDALDataset *baseImg, std::vector<std::string> inputImages, std::vector<int> pyraScaleVals)
    {
        try
        {
            RSGISImageUtils imgUtils;
            int numberBands = baseImg->GetRasterCount();
            int numOverviews = pyraScaleVals.size();
            std::string projection;
            GDALDataset *dataset = NULL;
            
            if( numOverviews == 0 )
            {
                throw RSGISImageException("There must be at least 1 overview.");
            }
            
            for(std::vector<std::string>::iterator iterImgFile = inputImages.begin(); iterImgFile != inputImages.end(); ++iterImgFile)
            {                
                dataset = (GDALDataset *) GDALOpenShared((*iterImgFile).c_str(), GA_ReadOnly);
                if(dataset == NULL)
                {
                    std::string message = std::string("Could not open image ") + (*iterImgFile);
                    throw RSGISImageException(message.c_str());
                }
                
                if(dataset->GetRasterCount() != numberBands)
                {
                    throw RSGISImageBandException("All input images need to have the same number of bands.");
                }
  
                // Check the number of overviews is the same for all input images and number of levels specified.
                for(int i = 0; i < numberBands; ++i)
                {
                    if(dataset->GetRasterBand(i+1)->GetOverviewCount() != numOverviews)
                    {
                        throw RSGISImageBandException("All input images and bands need to have the same number of overviews as list provided.");
                    }
                }
                
                GDALClose(dataset);
            }
            
            int width;
            int height;
            double *transformation = new double[6];
            double *baseTransform = new double[6];
            
            imgUtils.getImagesExtent(inputImages, &width, &height, transformation);
            
            baseImg->GetGeoTransform(baseTransform);
            
            double baseExtentX = baseTransform[0] + (baseImg->GetRasterXSize() * baseTransform[1]);
            double baseExtentY = baseTransform[3] + (baseImg->GetRasterYSize() * baseTransform[5]);
            double imgExtentX = transformation[0] + (width * transformation[1]);
            double imgExtentY = transformation[3] + (height * transformation[5]);
            
            // Check datasets fit within the base image.
            if(transformation[0] < baseTransform[0])
            {
                std::cerr << "transformation[0] = " << transformation[0] << std::endl;
                std::cerr << "baseTransform[0] = " << baseTransform[0] << std::endl;
                throw RSGISImageException("Images do not fit within the base image (Eastings Min)");
            }
            if(transformation[3] > baseTransform[3])
            {
                std::cerr << "transformation[3] = " << transformation[3] << std::endl;
                std::cerr << "baseTransform[3] = " << baseTransform[3] << std::endl;
                throw RSGISImageException("Images do not fit within the base image (Northings Max)");
            }
            if(imgExtentX > baseExtentX)
            {
                std::cerr << "imgExtentX = " << imgExtentX << std::endl;
                std::cerr << "baseExtentX = " << baseExtentX << std::endl;
                throw RSGISImageException("Images do not fit within the base image (Eastings Max)");
            }
            if(imgExtentY < baseExtentY)
            {
                std::cerr << "imgExtentY = " << imgExtentY << std::endl;
                std::cerr << "baseExtentY = " << baseExtentY << std::endl;
                throw RSGISImageException("Images do not fit within the base image (Northings Min)");
            }
            
            double baseTLX = baseTransform[0];
            double baseTLY = baseTransform[3];
            long baseWidth = baseImg->GetRasterXSize();
            long baseHeight = baseImg->GetRasterYSize();
            double baseResX = baseTransform[1];
            double baseResY = baseTransform[5];
            if(baseResY < 0)
            {
                baseResY = baseResY * (-1);
            }
            long tileWidth = 0;
            long tileHeight = 0;
            long overWidth = 0;
            long overHeight = 0;
            long calcOverWidth = 0;
            long calcOverHeight = 0;
            
            std::cout << "Base Image " << ": [" << baseWidth << ", " << baseHeight << "]\n";
            unsigned long **overviewImgsSize = new unsigned long*[numOverviews];
            for(int i = 0; i < numOverviews; ++i)
            {
                overviewImgsSize[i] = new unsigned long[2];
                overviewImgsSize[i][0] = baseWidth/pyraScaleVals.at(i);
                overviewImgsSize[i][1] = baseHeight/pyraScaleVals.at(i);
                std::cout << "Overview " << i << " (Level: " << pyraScaleVals.at(i) << ") [" << overviewImgsSize[i][0] << ", " << overviewImgsSize[i][1] << "]\n";
            }
            
            for(std::vector<std::string>::iterator iterImgFile = inputImages.begin(); iterImgFile != inputImages.end(); ++iterImgFile)
            {
                dataset = (GDALDataset *) GDALOpenShared((*iterImgFile).c_str(), GA_ReadOnly);
                if(dataset == NULL)
                {
                    std::string message = std::string("Could not open image ") + (*iterImgFile);
                    throw RSGISImageException(message.c_str());
                }
                GDALRasterBand *imgBand = dataset->GetRasterBand(1);
                GDALRasterBand *gdalBandOver = NULL;
                tileWidth = dataset->GetRasterXSize();
                tileHeight = dataset->GetRasterYSize();
                for(int i = 0; i < numOverviews; ++i)
                {
                    gdalBandOver = imgBand->GetOverview(i);
                    overWidth = gdalBandOver->GetXSize();
                    overHeight = gdalBandOver->GetYSize();
                    
                    calcOverWidth = tileWidth / pyraScaleVals.at(i);
                    calcOverHeight = tileHeight / pyraScaleVals.at(i);
                    
                    if((overWidth != calcOverWidth) | (overHeight != calcOverHeight))
                    {
                        std::cout << "Image: " << *iterImgFile << std::endl;
                        std::cout << "Image DIMS: " << ": [" << tileWidth << ", " << tileHeight << "]\n";
                        std::cout << "Overview (Img) " << i << ": [" << overWidth << ", " << overHeight << "]\n";
                        std::cout << "Overview (Cal) " << i << ": [" << calcOverWidth << ", " << calcOverHeight << "]\n\n";
                        throw RSGISImageException("Overview is not the correct size for the specified levels.");
                    }
                }
            }
            
            std::cout << "Inputs all seem correct, adding pyramids to base image\n";
            
            kealib::KEAImageIO *keaBaseImgIO;
            void *internalData = baseImg->GetInternalHandle("");
            if(internalData != NULL)
            {
                try
                {
                    keaBaseImgIO = static_cast<kealib::KEAImageIO*>(internalData);
                    
                    if((keaBaseImgIO == NULL) | (keaBaseImgIO == 0))
                    {
                        throw RSGISImageException("Could not get hold of the internal KEA Image IO Object - is input image a KEA file?");
                    }
                }
                catch(RSGISImageException& e)
                {
                    throw e;
                }
            }
            else
            {
                throw RSGISImageException("Internal data on GDAL Dataset was NULL - check input file is KEA.");
            }
            
            float *data = NULL;
            for(int j = 0; j < numOverviews; ++j)
            {
                data = new float[overviewImgsSize[j][0]*overviewImgsSize[j][1]];
                for(long n = 0; n < (overviewImgsSize[j][0]*overviewImgsSize[j][1]); ++n)
                {
                    data[n] = 0.0;
                }
                for(int i = 0; i < numberBands; ++i)
                {
                    keaBaseImgIO->createOverview(i+1, j+1, overviewImgsSize[j][0], overviewImgsSize[j][1]);
                    keaBaseImgIO->writeToOverview(i+1, j+1, data, 0, 0, overviewImgsSize[j][0], overviewImgsSize[j][1], overviewImgsSize[j][0], overviewImgsSize[j][1], kealib::kea_32float);
                }
                delete[] data;
            }
            
            GDALRasterBand *imgBand = NULL;
            GDALRasterBand *gdalBandOver = NULL;
            double xDiff = 0.0;
            double yDiff = 0.0;
            long xDiffBasePxl = 0;
            long yDiffBasePxl = 0;
            long xDiffOvPxl = 0;
            long yDiffOvPxl = 0;
            double tileTLX = 0.0;
            double tileTLY = 0.0;
            for(std::vector<std::string>::iterator iterImgFile = inputImages.begin(); iterImgFile != inputImages.end(); ++iterImgFile)
            {
                std::cout << "Adding: " << *iterImgFile << std::endl;
                dataset = (GDALDataset *) GDALOpenShared((*iterImgFile).c_str(), GA_ReadOnly);
                if(dataset == NULL)
                {
                    std::string message = std::string("Could not open image ") + (*iterImgFile);
                    throw RSGISImageException(message.c_str());
                }

                tileWidth = dataset->GetRasterXSize();
                tileHeight = dataset->GetRasterYSize();
                dataset->GetGeoTransform(transformation);
                tileTLX = transformation[0];
                tileTLY = transformation[3];

                xDiff = tileTLX - baseTLX;
                yDiff = baseTLY - tileTLY;
                
                std::cout << "Diff = [" << xDiff << ", " << yDiff << "]\n";
                
                xDiffBasePxl = floor((xDiff/baseResX)+0.5);
                yDiffBasePxl = floor((yDiff/baseResY)+0.5);
                
                std::cout << "Base Pxl Diff = [" << xDiffBasePxl << ", " << yDiffBasePxl << "]\n";
                
                for(int i = 0; i < numOverviews; ++i)
                {
                    xDiffOvPxl = xDiffBasePxl / pyraScaleVals.at(i);
                    yDiffOvPxl = yDiffBasePxl / pyraScaleVals.at(i);
                    std::cout << "\tBase Overview Diff = [" << xDiffOvPxl << ", " << yDiffOvPxl << "]\n";
                    
                    overWidth = tileWidth / pyraScaleVals.at(i);
                    overHeight = tileHeight / pyraScaleVals.at(i);
                    
                    data = new float[overWidth*overHeight];
                    
                    for(int j = 0; j < numberBands; ++j)
                    {
                        imgBand = dataset->GetRasterBand(j+1);
                        gdalBandOver = imgBand->GetOverview(i);
                        gdalBandOver->RasterIO(GF_Read, 0, 0, overWidth, overHeight, data, overWidth, overHeight, GDT_Float32, 0, 0);
                        keaBaseImgIO->writeToOverview(j+1, i+1, data, xDiffOvPxl, yDiffOvPxl, overWidth, overHeight, overWidth, overHeight, kealib::kea_32float);
                    }
                    delete[] data;
                }
                
            }
            
            delete[] transformation;
            delete[] baseTransform;
            for(int i = 0; i < numOverviews; ++i)
            {
                delete[] overviewImgsSize[i];
            }
            
        }
        catch(RSGISImageException& e)
        {
            throw e;
        }
        catch(RSGISException& e)
        {
            throw RSGISImageException(e.what());
        }
        catch(std::exception& e)
        {
            throw RSGISImageException(e.what());
        }
    }
    
    RSGISCombineImgTileOverview::~RSGISCombineImgTileOverview()
    {
        
    }
    
    
    RSGISIncludeSingleImgCalcImgVal::RSGISIncludeSingleImgCalcImgVal(int numBands, bool useNoData, float noDataVal):RSGISCalcImageValue(numBands)
    {
        this->useNoData = useNoData;
        this->noDataVal = noDataVal;
    }
    
    void RSGISIncludeSingleImgCalcImgVal::calcImageValue(float *bandValues, int numBands, double *output) 
    {
        if((this->numOutBands*2) != numBands)
        {
            throw RSGISImageCalcException("The number of bands does not fit the number specified.");
        }
        
        bool foundNoData = false;
        if(this->useNoData)
        {
            for(int i = this->numOutBands; i < numBands; ++i)
            {
                if(bandValues[i] == noDataVal)
                {
                    foundNoData = true;
                    break;
                }
            }
            
            for(int i = 0; i < this->numOutBands; ++i)
            {
                 if(!foundNoData)
                 {
                     output[i] = bandValues[i+this->numOutBands];
                 }
                 else
                 {
                     output[i] = bandValues[i];
                 }
            }
        }
        else
        {
            for(int i = 0; i < this->numOutBands; ++i)
            {
                output[i] = bandValues[this->numOutBands+i];
            }
        }
    }
    
    RSGISIncludeSingleImgCalcImgVal::~RSGISIncludeSingleImgCalcImgVal()
    {
        
    }
    

}}

