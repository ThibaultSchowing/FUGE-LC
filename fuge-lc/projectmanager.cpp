#include "projectmanager.h"

#include <iostream>

#include <QFile>
#include <QStandardPaths>
#include <QDir>
#include "systemparameters.h"

#define MAX_DISPLAYED_RECENT_GLOBAL 15
#define MAX_DISPLAYED_RECENT_PROJECT 5

const QString ProjectManager::FUGE_SHARED_FOLDER = "FUGE/";
const QString ProjectManager::FUZZY_FOLDER = "fuzzySystems/";
const QString ProjectManager::CONFIG_FOLDER = "configs/";
const QString ProjectManager::SCRIPT_FOLDER = "scripts/";
const QString ProjectManager::PROJECT_FOLDER = ".FUGE/";
const QString ProjectManager::PROJECT_DATA_FILE_NAME = "recent";
const QString ProjectManager::SHARED_DATA_FILE_NAME = "recent";
const QString ProjectManager::FIELD_DATASETS = "recentDatasets";
const QString ProjectManager::FIELD_PROJECTS = "recentProjects";
const QString ProjectManager::GENERATED_DATASET_FOLDER = "generatedDatasets/";

ProjectManager::~ProjectManager() {

}

ProjectManager::ProjectManager() {
    datasetName = "";
    savePath= "./";
    defaultFilePath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/";
    globalFilesPath = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/";
    QDir fugeDir;
    fugeDir.mkdir(defaultFilePath + FUGE_SHARED_FOLDER);
    fugeDir.mkdir(defaultFilePath + FUGE_SHARED_FOLDER + CONFIG_FOLDER);
    QFile inifile(globalFilesPath + SHARED_DATA_FILE_NAME);

    // should be called only the first time FUGE is launched on the user
    if (!fugeDir.exists(globalFilesPath)) {
        fugeDir.mkdir(globalFilesPath);
        writeSharedData();
    } else {
        readSharedData();
        readProjectData();
    }
}


/**
 * @brief Creates a project with all the necessary folders
 * @param path for the project
 * @return True if created, false otherwise
 */
bool ProjectManager::newProjectFolder(const QString& path) {
    QDir workDir;

    // make sure the project doesn't exist already
    if (openExistingProject(path)) {
        return true;
    }

    if (path != "/" && workDir.exists(path)) {
        recentDatasetsProject.clear();
        QString projectPath = path + PROJECT_FOLDER;
        if (!workDir.exists(projectPath)) {
            workDir.mkdir(projectPath);
            QFile::remove(projectPath + PROJECT_DATA_FILE_NAME);
            QFile recent(projectPath + PROJECT_DATA_FILE_NAME);
            if (!recent.open(QIODevice::WriteOnly | QIODevice::Text)){
                qDebug() << "Could not create the file for recent.";
                return false;
            }

            recent.close();
        }

        QString fuzzyPath = path + FUZZY_FOLDER;
        if (!workDir.exists(fuzzyPath)) {
            workDir.mkdir(fuzzyPath);
        }
        QString configPath = path + CONFIG_FOLDER;
        if (!workDir.exists(configPath)) {
            workDir.mkdir(configPath);
        }
        QString scriptPath = path + SCRIPT_FOLDER;
        if (!workDir.exists(scriptPath)) {
            workDir.mkdir(scriptPath);
        }

        QString generatedDatasetPath = path + GENERATED_DATASET_FOLDER;
        if (!workDir.exists(generatedDatasetPath)) {
            workDir.mkdir(generatedDatasetPath);
        }

        if (!workDir.exists(scriptPath) || !workDir.exists(configPath) || !workDir.exists(scriptPath) || !workDir.exists(generatedDatasetPath)) {
            return false ;
        }

        SystemParameters& sysParams = SystemParameters::getInstance();
        sysParams.setSavePath(path);
        savePath = path;
        datasetName = "";
        updateRecentProjects(path);
        writeSharedData();

        return true;
    }

    return false;
}

/**
 * @brief Opens a project that already exists.
 *        The existence of the project is defined by the existence of the .FUGE/ folder
 * @param path
 * @return True if the project exists, false otherwise
 */
bool ProjectManager::openExistingProject(const QString& path) {
    recentDatasetsProject.clear();
    QDir workDir;
    QString projectPath = path + PROJECT_FOLDER + PROJECT_DATA_FILE_NAME;
    if (path != "/" && workDir.exists(projectPath)) {
        SystemParameters& sysParams = SystemParameters::getInstance();
        sysParams.setSavePath(path);
        savePath = path;
        updateRecentProjects(path);
        readProjectData();
        readSharedData();
        return true;
    }

    return false;
}

/**
 * Writes data in AppLocalDataFolder, that is going to be shared between projects
 */
void ProjectManager::writeSharedData() {
    QFile::remove(globalFilesPath + SHARED_DATA_FILE_NAME);
    QFile inifile(globalFilesPath + SHARED_DATA_FILE_NAME);
    if (!inifile.open(QIODevice::WriteOnly | QIODevice::Text)){
        qDebug() << "Could not create the ini file.";
    }else{
        QTextStream out(&inifile);
        for (int i = 0; i < recentDatasetsGlobal.length(); i++){
            out << FIELD_DATASETS << "=" << recentDatasetsGlobal.at(i) << "\n";
        }
        for (int i = 0; i < recentProjects.length(); i++){
            out << FIELD_PROJECTS << "=" << recentProjects.at(i) << "\n";
        }
        inifile.close();
    }
}

/**
 * Updates the data for a project
 */
void ProjectManager::writeProjectData() {
    QFile::remove(savePath + PROJECT_FOLDER + PROJECT_DATA_FILE_NAME);
    QFile recentFile(savePath + PROJECT_FOLDER + PROJECT_DATA_FILE_NAME);
    if (!recentFile.open(QIODevice::WriteOnly | QIODevice::Text)){
        qDebug() << "Could not create the recent file.";
    }else{
        QTextStream out(&recentFile);
        for (int i = 0; i < recentDatasetsProject.length(); i++){
            out << FIELD_DATASETS << "=" << recentDatasetsProject.at(i) << "\n";
        }
        recentFile.close();
    }
}

/**
 * Retrieves all the data related to a project
 */
void ProjectManager::readProjectData() {
    recentDatasetsProject.clear();
    QFile fileRecent(savePath + PROJECT_FOLDER + PROJECT_DATA_FILE_NAME);
    if (fileRecent.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&fileRecent);
        QString line;
        QStringList content;
        while(!(line = in.readLine()).isEmpty()) {
            content = line.split("=");

            if(content.size() == 2){
                if(content.at(0).compare(FIELD_DATASETS, Qt::CaseInsensitive)==0){
                    recentDatasetsProject.push_back(content.mid(1).join(""));
                }
            }
        }
        fileRecent.close();

        datasetName = "";
        if (!recentDatasetsProject.isEmpty()) {
            datasetName = recentDatasetsProject.at(0);
        }
    }
    else {
        qDebug() << "Could not open recent file";
    }
}

/**
 * @brief Retrieves the data in AppLocalDataFolder, that is going to be shared between projects
 */
void ProjectManager::readSharedData() {
    recentDatasetsGlobal.clear();
    recentProjects.clear();
    SystemParameters& sysParams = SystemParameters::getInstance();
    QFile inifile(globalFilesPath + SHARED_DATA_FILE_NAME);
    if (inifile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&inifile);
        QString line;
        QStringList content;
        while(!(line = in.readLine()).isEmpty()) {
            content = line.split("=");

            if(content.size() == 2){
                if(content.at(0).compare(FIELD_DATASETS, Qt::CaseInsensitive)==0){
                    recentDatasetsGlobal.push_back(content.mid(1).join(""));
                }
                else if(content.at(0).compare(FIELD_PROJECTS, Qt::CaseInsensitive)==0){
                    recentProjects.push_back(content.mid(1).join(""));
                }
            }
        }
        inifile.close();

        if (!recentProjects.isEmpty()) {
            savePath = recentProjects.at(0);
            sysParams.setSavePath(savePath);
        }
    }
    else {
        qDebug() << "Could not open ini file";
    }
}

/**
 * @brief Updates the list of all the recently loaded datasets.
 *        When an existing dataset gets added, its position gets updated on top, both in the global and the recent datasets.
 */
void ProjectManager::handleLoadedDataset(const QString& path) {
    datasetName = path;
    for (int i = 0; i < recentDatasetsProject.length(); i++) {
        if (path == recentDatasetsProject.at(i)) {
            recentDatasetsProject.remove(i, 1);
            break;
        }
    }

    if (recentDatasetsProject.length() >= MAX_DISPLAYED_RECENT_PROJECT) {
        recentDatasetsProject.removeLast();
    }

    recentDatasetsProject.push_front(path);

    for (int i = 0; i < recentDatasetsGlobal.length(); i++) {
        if (path == recentDatasetsGlobal.at(i)) {
            recentDatasetsGlobal.remove(i, 1);
            break;
        }
    }

    if (recentDatasetsGlobal.length() >= MAX_DISPLAYED_RECENT_GLOBAL) {
        recentDatasetsGlobal.removeLast();
    }

    recentDatasetsGlobal.push_front(path);
    writeProjectData();
    writeSharedData();
}

/**
 * @brief Updates the list of all the recent projects.
 *        When an existing project gets added, its position gets updated on top.
 */
void ProjectManager::updateRecentProjects(const QString& path) {
    savePath = path;
    for (int i = 0; i < recentProjects.length(); i++) {
        if (path == recentProjects.at(i)) {
            recentProjects.remove(i, 1);
            break;
        }
    }

    if (recentProjects.length() >= MAX_DISPLAYED_RECENT_GLOBAL) {
        recentProjects.removeLast();
    }

    recentProjects.push_front(path);
    writeSharedData();
}

/**
 * @brief Returns all the recently loaded data sets, that were not loaded in the current project
 */
const QVector<QString> ProjectManager::getGlobalRecentDatasets() {
    QVector<QString> temp;
    for (int i = 0; i < recentDatasetsGlobal.length(); i++){
        bool isNotHere = true;
        for (int j = 0; j < recentDatasetsProject.length(); j++) {
            if (recentDatasetsGlobal.at(i) == recentDatasetsProject.at(j)) {
                isNotHere = false;
                break;
            }
        }
        if (isNotHere) {
            temp.push_back(recentDatasetsGlobal.at(i));
        }
    }

    return temp;
}
