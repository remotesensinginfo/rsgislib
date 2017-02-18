/*
 *  RSGISHistoCubeFileIO.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 17/02/2017.
 *
 *  Copyright 2017 RSGISLib.
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



#include "RSGISHistoCubeFileIO.h"


namespace rsgis {namespace histocube{
    

    RSGISHistoCubeFile::RSGISHistoCubeFile()
    {
        fileOpen = false;
        rwAccess = false;
        H5::Exception::dontPrint();
    }
    
    void RSGISHistoCubeFile::openFile(std::string filePath, bool fileRWAccess, int mdcElmts, hsize_t rdccNElmts, hsize_t rdccNBytes, double rdccW0, hsize_t sieveBuf, hsize_t metaBlockSize)throw(rsgis::RSGISHistoCubeException)
    {
        try
        {
            this->rwAccess = fileRWAccess;
            H5::FileAccPropList hcAccessPlist = H5::FileAccPropList(H5::FileAccPropList::DEFAULT);
            hcAccessPlist.setCache(mdcElmts, rdccNElmts, rdccNBytes, rdccW0);
            hcAccessPlist.setSieveBufSize(sieveBuf);
            hcAccessPlist.setMetaBlockSize(metaBlockSize);
            
            const H5std_string h5FilePath(filePath);
            if( fileRWAccess )
            {
                hcH5File = new H5::H5File(h5FilePath, H5F_ACC_RDWR, H5::FileCreatPropList::DEFAULT, hcAccessPlist);
                fileOpen = true;
            }
            else
            {
                hcH5File = new H5::H5File(h5FilePath, H5F_ACC_RDONLY, H5::FileCreatPropList::DEFAULT, hcAccessPlist);
                fileOpen = true;
            }
            
            // Read metadata into file object.
            
        }
        catch( H5::FileIException &e )
        {
            throw rsgis::RSGISHistoCubeException(e.getCDetailMsg());
        }
        catch( H5::DataSetIException &e )
        {
            throw rsgis::RSGISHistoCubeException(e.getCDetailMsg());
        }
        catch( H5::DataSpaceIException &e )
        {
            throw rsgis::RSGISHistoCubeException(e.getCDetailMsg());
        }
        catch( H5::DataTypeIException &e )
        {
            throw rsgis::RSGISHistoCubeException(e.getCDetailMsg());
        }
        catch ( rsgis::RSGISHistoCubeException &e)
        {
            throw e;
        }
        catch ( std::exception &e)
        {
            throw rsgis::RSGISHistoCubeException(e.what());
        }

    }
    
    void RSGISHistoCubeFile::createNewFile(std::string filePath, int mdcElmts, hsize_t rdccNElmts, hsize_t rdccNBytes, double rdccW0, hsize_t sieveBuf, hsize_t metaBlockSize) throw(rsgis::RSGISHistoCubeException)
    {
        try
        {
            this->rwAccess = true;
            H5::FileAccPropList hcAccessPlist = H5::FileAccPropList(H5::FileAccPropList::DEFAULT);
            hcAccessPlist.setCache(mdcElmts, rdccNElmts, rdccNBytes, rdccW0);
            hcAccessPlist.setSieveBufSize(sieveBuf);
            hcAccessPlist.setMetaBlockSize(metaBlockSize);
            
            // CREATE THE HDF FILE - EXISTING FILE WILL BE TRUNCATED
            hcH5File = new H5::H5File( filePath, H5F_ACC_TRUNC, H5::FileCreatPropList::DEFAULT, hcAccessPlist);
            fileOpen = true;
            
            //////////// CREATE GLOBAL HEADER ////////////////
            H5::StrType strType(H5::PredType::C_S1, H5T_VARIABLE);
            H5::DataSpace attSpace(H5S_SCALAR);
            H5::Attribute fileTypeAtt = hcH5File->createAttribute( HC_FILE_ATT_FILETYPE, strType, attSpace);
            const H5std_string strFileTypeVal ("RSGISLibHistoCube");
            fileTypeAtt.write(strType, strFileTypeVal);
            fileTypeAtt.close();
            
            H5::Attribute fileVersionAtt = hcH5File->createAttribute( HC_FILE_ATT_VERSION, strType, attSpace);
            const H5std_string strVersionVal ("1.0");
            fileVersionAtt.write(strType, strVersionVal);
            fileVersionAtt.close();
            
            hcH5File->createGroup( HC_DATASETNAME_DATA );
            hcH5File->createGroup( HC_DATASETNAME_METADATA );
        }
        catch( H5::FileIException &e )
        {
            throw rsgis::RSGISHistoCubeException(e.getCDetailMsg());
        }
        catch( H5::DataSetIException &e )
        {
            throw rsgis::RSGISHistoCubeException(e.getCDetailMsg());
        }
        catch( H5::DataSpaceIException &e )
        {
            throw rsgis::RSGISHistoCubeException(e.getCDetailMsg());
        }
        catch( H5::DataTypeIException &e )
        {
            throw rsgis::RSGISHistoCubeException(e.getCDetailMsg());
        }
        catch ( rsgis::RSGISHistoCubeException &e)
        {
            throw e;
        }
        catch ( std::exception &e)
        {
            throw rsgis::RSGISHistoCubeException(e.what());
        }
    }
    
    void RSGISHistoCubeFile::createDataset(std::string name, unsigned int lowRange, unsigned int upRange, int scale, int offset) throw(rsgis::RSGISHistoCubeException)
    {
        
    }
    
    void RSGISHistoCubeFile::closeFile() throw(rsgis::RSGISHistoCubeException)
    {
        this->hcH5File->close();
        delete this->hcH5File;
        this->hcH5File = NULL;
        this->fileOpen = false;
    }
    
    RSGISHistoCubeFile::~RSGISHistoCubeFile()
    {
        if(this->fileOpen)
        {
            this->closeFile();
        }
    }
    
    
}}



