#include "projectmanager.h"

#include <iostream>

#include <QFile>
#include <QStandardPaths>
#include <QDir>
#include "systemparameters.h"

ProjectManager::~ProjectManager() {

}

ProjectManager::ProjectManager() {
    datasetName = "";
    savePath= "./";
    defaultFilePath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/";
    globalFilesPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/";
    QDir fugeDir;
    QFile inifile(globalFilesPath + "/currentSession.ini");
    if (!fugeDir.exists(globalFilesPath)) {
        fugeDir.mkdir(globalFilesPath);
        writeIni();
    } else {
        readIni();
    }
    assert(fugeDir.exists(globalFilesPath)); // DEBUG
}


bool ProjectManager::newWorkFolder(const QString& path) {
    QDir workDir;
    if (path != "/" && workDir.exists(path)) {
        QString fuzzyPath = path + "fuzzySystems";
        if (!workDir.exists(fuzzyPath)) {
            workDir.mkdir(fuzzyPath);
        }
        QString configPath = path + "configs";
        if (!workDir.exists(configPath)) {
            workDir.mkdir(configPath);
        }
        QString scriptPath = path + "scripts";
        if (!workDir.exists(scriptPath)) {
            workDir.mkdir(scriptPath);
        }

        if (!workDir.exists(scriptPath) || !workDir.exists(configPath) || !workDir.exists(scriptPath)) {
            return false ;
        }
        SystemParameters& sysParams = SystemParameters::getInstance();
        sysParams.setSavePath(path);
        savePath = path;
        datasetName = "";
        writeIni();

        return true;
    }

    return false;
}

void ProjectManager::writeIni() {
    QFile::remove(globalFilesPath + "currentSession.ini");
    QFile inifile(globalFilesPath + "currentSession.ini");
    if (!inifile.open(QIODevice::WriteOnly | QIODevice::Text)){
        qDebug() << "Could not create the file.";
    }else{
        QTextStream out(&inifile);
        if (!savePath.isEmpty())
            out << "savePath=" << savePath << "\n";
        if (!datasetName.isEmpty())
            out << "dataSetName=" << datasetName << "\n";
        inifile.close();
    }
}

void ProjectManager::readIni() {
    SystemParameters& sysParams = SystemParameters::getInstance();

    QFile inifile(globalFilesPath + "currentSession.ini");
    if (inifile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&inifile);

        QString line;
        QStringList content;
        while(!(line = in.readLine()).isEmpty()) {
            // Remove all spaces
            line = line.trimmed();
            line.replace(" ", "");
            content = line.split("=");

            if(content.size() == 2){
                if(content.at(0).compare("savePath", Qt::CaseInsensitive)==0) {
                    savePath = content.at(1);
                    sysParams.setSavePath(savePath);
                }
                else if(content.at(0).compare("dataSetName", Qt::CaseInsensitive)==0){
                    datasetName = content.at(1);
                }
            }
        }
        inifile.close();
    }
    else {
        qDebug() << "Could not open ini file";
    }
}

void ProjectManager::handleLoadedDataset(const QString& path) {
    datasetName = path;
    writeIni();
}
