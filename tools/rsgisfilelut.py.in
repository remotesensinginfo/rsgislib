#!/usr/bin/env python

#############################################
# rsgisfilelut.py
#
#  Copyright 2019 RSGISLib.
#
#  RSGISLib: 'The remote sensing and GIS Software Library'
#
#  RSGISLib is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  RSGISLib is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with RSGISLib.  If not, see <http://www.gnu.org/licenses/>.
#
# Purpose:  Utilities to handle file LUTs. 
#
# Author: Pete Bunting
# Email: petebunting@mac.com
# Date: 28/02/2019
# Version: 1.0
# 
#############################################

import osgeo.gdal as gdal
import osgeo.osr as osr
import osgeo.ogr as ogr

import math
import os.path
import sys
import argparse

def readVecLyr2Mem(vecfile, veclyrname):
    """
A function which reads a vector layer to an OGR in memory layer.

* vecfile - input vector file
* veclyrname - input vector layer within the input file.

returns layer and datasets
"""
    gdal.UseExceptions()
    try:
        vecDS = gdal.OpenEx(vecfile, gdal.OF_READONLY )
        if vecDS is None:
            raise Exception("Could not open '" + vecfile + "'")
        
        veclyr = vecDS.GetLayerByName( veclyrname )
        if veclyr is None:
            raise Exception("Could not find layer '" + veclyrname + "'")
            
        mem_driver = ogr.GetDriverByName('MEMORY')
        
        mem_ds = mem_driver.CreateDataSource('MemSelData')
        mem_lyr = mem_ds.CopyLayer(veclyr, veclyrname, ['OVERWRITE=YES'])
            
    except Exception as e:
        print("Error Vector File: {}".format(vecfile), file=sys.stderr)
        print("Error Vector Layer: {}".format(veclyrname), file=sys.stderr)
        raise e
    return mem_ds, mem_lyr

def subsetEnvsVecLyrObj(lyrVecObj, bbox):
    """
Function to get an ogr vector layer for the defined bounding box. The returned
layer is returned as an in memory ogr Layer object.

* lyrVecObj - OGR Layer Object.
* bbox - region of interest (bounding box). Define as [xMin, xMax, yMin, yMax].

returns OGR Layer and Dataset objects.
"""
    gdal.UseExceptions()
    if lyrVecObj is None:
        raise Exception("Could not find layer '" + vecLyr + "'")
        
    lyr_spatial_ref = lyrVecObj.GetSpatialRef()
    lyrDefn = lyrVecObj.GetLayerDefn()
    
    # Copy the Layer to a new in memory OGR Layer.
    mem_driver = ogr.GetDriverByName('MEMORY')
    mem_result_ds = mem_driver.CreateDataSource('MemResultData')    
    mem_result_lyr = mem_result_ds.CreateLayer("MemResultLyr", lyr_spatial_ref, geom_type=lyrVecObj.GetGeomType())
    for i in range(lyrDefn.GetFieldCount()):
        fieldDefn = lyrDefn.GetFieldDefn(i)
        mem_result_lyr.CreateField(fieldDefn)
        
    openTransaction = False
    trans_step = 20000
    next_trans = trans_step
    nFeats = lyrVecObj.GetFeatureCount(True)
    step = math.floor(nFeats/10)
    feedback = 10
    feedback_next = step
    counter = 0
    print("Started .0.", end='', flush=True)
    outenvs = []
    # loop through the input features
    inFeature = lyrVecObj.GetNextFeature()
    while inFeature:
        if (nFeats>10) and (counter == feedback_next):
            print(".{}.".format(feedback), end='', flush=True)
            feedback_next = feedback_next + step
            feedback = feedback + 10
            
        if not openTransaction:
            mem_result_lyr.StartTransaction()
            openTransaction = True
        
        if inFeature is not None:
            geom = inFeature.GetGeometryRef()
            if geom is not None:
                env = geom.GetEnvelope()
            
                if bbox[0] <= env[1] and bbox[1] >= env[0] and bbox[2] <= env[3] and bbox[3] >= env[2]:
                    mem_result_lyr.CreateFeature(inFeature)
        
        if (counter == next_trans) and openTransaction:
            mem_result_lyr.CommitTransaction()
            openTransaction = False
            next_trans = next_trans + trans_step
        
        inFeature = lyrVecObj.GetNextFeature()
        counter = counter + 1
    print(" Completed")
    
    if openTransaction:
        mem_result_lyr.CommitTransaction()
        openTransaction = False
        
    return mem_result_ds, mem_result_lyr

def getMemVecLyrSubset(vecFile, vecLyr, bbox):
    """
Function to get an ogr vector layer for the defined bounding box. The returned
layer is returned as an in memory ogr Layer object.

* vecFile - vector layer from which the attribute data comes from.
* vecLyr - the layer name from which the attribute data comes from.
* bbox - region of interest (bounding box). Define as [xMin, xMax, yMin, yMax].

returns OGR Layer and Dataset objects.
"""
    gdal.UseExceptions()
    try:
        dsVecFile = gdal.OpenEx(vecFile, gdal.OF_READONLY )
        if dsVecFile is None:
            raise Exception("Could not open '" + vecFile + "'")
        
        lyrVecObj = dsVecFile.GetLayerByName( vecLyr )
        if lyrVecObj is None:
            raise Exception("Could not find layer '" + vecLyr + "'")
            
        mem_result_ds, mem_result_lyr = subsetEnvsVecLyrObj(lyrVecObj, bbox)
        
    except Exception as e:
        print("Error: Layer: {} File: {}".format(vecLyr, vecFile))
        raise e
    return mem_result_ds, mem_result_lyr


def getAttLstSelectFeatsLyrObjs(vecLyrObj, attNames, selVecLyrObj):
    """
Function to get a list of attribute values from features which intersect
with the select layer.
* vecLyrObj - the OGR layer object from which the attribute data comes from.
* attNames - a list of attribute names to be outputted.
* selVecLyrObj - the OGR layer object which will be intersected within the vector file.

returns list of dictionaries with the output values.
"""
    gdal.UseExceptions()
    att_vals = []
    try:
        if vecLyrObj is None:
            raise Exception("The vector layer passed into the function was None.")
        
        if selVecLyrObj is None:
            raise Exception("The select vector layer passed into the function was None.")
        
        lyrDefn = vecLyrObj.GetLayerDefn()
        feat_idxs = dict()
        feat_types= dict()
        found_atts = dict()
        for attName in attNames:
            found_atts[attName] = False
        
        for i in range(lyrDefn.GetFieldCount()):
            if lyrDefn.GetFieldDefn(i).GetName() in attNames:
                attName = lyrDefn.GetFieldDefn(i).GetName()
                feat_idxs[attName] = i
                feat_types[attName] = lyrDefn.GetFieldDefn(i).GetType()
                found_atts[attName] = True
                
        for attName in attNames:
            if not found_atts[attName]:
                raise Exception("Could not find the attribute ({}) specified within the vector layer.".format(attName))
            
        mem_driver = ogr.GetDriverByName('MEMORY')
                
        mem_result_ds = mem_driver.CreateDataSource('MemResultData')
        mem_result_lyr = mem_result_ds.CreateLayer("MemResultLyr", geom_type=vecLyrObj.GetGeomType())
        
        for attName in attNames:
            mem_result_lyr.CreateField(ogr.FieldDefn(attName, feat_types[attName]))
        
        vecLyrObj.Intersection(selVecLyrObj, mem_result_lyr)
        
        # loop through the input features
        reslyrDefn = mem_result_lyr.GetLayerDefn()
        inFeat = mem_result_lyr.GetNextFeature()
        outvals = []
        while inFeat:
            outdict = dict()
            for attName in attNames:
                feat_idx = reslyrDefn.GetFieldIndex(attName)
                if feat_types[attName] == ogr.OFTString:
                    outdict[attName] = inFeat.GetFieldAsString(feat_idx)
                elif feat_types[attName] == ogr.OFTReal:
                    outdict[attName] = inFeat.GetFieldAsDouble(feat_idx)
                elif feat_types[attName] == ogr.OFTInteger:
                    outdict[attName] = inFeat.GetFieldAsInteger(feat_idx)
                else:
                    outdict[attName] = feat.GetField(feat_idx)
            outvals.append(outdict)
            inFeat = mem_result_lyr.GetNextFeature()
        
        mem_result_ds = None
    except Exception as e:
        raise e
    return outvals
    

def getVecLyrsLst(vecFile):
    """
    A function which returns a list of available layers within the inputted vector file.
    
    * vecFile - file name and path to input vector layer.
    returns: list of layer names (can be used with gdal.Dataset.GetLayerByName()).
    """
    gdalDataset = gdal.OpenEx(vecFile, gdal.OF_VECTOR )
    layerList = []
    for lyr_idx in range(gdalDataset.GetLayerCount()):
        lyr = gdalDataset.GetLayerByIndex(lyr_idx)
        tLyrName = lyr.GetName()
        if not tLyrName in layerList:
            layerList.append(tLyrName)
    gdalDataset = None
    return layerList
    
def writeList2File(dataList, outFile):
    """
    Write a list a text file, one line per item.
    """
    try:
        f = open(outFile, 'w')
        for item in dataList:
           f.write(str(item)+'\n')
        f.flush()
        f.close()
    except Exception as e:
        raise e


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-i", "--lutfile", type=str, required=True, help="Input LUT vector file.")
    parser.add_argument("--lutlyr", type=str, required=False, help="Input LUT vector file layer.")
    parser.add_argument("--lyrs", action='store_true', default=False, help="List the layers in LUT file.")
    parser.add_argument("--roifile", type=str, required=False, help="Input vector file defining the region of interest.")
    parser.add_argument("--roilyr", type=str, required=False, help="Input vector file layer defining the region of interest.")
    parser.add_argument("--intersect", action='store_true', default=False, help="List files from the LUT which intersect with the ROI.")
    parser.add_argument("--dest", type=str, required=False, help="Output destination (file or folder) depending on the option specified.")
    parser.add_argument("-o", "--outputfile", type=str, required=False, help="Output file for commands.")
    parser.add_argument("--targz", action='store_true', default=False, help="Create tar command to archive the list of files.")
    parser.add_argument("--copy", action='store_true', default=False, help="Create copy command for the list of files.")
    args = parser.parse_args()
    
    if args.lyrs:
        veclyrs = getVecLyrsLst(args.lutfile)
        i = 1
        for lyr in veclyrs:
            print("{0}: {1}".format(i, lyr))
            i = i + 1
            
    lutlyr = args.lutlyr
    if args.lutlyr is None:
        lutlyr = os.path.splitext(os.path.basename(args.lutfile))[0]
    
    if args.intersect:
        if args.roifile is None:
            raise Exception("If --intersect is specified then an roi file must be specified.")
            
        roilyr = args.roilyr
        if args.roilyr is None:
            roilyr = os.path.splitext(os.path.basename(args.roifile))[0]
                
        roi_mem_ds, roi_mem_lyr = readVecLyr2Mem(args.roifile, roilyr)
        
        roi_bbox = roi_mem_lyr.GetExtent(True)
        
        lut_mem_ds, lut_mem_lyr = getMemVecLyrSubset(args.lutfile, lutlyr, roi_bbox)
        
        fileListDict = getAttLstSelectFeatsLyrObjs(lut_mem_lyr, ['path','filename'], roi_mem_lyr)
        
        if args.targz:
            if args.dest is None:
                raise Exception("Must provide a destination (--dest) with --targz option.")
            
            cmd = 'tar -czf ' + args.dest
            for fileItem in fileListDict:
                filepath = os.path.join(fileItem['path'], fileItem['filename'])
                cmd = cmd + " " + filepath
            
            if args.outputfile is None:
                print(cmd)
            else:
                writeList2File([cmd], args.outputfile)
        elif args.copy:
            if args.dest is None:
                raise Exception("Must provide a destination (--dest) with --copy option.")
                
            cmds = []
            for fileItem in fileListDict:
                filepath = os.path.join(fileItem['path'], fileItem['filename'])
                cmds.append("cp {0} {1}".format(filepath, args.dest))
            
            if args.outputfile is None:
                for cmd in cmds:
                    print(cmd)
            else:
                writeList2File(cmds, args.outputfile)
        else:
            if args.outputfile is None:
                files = []
                for fileItem in fileListDict:
                    filepath = os.path.join(fileItem['path'], fileItem['filename'])
                    files.append(filepath)
                
            if args.outputfile is None:
                for filepath in files:
                    print(filepath)
            else:
                writeList2File(files, args.outputfile)
                
            
        
