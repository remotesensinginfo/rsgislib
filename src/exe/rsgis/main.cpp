/*
 *  main.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 03/12/2008.
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
 */

#include <iostream>
#include <string>
#include <time.h>

#include "common/rsgis-config.h"

#include "common/RSGISCommandLineParser.h"
#include "common/RSGISCommandLineException.h"
#include "common/RSGISException.h"
#include "common/RSGISParseXMLArguments.h"
#include "common/RSGISAlgorithmParameters.h"
#include "common/RSGISXMLArgumentsException.h"
#include "common/RSGISAlgorParamsFactory.h"

#include "utils/RSGISFileUtils.h"
#include "utils/RSGISTextUtils.h"

#include "RSGISExeStackBands.h"
#include "RSGISExeSARSaatchiBiomass.h"
#include "RSGISExeSARBayesianBiomass.h"
#include "RSGISExeFFTUtils.h"
#include "RSGISExeCreateTestImages.h"
#include "RSGISExeMathsUtilities.h"
#include "RSGISExeZonalStats.h"
#include "RSGISExeClassification.h"
#include "RSGISExeImageUtils.h"
#include "RSGISExeImageCalculation.h"
#include "RSGISExeImageConversion.h"
#include "RSGISExeFilterImages.h"
#include "RSGISExeVectorUtils.h"
#include "RSGISExeFitting.h"
#include "RSGISExeEstimationAlgorithm.h"
#include "RSGISExePostClassification.h"
#include "RSGISExeRadarUtils.h"
#include "RSGISExeVisualisation.h"
#include "RSGISExeCommandLine.h"
#include "RSGISExeTransectModel.h"
#include "RSGISExeSegment.h"
#include "RSGISExeImageRegistration.h"
#include "RSGISExeImageCalibration.h"
#include "RSGISExeElevationDataTools.h"
//#include "RSGISExeRasterGIS_old.h"
#include "RSGISExeImageMorphology.h"
//#include "RSGISExeRasterGIS.h"

class Control
	{
	public:
		Control() throw(rsgis::RSGISException);
		void runXML(std::string xmlFile);
		void printParameters(std::string xmlFile);
		void listAlgorithms();
        void createBlankXMLFile(std::string xmlFile);
		void help(std::string algor);
		void help();
        void versionInfo();
		~Control();
	private:
		void setUpAlgorithmFactory() throw(rsgis::RSGISException);
        rsgis::RSGISAlgorParamsFactory *factory;
	};


Control::Control() throw(rsgis::RSGISException)
{
	this->setUpAlgorithmFactory();
}

void Control::setUpAlgorithmFactory() throw(rsgis::RSGISException)
{
	std::vector<rsgis::RSGISAlgorithmParameters*> *algorParams = new std::vector<rsgis::RSGISAlgorithmParameters*>();

	algorParams->push_back(new rsgisexe::RSGISExeImageUtils());
	algorParams->push_back(new rsgisexe::RSGISExeStackBands());
	algorParams->push_back(new rsgisexe::RSGISExeImageCalculation());
	algorParams->push_back(new rsgisexe::RSGISExeImageConversion());
	algorParams->push_back(new rsgisexe::RSGISExeClassification());
	algorParams->push_back(new rsgisexe::RSGISExeCreateTestImages());
	algorParams->push_back(new rsgisexe::RSGISExeFilterImages());
	algorParams->push_back(new rsgisexe::RSGISExeFFTUtils());
	algorParams->push_back(new rsgisexe::RSGISExeVectorUtils());
	algorParams->push_back(new rsgisexe::RSGISExeMathsUtilities());
	algorParams->push_back(new rsgisexe::RSGISExeFitting());
	algorParams->push_back(new rsgisexe::RSGISExeSARSaatchiBiomass());
	algorParams->push_back(new rsgisexe::RSGISExeSARBayesianBiomass());
	algorParams->push_back(new rsgisexe::RSGISExeEstimationAlgorithm());
	algorParams->push_back(new rsgisexe::RSGISExeRadarUtils());
	algorParams->push_back(new rsgisexe::RSGISExePostClassification());
	algorParams->push_back(new rsgisexe::RSGISExeVisualisation());
	algorParams->push_back(new rsgisexe::RSGISExeCommandLine());
	algorParams->push_back(new rsgisexe::RSGISExeTransectModel());
	algorParams->push_back(new rsgisexe::RSGISExeSegment());
	algorParams->push_back(new rsgisexe::RSGISExeImageRegistration());
	algorParams->push_back(new rsgisexe::RSGISExeZonalStats());
    algorParams->push_back(new rsgisexe::RSGISExeImageCalibration());
    algorParams->push_back(new rsgisexe::RSGISExeElevationDataTools());
    //algorParams->push_back(new rsgisexe::RSGISExeRasterGIS_old());
    algorParams->push_back(new rsgisexe::RSGISExeImageMorphology());
    //algorParams->push_back(new rsgisexe::RSGISExeRasterGIS());

	factory = new rsgis::RSGISAlgorParamsFactory(algorParams);
}

void Control::runXML(std::string xmlFile)
{
    rsgis::utils::RSGISFileUtils fileUtils;
	rsgis::RSGISAlgorithmParameters **algorParams = NULL;
	int numCommands = 0;
	time_t rawStartTime = 0;
	struct tm *timeInfo = 0;
	time_t rawFinishTime = 0;
	std::string formatedTime = "";
	double timeDiff = 0;
	try
	{
		if(fileUtils.checkFilePresent(xmlFile))
		{
			rsgis::RSGISParseXMLArguments parseXMLArguments = rsgis::RSGISParseXMLArguments(xmlFile);

			algorParams = parseXMLArguments.parseArguments(&numCommands, factory);
			if (numCommands == 1)
			{
				std::cout << "There is " << numCommands << " command to be run:\n";
			}
			else
			{
				std::cout << "There are " << numCommands << " commands to be run:\n";
			}
			for(int i = 0; i < numCommands; i++)
			{
				time(&rawStartTime);
				timeInfo = localtime(&rawStartTime);
				formatedTime = std::string(asctime(timeInfo));
				formatedTime = formatedTime.substr(0, (formatedTime.length()-1));

				std::cout << "\n[" << i + 1 << "/" << numCommands <<"] Running Command: " << algorParams[i]->getAlgorithm() << " (Start Time: " << formatedTime << ") " << "..." << std::endl;
				algorParams[i]->runAlgorithm();

				std::cout << "Algorithm Completed in " << std::flush;
				time(&rawFinishTime);
				timeDiff = difftime (rawFinishTime,rawStartTime);
				if (timeDiff == 0)
				{
					std::cout << "less than a second\n";
				}
				else if(timeDiff > 3600)
				{
					timeDiff = timeDiff/3600;
					std::cout << timeDiff << " hours\n";
				}
				else if(timeDiff > 60)
				{
					timeDiff = timeDiff/60;
					std::cout << timeDiff << " minutes\n";
				}
				else
				{
					std::cout << timeDiff << " seconds\n";
				}
			}
			time(&rawFinishTime);
			timeInfo = localtime(&rawFinishTime);
			formatedTime = std::string(asctime(timeInfo));
			formatedTime = formatedTime.substr(0, (formatedTime.length()-1));
			std::cout << "Finished (Time: " << formatedTime << ")\n";

			//if(timeInfo != NULL)
			//{
			//	delete timeInfo;
			//}
		}
		else
		{
			throw rsgis::RSGISXMLArgumentsException("Input XML file is not present or accessable");
		}

	}
	catch(rsgis::RSGISXMLArgumentsException &e)
	{
		time(&rawStartTime);
		timeInfo = localtime(&rawStartTime);
		formatedTime = std::string(asctime(timeInfo));
		formatedTime = formatedTime.substr(0, (formatedTime.length()-1));

		std::cerr << "ERROR: " << e.what() << std::endl;
		std::cerr << "Error Occurred on " << formatedTime << "\n";
	}
	catch(rsgis::RSGISException &e)
	{
		time(&rawStartTime);
		timeInfo = localtime(&rawStartTime);
		formatedTime = std::string(asctime(timeInfo));
		formatedTime = formatedTime.substr(0, (formatedTime.length()-1));

		std::cerr << "ERROR: " << e.what() << std::endl;
		std::cerr << "Error Occurred on " << formatedTime << "\n";
	}



	if(algorParams != NULL)
	{
		for(int i = 0; i < numCommands; i++)
		{
			delete algorParams[i];
		}
		delete[] algorParams;
	}
}

void Control::printParameters(std::string xmlFile)
{
    rsgis::utils::RSGISFileUtils fileUtils;
    rsgis::RSGISAlgorithmParameters **algorParams = NULL;
	int numCommands = 0;
	try
	{
		if(fileUtils.checkFilePresent(xmlFile))
		{
            rsgis::RSGISParseXMLArguments parseXMLArguments = rsgis::RSGISParseXMLArguments(xmlFile);
			algorParams = parseXMLArguments.parseArguments(&numCommands, factory);
			std::cout << "There are " << numCommands << " commands to be printed:\n";
			for(int i = 0; i < numCommands; i++)
			{
				algorParams[i]->printParameters();
			}
			std::cout << "Finished\n";
		}
		else
		{
			std::cerr << "ERROR: Input XML file is not present or accessable\n";
		}
	}
	catch(rsgis::RSGISException& e)
	{
		std::cerr << "ERROR: " << e.what() << std::endl;
	}

	if(algorParams != NULL)
	{
		for(int i = 0; i < numCommands; i++)
		{
			delete algorParams[i];
		}
		delete[] algorParams;
	}
}

void Control::listAlgorithms()
{
	std::string *algorithms = NULL;
	int numAlgorithms = 0;
	try
	{
		algorithms = factory->availableAlgorithms(&numAlgorithms);
		std::cout << "There are " << numAlgorithms << " available:\n";
		for(int i = 0; i < numAlgorithms; i++)
		{
			std::cout << i << ": " << algorithms[i] << std::endl;
		}
	}
	catch(rsgis::RSGISException& e)
	{
		std::cerr << "ERROR: " << e.what() << std::endl;
	}

	if(algorithms != NULL)
	{
		delete[] algorithms;
	}
}

void Control::createBlankXMLFile(std::string xmlFile)
{
    try
    {
        struct tm *timeInfo = 0;
        time_t rawStartTime = 0;

        time(&rawStartTime);
        timeInfo = localtime(&rawStartTime);
        std::string formatedTime = std::string(asctime(timeInfo));
        formatedTime = formatedTime.substr(0, (formatedTime.length()-1));
        char buffer [80];
        strftime (buffer,80,"%Y",timeInfo);
        std::string year = std::string(buffer);

        std::string newFileText = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n";
		newFileText += "<!--\n";
		newFileText += "    Description:\n";
		newFileText += "        XML File for execution within RSGISLib\n";
		newFileText += std::string("    Created by **ME** on ") + formatedTime + std::string(".\n");
		newFileText += std::string("    Copyright (c) ") + year + std::string(" **Organisation**. All rights reserved.\n");
		newFileText += "-->\n\n";
		newFileText += "<rsgis:commands xmlns:rsgis=\"http://www.rsgislib.org/xml/\">\n";
		newFileText += "\n    <!-- ENTER YOUR XML HERE -->\n\n";
		newFileText += "</rsgis:commands>\n";


        rsgis::utils::RSGISTextUtils textUtils;
        textUtils.writeStringToFile(xmlFile, newFileText);

        std::cout << xmlFile << " has been created\n";
    }
    catch(rsgis::RSGISException& e)
	{
		std::cerr << "ERROR: " << e.what() << std::endl;
	}
}

void Control::help(std::string algor)
{
    rsgis::RSGISAlgorithmParameters *algorParams = NULL;
	try
	{
		algorParams = factory->getAlgorithmParamterObj(algor);
		if(algorParams != NULL)
		{
			algorParams->help();
		}
		else
		{
			std::cout << "Algorithm \"" << algor	<< "\" cannot be found\n";
		}
	}
	catch(rsgis::RSGISException& e)
	{
		std::cerr << "ERROR: " << e.what() << std::endl;
	}

	if(algorParams != NULL)
	{
		delete algorParams;
	}
}

void Control::help()
{
	std::cout << "RSGISLib, version " << RSGISLIB_PACKAGE_VERSION << std::endl;
	std::cout << "usage: rsgisexe [-h] [-l] [-h algorithm] [-p xml file] [-x xml file]\n\n";
	std::cout << "-h - Prints this message\n";
	std::cout << "-l - List available algorithms\n";
	std::cout << "-h algorithm - Print help for the algorithm specified\n";
    std::cout << "-b xml file - Creates a blank rsgislib xml file to be populated with commands\n";
	std::cout << "-p xml file - Parses and prints the parameters for the xml file specified\n";
	std::cout << "-x xml file - Executes the application with the algorithms and parameters specified\n";
    std::cout << "-v - Prints the version information for the software\n";
	std::cout << "For support please email " << RSGISLIB_PACKAGE_BUGREPORT << std::endl << std::endl;
}

void Control::versionInfo()
{
    std::cout << "You are using Mercurial version " << RSGISLIB_HG_COMMIT << std::endl << std::endl;

    std::cout << "With library versions:\n";
    std::cout << "Calibration Library Version: " << RSGISLIB_CALIBRATION_VERSION << std::endl;
    std::cout << "Classify Library Version: " << RSGISLIB_CLASSIFY_VERSION << std::endl;
    std::cout << "Common Library Version: " << RSGISLIB_COMMONS_VERSION << std::endl;
    std::cout << "Data Structures Library Version: " << RSGISLIB_DATASTRUCT_VERSION << std::endl;
    std::cout << "Filtering Library Version: " << RSGISLIB_FILTERING_VERSION << std::endl;
    std::cout << "Geometry Library Version: " << RSGISLIB_GEOM_VERSION << std::endl;
    std::cout << "Imaging Library Version: " << RSGISLIB_IMG_VERSION << std::endl;
    std::cout << "Maths Library Version: " << RSGISLIB_MATHS_VERSION << std::endl;
    std::cout << "Modeling Library Version: " << RSGISLIB_MODELING_VERSION << std::endl;
    std::cout << "Radar Library Version: " << RSGISLIB_RADAR_VERSION << std::endl;
    std::cout << "Raster GIS Library Version: " << RSGISLIB_RASTERGIS_VERSION << std::endl;
    std::cout << "Registration Library Version: " << RSGISLIB_REGISTRATION_VERSION << std::endl;
    std::cout << "Segmentation Library Version: " << RSGISLIB_SEGMENTATION_VERSION << std::endl;
    std::cout << "Utilities Library Version: " << RSGISLIB_UTILS_VERSION << std::endl;
    std::cout << "Vector Library Version: " << RSGISLIB_VECTOR_VERSION << std::endl;
}

Control::~Control()
{
	delete factory;
}

int main(int argc, char **argv)
{
	std::cout << RSGISLIB_PACKAGE_STRING << " Copyright (C) " << RSGISLIB_COPYRIGHT_YEAR << "  Peter Bunting and Daniel Clewley\n";
    std::cout << "This program comes with ABSOLUTELY NO WARRANTY.\n";
    std::cout << "This is free software, and you are welcome to redistribute it\n";
    std::cout << "under certain conditions; See website (http://www.rsgislib.org).\n";
	std::cout << "For support please email " << RSGISLIB_PACKAGE_BUGREPORT << std::endl << std::endl;

	Control *ctrl = NULL;

	try
	{
		if(argc > 1)
		{
            rsgis::RSGISCommandLineParser *cmdParser = new rsgis::RSGISCommandLineParser();
			ctrl = new Control();

			cmdParser->parseArguments(argc, argv);

			if(cmdParser->argumentPresent(std::string("-h")))
			{
				rsgis::argpair *arghelp = cmdParser->findArgument(std::string("-h"));
				if(arghelp == NULL)
				{
					ctrl->help();
				}
				else if(arghelp->numVals == 1)
				{
					ctrl->help(arghelp->value[0]);
				}
				else
				{
					ctrl->help();
				}
				delete arghelp;
			}
			else if(cmdParser->argumentPresent(std::string("-l")))
			{
				ctrl->listAlgorithms();
			}
			else if(cmdParser->argumentPresent(std::string("-x")))
			{
				rsgis::argpair *argXML = cmdParser->findArgument(std::string("-x"));
				if(argXML == NULL)
				{
					throw rsgis::RSGISCommandLineException("An XML file needs to be provided");
				}
				else if(argXML->numVals == 1)
				{
					ctrl->runXML(argXML->value[0]);
				}
				else
				{
					ctrl->help();
				}
				delete argXML;
			}
			else if(cmdParser->argumentPresent(std::string("-p")))
			{
				rsgis::argpair *argXML = cmdParser->findArgument(std::string("-p"));
				if(argXML == NULL)
				{
					throw rsgis::RSGISCommandLineException("An XML file needs to be provided");
				}
				else if(argXML->numVals == 1)
				{
					ctrl->printParameters(argXML->value[0]);
				}
				else
				{
					ctrl->help();
				}
				delete argXML;
			}
            else if(cmdParser->argumentPresent(std::string("-b")))
			{
				rsgis::argpair *argXML = cmdParser->findArgument(std::string("-b"));
				if(argXML == NULL)
				{
					throw rsgis::RSGISCommandLineException("An XML file needs to be provided");
				}
				else if(argXML->numVals == 1)
				{
					ctrl->createBlankXMLFile(argXML->value[0]);
				}
				else
				{
					ctrl->help();
				}
				delete argXML;
			}
            else if(cmdParser->argumentPresent(std::string("-v")))
			{
                ctrl->versionInfo();
            }
			else
			{
				ctrl->help();
			}

			delete cmdParser;
		}
		else
		{
			ctrl->help();
		}
	}
	catch(rsgis::RSGISCommandLineException& e)
	{
		std::cerr << "ERROR: " << e.what() << std::endl;
		ctrl->help();
	}
	catch(rsgis::RSGISException& e)
	{
		std::cerr << "ERROR: " << e.what() << std::endl;
	}


	if(ctrl != NULL)
	{
		delete ctrl;
	}
}

