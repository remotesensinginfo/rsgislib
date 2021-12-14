/*
 *  RSGISVectorUtils.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 29/04/2008.
 *  Copyright 2008 RSGISLib. All rights reserved.
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

#include "RSGISVectorUtils.h"


namespace rsgis{namespace vec{
	
        std::string RSGISVectorUtils::getLayerName(std::string filepath)
        {
            int strSize = filepath.size();
            int lastSlash = 0;
            for(int i = 0; i < strSize; i++)
            {
                if(filepath.at(i) == '/')
                {
                    lastSlash = i;
                }
            }
            std::string filename = filepath.substr(lastSlash+1);

            strSize = filename.size();
            int lastpt = 0;
            for(int i = 0; i < strSize; i++)
            {
                if(filename.at(i) == '.')
                {
                    lastpt = i;
                }
            }

            std::string layerName = filename.substr(0, lastpt);
            return layerName;
        }

        bool RSGISVectorUtils::checkDIR4SHP(std::string dir, std::string shp)
        {
            rsgis::utils::RSGISFileUtils fileUtils;
            std::string *dirList = NULL;
            int numFiles = 0;
            bool returnVal = false;

            try
            {
                dirList = fileUtils.getFilesInDIRWithName(dir, shp, &numFiles);
                if(numFiles > 0)
                {
                    for(int i = 0; i < numFiles; i++)
                    {
                        if(fileUtils.getExtension(dirList[i]) == ".shp")
                        {
                            returnVal = true;
                        }
                    }
                }
            }
            catch(RSGISException &e)
            {
                std::cout << e.what() << std::endl;
                throw RSGISVectorException(e.what());
            }
            delete[] dirList;

            return returnVal;
        }

        void RSGISVectorUtils::deleteSHP(std::string dir, std::string shp)
        {
            rsgis::utils::RSGISFileUtils fileUtils;
            std::string *dirList = NULL;
            int numFiles = 0;

            try
            {
                dirList = fileUtils.getFilesInDIRWithName(dir, shp, &numFiles);
                if(numFiles > 0)
                {
                    std::cout << "Deleting shapefile...\n";
                    for(int i = 0; i < numFiles; i++)
                    {
                        if(fileUtils.getExtension(dirList[i]) == ".shp")
                        {
                            std::cout << dirList[i];
                            if( fileUtils.removeFileIfPresent(dirList[i]) )
                            {
                                throw RSGISException("Could not delete file.");
                            }
                            std::cout << " deleted\n";
                        }
                        else if(fileUtils.getExtension(dirList[i]) == ".shx")
                        {
                            std::cout << dirList[i];
                            if( fileUtils.removeFileIfPresent(dirList[i]) )
                            {
                                throw RSGISException("Could not delete file.");
                            }
                            std::cout << " deleted\n";
                        }
                        else if(fileUtils.getExtension(dirList[i]) == ".sbx")
                        {
                            std::cout << dirList[i];
                            if( fileUtils.removeFileIfPresent(dirList[i]) )
                            {
                                throw RSGISException("Could not delete file.");
                            }
                            std::cout << " deleted\n";
                        }
                        else if(fileUtils.getExtension(dirList[i]) == ".sbn")
                        {
                            std::cout << dirList[i];
                            if( fileUtils.removeFileIfPresent(dirList[i]) )
                            {
                                throw RSGISException("Could not delete file.");
                            }
                            std::cout << " deleted\n";
                        }
                        else if(fileUtils.getExtension(dirList[i]) == ".dbf")
                        {
                            std::cout << dirList[i];
                            if( fileUtils.removeFileIfPresent(dirList[i]) )
                            {
                                throw RSGISException("Could not delete file.");
                            }
                            std::cout << " deleted\n";
                        }
                        else if(fileUtils.getExtension(dirList[i]) == ".prj")
                        {
                            std::cout << dirList[i];
                            if( fileUtils.removeFileIfPresent(dirList[i]) )
                            {
                                throw RSGISException("Could not delete file.");
                            }
                            std::cout << " deleted\n";
                        }
                    }
                }
            }
            catch(RSGISException &e)
            {
                throw RSGISVectorException(e.what());
            }
            delete[] dirList;
        }

        OGRPolygon* RSGISVectorUtils::createOGRPolygon(double tlX, double tlY, double brX, double brY)
        {
            OGRPolygon *ogrPoly = new OGRPolygon();

            OGRLinearRing *ogrRing = new OGRLinearRing();
            ogrRing->addPoint(tlX, tlY, 0);
            ogrRing->addPoint(brX, tlY, 0);
            ogrRing->addPoint(brX, brY, 0);
            ogrRing->addPoint(tlX, brY, 0);
            ogrRing->addPoint(tlX, tlY, 0);
            ogrPoly->addRingDirectly(ogrRing);

            return ogrPoly;
        }

        OGRPolygon* RSGISVectorUtils::checkCloseOGRPolygon(OGRPolygon *poly)
        {
            OGRPolygon *ogrPoly = new OGRPolygon();
            OGRLinearRing *ogrRing = new OGRLinearRing();
            OGRLinearRing *exteriorRing = poly->getExteriorRing();

            int numPoints = exteriorRing->getNumPoints();

            if(numPoints < 4)
            {
                throw RSGISVectorException("Too few point, need to have at least 4 points to make a polygon.");
            }

            OGRPoint *pt = new OGRPoint();
            OGRPoint *pt0 = new OGRPoint();
            for(int i = 0; i < numPoints; i++)
            {
                exteriorRing->getPoint(i, pt);

                if(i == 0)
                {
                    pt0->setX(pt->getX());
                    pt0->setY(pt->getY());
                    pt0->setZ(pt->getZ());
                }
                ogrRing->addPoint(pt->getX(), pt->getY(), pt->getZ());
            }

            if(pt != pt0)
            {
                ogrRing->addPoint(pt0->getX(), pt0->getY(), pt0->getZ());
            }

            delete pt;
            delete pt0;

            ogrPoly->addRingDirectly(ogrRing);

            return ogrPoly;

        }

        OGRPolygon* RSGISVectorUtils::removeHolesOGRPolygon(OGRPolygon *poly)
        {
            OGRPolygon *ogrPoly = new OGRPolygon();
            OGRLinearRing *ogrRing = new OGRLinearRing();
            OGRLinearRing *exteriorRing = poly->getExteriorRing();

            int numPoints = exteriorRing->getNumPoints();

            if(numPoints < 4)
            {
                throw RSGISVectorException("Too few point, need to have at least 4 points to make a polygon.");
            }

            OGRPoint *pt = new OGRPoint();
            OGRPoint *pt0 = new OGRPoint();
            for(int i = 0; i < numPoints; i++)
            {
                exteriorRing->getPoint(i, pt);

                if(i == 0)
                {
                    pt0->setX(pt->getX());
                    pt0->setY(pt->getY());
                    pt0->setZ(pt->getZ());
                }
                ogrRing->addPoint(pt->getX(), pt->getY(), pt->getZ());
            }

            if(pt != pt0)
            {
                ogrRing->addPoint(pt0->getX(), pt0->getY(), pt0->getZ());
            }

            delete pt;
            delete pt0;

            ogrPoly->addRingDirectly(ogrRing);

            return ogrPoly;

        }

        OGRPolygon* RSGISVectorUtils::removeHolesOGRPolygon(OGRPolygon *poly, float areaThreshold)
        {
            OGRPolygon *ogrPoly = new OGRPolygon();
            OGRLinearRing *ogrRing = new OGRLinearRing();
            OGRLinearRing *exteriorRing = poly->getExteriorRing();

            int numPoints = exteriorRing->getNumPoints();

            if(numPoints < 4)
            {
                throw RSGISVectorException("Too few point, need to have at least 4 points to make a polygon.");
            }

            OGRPoint *pt = new OGRPoint();
            OGRPoint *pt0 = new OGRPoint();
            for(int i = 0; i < numPoints; i++)
            {
                exteriorRing->getPoint(i, pt);

                if(i == 0)
                {
                    pt0->setX(pt->getX());
                    pt0->setY(pt->getY());
                    pt0->setZ(pt->getZ());
                }
                ogrRing->addPoint(pt->getX(), pt->getY(), pt->getZ());
            }

            if(pt != pt0)
            {
                ogrRing->addPoint(pt0->getX(), pt0->getY(), pt0->getZ());
            }
            ogrPoly->addRingDirectly(ogrRing);


            if(poly->getNumInteriorRings() > 0)
            {
                const OGRLinearRing *tmpRing = NULL;
                int numHoles = poly->getNumInteriorRings();
                for(int n = 0; n < numHoles; ++n)
                {
                    tmpRing = poly->getInteriorRing(n);
                    if(tmpRing->get_Area() > areaThreshold)
                    {
                        OGRLinearRing *ogrIntRing = new OGRLinearRing();
                        int numIntPoints = tmpRing->getNumPoints();

                        if(numIntPoints < 4)
                        {
                            throw RSGISVectorException("Too few point, need to have at least 4 points to make a polygon.");
                        }

                        for(int i = 0; i < numIntPoints; i++)
                        {
                            tmpRing->getPoint(i, pt);

                            if(i == 0)
                            {
                                pt0->setX(pt->getX());
                                pt0->setY(pt->getY());
                                pt0->setZ(pt->getZ());
                            }
                            ogrIntRing->addPoint(pt->getX(), pt->getY(), pt->getZ());
                        }

                        if(pt != pt0)
                        {
                            ogrIntRing->addPoint(pt0->getX(), pt0->getY(), pt0->getZ());
                        }
                        ogrPoly->addRingDirectly(ogrIntRing);
                    }
                }
            }



            delete pt;
            delete pt0;

            return ogrPoly;

        }

        OGRPolygon* RSGISVectorUtils::moveOGRPolygon(OGRPolygon *poly, double shiftX, double shiftY, double shiftZ)
        {
            OGRPolygon *ogrPoly = new OGRPolygon();
            OGRLinearRing *ogrRing = new OGRLinearRing();
            OGRLinearRing *exteriorRing = poly->getExteriorRing();

            int numPoints = exteriorRing->getNumPoints();

            if(numPoints < 4)
            {
                throw RSGISVectorException("Too few point, need to have at least 4 points to make a polygon.");
            }

            OGRPoint *pt = new OGRPoint();
            for(int i = 0; i < numPoints; i++)
            {
                exteriorRing->getPoint(i, pt);
                ogrRing->addPoint(pt->getX() + shiftX, pt->getY() + shiftY, pt->getZ() + shiftZ);
            }

            delete pt;

            ogrPoly->addRingDirectly(ogrRing);

            return ogrPoly;
        }

        std::vector<std::string>* RSGISVectorUtils::findUniqueVals(OGRLayer *layer, std::string attribute)
        {
            std::vector<std::string>::iterator iterVals;
            std::vector<std::string> *values = new std::vector<std::string>();

            OGRFeature *feature = NULL;
            OGRFeatureDefn *featureDefn = layer->GetLayerDefn();

            bool found = false;

            layer->ResetReading();
            while( (feature = layer->GetNextFeature()) != NULL )
            {
                std::string attrVal = feature->GetFieldAsString(featureDefn->GetFieldIndex(attribute.c_str()));

                found = false;
                for(iterVals = values->begin(); iterVals != values->end(); ++iterVals)
                {
                    if((*iterVals) == attrVal)
                    {
                        found = true;
                        break;
                    }
                }

                if(!found)
                {
                    values->push_back(attrVal);
                }

                OGRFeature::DestroyFeature(feature);
            }


            return values;
        }


        std::vector<std::string>* RSGISVectorUtils::getColumnNames(OGRLayer *layer)
        {
            std::vector<std::string> *colNames = new std::vector<std::string>();
            try
            {
                OGRFeatureDefn *ogrFeatDef = layer->GetLayerDefn();
                int numFields = ogrFeatDef->GetFieldCount();
                for(int i = 0; i < numFields; ++i)
                {
                    OGRFieldDefn *fieldDef = ogrFeatDef->GetFieldDefn(i);
                    colNames->push_back(std::string(fieldDef->GetNameRef()));
                }

            }
            catch (RSGISVectorException &e)
            {
                throw e;
            }
            return colNames;
        }

        std::vector<std::string> RSGISVectorUtils::getColumnNamesLitVec(OGRLayer *layer)
        {
            std::vector<std::string> colNames = std::vector<std::string>();
            try
            {
                OGRFeatureDefn *ogrFeatDef = layer->GetLayerDefn();
                int numFields = ogrFeatDef->GetFieldCount();
                for(int i = 0; i < numFields; ++i)
                {
                    OGRFieldDefn *fieldDef = ogrFeatDef->GetFieldDefn(i);
                    colNames.push_back(std::string(fieldDef->GetNameRef()));
                }

            }
            catch (RSGISVectorException &e)
            {
                throw e;
            }
            return colNames;
        }



        std::vector<OGRPoint*>* RSGISVectorUtils::getRegularStepPoints(std::vector<OGRLineString*> *lines, double step)
        {
            std::vector<OGRPoint*> *pts = new std::vector<OGRPoint*>();
            try
            {
                long numPts = 0;
                OGRPoint *pt = NULL;
                for(std::vector<OGRLineString*>::iterator iterLines = lines->begin(); iterLines != lines->end(); ++iterLines)
                {
                    (*iterLines)->segmentize(step);
                    numPts = (*iterLines)->getNumPoints();
                    for(long i = 0; i < numPts; ++i)
                    {
                        pt = new OGRPoint();
                        (*iterLines)->getPoint(i, pt);
                        pts->push_back(pt);
                    }
                }

            }
            catch (RSGISVectorException &e)
            {
                throw e;
            }
            return pts;
        }


        OGREnvelope* RSGISVectorUtils::getEnvelope(OGRGeometry *geom)
        {
            OGREnvelope *env = new OGREnvelope();
            geom->getEnvelope(env);
            return env;
        }

        OGREnvelope* RSGISVectorUtils::getEnvelopePixelBuffer(OGRGeometry *geom, double imageRes)
        {
            /// Gets the envelope of an OGRGeometry buffered by one pixel.
            // When rasterising small polygons, getEnvelope can return an envelope that is smaller than a pixel.
            // This class buffers the envelope by a 1/2 a pixel to ensure the envelope covers at least 1 pixel

            OGREnvelope *env = new OGREnvelope();
            geom->getEnvelope(env);

            double buffer = imageRes / 2;

            env->MinX = env->MinX - buffer;
            env->MaxX = env->MaxX + buffer;
            env->MinY = env->MinY - buffer;
            env->MaxY = env->MaxY + buffer;

            return env;
        }
	
}}
