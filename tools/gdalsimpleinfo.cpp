/*
 *  main.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 10/05/2013.
 *  Copyright 2013 RSGISLib.
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

#include <iostream>
#include <exception>
#include <string>
#include <time.h>

#include "gdal_priv.h"
#include "ogrsf_frmts.h"
#include "ogr_api.h"


int main(int argc, char **argv)
{    
	try
	{
		if(argc != 1)
		{
            std::cout.precision(12);
            GDALAllRegister();
            GDALDataset *dataset = (GDALDataset *) GDALOpen(argv[1], GA_ReadOnly);
            if(dataset == NULL)
            {
                std::string message = std::string("Could not open image ") + std::string(argv[1]);
                throw std::exception();
            }
            
            printf( "Driver: %s/%s\n",
                   dataset->GetDriver()->GetDescription(),
                   dataset->GetDriver()->GetMetadataItem( GDAL_DMD_LONGNAME ) );
                        
            printf( "Image Size is %dx%d\n",
                   dataset->GetRasterXSize(), dataset->GetRasterYSize() );
            printf( "Image has %d image bands\n",
                   dataset->GetRasterCount());
            
            double *trans = new double[6];
            if(dataset->GetGeoTransform( trans ) == CE_None )
            {
                printf( "Origin = (%.6f,%.6f)\n", trans[0], trans[3] );
                printf( "Pixel Size = (%.6f,%.6f)\n", trans[1], trans[5] );
            }
            delete[] trans;
            
            if( dataset->GetProjectionRef()  != NULL )
            {
                printf( "Projection is `%s'\n", dataset->GetProjectionRef() );
            }
            
            GDALClose(dataset);
            GDALDestroyDriverManager();
		}
		else
		{
            std::cout << "Run using: gdalsimpleinfo filename.xxx\n";
		}
	}
	catch(std::exception &e)
	{
		std::cerr << "ERROR: " << e.what() << std::endl;
        std::cout << "Run using: gdalsimpleinfo filename.xxx\n";
	}
}

