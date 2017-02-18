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
        this->numOfFeats = 0;
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
            
            // Check file is the correct type
            H5std_string strFileTypeReadVal("");
            H5::StrType strType(H5::PredType::C_S1, H5T_VARIABLE);
            try
            {
                H5::Attribute fileTypeAttribute = hcH5File->openAttribute(HC_FILE_ATT_FILETYPE);
                fileTypeAttribute.read(strType, &strFileTypeReadVal);
                fileTypeAttribute.close();
            }
            catch ( H5::Exception &e)
            {
                throw rsgis::RSGISHistoCubeException("Could not read the file type attribute.");
            }
            std::string inFileType = std::string(strFileTypeReadVal.c_str());
            if(inFileType != "RSGISLibHistoCube")
            {
                throw rsgis::RSGISHistoCubeException("Input file is not of type 'RSGISLibHistoCube'.");
            }
            
            // Check Version Number
            H5std_string strVersionReadVal("");
            try
            {
                H5::Attribute fileVersionAttribute = hcH5File->openAttribute(HC_FILE_ATT_VERSION);
                fileVersionAttribute.read(strType, &strVersionReadVal);
                fileVersionAttribute.close();
            }
            catch ( H5::Exception &e)
            {
                throw rsgis::RSGISHistoCubeException("Could not read the file type attribute.");
            }
            std::string inFileVersion = std::string(strVersionReadVal.c_str());
            if(inFileVersion != "1.0")
            {
                throw rsgis::RSGISHistoCubeException("Input file is not a reconsigned version.");
            }
            
            // Read metadata from file object.
            try
            {
                // Read the number of features.
                unsigned long numFeatSize[1];
                hsize_t dimsValue[1];
                dimsValue[0] = 1;
                H5::DataSpace valueDataSpace(1, dimsValue);
                H5::DataSet datasetAttSize = hcH5File->openDataSet( HC_NUM_OF_FEATS );
                datasetAttSize.read(numFeatSize, H5::PredType::NATIVE_ULONG, valueDataSpace);
                datasetAttSize.close();
                valueDataSpace.close();
                
                this->numOfFeats = numFeatSize[0];
            }
            catch (H5::Exception &e)
            {
                throw rsgis::RSGISHistoCubeException("The dataset for the number of features within the file is not present.");
            }
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
    
    void RSGISHistoCubeFile::createNewFile(std::string filePath, unsigned long numFeats, int mdcElmts, hsize_t rdccNElmts, hsize_t rdccNBytes, double rdccW0, hsize_t sieveBuf, hsize_t metaBlockSize) throw(rsgis::RSGISHistoCubeException)
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
            
            unsigned long numFeatsObj[] = { numFeats };
            hsize_t numFeatsSize[] = { 1 };
            H5::DataSpace numFeatsDataSpace(1, numFeatsSize);
            H5::DataSet cubeNumFeatsDataset = hcH5File->createDataSet(HC_NUM_OF_FEATS, H5::PredType::STD_U64LE, numFeatsDataSpace);
            cubeNumFeatsDataset.write( numFeatsObj, H5::PredType::NATIVE_ULONG );
            cubeNumFeatsDataset.close();
            numFeatsDataSpace.close();
            
            this->numOfFeats = numFeats;
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
    
    void RSGISHistoCubeFile::createDataset(std::string name, std::vector<int> bins, float scale, float offset) throw(rsgis::RSGISHistoCubeException)
    {
        try
        {
            if(!this->fileOpen)
            {
                throw rsgis::RSGISHistoCubeException("HCF file is not open.");
            }
            
            if(!this->rwAccess)
            {
                throw rsgis::RSGISHistoCubeException("HCF file has been openned in read only mode.");
            }
            
            
            
            
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



