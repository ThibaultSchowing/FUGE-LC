/**
  * @file   main.cpp
  * @author Jean-Philippe Meylan <jean-philippe.meylan_at_heig-vd.ch>
  * @author ReDS (Reconfigurable and embedded digital systems) <www.reds.ch>
  * @author HEIG-VD (Haute école d'ingénierie et de gestion) <www.heig-vd.ch>
  * @date   07.2009
  * @section LICENSE
  *
  * This application is free software; you can redistribute it and/or
  * modify it under the terms of the GNU Lesser General Public
  * License as published by the Free Software Foundation; either
  * version 2.1 of the License, or (at your option) any later version.
  *
  * This library is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  * Lesser General Public License for more details.
  *
  * You should have received a copy of the GNU Lesser General Public
  * License along with this library; if not, write to the Free Software
  * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
  *
  * @brief FUGE-LC main file. It is responsible to parse the command line arguments
  * and run the application main window.
  */

#include <QtGui/QApplication>
#include <QString>
#include <iostream>

#include "fugemain.h"
#include "systemparameters.h"

QString datasetFile;
QString scriptFile;
QString fuzzyFile;
bool useGUI= true;
bool runFromCmd = false;
bool dataLoaded = false;
bool scriptLoaded = false;
bool fuzzyLoaded = false;
bool verbose = false;
bool eval = false;
bool predict = false;


/**
  * Displays the command line help.
  */
void showHelp()
{
    std::cout << std::endl << "Valid parameters are :" << std::endl << std::endl;
    std::cout << " --verbose : Verbose output" << std::endl << std::endl;
    std::cout << " --evaluate : Perform an evaluation of the given fuzzy system on the specified database" << std::endl << std::endl;
    std::cout << " --predict : Perform a prediction of the given fuzzy system on the specified database" << std::endl << std::endl;
    std::cout << " -d  : Dataset  (required to run automatically from command line)" << std::endl;
    std::cout << "       Value : Path to the dataset" << std::endl << std::endl;
    std::cout << " -s  : Script   (required to run automatically from command line)" << std::endl;
    std::cout << "       Value : Path to the execution script" << std::endl << std::endl;;
    std::cout << " -f  : Fuzzy system   (required for evalation/prediction)" << std::endl;
    std::cout << "       Value : Path to the fuzzy system file" << std::endl << std::endl;;
    std::cout << " -g  : GUI  (optionnal)" << std::endl;
    std::cout << "       Value : yes (Show the GUI) " << std::endl;
    std::cout << "               no  (Do not show the GUI) " << std::endl << std::endl;
    std::cout << " --help : this message" << std::endl << std::endl;;
}

/**
  * Prompts the invalid usage error message.
  *
  * @param progName Name of this executable.
  */
void invalidUsage(QString progName)
{
    std::cout << std::endl << "ERROR : Invalid parameters format !" << std::endl << std::endl;
    std::cout << "Usage : " << progName.toStdString() << " -p1 value -p2 value ..." << std::endl << std::endl;
    std::cout << "For parameter list run with --help" << std::endl << std::endl;
}

/**
  * Prompts the invalid parameter error message.
  */
void invalidParam()
{
    std::cout << std::endl << "ERROR : Invalid parameter !" << std::endl;
    showHelp();
}

/**
  * Parse the command line arguments.
  *
  * @param args Arguments passed in a StringList structure.
  */
bool parseArguments(QStringList args)
{
    // Look if we run directly from cmdline
    if (args.size() == 1) {
        runFromCmd = false;
        return true;
    }

    // Help parameter
    if (args.at(1) == QString("--help")) {
            showHelp();
            return false;
    }
    for (int i = 1; i < args.size(); i+=2) {
        // Look for the argument marker : '-'
        if (args.at(i).at(0) != QChar('-')) {
            invalidUsage(args.at(0));
            return false;
        }
        // Dataset file parameter
        else if (args.at(i).at(1) == QChar('d')) {
            QFile file(args.at(i+1));
            if (file.exists()) {
                datasetFile = args.at(i+1);
                SystemParameters& sysParams = SystemParameters::getInstance();
                sysParams.setDatasetName(datasetFile);
                dataLoaded = true;
            }
            else {
                std::cout << std::endl << "ERROR : file \"" << args.at(i+1).toStdString() << "\" not found !" << std::endl << std::endl;
                return false;
            }
        }
        // Script file parameter
        else if (args.at(i).at(1) == QChar('s')) {
            QFile file(args.at(i+1));
            if (file.exists()) {
                scriptFile = args.at(i+1);
                scriptLoaded = true;
            }
            else {
                std::cout << std::endl << "ERROR : file \"" << args.at(i+1).toStdString() << "\" not found !" << std::endl << std::endl;
                return false;
            }
        }
        // Fuzzy file parameter
        else if (args.at(i).at(1) == QChar('f')) {
            QFile file(args.at(i+1));
            if (file.exists()) {
                fuzzyFile = args.at(i+1);
                fuzzyLoaded = true;
            }
            else {
                std::cout << std::endl << "ERROR : file \"" << args.at(i+1).toStdString() << "\" not found !" << std::endl << std::endl;
                return false;
            }
        }
        // Gui parameter
        else if (args.at(i).at(1) == QChar('g')) {
            if (args.at(i+1) == QString("yes")) {
                useGUI = true;
            }
            else if (args.at(i+1) == QString("no")) {
                useGUI = false;
            }
            else {
                std::cout << std::endl << "Error : incorrect value \"" << args.at(i+1).toStdString() << "\" !" << std::endl << std::endl;
                return false;
            }
        }
        else if (args.at(i).at(1) == QChar('-')) {
            if (args.at(i) == "--verbose") {
                verbose = true;
            }
            else if (args.at(i) == "--evaluate") {
                eval = true;
            }
            else if (args.at(i) == "--predict") {
                predict = true;
            }
            else {
                invalidParam();
                return false;
            }
            i--;
        }
        else {
            invalidParam();
            return false;
        }
    }
    if (eval || predict) {
        if (eval && predict) {
            std::cout << std::endl << "ERROR : yout cannot perform both a prediction and a evaluation !" << std::endl << std::endl;
            return false;
        }
        if ((eval || predict) && !fuzzyLoaded) {
            std::cout << std::endl << "ERROR : you must specify a fuzzy system to perform a evaluation/prediction !" << std::endl << std::endl;
            return false;
        }
        if (!dataLoaded) {
            std::cout << std::endl << "ERROR : yout must specify a dataset to perform a evaluation/prediction !" << std::endl << std::endl;
            return false;
        }
    }
    else {
        if (!(dataLoaded && scriptLoaded)) {
            std::cout << std::endl << "ERROR : you must load a dataset AND a script to run automatically from command line !" << std::endl << std::endl;
            return false;
        }
    }

    runFromCmd = true;
    return true;
}

/**
  * Main function.
  */
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    if (parseArguments(a.arguments())) {
        FugeMain w;
        // Load the GUI if needed
        if (useGUI)
            w.show();

        // Run automatically from command line if needed
        if (runFromCmd) {
            if (!eval && !predict) {
                w.runFromCmdLine(datasetFile, scriptFile, fuzzyFile, eval, predict, verbose);
                return a.exec();
            }
            else {
                w.runFromCmdLine(datasetFile, scriptFile, fuzzyFile, eval, predict, verbose);
            }
        }
        else {
            // Start the event loop
            return a.exec();
        }
    }
}
