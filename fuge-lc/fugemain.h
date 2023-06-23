/**
  * @file   fugemain.h
  * @author Jean-Philippe Meylan <jean-philippe.meylan_at_heig-vd.ch>
  * @author ReDS (Reconfigurable and embedded digital systems) <www.reds.ch>
  * @author HEIG-VD (Haute école d'ingénierie et de gestion) <www.heig-vd.ch>
  * @author Yvan Da Silva <yvan.dasilva_at_heig-vd.ch>
  * @date   06.2012
  * @date   03.2010
  * @section LICENSE
  *
  * This application is free software; you can redistribute it and/or
  * modify it under the terms of the GNU Lesser General Public
  * License as published by the Free Software Foundation; either
  * version 3.0 of the License, or (at your option) any later version.
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
  * @class FugeMain
  *
  * @brief This class implements the FUGE-LC main window.
  */

#ifndef FUGEMAIN_H
#define FUGEMAIN_H

#include <QMainWindow>
#include <QFile>
#include <iostream>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <numeric>
#include <unistd.h>

#include <QFileDialog>
#include <QLabel>
#include <QTime>
#include <QTextStream>
#include <QStringList>
#include <QMessageBox>
#include <QSemaphore>
#include <QProcess>

#include "fuzzysystem.h"

#include "aboutdialog.h"
#include "helpdialog.h"
#include "statsplot.h"
#include "computethread.h"
#include "evalplot.h"
#include "scriptmanager.h"
#include "ui_fugemain.h"
#include "errordialog.h"
#include "infodialog.h"
#include "systemparameters.h"
#include "editparamsdialog.h"
#include "fuzzyeditor.h"
#include "fuzzyvariable.h"
#include "fuzzyset.h"
#include "fuzzyrulegenome.h"
#include "fuzzymembershipsgenome.h"
#include "fuzzyrule.h"
#include "fuzzysystem.h"

class ComputeThread;

namespace Ui
{
    class FugeMain;
}

extern QFile *fitLogFile;

class FugeMain : public QMainWindow
{
    Q_OBJECT

public:
    FugeMain(QWidget *parent = 0);
    ~FugeMain();

    void runFromCmdLine(QString dataSet, QString scriptFile, QString fuzzyFile,
                        bool eval, bool predict, bool verbose);
    static QList<QStringList>* listFile;
    static FuzzySystem* getNewFuzzySystem(QList<QStringList>* listFile);

protected:
    virtual void changeEvent(QEvent *e);

private:
    void createActions();
    void resetDisplay();
    void loadFromIni();
    void loadDataSet(const QString& fileName);
    Ui::FugeMain* ui;
    AboutDialog* aboutDial;
    HelpDialog* help;
    StatsPlot* statsPlot;
    EvalPlot* evalPlot;
    ComputeThread* computeThread;

    FuzzySystem* fSystemVars;
    FuzzySystem* fSystemRules;

    ScriptManager* sMan;
    QAction* actRun;
    QAction* actStop;
    QAction* actOpenData;
    QAction* actCloseData;
    QAction* actNewFuzzy;
    QAction* actOpenFuzzy;
    QAction* actCloseFuzzy;
    QAction* actSaveFuzzy;
    QAction* actEditFuzzy;
    QAction* actValidFuzzy;
    QAction* actEvalFuzzy;
    QAction* actPredictFuzzy;
    QAction* actEditParams;
    QAction* actOpenScript;
    QAction* actCloseScript;
    QAction* actRunScript;
    QAction* actQuit;
    QAction* actAbout;
    QAction* actHelp;
    QAction* actEnableScript;
    QAction* actSetWorkFolder;
    QMenu* fileMenu;
    QMenu* dataMenu;
    QMenu* fuzzyMenu;
    QMenu* paramsMenu;
    QMenu* scriptMenu;
    QMenu* helpMenu;
    QMenu* recentDatasetMenu;
    EditParamsDialog *editParams;
    QString currentOpennedSystem;

    bool fuzzyLoaded;
    bool dataLoaded;
    bool scriptLoaded;
    bool paramsLoaded;
    bool isRunning;
    bool isScriptEnabled;

public slots:
    void onComputeFinished();
    void onScriptFinished();

private slots:
    void onActRun();
    void onActQuit();
    void onActOpenData();
    void onActCloseData();
    void onActNewFuzzy();
    void onActOpenFuzzy();
    void onActCloseFuzzy();
    void onActValidFuzzy();
    void onActEvalFuzzy(bool doValid = false, bool fromCmd = false);
    void onActPredictFuzzy(bool fromCmd = false);
    void onActSaveFuzzy();
    void onActEditFuzzy();
    void onActEditParams();
    void onActOpenScript();
    void onActCloseScript();
    void onActRunScript();
    void onActAbout();
    void onActHelp();
    void closeEvent(QCloseEvent*);
    void onShowScriptClicked();
    void onSettingWordFolder();
    void onShowRecentDatasets();

signals:
    void clearStats();
    void closeStats();
    void saveFuzzySystem(QString fileName);
    void openFuzzySystem(QString fileName);
};

#endif // FUGEMAIN_H
