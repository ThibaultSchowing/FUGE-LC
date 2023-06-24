#include "projectmanager.h"

#include <iostream>

#include <QFile>
#include <QStandardPaths>
#include <QDir>
#include "systemparameters.h"

#define MAX_DISPLAYED_RECENT 10

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
        updateRecentProjects(path);
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
        for (int i = 1; i < recentDatasets.length(); i++){
            out << "recentDataset=" << recentDatasets.at(i) << "\n";
        }
        for (int i = 1; i < recentProjects.length(); i++){
            out << "recentProject=" << recentProjects.at(i) << "\n";
        }
        inifile.close();
    }
}

void ProjectManager::readIni() {
    SystemParameters& sysParams = SystemParameters::getInstance();
    QFile inifile(globalFilesPath + "currentSession.ini");
    if (inifile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&inifile);

        recentDatasets.clear();
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
                    recentProjects.push_back(savePath);
                }
                else if(content.at(0).compare("dataSetName", Qt::CaseInsensitive)==0){
                    datasetName = content.at(1);
                    recentDatasets.push_back(datasetName);
                }
                else if(content.at(0).compare("recentDataset", Qt::CaseInsensitive)==0){
                    recentDatasets.push_back(content.at(1));
                }
                else if(content.at(0).compare("recentProject", Qt::CaseInsensitive)==0){
                    recentProjects.push_back(content.at(1));
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
    for (int i = 0; i < recentDatasets.length(); i++) {
        if (path == recentDatasets.at(i)) {
            recentDatasets.remove(i, 1);
            break;
        }
    }

    if (recentDatasets.length() >= MAX_DISPLAYED_RECENT) {
        recentDatasets.removeLast();
    }

    recentDatasets.push_front(path);
    writeIni();
}

void ProjectManager::updateRecentProjects(const QString& path) {
    savePath = path;
    for (int i = 0; i < recentProjects.length(); i++) {
        if (path == recentProjects.at(i)) {
            recentProjects.remove(i, 1);
            break;
        }
    }

    if (recentProjects.length() >= MAX_DISPLAYED_RECENT) {
        recentProjects.removeLast();
    }

    recentProjects.push_front(path);
    writeIni();
}
