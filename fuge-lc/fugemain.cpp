/**
  * @file   fugemain.cpp
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
#include "fugemain.h"


QFile *fitLogFile;
QSemaphore scriptSema(0);
bool doRunFromCmd = false;
QList<QStringList>* FugeMain::listFile = 0;

FugeMain::FugeMain(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::FugeMain),
      fSystemRules(0), fSystemVars(0), editParams(0)
{
    ui->setupUi(this);
    ui->btRunScript->setEnabled(false);
    ui->btRun->setEnabled(false);
    ui->btValidate->setEnabled(false);
    ui->btEvaluate->setEnabled(false);
    ui->btPredict->setEnabled(false);

    // Initialise the random generator
    QTime time;
    //srand(QDateTime::currentDateTime().toTime_t());
    srand(QDateTime::currentDateTime().toSecsSinceEpoch());

    fuzzyLoaded = false;
    dataLoaded = false;
    scriptLoaded = false;
    paramsLoaded = false;
    isRunning = false;

    ComputeThread::bestFSystem = 0;
    fSystemRules = 0;
    fSystemVars = 0;
    listFile = new QList<QStringList>;

    help = new HelpDialog();
    aboutDial = new AboutDialog();
    statsPlot = new StatsPlot();
    CoevStats& fitStats = CoevStats::getInstance();
    sMan = new ScriptManager();
    // TODO: Rename the signal and slots to have the same name and get ride of those connect declarations.
    // Clicked should be linked to OnClicked etc. (See Qt documentation for more information).
    connect(sMan, SIGNAL(startRun()), this, SLOT(onActRun()));
    connect(sMan, SIGNAL(scriptFinished()), this, SLOT(onScriptFinished()));
    computeThread = new ComputeThread();
    connect(computeThread, SIGNAL(computeFinished()), this, SLOT(onComputeFinished()));
    connect(ui->btStop, SIGNAL(clicked()), computeThread, SLOT(onStopEvo()));
    connect(&fitStats, SIGNAL(transmit(QString)), statsPlot, SLOT(receiveData(QString)), Qt::BlockingQueuedConnection);
    connect(this, SIGNAL(clearStats()), statsPlot, SLOT(onClearStats()));
    //connect(this, SIGNAL(closeStats()), statsPlot, SLOT(onCloseStats()));
    connect(ui->btGraph, SIGNAL(clicked()), statsPlot, SLOT(onShowGraph()));
    connect(ui->btRunScript, SIGNAL(clicked()), this, SLOT(onActRunScript()));
    connect(ui->btRun, SIGNAL(clicked()), this, SLOT(onActRun()));
    connect(ui->btValidate, SIGNAL(clicked()), this, SLOT(onActValidFuzzy()));
    connect(ui->btEvaluate, SIGNAL(clicked()), this, SLOT(onActEvalFuzzy()));
    connect(ui->btPredict, SIGNAL(clicked()), this, SLOT(onActPredictFuzzy()));
    connect(ui->btNewFuzzy, SIGNAL(clicked()), this, SLOT(onActNewFuzzy()));
    connect(ui->btOpenFuzzy, SIGNAL(clicked()), this, SLOT(onActOpenFuzzy()));
    connect(ui->btCloseFuzzy, SIGNAL(clicked()), this, SLOT(onActCloseFuzzy()));
    connect(ui->btSaveFuzzy, SIGNAL(clicked()), this, SLOT(onActSaveFuzzy()));
    connect(ui->btEditFuzzy, SIGNAL(clicked()), this, SLOT(onActEditFuzzy()));
    connect(ui->btOpenData, SIGNAL(clicked()), this, SLOT(onActOpenData()));
    connect(ui->btCloseData, SIGNAL(clicked()), this, SLOT(onActCloseData()));
    connect(ui->btEditParams, SIGNAL(clicked()), this, SLOT(onActEditParams()));
    connect(ui->btOpenScript, SIGNAL(clicked()), this, SLOT(onActOpenScript()));
    connect(ui->btCloseScript, SIGNAL(clicked()), this, SLOT(onActCloseScript()));

    this->createActions();
    // TODO: There's a ui, form file for this, it shouldn't be implemented in code
    fileMenu = menuBar()->addMenu(tr("&FUGE-LC"));
    fuzzyMenu = menuBar()->addMenu(tr("F&uzzy system"));
    dataMenu = menuBar()->addMenu(tr("&Dataset"));
    paramsMenu = menuBar()->addMenu(tr("&Parameters"));
    scriptMenu = menuBar()->addMenu(tr("&Script"));
    helpMenu = menuBar()->addMenu(tr("&Help"));
    fileMenu->addAction(actRun);
    fileMenu->addAction(actStop);
    dataMenu->addAction(actOpenData);
    dataMenu->addAction(actCloseData);
    fuzzyMenu->addAction(actNewFuzzy);
    fuzzyMenu->addAction(actOpenFuzzy);
    fuzzyMenu->addAction(actCloseFuzzy);
    fuzzyMenu->addAction(actSaveFuzzy);
    fuzzyMenu->addAction(actEditFuzzy);
    fuzzyMenu->addAction(actValidFuzzy);
    fuzzyMenu->addAction(actEvalFuzzy);
    fuzzyMenu->addAction(actPredictFuzzy);
    paramsMenu->addAction(actEditParams);
    scriptMenu->addAction(actOpenScript);
    scriptMenu->addAction(actCloseScript);
    scriptMenu->addAction(actRunScript);
    fileMenu->addAction(actQuit);
    helpMenu->addAction(actHelp);
    helpMenu->addAction(actAbout);
    currentOpennedSystem = "";

    // Fuzzy system manual creation not implemented yet...
    ui->btNewFuzzy->setEnabled(false);
}

FugeMain::~FugeMain()
{
    delete computeThread;
    delete listFile;
    delete statsPlot;
    delete aboutDial;
    delete ui;
    delete sMan;
}

/**
  * Method called by main when the application is run in command line mode.
  * It is responsible to set the correct parameters and perform the task specified
  * on the command line.
  *
  * @param dataSet Dataset on which the evolution/evaluation/prediction is performed
  * @param fuzzyFile Name of the fuzzy system specified on the command line (if any)
  * @param eval Evaluation mode flag
  * @param predict Prediction mode flag
  * @param verbose Verbose mode flag
  */
void FugeMain::runFromCmdLine(QString dataSet, QString scriptFile, QString fuzzyFile,
                              bool eval, bool predict, bool verbose)
{
    doRunFromCmd = true;

    // First open the dataset
    QFile file(dataSet);
    file.open(QIODevice::ReadOnly);
    QTextStream csvFile(&file);
    QString line;
    QStringList list;

    // Read the csv file and store info in a double dimension list.
    while (!csvFile.atEnd()) {
        line = csvFile.readLine();
        list = line.split(';');
        listFile->append(list);
    }
    ui->label_dataInfo->setText("<font color = green> Dataset loaded : " + dataSet + "<font>");
    dataLoaded = true;
    // Set the dataset name in the parameters
    SystemParameters& sysParams = SystemParameters::getInstance();
    sysParams.setVerbose(verbose);

    // Check whether we make an evolution or a validation/prediction
    if (eval || predict) {
        fSystemVars = new FuzzySystem();
        fSystemRules = new FuzzySystem();
        ComputeThread::bestFSystem = fSystemVars;
        connect(this, SIGNAL(openFuzzySystem(QString)), fSystemVars, SLOT(loadFromFile(QString)));
        connect(this, SIGNAL(openFuzzySystem(QString)), fSystemRules, SLOT(loadFromFile(QString)));
        currentOpennedSystem = fuzzyFile;
        emit openFuzzySystem(fuzzyFile);
        ui->label_fuzzyInfo->setText("<font color = green> Fuzzy system loaded : "  + fuzzyFile + "<font>");
        this->actSaveFuzzy->setEnabled(true);
        this->actCloseFuzzy->setEnabled(true);
        this->actEvalFuzzy->setEnabled(true);
        this->actValidFuzzy->setEnabled(true);
        this->actPredictFuzzy->setEnabled(true);
        this->actEditFuzzy->setEnabled(true);
        ui->btValidate->setEnabled(true);
        ui->btEvaluate->setEnabled(true);
        ui->btPredict->setEnabled(true);
        ui->btSaveFuzzy->setEnabled(true);
        ui->btEditFuzzy->setEnabled(true);
        ui->btCloseFuzzy->setEnabled(true);
        sysParams.setDatasetName(dataSet);
        if (eval) {
            this->onActEvalFuzzy(true, true);
            std::cout << "[Fitness] : " << ComputeThread::bestFSystem->getFitness() << std::endl;
            std::cout << "[Sensitivity] : " << ComputeThread::bestFSystem->getSensitivity() << std::endl;
            std::cout << "[Specificity] : " << ComputeThread::bestFSystem->getSpecificity() << std::endl;
            std::cout << "[Accuracy] : " << ComputeThread::bestFSystem->getAccuracy() << std::endl;
            std::cout << "[PPV] : " << ComputeThread::bestFSystem->getPpv() << std::endl;
            std::cout << "[RMSE] : " << ComputeThread::bestFSystem->getRmse() << std::endl;
            std::cout << "[RRSE] : " << ComputeThread::bestFSystem->getRrse() << std::endl;
            std::cout << "[RAE] : " << ComputeThread::bestFSystem->getRae() << std::endl;
            std::cout << "[MSE] : " << ComputeThread::bestFSystem->getMse() << std::endl;
            std::cout << "[ADM] : " << ComputeThread::bestFSystem->getDistanceThreshold() << std::endl;
            std::cout << "[MDM] : " << ComputeThread::bestFSystem->getDistanceMinThreshold() << std::endl;
            std::cout << "[SIZE] : " << ComputeThread::bestFSystem->getDontCare() << std::endl;
            std::cout << "[OverLearn] : " << ComputeThread::bestFSystem->getOverLearn() << std::endl;
        }
        else if (predict) {
            this->onActPredictFuzzy(true);
        }
        this->close();
    }
    else {
        // Then open the script
        scriptLoaded = true;
        ui->label_scriptInfo->setText("<font color = green> Script loaded : " + scriptFile +"<font>");
        ui->label_paramInfo->setText("<font color = green> Parameters loaded from script <font>");
        sMan->setScriptFileName(scriptFile);
        sMan->readScript();
        sMan->start();
        this->actSaveFuzzy->setEnabled(true);
    }
}

/**
  * Create the actions available in the menus.
  */
void FugeMain::createActions()
{
    // TODO: It's difficult to update and to maintain.
    // All default values can be set on the form files as the default text.
    actRun = new QAction(tr("&Run..."), this);
    actStop = new QAction(tr("&Stop"), this);
    actOpenData = new QAction(tr("&Open dataset..."), this);
    actCloseData = new QAction(tr("&Close dataset"), this);
    actNewFuzzy = new QAction(tr("&New fuzzy system"), this);
    actOpenFuzzy = new QAction(tr("&Open fuzzy system..."), this);
    actCloseFuzzy = new QAction(tr("&Close fuzzy system"), this);
    actSaveFuzzy = new QAction(tr("&Save fuzzy system..."), this);
    actEditFuzzy = new QAction(tr("&Edit fuzzy system..."), this);
    actValidFuzzy = new QAction(tr("&Validate fuzzy system..."), this);
    actEvalFuzzy = new QAction(tr("Ev&aluate fuzzy system..."), this);
    actPredictFuzzy = new QAction(tr("&Predict fuzzy system..."), this);
    actEditParams = new QAction(tr("&Edit parameters..."), this);
    actOpenScript= new QAction(tr("&Open script..."), this);
    actCloseScript= new QAction(tr("&Close script"), this);
    actRunScript= new QAction(tr("&Run script"), this);
    actQuit = new QAction(tr("&Quit"), this);
    actHelp = new QAction(tr("&Help..."), this);
    actAbout = new QAction(tr("&About..."), this);
    actRun->setEnabled(false);
    actStop->setEnabled(false);
    actRunScript->setEnabled(false);
    actSaveFuzzy->setEnabled(false);
    actCloseFuzzy->setEnabled(false);
    actEditFuzzy->setEnabled(false);
    actCloseData->setEnabled(false);
    actCloseScript->setEnabled(false);
    actValidFuzzy->setEnabled(false);
    actEvalFuzzy->setEnabled(false);
    actPredictFuzzy->setEnabled(false);
    ui->btStop->setEnabled(false);
    ui->btSaveFuzzy->setEnabled(false);
    ui->btEditFuzzy->setEnabled(false);
    ui->btCloseFuzzy->setEnabled(false);
    ui->btCloseData->setEnabled(false);
    ui->btCloseScript->setEnabled(false);
    ui->btStop->setEnabled(false);

    // TODO: triggered should be auto-linked to slots onXXXTriggered.
    // It's possible to do it by renaming slots (See Qt Documentation)
    connect(actRun, SIGNAL(triggered()), this, SLOT(onActRun()));
    connect(actStop, SIGNAL(triggered()), computeThread, SLOT(onStopEvo()));
    connect(actOpenData, SIGNAL(triggered()), this, SLOT(onActOpenData()));
    connect(actCloseData, SIGNAL(triggered()), this, SLOT(onActCloseData()));
    connect(actNewFuzzy, SIGNAL(triggered()), this, SLOT(onActNewFuzzy()));
    connect(actOpenFuzzy, SIGNAL(triggered()), this, SLOT(onActOpenFuzzy()));
    connect(actCloseFuzzy, SIGNAL(triggered()), this, SLOT(onActCloseFuzzy()));
    connect(actSaveFuzzy, SIGNAL(triggered()), this, SLOT(onActSaveFuzzy()));
    connect(actEditFuzzy, SIGNAL(triggered()), this, SLOT(onActEditFuzzy()));
    connect(actEvalFuzzy, SIGNAL(triggered()), this, SLOT(onActEvalFuzzy()));
    connect(actPredictFuzzy, SIGNAL(triggered()), this, SLOT(onActPredictFuzzy()));
    connect(actEditParams, SIGNAL(triggered()), this, SLOT(onActEditParams()));
    connect(actOpenScript, SIGNAL(triggered()), this, SLOT(onActOpenScript()));
    connect(actCloseScript, SIGNAL(triggered()), this, SLOT(onActCloseScript()));
    connect(actRunScript, SIGNAL(triggered()), this, SLOT(onActRunScript()));
    connect(actQuit, SIGNAL(triggered()), this, SLOT(onActQuit()));
    connect(actHelp, SIGNAL(triggered()), this, SLOT(onActHelp()));
    connect(actAbout, SIGNAL(triggered()), this, SLOT(onActAbout()));
}

/**
 * @brief FugeMain::getNewFuzzySystem Returns a new fuzzy system fully loaded.
 * @param listFile
 * @return a new loaded SystemFuzzy
 */
FuzzySystem* FugeMain::getNewFuzzySystem(QList<QStringList>* listFile){
    FuzzySystem *fSystem = new FuzzySystem();
    ComputeThread::sysParams = &SystemParameters::getInstance();
    fSystem->setParameters(ComputeThread::sysParams->getNbRules(), ComputeThread::sysParams->getNbVarPerRule(), ComputeThread::sysParams->getNbOutVars(),
                           ComputeThread::sysParams->getNbInSets(), ComputeThread::sysParams->getNbOutSets(), ComputeThread::sysParams->getInVarsCodeSize(),
                           ComputeThread::sysParams->getOutVarsCodeSize(), ComputeThread::sysParams->getInSetsCodeSize(), ComputeThread::sysParams->getOutSetsCodeSize(),
                           ComputeThread::sysParams->getInSetsPosCodeSize(), ComputeThread::sysParams->getOutSetPosCodeSize());
    fSystem->loadData(listFile);
    return fSystem;
}

/**
  * Method called when a run needs to be performed. It sets the interface to the required
  * sate and runs the computation thread.
 */
void FugeMain::onActRun()
{
    if(fSystemRules != 0)
        delete fSystemRules;
    if(fSystemVars != 0)
        delete fSystemVars;
    emit clearStats();

    ui->btStop->setEnabled(true);
    ui->btRun->setEnabled(false);
    if ((dataLoaded && scriptLoaded) || (dataLoaded && paramsLoaded)) {
        fSystemVars = getNewFuzzySystem(listFile);
        fSystemRules = getNewFuzzySystem(listFile);
        // At least attribute it a pointer.
        ComputeThread::bestFSystem = fSystemVars;
        ComputeThread::bestFitness = 0;
        ComputeThread::bestFuzzySystemDescription.clear();

        if (computeThread == NULL) {
            computeThread = new ComputeThread();
            connect (computeThread, SIGNAL(computeFinished()), this, SLOT(onComputeFinished()));
            connect(ui->btStop, SIGNAL(clicked()), computeThread, SLOT(onStopEvo()));
        }

        connect(this, SIGNAL(saveFuzzySystem(QString)), computeThread, SLOT(onSaveSystem(QString)));
        computeThread->setFuzzySystem(fSystemVars, fSystemRules);

        computeThread->start();
        isRunning = true;

        // TODO: this logic should be implemented in a method.
        actStop->setEnabled(true);
        ui->btStop->setEnabled(true);
        actCloseData->setEnabled(false);
        actOpenData->setEnabled(false);
        actOpenFuzzy->setEnabled(false);
        actNewFuzzy->setEnabled(false);
        ui->btNewFuzzy->setEnabled(false);
        ui->btOpenFuzzy->setEnabled(false);
        ui->btCloseData->setEnabled(false);
        ui->btOpenFuzzy->setEnabled(false);
        ui->btEditParams->setEnabled(false);

    }
    else if (!dataLoaded) {
        ErrorDialog errDiag;
        errDiag.setError("Error : no data Loaded !");
        errDiag.setInfo("Please choose a dataset");
        errDiag.exec();
    }
    else {
        ErrorDialog errDiag;
        errDiag.setError("Error : no parameters loaded !");
        errDiag.setInfo("Please load from script or edit from menu...");
        errDiag.exec();
    }
    ui->btSaveFuzzy->setEnabled(true);
    this->actSaveFuzzy->setEnabled(true);
    ui->btPredict->setEnabled(false);
    this->actPredictFuzzy->setEnabled(false);
    ui->btValidate->setEnabled(false);
    this->actValidFuzzy->setEnabled(false);
    ui->btEvaluate->setEnabled(false);
    this->actEvalFuzzy->setEnabled(false);
    ui->btEditFuzzy->setEnabled(false);
    this->actEditFuzzy->setEnabled(false);
}

/**
  * Slot called when the user quits the application.
  */
void FugeMain::onActQuit()
{
    this->close();
}

/**
  * Slot called when the user opens a dataset.
  */
void FugeMain::onActOpenData()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open dataset"), "../../../../datasets", "*.csv");
    if (!fileName.isEmpty()) {
        // Clear previous loaded data
        if (dataLoaded)
            listFile->clear();
        QFile file(fileName);
        file.open(QIODevice::ReadOnly);
        QTextStream csvFile(&file);
        QString line;
        QStringList list;

        // Save the name of the dataset
        SystemParameters& sysParams = SystemParameters::getInstance();
        sysParams.setDatasetName(fileName);

        // Read the csv file and store info in a double dimension list.
        while (!csvFile.atEnd()) {
            line = csvFile.readLine();
            list = line.split(';');
            listFile->append(list);
        }
        dataLoaded = true;
        file.close();
        if (paramsLoaded) {
            ui->btRun->setEnabled(true);
            actRun->setEnabled(true);
        }
        if (scriptLoaded) {
            ui->btRunScript->setEnabled(true);
            actRunScript->setEnabled(true);
        }
        ui->label_dataInfo->setText("<font color = green> Dataset loaded : " + fileName + "<font>");
        ui->label_dataVars->setText(QString::number(listFile->at(0).size()-1) + " variables");
        ui->label_dataSamples->setText(QString::number(listFile->size()-1) + " samples");
        actCloseData->setEnabled(true);
        ui->btCloseData->setEnabled(true);
    }
}

/**
  * Slot called when the user closes a dataset.
  */
void FugeMain::onActCloseData()
{
    ui->label_dataInfo->setText("<font color = red> No dataset loaded <font>");
    ui->label_dataVars->setText("");
    ui->label_dataSamples->setText("");
    listFile->clear();
    dataLoaded = false;
    ui->btRun->setEnabled(false);
    actRun->setEnabled(false);
}

/**
  * NOT IMPLEMENTED YET !!!
  */
void FugeMain::onActNewFuzzy()
{
#if 0
    /*ComputeThread::bestFSystem = new FuzzySystem(listFile, sysParams.getNbRules(), sysParams.getNbVarPerRule(), sysParams.getNbOutVars(),
                              sysParams.getNbInSets(), sysParams.getNbOutSets(), sysParams.getInVarsCodeSize(),
                              sysParams.getOutVarsCodeSize(), sysParams.getInSetsCodeSize(), sysParams.getOutSetsCodeSize(),
                              sysParams.getInSetsPosCodeSize(), sysParams.getOutSetPosCodeSize());*/

    if (!dataLoaded) {
        ErrorDialog errDiag;
        errDiag.setError("Error : no data Loaded !");
        errDiag.setInfo("Please choose a dataset");
        errDiag.exec();
    }
    else {

        // NOT IMPLEMENTED YET ////
        ErrorDialog errDiag;
        errDiag.setError("Error : functionnality not implemented yet !");
        errDiag.setInfo("");
        errDiag.exec();
        return;
        ///////////////////////////

        ComputeThread::bestFSystem = new FuzzySystem();
        ComputeThread::bestFSystem->loadData(listFile);
        //connect(this, SIGNAL(saveFuzzySystem(QString, float)), ComputeThread::bestFSystem, SLOT(saveToFile(QString, float)));
        this->actSaveFuzzy->setEnabled(true);
        this->actCloseFuzzy->setEnabled(true);
        this->actEvalFuzzy->setEnabled(true);
        this->actValidFuzzy->setEnabled(true);
        this->actPredictFuzzy->setEnabled(true);
        this->actEditFuzzy->setEnabled(true);
        ui->btEvaluate->setEnabled(true);
        ui->btValidate->setEnabled(true);
        ui->btPredict->setEnabled(true);
        ui->btSaveFuzzy->setEnabled(true);
        ui->btEditFuzzy->setEnabled(true);
        ui->btCloseFuzzy->setEnabled(true);
    }
#endif
}

/**
  * Slot called when the user opens a fuzzy system file.
  */
void FugeMain::onActOpenFuzzy()
{
    SystemParameters& sysParams = SystemParameters::getInstance();


    QFile tempFile(sysParams.getSavePath() + "temp.ffs");
    if (tempFile.exists())
        tempFile.remove();

    //connect(this, SIGNAL(evalFuzzySystem()), ComputeThread::bestFSystem, SLOT(doEvaluateFitness()));
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open fuzzy system"), sysParams.getSavePath()+"fuzzySystems", "*.ffs");
    if (!fileName.isEmpty()) {
        if (ComputeThread::bestFSystem != 0) {
            delete ComputeThread::bestFSystem;
            ComputeThread::bestFSystem = NULL;
        }
        ComputeThread::bestFSystem = new FuzzySystem();
        connect(this, SIGNAL(openFuzzySystem(QString)), ComputeThread::bestFSystem, SLOT(loadFromFile(QString)));
        currentOpennedSystem = fileName;
        emit openFuzzySystem(fileName);

        // TODO: this logic should be in a function.
        ui->label_fuzzyInfo->setText("<font color = green> Fuzzy system loaded : "  + fileName + "<font>");
        this->actSaveFuzzy->setEnabled(true);
        this->actCloseFuzzy->setEnabled(true);
        this->actEvalFuzzy->setEnabled(true);
        this->actValidFuzzy->setEnabled(true);
        this->actPredictFuzzy->setEnabled(true);
        this->actEditFuzzy->setEnabled(true);
        ui->btValidate->setEnabled(true);
        ui->btEvaluate->setEnabled(true);
        ui->btPredict->setEnabled(true);
        ui->btSaveFuzzy->setEnabled(true);
        ui->btEditFuzzy->setEnabled(true);
        ui->btCloseFuzzy->setEnabled(true);
    }
}

/**
  * Slot called when the user closes a fuzzy system file.
  */
void FugeMain::onActCloseFuzzy()
{
    SystemParameters& sysParams = SystemParameters::getInstance();

    currentOpennedSystem = "";
    if (ComputeThread::bestFSystem != 0) {
        delete ComputeThread::bestFSystem;
        ComputeThread::bestFSystem = 0;
    }
    QFile tempFile(sysParams.getSavePath()+"temp.ffs");
    if (tempFile.exists())
        tempFile.remove();
    // Clear previous loaded data
    if (dataLoaded)
        this->onActCloseData();

    // TODO: This logic should be in a function.
    ui->label_fuzzyInfo->setText("<font color = ""#FF6F00""> No fuzzy system loaded<font>");
    this->actSaveFuzzy->setEnabled(false);
    this->actCloseFuzzy->setEnabled(false);
    this->actEditFuzzy->setEnabled(false);
    this->actEvalFuzzy->setEnabled(false);
    this->actValidFuzzy->setEnabled(false);
    this->actPredictFuzzy->setEnabled(false);
    ui->btValidate->setEnabled(false);
    ui->btEvaluate->setEnabled(false);
    ui->btPredict->setEnabled(false);
    ui->btSaveFuzzy->setEnabled(false);
    ui->btEditFuzzy->setEnabled(false);
    ui->btCloseFuzzy->setEnabled(false);
}

/**
  * Slot called when the user saves a fuzzy system file.
  */
void FugeMain::onActSaveFuzzy()
{
    SystemParameters& sysParams = SystemParameters::getInstance();

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save fuzzy system"), sysParams.getSavePath()+"fuzzySystems" , "*.ffs");
    if (!fileName.isEmpty()) {
        // If the evolution is running, we ask the evaluation operator to save the best system
        if (isRunning) {
            emit saveFuzzySystem(fileName);
        }
        // If the evolution is finished, fSystem is the best system
        else {
            CoevStats& stats = CoevStats::getInstance();
            ComputeThread::bestFSystem->saveToFile(fileName, stats.getFitMaxPop1());
        }
    }
}

/**
  * Slot called when the user edits a fuzzy system file.
  */
void FugeMain::onActEditFuzzy()
{
    SystemParameters& sysParams = SystemParameters::getInstance();

    QString name = ComputeThread::bestFSystem->getInVar(0)->getName();

    // Ensure that a temp directory exists. If not we create one.
    QDir tempDir;
    if (!tempDir.exists(sysParams.getSavePath()+"temp")) {
        tempDir.mkdir(sysParams.getSavePath()+"temp");
    }

    FuzzyEditor fEditor(this, ComputeThread::bestFSystem);
    if (currentOpennedSystem == "")
        currentOpennedSystem = QString(sysParams.getSavePath()+"temp/currentBest_") + QString::number(getpid()) + QString(".ffs");

    fEditor.setSystemFile(currentOpennedSystem);

    fEditor.setDataFile(listFile);
    fEditor.exec();
}

/**
  * Slot called when the user asks for a prediction.
  */
void FugeMain::onActPredictFuzzy(bool fromCmd)
{
    SystemParameters& sysParams = SystemParameters::getInstance();
    QString fileName;

    if (!fromCmd)
        fileName = QFileDialog::getOpenFileName(this, tr("Open a test dataset (WITHOUT OUPTUT VALUES)"), "../../../../datasets", "*.csv");
    else {
        fileName = sysParams.getDatasetName();
    }

    if (!fileName.isEmpty()) {
        // Clear previous loaded data
        if (dataLoaded)
            listFile->clear();
        QFile file(fileName);
        file.open(QIODevice::ReadOnly);
        QTextStream csvFile(&file);
        QString line;
        QStringList list;

        // Read the csv file and store info in a double dimension list.
        while (!csvFile.atEnd()) {
            line = csvFile.readLine();
            list = line.split(';');
            listFile->append(list);
        }
        ComputeThread::bestFSystem->loadData(listFile);
        dataLoaded = true;

        file.close();

        int nbOutVars = sysParams.getNbOutVars();

        evalPlot = new EvalPlot();
        evalPlot->loadData(listFile);
        QVector<float> computedResults;
        QVector<float> reverseComputedResults;
        QVector<float> predictedResults;

        computedResults = ComputeThread::bestFSystem->doEvaluateFitness();

        // Reorder the results in order to simplify their display by EvalPlot
        if(nbOutVars > 1) {
            reverseComputedResults.resize(computedResults.size());
            for (int i = 0; i <  nbOutVars; i++) {
                for (int k = 0; k < listFile->size()-1; k++) {
                    reverseComputedResults.replace(i*(listFile->size()-1) + k, computedResults.at(k*nbOutVars+i));
                }
            }
        }
        else {
            reverseComputedResults.resize(computedResults.size());
            for (int k = 0; k < listFile->size()-1; k++) {
                reverseComputedResults.replace(k, computedResults.at(k));
            }
        }

        evalPlot->setPredictive(true);
        evalPlot->setNbOutVars(nbOutVars);
        predictedResults.resize(reverseComputedResults.size());
        // Compute the predicted results by applying the threshold
        for (int i = 0; i < reverseComputedResults.size(); i++) {
            predictedResults.replace(i, ComputeThread::bestFSystem->threshold(i / (reverseComputedResults.size()/nbOutVars), reverseComputedResults.at(i)));
        }

        evalPlot->setFitness(ComputeThread::bestFSystem->getFitness());
        evalPlot->setSensitivity(ComputeThread::bestFSystem->getSensitivity());
        evalPlot->setSpecificity(ComputeThread::bestFSystem->getSpecificity());
        evalPlot->setAccuracy(ComputeThread::bestFSystem->getAccuracy());
        evalPlot->setPpv(ComputeThread::bestFSystem->getPpv());
        evalPlot->setRmse(ComputeThread::bestFSystem->getRmse());
        evalPlot->setRrse(ComputeThread::bestFSystem->getRrse());
        evalPlot->setRae(ComputeThread::bestFSystem->getRae());
        evalPlot->setMse(ComputeThread::bestFSystem->getMse());
        evalPlot->setAdm(ComputeThread::bestFSystem->getDistanceThreshold());
        evalPlot->setMdm(ComputeThread::bestFSystem->getDistanceMinThreshold());
        evalPlot->setSize(ComputeThread::bestFSystem->getDontCare());
        evalPlot->setOverLearn(ComputeThread::bestFSystem->getOverLearn());
        evalPlot->setMesuredValues(reverseComputedResults);
        evalPlot->setPredictedValues(predictedResults);
        evalPlot->setDescription(ComputeThread::bestFSystem->getSystemDescritpion());
        if (!fromCmd) {
            evalPlot->exec();
        }
        else {
            evalPlot->saveEval(fileName);
        }
        delete evalPlot;
    }
}

/**
  * Slot called when the user asks for a validation.
  */
void FugeMain::onActValidFuzzy()
{
    this->onActEvalFuzzy(true);
}

/**
  * Slot called when the user asks for an evalutation.
  */
void FugeMain::onActEvalFuzzy(bool doValid, bool fromCmd)
{
    QVector<float> computedResults;
    QVector<float> reverseComputedResults;
    QVector<float> expectedResults;
    QVector<float> predictedResults;
    SystemParameters& sysParams = SystemParameters::getInstance();

    QString fileName;

    if (doValid) {
        // Retrieve the dataset corresponding to the system according to the name in the XML
        fileName = sysParams.getDatasetName();
        QFile file(fileName);
        //fileExists = file.exists();
    }
    else {
        fileName = QFileDialog::getOpenFileName(this, tr("Open a test dataset"), "../../../../datasets", "*.csv");
    }

    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly)) {
        // Clear previous loaded data
        if (dataLoaded)
            listFile->clear();
        QTextStream csvFile(&file);
        QString line;
        QStringList list;

        // Read the csv file and store info in a double dimension list.
        while (!csvFile.atEnd()) {
            line = csvFile.readLine();
            list = line.split(';');
            listFile->append(list);
        }
    }
    else {
        ErrorDialog errDiag;
        errDiag.setError("Error : dataset " + sysParams.getDatasetName() + " not found !! ");
        errDiag.setInfo("Please load the corresponding dataset manually !");
        errDiag.exec();
        return;
    }
    ComputeThread::bestFSystem->loadData(listFile);
    dataLoaded = true;
    file.close();

    int nbOutVars = sysParams.getNbOutVars();
    int nbInVars = listFile->at(0).size() - 1 - nbOutVars;

    expectedResults.resize((listFile->size()-1)*nbOutVars);

    if (/*dataLoaded*/1) {
        evalPlot = new EvalPlot();
        evalPlot->loadData(listFile);
        for (int k = 0; k < nbOutVars; k++) {
            for (int i = 1, j = 0; i < listFile->size(); i++, j++) {
                expectedResults.replace((listFile->size()-1)*k + j, (listFile->at(i).at(nbInVars+k+1)).toFloat());
            }
        }

        computedResults = ComputeThread::bestFSystem->doEvaluateFitness();

        predictedResults.resize(computedResults.size());

        // Reorder the results in order to simplify their display by EvalPlot
        if(nbOutVars > 1) {
            reverseComputedResults.resize(computedResults.size());
            for (int i = 0; i <  nbOutVars; i++) {
                for (int k = 0; k < listFile->size()-1; k++) {
                    reverseComputedResults.replace(i*(listFile->size()-1) + k, computedResults.at(k*nbOutVars+i));
                }
            }
        }
        else {
            reverseComputedResults.resize(computedResults.size());
            for (int k = 0; k < listFile->size()-1; k++) {
                reverseComputedResults.replace(k, computedResults.at(k));
            }
        }

        // Compute the predicted results by applying the threshold
        predictedResults.resize(reverseComputedResults.size());
        for (int i = 0; i < reverseComputedResults.size(); i++) {
            predictedResults.replace(i, ComputeThread::bestFSystem->threshold(i / (reverseComputedResults.size()/nbOutVars), reverseComputedResults.at(i)));
        }

        // Update the GUI
        if (!fromCmd) {
            evalPlot->setFitness(ComputeThread::bestFSystem->getFitness());
            evalPlot->setSensitivity(ComputeThread::bestFSystem->getSensitivity());
            evalPlot->setSpecificity(ComputeThread::bestFSystem->getSpecificity());
            evalPlot->setAccuracy(ComputeThread::bestFSystem->getAccuracy());
            evalPlot->setPpv(ComputeThread::bestFSystem->getPpv());
            evalPlot->setRmse(ComputeThread::bestFSystem->getRmse());
            //MODIF - BUJARD - 1.04.2010
            // add for regression problem
            evalPlot->setRrse(ComputeThread::bestFSystem->getRrse());
            evalPlot->setRae(ComputeThread::bestFSystem->getRae());
            evalPlot->setMse(ComputeThread::bestFSystem->getMse());
            evalPlot->setAdm(ComputeThread::bestFSystem->getDistanceThreshold());
            evalPlot->setMdm(ComputeThread::bestFSystem->getDistanceMinThreshold());
            evalPlot->setSize(ComputeThread::bestFSystem->getDontCare());
            evalPlot->setOverLearn(ComputeThread::bestFSystem->getOverLearn());
            // FIN - MODIF - BUJARD - 1.04.2010
            evalPlot->setNbOutVars(nbOutVars);
            evalPlot->setMesuredValues(reverseComputedResults);
            evalPlot->setExpectedValues(expectedResults);
            evalPlot->setPredictedValues(predictedResults);
            evalPlot->setDescription(ComputeThread::bestFSystem->getSystemDescritpion());
            evalPlot->setPredictive(false);
            evalPlot->exec();
            delete evalPlot;
        }
    }
}

/**
  * Slot called when the user edits the parameters.
  */
void FugeMain::onActEditParams()
{
    scriptLoaded = false;
    if(!editParams)
        editParams = new EditParamsDialog(this, &paramsLoaded, scriptLoaded);
    editParams->show();
    if (dataLoaded) {
        paramsLoaded = true;
        ui->btRun->setEnabled(true);
        actRun->setEnabled(true);
        onActCloseScript();
        ui->label_paramInfo->setText("<font color = green> Parameters loaded from menu<font>");
    }

}

/**
  * Slot called when the user opens a script file.
  */
void FugeMain::onActOpenScript()
{
    QString fileName = QFileDialog::getOpenFileName(NULL, tr("Open script File"), "scripts", "*.fs");
    if (!fileName.isEmpty()) {
        scriptLoaded = true;
        paramsLoaded = true;
        actCloseScript->setEnabled(true);
        ui->btCloseScript->setEnabled(true);
        this->actCloseScript->setEnabled(true);
        ui->label_scriptInfo->setText("<font color = green> Script loaded : " + fileName +"<font>");
        ui->label_paramInfo->setText("<font color = green> Parameters loaded from script <font>");
        if (dataLoaded) {
            ui->btRunScript->setEnabled(true);
            actRunScript->setEnabled(true);
            ui->btRun->setEnabled(false);
            actRun->setEnabled(false);
        }

        sMan->setScriptFileName(fileName);
        sMan->readScript();
    }
}

/**
  * Slot called when the user closes a script file.
  */
void FugeMain::onActCloseScript()
{
    sMan->setScriptFileName(NULL);
    ui->label_scriptInfo->setText("<font color = ""#FF6F00""> No script loaded <font>");
    if (!paramsLoaded)
        ui->label_paramInfo->setText("<font color = red> No parameters selected <font>");
    ui->btRunScript->setEnabled(false);
    actRunScript->setEnabled(false);
    scriptLoaded = false;
}

/**
  * Slot called when the user performs a script run.
  */
void FugeMain::onActRunScript()
{
    if (dataLoaded) {
        sMan->start();
        actStop->setEnabled(true);
        ui->btStop->setEnabled(true);
        ui->btEditParams->setEnabled(false);
        actSaveFuzzy->setEnabled(true);
        ui->btSaveFuzzy->setEnabled(true);
    }
    else {
        ErrorDialog errDiag;
        errDiag.setError("Error : no data loaded");
        errDiag.setInfo("Please choose a dataset !");
        errDiag.exec();
    }
}

/**
  * Slot called when the user accesses the About dialog.
  */
void FugeMain::onActAbout()
{
    aboutDial->exec();
}

/**
  * Slot called by the computational thread when an evolution run is finished.
  */
void FugeMain::onComputeFinished()
{
    SystemParameters& sysParams = SystemParameters::getInstance();

    computeThread->wait();
    delete computeThread;
    computeThread = 0;

    isRunning = false;
    actCloseData->setEnabled(true);
    actOpenFuzzy->setEnabled(true);
    actNewFuzzy->setEnabled(true);
    actOpenData->setEnabled(true);
    actStop->setEnabled(false);
    ui->btStop->setEnabled(false);
    ui->btRun->setEnabled(true);
    ui->btCloseData->setEnabled(true);
    ui->btOpenFuzzy->setEnabled(true);
    ui->btNewFuzzy->setEnabled(true);
    ui->btOpenData->setEnabled(true);
    ui->btEditParams->setEnabled(true);

    emit closeStats();

    // Dont interrupt the script execution with a save dialog
    if (!scriptLoaded) {
        InfoDialog infDiag;
        int isOk;

        infDiag.setWindowTitle("Info");
        infDiag.setInfo("Co-evolution finished !");
        infDiag.setQuestion("Do you want to save the best fuzzy system ?");
        isOk = infDiag.exec();

        if (isOk == 1) {
            QFileDialog saveFileDiag(this, tr("Save fuzzy system"), sysParams.getSavePath()+"fuzzySystems", "*.ffs");
            saveFileDiag.setAcceptMode(QFileDialog::AcceptSave);
            saveFileDiag.setDefaultSuffix("ffs");
            bool ifDiagOk = saveFileDiag.exec();
            if (ifDiagOk) {
                QString fileName = saveFileDiag.selectedFiles().at(0);
                CoevStats& stats = CoevStats::getInstance();
                ComputeThread::bestFSystem->saveToFile(fileName, stats.getFitMaxPop1());
            }
        }
    }
    // Script execution
    else {
        QString newName;
        QTextStream newNameStream(&newName);
        QTime time;
        QString fileN;
        // Ensure that a temp directory exists. If not we create one.
        QDir tempDir;
        if (!tempDir.exists(sysParams.getSavePath()+"temp")) {
            tempDir.mkdir(sysParams.getSavePath()+"temp");
        }
        fileN = QString(sysParams.getSavePath()+"temp/currentBest_") + QString::number(getpid()) + QString(".ffs");
        QFile file(fileN);
        SystemParameters& sysParams = SystemParameters::getInstance();
        CoevStats& stats = CoevStats::getInstance();

        // Ensure that a fuzzy systems directory exists. If not we create one.
        QDir logsDir;
        if (!logsDir.exists(sysParams.getSavePath()+"fuzzySystems")) {
            logsDir.mkdir(sysParams.getSavePath()+"fuzzySystems");
        }

        int randomNumber = rand();
        newNameStream << sysParams.getSavePath() +"fuzzySystems/" << sysParams.getExperimentName() << "_" << time.currentTime().toString() << "." << randomNumber << "Gen" << sysParams.getMaxGenPop1()
                      << "_" << "Pop" << stats.getSizePop1() << "_" << "Rules" << QString::number(sysParams.getNbRules()) << "_" << "Elt" << QString::number(sysParams.getEliteSizePop1())
                      << "_" << "CX" << QString::number(sysParams.getCxProbPop1()) << "_" << "MutI" << sysParams.getMutFlipIndPop1() << "_" << "MutB" << sysParams.getMutFlipBitPop1()
                      << "_" << "FixedV" << sysParams.getFixedVars() << "_" << "Fit" << stats.getFitMaxPop1() << ".ffs";
        newNameStream.flush();
        file.copy(newName);
    }

    ui->label_fuzzyInfo->setText("<font color = green> Fuzzy system found by co-evolution<font>");
    this->actSaveFuzzy->setEnabled(true);
    this->actCloseFuzzy->setEnabled(true);
    this->actEvalFuzzy->setEnabled(true);
    this->actValidFuzzy->setEnabled(true);
    this->actPredictFuzzy->setEnabled(true);
    this->actStop->setEnabled(false);
    this->actEditFuzzy->setEnabled(true);
    ui->btStop->setEnabled(false);
    ui->btValidate->setEnabled(true);
    ui->btEvaluate->setEnabled(true);
    ui->btPredict->setEnabled(true);
    ui->btSaveFuzzy->setEnabled(true);
    ui->btEditFuzzy->setEnabled(true);
    ui->btCloseFuzzy->setEnabled(true);

    // Allow the next script execution
    scriptSema.release();
}

/**
  * Slot called by the script manager thread when an evolution run is finished.
  */
void FugeMain::onScriptFinished()
{
    if (doRunFromCmd)
        this->close();
}

/**
  * Slot called when the user accesses the Help dialog.
  */
void FugeMain::onActHelp()
{
    help->show();
}

void FugeMain::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

/**
  * Slot called when the user quits the application.
  */
void FugeMain::closeEvent(QCloseEvent*)
{
    SystemParameters& sysParams = SystemParameters::getInstance();

    // Delete the temporary fuzzy system file
    QString fileN = QString(sysParams.getSavePath()+"temp/currentBest_") + QString::number(getpid()) + QString(".ffs");
    QString logFile = QString(sysParams.getSavePath()+"temp/running_") + QString::number(getpid()) + QString(".csv");
    QFile file(fileN);
    QFile lFile(logFile);
    file.remove();
    lFile.remove();

    statsPlot->close();
    this->close();
}
