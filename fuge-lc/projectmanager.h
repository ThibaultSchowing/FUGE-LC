#ifndef PROJECTMANAGER_H
#define PROJECTMANAGER_H

#include <QObject>
#include <QVector>

class ProjectManager: public QObject
{
    Q_OBJECT

private:
    ProjectManager();
    ~ProjectManager();
    ProjectManager(const ProjectManager&);
    ProjectManager& operator=(const ProjectManager&);

    void writeIni();
    void readIni();

    // Experiment name
    QString experimentName;
    // Database name
    QString datasetName;
    // Save path
    QString savePath;
    // Path to project
    QString defaultFilePath;
    // Global FUGE files
    QString globalFilesPath;
    QVector<QString> recentDatasets;
    QVector<QString> recentProjects;

public:
    static ProjectManager& getInstance() {
        static ProjectManager instance;
        return instance;
    }

    bool newProjectFolder(const QString& path);
    bool openExistingProject(const QString& path);
    inline QString getSavePath() {return savePath;}
    inline QString getDefaultFilePath() {return defaultFilePath;}

    inline void setExperimentName(QString name) {experimentName = name;}
    inline void setSavePath(QString path) {savePath = path;}
    inline QString getExperimentName() {return experimentName;}
    inline QString getDatasetName() {return datasetName;}
    void handleLoadedDataset(const QString& path);
    void updateRecentProjects(const QString& path);
    inline const QVector<QString> getRecentDatasets() {return recentDatasets;}
    inline const QVector<QString> getRecentProjects() {return recentProjects;}
};

#endif // PROJECTMANAGER_H
