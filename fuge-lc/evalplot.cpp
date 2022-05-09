/**
  * @file   evalplot.cpp
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
  * @class EvalPlot
  *
  * @brief This class implements the evaluation/validation/prediction plot. The interface
  * differs a bit according to the mode selected.
  */

#include <iostream>

#include <QFileDialog>
#include <QTextStream>

#include "evalplot.h"
#include "ui_evalplot.h"
#include "systemparameters.h"
#include "coev/coevstats.h"

EvalPlot::EvalPlot(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::EvalPlot)
{
    m_ui->setupUi(this);

    SystemParameters& sysParams = SystemParameters::getInstance();

    isPredictive = false;
    connect(m_ui->btSave, SIGNAL(clicked()), this, SLOT(onSaveEval()));
    connect(m_ui->btOk, SIGNAL(clicked()), this, SLOT(onCloseEval()));
    connect(m_ui->cbOut, SIGNAL(currentIndexChanged(int)), this, SLOT(onSelectOut()));
    connect(m_ui->chbSort, SIGNAL(stateChanged(int)), this, SLOT(onSort()));
    myPlot = new QwtPlot((QWidget*) this);
    legend = new QwtLegend();
    myPlot->setAxisTitle(QwtPlot::xBottom, "Samples");
    myPlot->setAxisTitle(QwtPlot::yLeft, "Output");
    xVals = new QVector<double>();
    yValsMesured = new QVector<double>();
    yValsExpected = new QVector<double>();
    yValsPredicted = new QVector<double>();
    yValsThresh = new QVector<double>();
    valsMesured = new QwtPlotCurve("Measured output");
    valsExpected = new QwtPlotCurve("Expected output");
    valsPredicted = new QwtPlotCurve("Predicted output");
    threshCurve = new QwtPlotCurve("Threshold");

    QwtSymbol sym;
    sym.setStyle(QwtSymbol::XCross);
    sym.setPen(QPen(Qt::red));
    sym.setSize(5);

    //valsMesured->setPen(QPen (Qt::red,2));
    valsMesured->setStyle(QwtPlotCurve::Steps);
    valsMesured->setPen(QPen(Qt::red));
    valsMesured->setSymbol(sym);

    sym.setPen(QPen(Qt::blue));
    //valsExpected->setPen(QPen (Qt::blue,3));
    valsExpected->setStyle(QwtPlotCurve::Steps);
    valsExpected->setPen(QPen(Qt::blue));
    valsExpected->setSymbol(sym);

    valsPredicted->setPen(QPen (Qt::green,1));
    valsPredicted->setStyle(QwtPlotCurve::Steps);
    threshCurve->setPen(QPen (Qt::black,1));
    valsMesured->attach(myPlot);
    valsExpected->attach(myPlot);
    valsPredicted->attach(myPlot);
    if (sysParams.getThreshActivated()) {
        threshCurve->attach(myPlot);
    }
    myPlot->insertLegend(legend, QwtPlot::TopLegend);
    m_ui->btSave->hide();
    m_ui->horizontalLayout->addWidget(myPlot);
}

EvalPlot::~EvalPlot()
{
    delete m_ui;
}

void EvalPlot::changeEvent(QEvent *e)
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
  * Loads the dataset.
  *
  * @param sysData Dataset to be loaded
  */
void EvalPlot::loadData(QList<QStringList>* sysData)
{
    systemData = sysData;
}

/**
  * Set the plot mode. Validation/evaluation have the same display. Prediction
  * is a bit different
  *
  * @param isPredict Flag that indicates whether we are in oredictive mode
  */
void EvalPlot::setPredictive(bool isPredict)
{
    isPredictive = isPredict;
    if (isPredictive)
        m_ui->btSave->show();
}

/**
  * Set the number of output variables.
  *
  * @param nbOutVars Number of output variables
  */
void EvalPlot::setNbOutVars(int nbOutVars)
{
    this->nbOutVars = nbOutVars;
    if (isPredictive) {
        for (int i = 0; i < nbOutVars; i++) {
            m_ui->cbOut->addItem("Out " + QString::number(i));
        }
    }
    else {
        for (int i = nbOutVars-1; i >= 0; i--) {
            m_ui->cbOut->addItem(systemData->at(0).at(systemData->at(0).size()-i-1)+" ");
        }
    }
}

/**
  * Set the values measured by the system
  *
  * @param mesValues Vector containing the mesured values
  */
void EvalPlot::setMesuredValues(QVector<float> mesValues)
{
    mesuredValues = mesValues;
    /// ICI passer la var out en param
    affectMesuredValues(mesuredValues.mid((systemData->size()-1)*m_ui->cbOut->currentIndex(), systemData->size()-1), m_ui->cbOut->currentIndex());
}

/**
  * Do the job of the setMesuredValues method.
  *
  * @param mesValues Vector containing the mesured values
  */
void EvalPlot::affectMesuredValues(QVector<float> mesValues, int outVar)
{
    SystemParameters& sysParams = SystemParameters::getInstance();

    yValsMesured->resize(mesValues.size());
    yValsThresh->resize(mesValues.size());
    for (int i = 0; i < mesValues.size(); i++) {
        yValsMesured->replace(i, mesValues.at(i));
        yValsThresh->replace(i, sysParams.getThresholdVal(outVar));
    }
    xVals->resize(mesValues.size());
    for (int i = 0; i < mesValues.size(); i++) {
        xVals->replace(i, i);
    }
    valsMesured->setData(*xVals, *yValsMesured);
    threshCurve->setData(*xVals, *yValsThresh);
    myPlot->replot();
}

/**
  * Set the expected values according to the training dataset
  *
  * @param expValues Vector containing the expected values
  */
void EvalPlot::setExpectedValues(QVector<float> expValues)
{
    expectedValues = expValues;
    affectExpectedValues(expectedValues.mid((systemData->size()-1)*m_ui->cbOut->currentIndex(), systemData->size()-1));
}

/**
  * Do the job of the setExpectedValues method.
  *
  * @param expValues Vector containing the expected values
  */
void EvalPlot::affectExpectedValues(QVector<float> expValues)
{
    yValsExpected->resize(expValues.size());
    for (int i = 0; i < expValues.size(); i++) {
        yValsExpected->replace(i, expValues.at(i));
    }
    xVals->resize(expValues.size());
    for (int i = 0; i < expValues.size(); i++) {
        xVals->replace(i, i);
    }
    valsExpected->setData(*xVals, *yValsExpected);
    myPlot->replot();
}

/**
  * Set the predicted values (thresholded measured values).
  *
  * @param expValues Vector containing the expected values
  */
void EvalPlot::setPredictedValues(QVector<float> predValues)
{
    predictedValues = predValues;
    affectPredictedValues(predictedValues.mid((systemData->size()-1)*m_ui->cbOut->currentIndex(), systemData->size()-1));
}

/**
  * Do the job of the setPredictedValues method.
  *
  * @param predValues Vector containing the predicted values
  */
void EvalPlot::affectPredictedValues(QVector<float> predValues)
{
    yValsPredicted->resize(predValues.size());
    for (int i = 0; i < predValues.size(); i++) {
        yValsPredicted->replace(i, predValues.at(i));
    }
    xVals->resize(predValues.size());
    for (int i = 0; i < predValues.size(); i++) {
        xVals->replace(i, i);
    }
    valsPredicted->setData(*xVals, *yValsPredicted);
    myPlot->replot();
}

/**
  * Display the description (rules + membership functions) of the system.
  *
  * @param desc Description of the fuzzy systems
  */
void EvalPlot::setDescription(QString desc)
{
    SystemParameters& sysParams = SystemParameters::getInstance();

    this->desc = desc;
    m_ui->tbRules->setText("Rules : \n" + desc);
    m_ui->tbFitness->setText("Fitness = " + QString::number(fitness) + "\n\n"
                             + "Sensitivity (" + QString::number(sysParams.getSensiW()) + ") = " + QString::number(sensitivity)
                             + "\nSpecificity (" + QString::number(sysParams.getSpeciW()) + ") = " + QString::number(specificity)
                             + "\nAccuracy (" + QString::number(sysParams.getAccuracyW()) + ") = " + QString::number(accuracy)
                             + "\nPPV  (" + QString::number(sysParams.getPpvW())  +") = " + QString::number(ppv)
                             + "\nRMSE (" + QString::number(sysParams.getRmseW()) +") = " + QString::number(rmse)
                             + "\nRRSE (" + QString::number(sysParams.getRrseW()) +") = " + QString::number(rrse)
                             + "\nRAE  (" + QString::number(sysParams.getRaeW())  +") = " + QString::number(rae)
                             + "\nMSE  (" + QString::number(sysParams.getMseW())  +") = " + QString::number(mse)
                             + "\nADM  (" + QString::number(sysParams.getDistanceThresholdW())  +") = " + QString::number(adm)
                             + "\nMDM  (" + QString::number(sysParams.getDistanceMinThresholdW())  +") = " + QString::number(mdm)
                             + "\nSize  (" + QString::number(sysParams.getDontCareW())  +") = " + QString::number(size)
                             + "\nOver Learn  (" + QString::number(sysParams.getOverLearnW())  +") = " + QString::number(overLearn)
                            );
}

/**
  * Save the prediction in a dataset without output variables.
  *
  * @param fileName Name of the file to be saved
  */
void EvalPlot::saveEval(QString fileName)
{
    QFile file(fileName);
    file.remove();
    file.open(QIODevice::ReadWrite | QIODevice::Text);
    QTextStream contents(&file);

    for (int i = 0; i < systemData->size(); i++) {
        // Fill the file with the original data
        for (int k = 0; k < systemData->at(i).size(); k++) {
            contents << systemData->at(i).at(k) << ";";
        }
        // Add the output predictions to the file
        for (int l = 0; l < nbOutVars; l++) {
            if (i == 0) {
                contents << "Predicted output "  << l << ";";
            }
            else {
                contents << predictedValues.at((i-1)+(systemData->size()-1)*l);
                contents << ";";
            }
        }
        contents << "\n";
    }

    contents.flush();
    file.close();
}

/**
  * Same as saveEval but pops a file selection dialog.
  */
void EvalPlot::onSaveEval()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save fuzzy system"), ".csv", "*.csv");
    if (fileName != NULL) {
        this->saveEval(fileName);
    }
}

/**
  * Slot called when the user quits the dialog. All the values are reset.
  */
void EvalPlot::onCloseEval()
{
    this->resetValues();
    close();
}

/**
  * Slot called when the user selects the output variable to be diplayed.
  */
void EvalPlot::onSelectOut()
{
    resetValues();
    affectMesuredValues(mesuredValues.mid((systemData->size()-1)*m_ui->cbOut->currentIndex(), systemData->size()-1), m_ui->cbOut->currentIndex());
    affectPredictedValues(predictedValues.mid((systemData->size()-1)*m_ui->cbOut->currentIndex(), systemData->size()-1));
    if (!isPredictive)
        affectExpectedValues(expectedValues.mid((systemData->size()-1)*m_ui->cbOut->currentIndex(), systemData->size()-1));

    myPlot->replot();
}

/**
  * Slot called when the sort values check box changes its state.
  */
void EvalPlot::onSort()
{
    if (m_ui->chbSort->isChecked()) {
        this->mesuredValuesOriginal = this->mesuredValues;
        this->expectedValuesOriginal = this->expectedValues;
        this->predictedValuesOriginal = this->predictedValues;

        qSort(this->mesuredValues);
        qSort(this->expectedValues);
        qSort(this->predictedValues);

        this->setMesuredValues(this->mesuredValues);
        this->setExpectedValues(this->expectedValues);
        this->setPredictedValues(this->predictedValues);
    }
    else {
        this->setMesuredValues(this->mesuredValuesOriginal);
        this->setExpectedValues(this->expectedValuesOriginal);
        this->setPredictedValues(this->predictedValuesOriginal);
    }

    myPlot->replot();
}

/**
  * Reset all the values (expected, predicted, measured).
  */
void EvalPlot::resetValues()
{
    xVals->clear();
    yValsMesured->clear();
    yValsExpected->clear();
    yValsPredicted->clear();
    yValsThresh->clear();
}

/**
  * Set the displayed fitness.
  *
  * @param fit Fitness to be displayed
  */
void EvalPlot::setFitness(float fit)
{
   fitness =  fit;
}
/**
  * Set the displayed sensitivity.
  *
  * @param sensi sensitivity to be displayed
  */
void EvalPlot::setSensitivity(float sensi)
{
   sensitivity =  sensi;
}

/**
  * Set the displayed specificity.
  *
  * @param speci specificity to be displayed
  */
void EvalPlot::setSpecificity(float speci)
{
    specificity = speci;
}

/**
  * Set the displayed accuracy.
  *
  * @param accu Accuracy to be displayed
  */
void EvalPlot::setAccuracy(float accu)
{
    accuracy = accu;
}

/**
  * Set the displayed PPV.
  *
  * @param p PPV to be displayed
  */
void EvalPlot::setPpv(float p)
{
    ppv = p;
}

/**
  * Set the displayed RMSE.
  *
  * @param r RMSE to be displayed
  */
void EvalPlot::setRmse(float r)
{
    rmse = r;
}

/**
  * Set the displayed RRSE.
  *
  * @param r RRSE to be displayed
  */
void EvalPlot::setRrse(float r)
{
    rrse = r;
}

/**
  * Set the displayed RAE.
  *
  * @param r RAE to be displayed
  */
void EvalPlot::setRae(float r)
{
    rae = r;
}

void EvalPlot::setMse(float r)
{
    mse = r;
}

void EvalPlot::setAdm(float r)
{
    adm = r;
}

void EvalPlot::setMdm(float r)
{
    mdm = r;
}

void EvalPlot::setSize(float r)
{
    size = r;
}

void EvalPlot::setOverLearn(float r)
{
    overLearn = r;
}

