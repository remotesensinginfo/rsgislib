/*
 *  RSGISPointZonalStats.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 22/09/2009.
 *  Copyright 2009 RSGISLib. All rights reserved.
 *  This file is part of RSGISLib.
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

#include "RSGISVectorZonalStats.h"

namespace rsgis{namespace vec{
	
	RSGISVectorZonalStats::RSGISVectorZonalStats(GDALDataset *image, std::string outZonalFileName, bool useBandNames, bool shortenFileNames)
	{
		rsgis::math::RSGISMathsUtils mathUtils;
        
        this->image = image;
		
        this->numImgBands = image->GetRasterCount();
        
		this->bands = new GDALRasterBand*[numImgBands];
        
        // Set up output names
        this->useBandNames = useBandNames;
        this->outNames = new std::string[numImgBands];
        
		for(int i = 0; i < numImgBands; ++i)
		{
			bands[i] = image->GetRasterBand(i+1);
            
            if(this->useBandNames)
            {
                std::string bandName = bands[i]->GetDescription();
                
                // Replace spaces and parentheses in file name
                boost::algorithm::replace_all(bandName, " ", "_");
                boost::algorithm::replace_all(bandName, "(", "_");
                boost::algorithm::replace_all(bandName, ")", "_");
                boost::algorithm::replace_all(bandName, "[", "_");
                boost::algorithm::replace_all(bandName, "]", "_");
                boost::algorithm::replace_all(bandName, ":", "");
                boost::algorithm::replace_all(bandName, "?", "");
                boost::algorithm::replace_all(bandName, ">", "gt");
                boost::algorithm::replace_all(bandName, "<", "lt");
                boost::algorithm::replace_all(bandName, "=", "eq");
                
                /* Check if band name is longer than maximum length for shapefile field name
                   No limit on CSV but makes management easier with shorter names */
                if((bandName.length() > 9) & shortenFileNames)
                {
                    // If not using all of name, append number so unique
                    std::cerr << "WARNING: "<< bandName << " will be truncated to \'" << bandName.substr(0, 7) << i+1 << "\'" << std::endl;
                    this->outNames[i] = bandName.substr(0, 7) + mathUtils.inttostring(i+1);
                }
                else if(bandName == "")
                {
                    // Description is empty, use b1 etc.,
                    this->outNames[i] = std::string("b") + mathUtils.inttostring(i+1);
                }
                else{this->outNames[i] = bandName;}
            }

            else
            {
                this->outNames[i] = std::string("b") + mathUtils.inttostring(i+1);
            }
		}
		
		double geoTransform[6];
		
		if( image->GetGeoTransform( geoTransform ) == CE_None )
		{
			double xMin = geoTransform[0];
			double yMax = geoTransform[3];
			
			double xMax = geoTransform[0] + (image->GetRasterXSize() * geoTransform[1]);
			double yMin = geoTransform[3] + (image->GetRasterYSize() * geoTransform[5]);
			
			imageExtent = new geos::geom::Envelope(xMin, xMax, yMin, yMax);
			
			imgRes = geoTransform[1];
		
			pxlValues = (float *) CPLMalloc(sizeof(float));
		}
        
        if (outZonalFileName != "")
		{
			this->outputToTextFile = true;
			this->outZonalFile.open(outZonalFileName.c_str());
			this->firstLine = true;
		}

	}
	
	void RSGISVectorZonalStats::processFeature(OGRFeature *inFeature, OGRFeature *outFeature, geos::geom::Envelope *env, long fid) throw(RSGISVectorException)
	{
		OGRGeometry *geometry = inFeature->GetGeometryRef();
		if( geometry != NULL && wkbFlatten(geometry->getGeometryType()) == wkbPolygon )
		{
			throw RSGISVectorException("This function is for point geometries, use \'pixelstats\' for polygons");
		} 
		else if( geometry != NULL && wkbFlatten(geometry->getGeometryType()) == wkbMultiPolygon )
		{
			throw RSGISVectorException("This function is for point geometries, use \'pixelstats\' for polygons");
		}
		else if( geometry != NULL && wkbFlatten(geometry->getGeometryType()) == wkbPoint )
		{
			OGRPoint *point = (OGRPoint *) geometry;
			
			OGRFeatureDefn *outFeatureDefn = outFeature->GetDefnRef();
			
			if( (point->getX() > imageExtent->getMinX()) &&
			    (point->getX() < imageExtent->getMaxX()) &&
				(point->getY() > imageExtent->getMinY()) &&
			    (point->getY() < imageExtent->getMaxY()))
			{
				double xDiff = point->getX() - imageExtent->getMinX();
				double yDiff = imageExtent->getMaxY() - point->getY();
				
				int xPxl = static_cast<int> (xDiff/imgRes);
				int yPxl = static_cast<int> (yDiff/imgRes);
				
				float *values = this->getPixelColumns(xPxl, yPxl);
				rsgis::math::RSGISMathsUtils mathUtils;
				std::string fieldname = "";
				for(int i = 0; i < this->numImgBands; ++i)
				{
					fieldname = this->outNames[i];
					outFeature->SetField(outFeatureDefn->GetFieldIndex(fieldname.c_str()), values[i]);
				}
			}
			else 
			{
				std::cerr << "WARNING: Point not within image\n";
				
                rsgis::math::RSGISMathsUtils mathUtils;
				std::string fieldname = "";
				
				for(int i = 0; i < numImgBands; ++i)
				{
					fieldname = this->outNames[i];
					outFeature->SetField(outFeatureDefn->GetFieldIndex(fieldname.c_str()), 0);
				}
			}

		}	
		else if( geometry != NULL && wkbFlatten(geometry->getGeometryType()) == wkbLineString )
		{
			throw RSGISVectorException("Polylines not implemented yet.");
		}
		else if(geometry != NULL)
		{
			std::string message = std::string("Unsupported data type: ") + std::string(geometry->getGeometryName());
			throw RSGISVectorException(message);
		}
		else 
		{
			throw RSGISVectorException("WARNING: NULL Geometry Present within input file");
		}
	}
	
	void RSGISVectorZonalStats::processFeature(OGRFeature *inFeature, geos::geom::Envelope *env, long fid) throw(RSGISVectorException)
	{
        // Add header info for first line
        if(this->firstLine)
        {
            this->outZonalFile << "FID";
            for(int i = 0; i < this->numImgBands; i++)
            {
                
                std::string fieldname = this->outNames[i];
                this->outZonalFile << "," << fieldname;
            }
            
            this->outZonalFile << "\n";
            this->firstLine = false;
        }
        
        
        OGRGeometry *geometry = inFeature->GetGeometryRef();
		if( geometry != NULL && wkbFlatten(geometry->getGeometryType()) == wkbPolygon )
		{
			throw RSGISVectorException("This function is for point geometries, use \'pixelstats\' for polygons");
		}
		else if( geometry != NULL && wkbFlatten(geometry->getGeometryType()) == wkbMultiPolygon )
		{
			throw RSGISVectorException("This function is for point geometries, use \'pixelstats\' for polygons");
		}
		else if( geometry != NULL && wkbFlatten(geometry->getGeometryType()) == wkbPoint )
		{
			OGRPoint *point = (OGRPoint *) geometry;
			if( (point->getX() > imageExtent->getMinX()) &&
               (point->getX() < imageExtent->getMaxX()) &&
               (point->getY() > imageExtent->getMinY()) &&
               (point->getY() < imageExtent->getMaxY()))
			{
				double xDiff = point->getX() - imageExtent->getMinX();
				double yDiff = imageExtent->getMaxY() - point->getY();
				
				int xPxl = static_cast<int> (xDiff/imgRes);
				int yPxl = static_cast<int> (yDiff/imgRes);
				
				float *values = this->getPixelColumns(xPxl, yPxl);
				rsgis::math::RSGISMathsUtils mathUtils;
				
                
                // Write out FID
                this->outZonalFile << fid;
                // Write out pixel value for each band
                for(int i = 0; i < this->numImgBands; ++i)
				{
                    this->outZonalFile << "," << values[i];
                }

                this->outZonalFile << "\n";
			}
			else
			{
				std::cerr << "WARNING: Point not within image\n";
				
                // Write out FID
                this->outZonalFile << fid;
                // Write out zero for each band
                for(int i = 0; i < this->numImgBands; ++i)
				{
                    this->outZonalFile << "," << 0;
                }
                
                this->outZonalFile << "\n";
			}
            
		}
		else if( geometry != NULL && wkbFlatten(geometry->getGeometryType()) == wkbLineString )
		{
			throw RSGISVectorException("Polylines not implemented yet.");
		}
		else if(geometry != NULL)
		{
			std::string message = std::string("Unsupported data type: ") + std::string(geometry->getGeometryName());
			throw RSGISVectorException(message);
		}
		else
		{
			throw RSGISVectorException("WARNING: NULL Geometry Present within input file");
		}
	}
	
	void RSGISVectorZonalStats::createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn) throw(RSGISVectorOutputException)
	{
        rsgis::math::RSGISMathsUtils mathUtils;
		std::string fieldname = "";
		for(int i = 0; i < numImgBands; ++i)
		{
			fieldname = this->outNames[i];;
			OGRFieldDefn shpField(fieldname.c_str(), OFTReal);
			shpField.SetPrecision(10);
			if(outputLayer->CreateField( &shpField ) != OGRERR_NONE )
			{
				std::string message = std::string("Creating shapefile field ") + fieldname + std::string(" has failed");
				throw RSGISVectorOutputException(message.c_str());
			}
		}
	}
	
	float* RSGISVectorZonalStats::getPixelColumns(int xPxl, int yPxl)
	{
		float *values = new float[numImgBands];
		for(int i = 0; i < numImgBands; ++i)
		{
			bands[i]->RasterIO(GF_Read, xPxl, yPxl, 1, 1, pxlValues, 1, 1, GDT_Float32, 0, 0);
			values[i] = pxlValues[0];
		}
		return values;
	}
	
	RSGISVectorZonalStats::~RSGISVectorZonalStats()
	{
        // Close text tile (if writing to).
        if (this->outputToTextFile)
		{
			this->outZonalFile.close();
		}
        delete[] this->bands;
		delete this->imageExtent;
		delete[] this->pxlValues;
        delete[] this->outNames;
	}
}}

