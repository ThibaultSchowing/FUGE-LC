/**
  * @file   statsplot.cpp
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
  * @class StatsPlot
  *
  * @brief This class is responsible to manage the script execution of the application.
  * The parameters are read from the scriptfile and the doRun() function is evaluated.
  */
#include "statsplot.h"


StatsPlot::StatsPlot(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::StatsPlot)
{
    m_ui->setupUi(this);

    bestFitPop1 = 0.0;
    bestFitPop2 = 0.0;
    isShowed = false;
    SystemParameters& sysParams = SystemParameters::getInstance();

    // Ensure that a logs directory exists. If not we create one.
    QDir tempDir;
    if (!tempDir.exists(sysParams.getSavePath()+"temp")) {
        tempDir.mkdir(sysParams.getSavePath()+"temp");
    }
    logFileName = QString(sysParams.getSavePath()+"temp/running_") +
            QString::number(QCoreApplication::applicationPid()) + QString(".csv");
    myPlot = new QwtPlot((QWidget*) this);
    legend = new QwtLegend();
    myPlot->setAxisTitle(QwtPlot::xBottom, "Generations");
    myPlot->setAxisTitle(QwtPlot::yLeft, "Fitness");
    xVals = new QVector<double>();
    yValsPop1 = new QVector<double>();
    yValsPop2 = new QVector<double>();
    yValsAvgPop1 = new QVector<double>();
    yValsAvgPop2 = new QVector<double>();
    fitMaxPop1Curve = new QwtPlotCurve("Pop1 : Membership functions (max)");
    fitMaxPop2Curve = new QwtPlotCurve("Pop2 : Rules (max)");
    fitAvgPop1Curve = new QwtPlotCurve("Pop1 : Membership functions (avg)");
    fitAvgPop2Curve = new QwtPlotCurve("Pop2 : Rules (avg)");
    fitMaxPop1Curve->attach(myPlot);
    fitMaxPop2Curve->attach(myPlot);
    fitAvgPop1Curve->attach(myPlot);
    fitAvgPop2Curve->attach(myPlot);
    myPlot->insertLegend(legend, QwtPlot::TopLegend);

    m_ui->horizontalLayout->addWidget(myPlot);
}

StatsPlot::~StatsPlot()
{
    delete m_ui;
    delete xVals;
    delete yValsPop1;
    delete yValsPop2;
}

void StatsPlot::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

/**
  * Sets the name of the log file.
  *
  * @param fileName Name of the log file.
  */
void StatsPlot::setLogFileName(QString fileName)
{
    logFileName = fileName;
}

/**
  * Slot called by CoevEvalOp when new statistics (each generation) are available. The plot
  * and the temporary log file are updated.
  */
void StatsPlot::receiveData(QString name)
{
    CoevStats& stats = CoevStats::getInstance();
    SystemParameters& sysParams = SystemParameters::getInstance();
    if(!xVals->contains(stats.getGenNumber()))
        xVals->append(stats.getGenNumber());
    // These things need to be done only once
    if (xVals->last() < 2) {
        m_ui->label_pops->setText("<font color = red>"+ QString::number(stats.getSizePop1()) +"<font>"+"\\"+"<font color = blue>"+ QString::number(stats.getSizePop2()) +"<font>");
        m_ui->label_exp->setText(sysParams.getExperimentName());
        m_ui->label_rules->setText(QString::number(sysParams.getNbRules()));
        m_ui->label_elite->setText(QString::number(sysParams.getEliteSizePop1()));
        m_ui->label_cx->setText(QString::number(sysParams.getCxProbPop1()));
        m_ui->label_mutInd->setText(QString::number(sysParams.getMutFlipIndPop1()));
        m_ui->label_mutBit->setText(QString::number(sysParams.getMutFlipBitPop1()));
        m_ui->label_cooperators->setText(QString::number(sysParams.getNbCooperators()));
    }
    // Not possible with the current set up, this slot being called by two threads.
    //        //Log file
    //        fitLogFile = new QFile(sysParams.getSavePath()+logFileName);
    //        fitLogFile->open(QIODevice::ReadWrite | QIODevice::Text);

    //        QTextStream out(fitLogFile);
    //        out << "                                                                                                                 "
    //            << "                                                                                                                 "
    //            << "                                                                                                                 "
    //            << "                                                                                                                 "
    //            << "                                                                                                                 "
    //            << "                                                                                                                 "
    //            << "                                                                                                                 "
    //            << "                                                                                                                 "
    //            << "                                                                                                                 "
    //            << "                                                                                                                 "
    //            << "                                                                                                                 "
    //            << "                                                                                                                 "
    //            << "                                                                                                                 "
    //            << "                                                                                                                 "
    //            << "                                                                                                                 "
    //            << "                                                                                                                 "
    //            << "                                                                                                                 "
    //            << "                                                                                                                 "
    //            << "                                                                                                                 "
    //            << "                                                                                                                 "
    //            << "                                                                                                               \n";
    //        out << "Fitness Log; FitMax Pop1; FitMax Pop2; FitAvg Pop1; FitAvg Pop2; Sensitivity; Specificity; Accuracy; PPV; RMSE\n";
    //        out.flush();
    //    }

    // Store the best fitnesses

    bestSystemDesc = *stats.getBestSysDesc();

    if(name == "RULES"){
        if (stats.getFitMaxPop2() >= bestFitPop2) {
            bestFitPop2 = stats.getFitMaxPop2();
        }
        if (stats.getFitStdPop2() >= bestDevPop2) {
            bestDevPop2 = stats.getFitStdPop2();
        }

        yValsPop2->append(stats.getFitMaxPop2());
        yValsAvgPop2->append(stats.getFitAvgPop2());
        m_ui->label_max2->setText(QString::number(yValsPop2->last()));
        m_ui->label_avg2->setText(QString::number(yValsAvgPop2->last()));
        m_ui->label_min2->setText(QString::number(stats.getFitMinPop2()));
        m_ui->label_std2->setText(QString::number(stats.getFitStdPop2()));
        m_ui->label_gen2->setText(QString::number(xVals->last()) + " / " + QString::number(sysParams.getMaxGenPop2()));
    }
    else{
        if (stats.getFitMaxPop1() >= bestFitPop1) {
            bestFitPop1 = stats.getFitMaxPop1();
        }
        if (stats.getFitStdPop1() >= bestDevPop1) {
            bestDevPop1 = stats.getFitStdPop1();
        }

        yValsPop1->append(stats.getFitMaxPop1());
        yValsAvgPop1->append(stats.getFitAvgPop1());
        m_ui->label_max1->setText(QString::number(yValsPop1->last()));
        m_ui->label_min1->setText(QString::number(stats.getFitMinPop1()));
        m_ui->label_avg1->setText(QString::number(yValsAvgPop1->last()));
        m_ui->label_gen->setText(QString::number(xVals->last()) + " / " + QString::number(sysParams.getMaxGenPop1()));
        m_ui->label_std1->setText(QString::number(stats.getFitStdPop1()));
    }


    if(name == "RULES"){
        fitMaxPop2Curve->setData(*xVals, *yValsPop2);
        fitMaxPop2Curve->setPen(QPen (Qt::blue,2));
        fitAvgPop2Curve->setData(*xVals, *yValsAvgPop2);
        fitAvgPop2Curve->setPen(QPen (Qt::blue,1,Qt::DashLine));
    } else {
        fitMaxPop1Curve->setData(*xVals, *yValsPop1);
        fitMaxPop1Curve->setPen(QPen (Qt::red,2));
        fitAvgPop1Curve->setData(*xVals, *yValsAvgPop1);
        fitAvgPop1Curve->setPen(QPen (Qt::red,1,Qt::DashLine));
    }

    m_ui->lblSensi->setText(QString::number(stats.getSensi()));
    m_ui->lblSpeci->setText(QString::number(stats.getSpeci()));
    m_ui->lblAccu->setText(QString::number(stats.getAccu()));
    m_ui->lblRmse->setText(QString::number(stats.getRmse()));
    m_ui->lblPpv->setText(QString::number(stats.getPpv()));

    //add information for regression
    m_ui->lblRrse->setText(QString::number(stats.getRrse()));
    m_ui->lblRae ->setText(QString::number(stats.getRae()));
    m_ui->lblMse ->setText(QString::number(stats.getMse()));
    m_ui->lblAdm ->setText(QString::number(stats.getDistanceThreshold()));
    m_ui->lblMdm ->setText(QString::number(stats.getDistanceMinThreshold()));
    m_ui->lblSize ->setText(QString::number(stats.getDontCare()));
    m_ui->lblOverLearn ->setText(QString::number(stats.getOverLearn()));

    //    QTextStream out(fitLogFile);

    //    out << "Gen" << xVals->last() << "; " << yValsPop1->last() << "; " << yValsPop2->last() << "; " << yValsAvgPop1->last() << "; " << yValsAvgPop2->last()
    //         << "; " << stats.getSensi() << "; " << stats.getSpeci() << "; " << stats.getAccu() << "; " << stats.getPpv() << ";" << stats.getRmse() << "\n";

    //    out.flush();
    if(isShowed)
        myPlot->replot();
}

/**
  * Slot called when the stats need to be reset.
  */
void StatsPlot::onClearStats()
{
    xVals->clear();
    yValsPop1->clear();
    yValsPop2->clear();
    yValsAvgPop1->clear();
    yValsAvgPop2->clear();
    if (isShowed)
        myPlot->replot();
}

/**
  * Slot called when the coevolution run is finished. The temporary log file
  * is updated with the summary and is renamed and saved as the final log file
  * for this run.
  */
void StatsPlot::onCloseStats()
{
    CoevStats& stats = CoevStats::getInstance();
    SystemParameters& sysParams = SystemParameters::getInstance();
    // Write the summary at the beginning of the file
    fitLogFile->seek(0);

    QTextStream summary(fitLogFile);

    summary  << "#Summary : \n"
             << "#Best fitness Pop 1;" << bestFitPop1 << "\n"
             << "#Best fitness Pop 2;" << bestFitPop2 << "\n"
             << "#Std deviation Pop 1;" << stats.getFitStdPop1() << "\n"
             << "#Std deviation Pop 2;" << stats.getFitStdPop2() << "\n"
             << "#Sensitivity;" << stats.getSensi() << "\n"
             << "#Specificity;" << stats.getSpeci() << "\n"
             << "#Accuracy;" << stats.getAccu() << "\n"
             << "#PPV;" << stats.getPpv() << "\n"
             << "#RMSE;" << stats.getRmse() << "\n"
             << "#RRSE;" << stats.getRrse() << "\n"
             << "#RAE;"  << stats.getRae() << "\n"
             << "#MSE;" << stats.getMse() << "\n"
             << "#ADM;" << stats.getDistanceThreshold() << "\n"
             << "#MDM;" << stats.getDistanceMinThreshold() << "\n"
             << "#Size;" << stats.getDontCare() << "\n"
             << "#OverLearn;" << stats.getOverLearn() << "\n\n"

             << "#Parameters : \n#Generations;" << sysParams.getMaxGenPop1()
             << "\n#Population size;" << stats.getSizePop1()
             << "\n#Number of rules;"           << sysParams.getNbRules()
             << "\n#Number of var per rule;"     << sysParams.getNbVarPerRule()
             << "\n#Number of output variables;" << sysParams.getNbOutVars()
             << "\n#Number of input sets;"       << sysParams.getNbInSets()
             << "\n#Number of output sets;"      << sysParams.getNbOutSets()
             << "\n#Input sets pos. code size;"  << sysParams.getInSetsPosCodeSize()
             << "\n#Output sets pos. code size;" << sysParams.getOutSetPosCodeSize()
             << "\n#Input vars bit code size;"   << sysParams.getInVarsCodeSize()
             << "\n#Output vars bit code size;"  << sysParams.getOutVarsCodeSize()
             << "\n#Input sets bit code size;"   << sysParams.getInSetsCodeSize()
             << "\n#Output sets bit code size;"  << sysParams.getOutSetsCodeSize()
             << "\n#Elite size;" << sysParams.getEliteSizePop1()
             << "\n#Crossover probability;" << sysParams.getCxProbPop1()
             << "\n#Individual mutation prob.;" << sysParams.getMutFlipIndPop1()
             << "\n#Bit mutation prob.;" << sysParams.getMutFlipBitPop1()
             << "\n#Fixed vars;" << sysParams.getFixedVars()
             << "\n#Sensitivity weight;" << sysParams.getSensiW()
             << "\n#Specificity weight;" << sysParams.getSpeciW()
             << "\n#Accuracy weight;" << sysParams.getAccuracyW()
             << "\n#PPV weight;" << sysParams.getPpvW()
             << "\n#RMSE weight;" << sysParams.getRmseW()
             << "\n#RRSE weight;" << sysParams.getRrseW()
             << "\n#RAE weight;" << sysParams.getRaeW()
             << "\n#MSE weight;" << sysParams.getMseW()
             << "\n#ADM weight;" << sysParams.getDistanceThresholdW()
             << "\n#MDM weight;" << sysParams.getDistanceMinThresholdW()
             << "\n#Size weight;" << sysParams.getDontCareW()
             << "\n#OverLearn weight;" << sysParams.getOverLearnW()
             << "\n\n"

             << "#Fuzzy system : \n" << bestSystemDesc << "\n\n";
    summary.flush();

    fitLogFile->close();

    QString newName;
    QTextStream newNameStream(&newName);
    QTime time;

    int randomNumber = qrand();

    // Ensure that a logs directory exists. If not we create one.
    QDir logsDir;
    if (!logsDir.exists(sysParams.getSavePath()+"logs")) {
        logsDir.mkdir(sysParams.getSavePath()+"logs");
    }

    newNameStream << sysParams.getSavePath()+"logs/" << sysParams.getExperimentName() << "_" << time.currentTime().toString() << "." << randomNumber << "Gen" << sysParams.getMaxGenPop1()
                  << "_" << "Pop" << stats.getSizePop1() << "_" << "Rules" << QString::number(sysParams.getNbRules()) << "_" << "Elt" << QString::number(sysParams.getEliteSizePop1())
                  << "_" << "CX" << QString::number(sysParams.getCxProbPop1()) << "_" << "MutI" << sysParams.getMutFlipIndPop1() << "_" << "MutB" << sysParams.getMutFlipBitPop1()
                  << "_" << "FixedV" << sysParams.getFixedVars() << "_" << "Fit" << bestFitPop1 << ".csv";
    newNameStream.flush();

    fitLogFile->copy(newName);
    fitLogFile->remove();

    bestFitPop1 = 0.0;
    bestFitPop2 = 0.0;}


/**
  * Slot called by the main window to show the plot.
  */
void StatsPlot::onShowGraph()
{
    this->show();
    isShowed = true;
}

/**
  * Slot called when the user closes this dialog.
  */
void StatsPlot::closeEvent(QCloseEvent *event)
{
    isShowed = false;
    event->accept();
}

