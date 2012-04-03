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
#include "RSGISExeRasterGIS.h"
#include "RSGISExeImageMorphology.h"

using namespace std;
using namespace rsgis;

class Control
	{
	public:
		Control() throw(RSGISException);
		void runXML(string xmlFile);
		void printParameters(string xmlFile);
		void listAlgorithms();
		void help(string algor);
		void help();
        void versionInfo();
		~Control();
	private:
		void setUpAlgorithmFactory() throw(RSGISException);
		RSGISAlgorParamsFactory *factory;
	};


Control::Control() throw(RSGISException)
{
	this->setUpAlgorithmFactory();
}

void Control::setUpAlgorithmFactory() throw(RSGISException)
{
	vector<RSGISAlgorithmParameters*> *algorParams = new vector<RSGISAlgorithmParameters*>();

	algorParams->push_back(new RSGISExeImageUtils());
	algorParams->push_back(new RSGISExeStackBands());
	algorParams->push_back(new RSGISExeImageCalculation());
	algorParams->push_back(new RSGISExeImageConversion());
	algorParams->push_back(new RSGISExeClassification());
	algorParams->push_back(new RSGISExeCreateTestImages());
	algorParams->push_back(new RSGISExeFilterImages());
	algorParams->push_back(new RSGISExeFFTUtils());
	algorParams->push_back(new RSGISExeVectorUtils());
	algorParams->push_back(new RSGISExeMathsUtilities());
	algorParams->push_back(new RSGISExeFitting());
	algorParams->push_back(new RSGISExeSARSaatchiBiomass());
	algorParams->push_back(new RSGISExeSARBayesianBiomass());
	algorParams->push_back(new RSGISExeEstimationAlgorithm());
	algorParams->push_back(new RSGISExeRadarUtils());
	algorParams->push_back(new RSGISExePostClassification());
	algorParams->push_back(new RSGISExeVisualisation());
	algorParams->push_back(new RSGISExeCommandLine());
	algorParams->push_back(new RSGISExeTransectModel());
	algorParams->push_back(new RSGISExeSegment());
	algorParams->push_back(new RSGISExeImageRegistration());
	algorParams->push_back(new RSGISExeZonalStats());
    algorParams->push_back(new RSGISExeImageCalibration());
    algorParams->push_back(new RSGISExeElevationDataTools());
    algorParams->push_back(new RSGISExeRasterGIS());
    algorParams->push_back(new RSGISExeImageMorphology());

	factory = new RSGISAlgorParamsFactory(algorParams);
}

void Control::runXML(string xmlFile)
{
	RSGISFileUtils fileUtils;
	RSGISAlgorithmParameters **algorParams = NULL;
	int numCommands = 0;
	time_t rawStartTime = 0;
	struct tm *timeInfo = 0;
	time_t rawFinishTime = 0;
	string formatedTime = "";
	double timeDiff = 0;
	try
	{
		if(fileUtils.checkFilePresent(xmlFile))
		{
			RSGISParseXMLArguments parseXMLArguments = RSGISParseXMLArguments(xmlFile);

			algorParams = parseXMLArguments.parseArguments(&numCommands, factory);
			if (numCommands == 1)
			{
				cout << "There is " << numCommands << " command to be run:\n";
			}
			else
			{
				cout << "There are " << numCommands << " commands to be run:\n";
			}
			for(int i = 0; i < numCommands; i++)
			{
				time(&rawStartTime);
				timeInfo = localtime(&rawStartTime);
				formatedTime = string(asctime(timeInfo));
				formatedTime = formatedTime.substr(0, (formatedTime.length()-1));

				cout << "\n[" << i + 1 << "/" << numCommands <<"] Running Command: " << algorParams[i]->getAlgorithm() << " (Start Time: " << formatedTime << ") " << "..." << endl;
				algorParams[i]->runAlgorithm();

				cout << "Algorithm Completed in " << flush;
				time(&rawFinishTime);
				timeDiff = difftime (rawFinishTime,rawStartTime);
				if (timeDiff == 0)
				{
					cout << "less than a second\n";
				}
				else if(timeDiff > 60)
				{
					timeDiff = timeDiff/60;
					cout << timeDiff << " minutes\n";
				}
				else
				{
					cout << timeDiff << " seconds\n";
				}
			}
			time(&rawFinishTime);
			timeInfo = localtime(&rawFinishTime);
			formatedTime = string(asctime(timeInfo));
			formatedTime = formatedTime.substr(0, (formatedTime.length()-1));
			cout << "Finished (Time: " << formatedTime << ")\n";

			//if(timeInfo != NULL)
			//{
			//	delete timeInfo;
			//}
		}
		else
		{
			throw RSGISXMLArgumentsException("Input XML file is not present or accessable");
		}

	}
	catch(RSGISXMLArgumentsException &e)
	{
		time(&rawStartTime);
		timeInfo = localtime(&rawStartTime);
		formatedTime = string(asctime(timeInfo));
		formatedTime = formatedTime.substr(0, (formatedTime.length()-1));

		cerr << "ERROR: " << e.what() << endl;
		cerr << "Error Occurred on " << formatedTime << "\n";
	}
	catch(RSGISException &e)
	{
		time(&rawStartTime);
		timeInfo = localtime(&rawStartTime);
		formatedTime = string(asctime(timeInfo));
		formatedTime = formatedTime.substr(0, (formatedTime.length()-1));

		cerr << "ERROR: " << e.what() << endl;
		cerr << "Error Occurred on " << formatedTime << "\n";
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

void Control::printParameters(string xmlFile)
{
	RSGISFileUtils fileUtils;
	RSGISAlgorithmParameters **algorParams = NULL;
	int numCommands = 0;
	try
	{
		if(fileUtils.checkFilePresent(xmlFile))
		{
			RSGISParseXMLArguments parseXMLArguments = RSGISParseXMLArguments(xmlFile);
			algorParams = parseXMLArguments.parseArguments(&numCommands, factory);
			cout << "There are " << numCommands << " commands to be printed:\n";
			for(int i = 0; i < numCommands; i++)
			{
				algorParams[i]->printParameters();
			}
			cout << "Finished\n";
		}
		else
		{
			cerr << "ERROR: Input XML file is not present or accessable\n";
		}
	}
	catch(RSGISException& e)
	{
		cerr << "ERROR: " << e.what() << endl;
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
	string *algorithms = NULL;
	int numAlgorithms = 0;
	try
	{
		algorithms = factory->availableAlgorithms(&numAlgorithms);
		cout << "There are " << numAlgorithms << " available:\n";
		for(int i = 0; i < numAlgorithms; i++)
		{
			cout << i << ": " << algorithms[i] << endl;
		}
	}
	catch(RSGISException& e)
	{
		cerr << "ERROR: " << e.what() << endl;
	}

	if(algorithms != NULL)
	{
		delete[] algorithms;
	}
}

void Control::help(string algor)
{
	RSGISAlgorithmParameters *algorParams;
	try
	{
		algorParams = factory->getAlgorithmParamterObj(algor);
		if(algorParams != NULL)
		{
			algorParams->help();
		}
		else
		{
			cout << "Algorithm \"" << algor	<< "\" cannot be found\n";
		}
	}
	catch(RSGISException& e)
	{
		cerr << "ERROR: " << e.what() << endl;
	}

	if(algorParams != NULL)
	{
		delete algorParams;
	}
}

void Control::help()
{
	cout << "RSGISLib, version " << RSGISLIB_PACKAGE_VERSION << endl;
	cout << "usage: rsgisexe [-h] [-l] [-h algorithm] [-p xml file] [-x xml file]\n\n";
	cout << "-h - Prints this messgae\n";
	cout << "-l - List available algorithms\n";
	cout << "-h algorithm - Print help for the algorithm specified\n";
	cout << "-p xml file - Parses and prints the parameters for the xml file specified\n";
	cout << "-x xml file - Executes the application with the algorithms and parameters specified\n";
    cout << "-v - Prints the version information for the software\n";
	cout << "Bugs are to be reported on the trac or directly to " << RSGISLIB_PACKAGE_BUGREPORT << endl << endl;
}

void Control::versionInfo()
{
    cout << "You are using SVN version " << RSGISLIB_SVN_COMMIT << endl << endl;
    
    cout << "With library versions:\n";
    cout << "Calibration Library Version: " << RSGISLIB_CALIBRATION_VERSION << endl;
    cout << "Classify Library Version: " << RSGISLIB_CLASSIFY_VERSION << endl;
    cout << "Common Library Version: " << RSGISLIB_COMMONS_VERSION << endl;
    cout << "Data Structures Library Version: " << RSGISLIB_DATASTRUCT_VERSION << endl;
    cout << "Filtering Library Version: " << RSGISLIB_FILTERING_VERSION << endl;
    cout << "Geometry Library Version: " << RSGISLIB_GEOM_VERSION << endl;
    cout << "Imaging Library Version: " << RSGISLIB_IMG_VERSION << endl;
    cout << "Maths Library Version: " << RSGISLIB_MATHS_VERSION << endl;
    cout << "Modeling Library Version: " << RSGISLIB_MODELING_VERSION << endl;
    cout << "Radar Library Version: " << RSGISLIB_RADAR_VERSION << endl;
    cout << "Raster GIS Library Version: " << RSGISLIB_RASTERGIS_VERSION << endl;
    cout << "Registration Library Version: " << RSGISLIB_REGISTRATION_VERSION << endl;
    cout << "Segmentation Library Version: " << RSGISLIB_SEGMENTATION_VERSION << endl;
    cout << "Utilities Library Version: " << RSGISLIB_UTILS_VERSION << endl;
    cout << "Vector Library Version: " << RSGISLIB_VECTOR_VERSION << endl;
}

Control::~Control()
{
	delete factory;
}

int main(int argc, char **argv)
{
	cout << RSGISLIB_PACKAGE_STRING << " Copyright (C) " << RSGISLIB_COPYRIGHT_YEAR << "  Peter Bunting and Daniel Clewley\n";
    cout << "This program comes with ABSOLUTELY NO WARRANTY.\n";
    cout << "This is free software, and you are welcome to redistribute it\n";
    cout << "under certain conditions; See website (http://www.rsgislib.org).\n";
	cout << "Bugs are to be reported on the trac or directly to " << RSGISLIB_PACKAGE_BUGREPORT << endl << endl;

	Control *ctrl = NULL;

	try
	{
		if(argc > 1)
		{
			RSGISCommandLineParser *cmdParser = new RSGISCommandLineParser();
			ctrl = new Control();

			cmdParser->parseArguments(argc, argv);

			if(cmdParser->argumentPresent(string("-h")))
			{
				argpair *arghelp = cmdParser->findArgument(string("-h"));
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
			else if(cmdParser->argumentPresent(string("-l")))
			{
				ctrl->listAlgorithms();
			}
			else if(cmdParser->argumentPresent(string("-x")))
			{
				argpair *argXML = cmdParser->findArgument(string("-x"));
				if(argXML == NULL)
				{
					throw RSGISCommandLineException("An XML file needs to be provided");
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
			else if(cmdParser->argumentPresent(string("-p")))
			{
				argpair *argXML = cmdParser->findArgument(string("-p"));
				if(argXML == NULL)
				{
					throw RSGISCommandLineException("An XML file needs to be provided");
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
            else if(cmdParser->argumentPresent(string("-v")))
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
	catch(RSGISCommandLineException& e)
	{
		cerr << "ERROR: " << e.what() << endl;
		ctrl->help();
	}
	catch(RSGISException& e)
	{
		cerr << "ERROR: " << e.what() << endl;
	}


	if(ctrl != NULL)
	{
		delete ctrl;
	}
}

