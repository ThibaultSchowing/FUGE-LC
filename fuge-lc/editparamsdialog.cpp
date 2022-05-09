/**
  * @file   editparamsdialog.cpp
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
  * @class EditParamsDialog
  *
  * @brief This class implements the parameters edition dialog.
  */

#include <iostream>
#include <cmath>

#include <QHelpEvent>
#include <QToolTip>
#include <QLineEdit>
#include <QFileDialog>

#include "editparamsdialog.h"
#include "ui_editparamsdialog.h"
#include "systemparameters.h"
#include "errordialog.h"

/**
  * Constructor
  *
  * @param parent Pointer to the parent widget
  * @param paramsLoaded Pointer to the flag which indicates whether the parameters are loaded
  * @param scriptLoaded Flag which indicates whether the script is loaded
  */
EditParamsDialog::EditParamsDialog(QWidget *parent, bool* paramsLoaded, bool scriptLoaded) :
    QDialog(parent),
    m_ui(new Ui::EditParamsDialog)
{
    m_ui->setupUi(this);
    // Set the focus on th Ok button
    m_ui->buttonBox->button(QDialogButtonBox::Ok)->setFocus();

    this->paramsLoaded = paramsLoaded;

    SystemParameters& sysParams = SystemParameters::getInstance();

    // Show the current values
    if (*paramsLoaded || scriptLoaded) {
        defaultValues = false;

        m_ui->lineExp->setText(sysParams.getExperimentName());
        m_ui->linePath->setText(sysParams.getSavePath());
        m_ui->lineRules->setText(QString::number(sysParams.getNbRules()));
        m_ui->lineVarsPerRule->setText(QString::number(sysParams.getNbVarPerRule()));
        m_ui->lineOutput->setText(QString::number(sysParams.getNbOutVars()));
        m_ui->lineInSets->setText(QString::number(sysParams.getNbInSets()));
        m_ui->lineOutSets->setText(QString::number(sysParams.getNbOutSets()));
        m_ui->lineInSetsPos->setText(QString::number(sysParams.getInSetsPosCodeSize()));
        m_ui->lineOutSetsPos->setText(QString::number(sysParams.getOutSetPosCodeSize()));
        m_ui->lineInVarsCode->setText(QString::number(sysParams.getInVarsCodeSize()));
        m_ui->lineOutVarsCode->setText(QString::number(sysParams.getOutVarsCodeSize()));
        m_ui->lineInSetsCode->setText(QString::number(sysParams.getInSetsCodeSize()));
        m_ui->lineOutSetsCode->setText(QString::number(sysParams.getOutSetsCodeSize()));
        m_ui->lineGenPop1->setText(QString::number(sysParams.getMaxGenPop1()));
        m_ui->lineGenPop2->setText(QString::number(sysParams.getMaxGenPop2()));
        m_ui->lineFitPop1->setText(QString::number(sysParams.getMaxFitPop1()));
        m_ui->lineFitPop2->setText(QString::number(sysParams.getMaxFitPop2()));
        m_ui->linePop1->setText(QString::number(sysParams.getPopSizePop1()));
        m_ui->linePop2->setText(QString::number(sysParams.getPopSizePop2()));
        m_ui->lineElitePop1->setText(QString::number(sysParams.getEliteSizePop1()));
        m_ui->lineElitePop2->setText(QString::number(sysParams.getEliteSizePop2()));
        m_ui->lineCxPop1->setText(QString::number(sysParams.getCxProbPop1()));
        m_ui->lineCxPop2->setText(QString::number(sysParams.getCxProbPop2()));
        m_ui->lineMutIndPop1->setText(QString::number(sysParams.getMutFlipIndPop1()));
        m_ui->lineMutIndPop2->setText(QString::number(sysParams.getMutFlipIndPop2()));
        m_ui->lineMutBitPop1->setText(QString::number(sysParams.getMutFlipBitPop1()));
        m_ui->lineMutBitPop2->setText(QString::number(sysParams.getMutFlipBitPop2()));
        m_ui->lineSensi->setText(QString::number(sysParams.getSensiW()));
        m_ui->lineSpeci->setText(QString::number(sysParams.getSpeciW()));
        m_ui->lineAccu->setText(QString::number(sysParams.getAccuracyW()));
        m_ui->linePpv->setText(QString::number(sysParams.getPpvW()));
        m_ui->lineRmse->setText(QString::number(sysParams.getRmseW()));

        // Add some indice, usefull for regression problems
        m_ui->lineRrse->setText(QString::number(sysParams.getRrseW    ()));
        m_ui->lineRae-> setText(QString::number(sysParams.getRaeW     ()));

        //m_ui->chkSizeBonus->setChecked(sysParams.getSizeBonus());
        m_ui->lineMse->setText(QString::number(sysParams.getMseW()));
        m_ui->lineDistance->setText(QString::number(sysParams.getDistanceThresholdW()));
        m_ui->lineMinDistance->setText(QString::number(sysParams.getDistanceMinThresholdW()));
        m_ui->lineDontCare->setText(QString::number(sysParams.getDontCareW()));
        m_ui->lineOverLearn->setText(QString::number(sysParams.getOverLearnW()));

        m_ui->lineThresh->setText(QString::number(sysParams.getThresholdVal(0)));
        for (int i = 0; i < sysParams.getNbOutVars(); i++) {
            m_ui->cbThresh->addItem("Out " + QString::number(i+1));
        }
        m_ui->chbThreshActive->setChecked(sysParams.getThreshActivated());
        m_ui->cbThresh->setEnabled(sysParams.getThreshActivated());
        m_ui->lineThresh->setEnabled(sysParams.getThreshActivated());
    }

    // No current values => put the default values
    else {
        defaultValues = true;
        m_ui->lineExp->setText("Exp 1");
        m_ui->lineRules->setText(QString::number(5));
        m_ui->lineVarsPerRule->setText(QString::number(5));
        m_ui->lineOutput->setText(QString::number(1));
        m_ui->lineInSets->setText(QString::number(2));
        m_ui->lineOutSets->setText(QString::number(2));
        m_ui->lineInSetsPos->setText(QString::number(4));
        m_ui->lineOutSetsPos->setText(QString::number(4));
        m_ui->lineInVarsCode->setText(QString::number(4));
        m_ui->lineOutVarsCode->setText(QString::number(1));
        m_ui->lineInSetsCode->setText(QString::number(2));
        m_ui->lineOutSetsCode->setText(QString::number(1));
        m_ui->lineGenPop1->setText(QString::number(100));
        m_ui->lineGenPop2->setText(QString::number(100));
        m_ui->lineFitPop1->setText(QString::number(0.95));
        m_ui->lineFitPop2->setText(QString::number(0.95));
        m_ui->linePop1->setText(QString::number(100));
        m_ui->linePop2->setText(QString::number(100));
        m_ui->lineElitePop1->setText(QString::number(5));
        m_ui->lineElitePop2->setText(QString::number(5));
        m_ui->lineCxPop1->setText(QString::number(0.5));
        m_ui->lineCxPop2->setText(QString::number(0.5));
        m_ui->lineMutIndPop1->setText(QString::number(0.5));
        m_ui->lineMutIndPop2->setText(QString::number(0.5));
        m_ui->lineMutBitPop1->setText(QString::number(0.01));
        m_ui->lineMutBitPop2->setText(QString::number(0.01));
        m_ui->lineSensi->setText(QString::number(1.0));
        m_ui->lineSpeci->setText(QString::number(0.8));
        m_ui->lineAccu->setText(QString::number(0.0));
        m_ui->linePpv->setText(QString::number(0.0));
        m_ui->lineRmse->setText(QString::number(0.0));
        m_ui->lineRrse->setText(QString::number(0.0));
        m_ui->lineRae ->setText(QString::number(0.0));

        m_ui->lineMse->setText(QString::number(0.0));
        m_ui->lineDistance->setText(QString::number(0.0));
        m_ui->lineMinDistance->setText(QString::number(0.0));
        m_ui->lineDontCare->setText(QString::number(0.0));
        m_ui->lineOverLearn->setText(QString::number(0.0));

        m_ui->lineThresh->setText(QString::number(0.5));
        m_ui->chbThreshActive->setChecked(true);
        m_ui->lineThresh->setEnabled(true);
        m_ui->cbThresh->addItem("Out 1");
        sysParams.resizeThreshold(1);
    }

    // Display the fitness function
    displayFitFct();


    // FIXME: Can be all done in the ui form.
    // Automatically check and set values inserted
    connect(m_ui->lineExp, SIGNAL(textChanged(QString)), this, SLOT(checkAndSetValues()));
    connect(m_ui->btPath, SIGNAL(pressed()), this , SLOT(onBtPathPressed()));
    connect(m_ui->linePath, SIGNAL(textChanged(QString)), this, SLOT(onLinePathEdited(QString)));
    connect(m_ui->lineRules, SIGNAL(textChanged(QString)), this, SLOT(checkAndSetValues()));
    connect(m_ui->lineVarsPerRule, SIGNAL(textChanged(QString)), this, SLOT(checkAndSetValues()));
    connect(m_ui->lineOutput, SIGNAL(textChanged(QString)), this, SLOT(checkAndSetValues()));
    connect(m_ui->lineInSets, SIGNAL(textChanged(QString)), this, SLOT(checkAndSetValues()));
    connect(m_ui->lineOutSets, SIGNAL(textChanged(QString)), this, SLOT(checkAndSetValues()));
    connect(m_ui->lineInSetsPos, SIGNAL(textChanged(QString)), this, SLOT(checkAndSetValues()));
    connect(m_ui->lineOutSetsPos, SIGNAL(textChanged(QString)), this, SLOT(checkAndSetValues()));
    connect(m_ui->lineInVarsCode, SIGNAL(textChanged(QString)), this, SLOT(checkAndSetValues()));
    connect(m_ui->lineOutVarsCode, SIGNAL(textChanged(QString)), this, SLOT(checkAndSetValues()));
    connect(m_ui->lineInSetsCode, SIGNAL(textChanged(QString)), this, SLOT(checkAndSetValues()));
    connect(m_ui->lineOutSetsCode, SIGNAL(textChanged(QString)), this, SLOT(checkAndSetValues()));
    connect(m_ui->lineGenPop1, SIGNAL(textChanged(QString)), this, SLOT(checkAndSetValues()));
    connect(m_ui->lineGenPop2, SIGNAL(textChanged(QString)), this, SLOT(checkAndSetValues()));
    connect(m_ui->lineFitPop1, SIGNAL(textChanged(QString)), this, SLOT(checkAndSetValues()));
    connect(m_ui->lineFitPop2, SIGNAL(textChanged(QString)), this, SLOT(checkAndSetValues()));
    connect(m_ui->linePop1, SIGNAL(textChanged(QString)), this, SLOT(checkAndSetValues()));
    connect(m_ui->linePop2, SIGNAL(textChanged(QString)), this, SLOT(checkAndSetValues()));
    connect(m_ui->lineElitePop1, SIGNAL(textChanged(QString)), this, SLOT(checkAndSetValues()));
    connect(m_ui->lineElitePop2, SIGNAL(textChanged(QString)), this, SLOT(checkAndSetValues()));
    connect(m_ui->lineCxPop1, SIGNAL(textChanged(QString)), this, SLOT(checkAndSetValues()));
    connect(m_ui->lineCxPop2, SIGNAL(textChanged(QString)), this, SLOT(checkAndSetValues()));
    connect(m_ui->lineMutIndPop1, SIGNAL(textChanged(QString)), this, SLOT(checkAndSetValues()));
    connect(m_ui->lineMutIndPop2, SIGNAL(textChanged(QString)), this, SLOT(checkAndSetValues()));
    connect(m_ui->lineMutBitPop1, SIGNAL(textChanged(QString)), this, SLOT(checkAndSetValues()));
    connect(m_ui->lineMutBitPop2, SIGNAL(textChanged(QString)), this, SLOT(checkAndSetValues()));
    connect(m_ui->lineSensi, SIGNAL(textChanged(QString)), this, SLOT(checkAndSetValues()));
    connect(m_ui->lineSpeci, SIGNAL(textChanged(QString)), this, SLOT(checkAndSetValues()));
    connect(m_ui->lineAccu, SIGNAL(textChanged(QString)), this, SLOT(checkAndSetValues()));
    connect(m_ui->linePpv, SIGNAL(textChanged(QString)), this, SLOT(checkAndSetValues()));
    connect(m_ui->lineRmse, SIGNAL(textChanged(QString)), this, SLOT(checkAndSetValues()));
    // Add some indice, usefull for regression problems
    connect(m_ui->lineRrse, SIGNAL(textChanged(QString)), this, SLOT(checkAndSetValues()));
    connect(m_ui->lineRae ,  SIGNAL(textChanged(QString)), this, SLOT(checkAndSetValues()));
    connect(m_ui->lineThresh, SIGNAL(textChanged(QString)), this, SLOT(checkAndSetValues()));
    connect(m_ui->chbThreshActive, SIGNAL(toggled(bool)), this, SLOT(onChbActivateThresh()));
    connect(m_ui->lineMse, SIGNAL(textChanged(QString)), this, SLOT(checkAndSetValues()));
    connect(m_ui->lineDistance, SIGNAL(textChanged(QString)), this, SLOT(checkAndSetValues()));
    connect(m_ui->lineMinDistance, SIGNAL(textChanged(QString)), this, SLOT(checkAndSetValues()));
    connect(m_ui->lineDontCare, SIGNAL(textChanged(QString)), this, SLOT(checkAndSetValues()));
    connect(m_ui->lineOverLearn, SIGNAL(textChanged(QString)), this, SLOT(checkAndSetValues()));

    // Display the fitness formula automatically
    connect(m_ui->lineSensi, SIGNAL(textChanged(QString)), this, SLOT(displayFitFct()));
    connect(m_ui->lineSpeci, SIGNAL(textChanged(QString)), this, SLOT(displayFitFct()));
    connect(m_ui->lineAccu, SIGNAL(textChanged(QString)), this, SLOT(displayFitFct()));
    connect(m_ui->linePpv, SIGNAL(textChanged(QString)), this, SLOT(displayFitFct()));
    connect(m_ui->lineRmse, SIGNAL(textChanged(QString)), this, SLOT(displayFitFct()));
}

EditParamsDialog::~EditParamsDialog()
{
    delete m_ui;
}

/**
  * Display the fitness formula
  *
  */
void EditParamsDialog::displayFitFct()
{
    m_ui->labelFitFct->setText("Fitness = " + m_ui->lineSensi->text() + "*Sensi + " + m_ui->lineSpeci->text() + "*Speci + "
                               + m_ui->linePpv->text() +"*Accuracy + " + m_ui->lineAccu->text() + "*PPV + " + m_ui->lineRmse->text() + "*RMSE");
}

/**
  * Check that the parameters values are correct and set them in the system parameters singleton.
  */
bool EditParamsDialog::checkAndSetValues()
{

    // FIXME: Very few of this logic is needed here, most can be done in the ui form.
    bool valid = true;

    SystemParameters& sysParams = SystemParameters::getInstance();

    // Experiment name
    sysParams.setExperimentName(m_ui->lineExp->text());

    // Number of rules
    if (validateIntEntry(m_ui->lineRules->text(), 1, 1024)) {
        sysParams.setNbRules(m_ui->lineRules->text().toInt());
        m_ui->lineRules->setStyleSheet("");
    }
    else {
        m_ui->lineRules->setStyleSheet("background-color: rgb(255, 0, 0);");
        valid = false;
    }

    // Maximum number of variables per rule
    if (validateIntEntry(m_ui->lineVarsPerRule->text(), 1, (sysParams.getNbInVars() == -1)?1024:sysParams.getNbInVars())) {
        sysParams.setNbVarPerRule(m_ui->lineVarsPerRule->text().toInt());
        m_ui->lineVarsPerRule->setStyleSheet("");
    }
    else {
        m_ui->lineVarsPerRule->setStyleSheet("background-color: rgb(255, 0, 0);");
        valid = false;
    }
    // Number of output variables
    if (validateIntEntry(m_ui->lineOutput->text(), 1, 1024)) {
        int oldOutVarNum = m_ui->cbThresh->count();
        /*
        if (defaultValues) {
            oldOutVarNum = 1;
        }
        else {
            oldOutVarNum = sysParams.getNbOutVars();
        }
        */
        int delta = m_ui->lineOutput->text().toInt() - oldOutVarNum;

        // We add the corresponding thresholds
        if (delta > 0) {
            sysParams.resizeThreshold(m_ui->lineOutput->text().toInt());
            sysParams.setNbOutVars(m_ui->lineOutput->text().toInt());
            for (int i = 1; i <= delta; i++) {
                m_ui->cbThresh->addItem("Out " + QString::number(oldOutVarNum+i));
                sysParams.setThresholdVal(oldOutVarNum+i-1, 0.5);
            }
        }
        else if (delta < 0) {
            for (int i = oldOutVarNum; i > oldOutVarNum-delta; i--) {
                m_ui->cbThresh->removeItem(i-1);
            }
            sysParams.setNbOutVars(m_ui->lineOutput->text().toInt());
        }
        else if (delta == 0) {
            sysParams.setNbOutVars(m_ui->lineOutput->text().toInt());
        }
        m_ui->lineOutput->setStyleSheet("");
    }
    else {
        m_ui->lineOutput->setStyleSheet("background-color: rgb(255, 0, 0);");
        valid = false;
    }
    // Number of input sets
    if (validateIntEntry(m_ui->lineInSets->text(), 2, 1024)) {
        sysParams.setNbInSets(m_ui->lineInSets->text().toInt());
        m_ui->lineInSets->setStyleSheet("");
    }
    else {
        m_ui->lineInSets->setStyleSheet("background-color: rgb(255, 0, 0);");
        valid = false;
    }
    // Number of output sets
    if (validateIntEntry(m_ui->lineOutSets->text(), 2, 1024)) {
        sysParams.setNbOutSets(m_ui->lineOutSets->text().toInt());
        m_ui->lineOutSets->setStyleSheet("");
    }
    else {
        m_ui->lineOutSets->setStyleSheet("background-color: rgb(255, 0, 0);");
        valid = false;
    }
    // Number of bits that encode the input sets positions
    if (validateIntEntry(m_ui->lineInSetsPos->text(), 1, 1024)) {
        sysParams.setInSetsPosCodeSize(m_ui->lineInSetsPos->text().toInt());
        m_ui->lineInSetsPos->setStyleSheet("");
    }
    else {
        m_ui->lineInSetsPos->setStyleSheet("background-color: rgb(255, 0, 0);");
        valid = false;
    }
    // Number of bits that encode the output sets positions
    if (validateIntEntry(m_ui->lineOutSetsPos->text(), 1, 1024)) {
        sysParams.setOutSetPosCodeSize(m_ui->lineOutSetsPos->text().toInt());
        m_ui->lineOutSetsPos->setStyleSheet("");
    }
    else {
        m_ui->lineOutSetsPos->setStyleSheet("background-color: rgb(255, 0, 0);");
        valid = false;
    }
    // Number of bits to code the input variables
    if (validateIntEntry(m_ui->lineInVarsCode->text(), ceil(log(sysParams.getNbInVars()) / log(2)), 1024)) {
        sysParams.setInVarsCodeSize(m_ui->lineInVarsCode->text().toInt());
        m_ui->lineInVarsCode->setStyleSheet("");
    }
    else {
        m_ui->lineInVarsCode->setStyleSheet("background-color: rgb(255, 0, 0);");
        valid = false;
    }
     // Number of bits to code the output variables
    if (validateIntEntry(m_ui->lineOutVarsCode->text(), ceil(log(sysParams.getNbOutVars()) / log(2)), 1024)) {
        sysParams.setOutVarsCodeSize(m_ui->lineOutVarsCode->text().toInt());
        m_ui->lineOutVarsCode->setStyleSheet("");
    }
    else {
        m_ui->lineOutVarsCode->setStyleSheet("background-color: rgb(255, 0, 0);");
        valid = false;
    }
    // Number of bits to code the input sets
    if (validateIntEntry(m_ui->lineInSetsCode->text(), ceil(log(sysParams.getNbInSets()) / log(2)), 1024)) {
        sysParams.setInSetsCodeSize(m_ui->lineInSetsCode->text().toInt());
        m_ui->lineInSetsCode->setStyleSheet("");
    }
    else {
        m_ui->lineInSetsCode->setStyleSheet("background-color: rgb(255, 0, 0);");
        valid = false;
    }
    // Number of bits to code the output sets
    if (validateIntEntry(m_ui->lineOutSetsCode->text(), ceil(log(sysParams.getNbOutSets()) / log(2)), 1024)) {
        sysParams.setOutSetsCodeSize(m_ui->lineOutSetsCode->text().toInt());
        m_ui->lineOutSetsCode->setStyleSheet("");
    }
    else {
        m_ui->lineOutSetsCode->setStyleSheet("background-color: rgb(255, 0, 0);");
        valid = false;
    }
    // Maximum generations number for population 1
    if (validateIntEntry(m_ui->lineGenPop1->text(), 1, 1000000000)) {
        sysParams.setMaxGenPop1(m_ui->lineGenPop1->text().toInt());
        m_ui->lineGenPop1->setStyleSheet("");
    }
    else {
        m_ui->lineGenPop1->setStyleSheet("background-color: rgb(255, 0, 0);");
        valid = false;
    }

    // Max gen. for pop 2 is the same as for pop 1
    //m_ui->lineGenPop2->setText(m_ui->lineGenPop1->text());

    // Maximum generations number for population 2
    if (validateIntEntry(m_ui->lineGenPop2->text(), 1, 1000000000)) {
        sysParams.setMaxGenPop2(m_ui->lineGenPop2->text().toInt());
        m_ui->lineGenPop2->setStyleSheet("");
    }
    else {
        m_ui->lineGenPop2->setStyleSheet("background-color: rgb(255, 0, 0);");
        valid = false;
    }
    // Fitness threshold that stops the evolution for population 1
    if (validateFloatEntry(m_ui->lineFitPop1->text(), 0, 1.0)) {
        sysParams.setMaxFitPop1(m_ui->lineFitPop1->text().toFloat());
        m_ui->lineFitPop1->setStyleSheet("");
    }
    else {
        m_ui->lineFitPop1->setStyleSheet("background-color: rgb(255, 0, 0);");
        valid = false;
    }

    // Fitness thresh. shwon for pop 1 is the same as for pop 2 but in reality it is set to 1
    // Only the first thread is used to stop the evolution
    //m_ui->lineFitPop2->setText(m_ui->lineFitPop1->text());

    // Fitness threshold that stops the evolution for population 2
    if (validateFloatEntry(m_ui->lineFitPop2->text(), 0, 1.0)) {
        sysParams.setMaxFitPop2(m_ui->lineFitPop2->text().toFloat());
        m_ui->lineFitPop2->setStyleSheet("");
    }
    else {
        m_ui->lineFitPop2->setStyleSheet("background-color: rgb(255, 0, 0);");
        valid = false;
    }
    // Set population 1 size
    if (validateIntEntry(m_ui->linePop1->text(), 0, 1000000000)) {
        sysParams.setPopSizePop1(m_ui->linePop1->text().toInt());
        m_ui->linePop1->setStyleSheet("");
    }
    else {
        m_ui->linePop1->setStyleSheet("background-color: rgb(255, 0, 0);");
        valid = false;
    }
    // Set population 2 size
    if (validateIntEntry(m_ui->linePop2->text(), 0, 1000000000)) {
        sysParams.setPopSizePop2(m_ui->linePop2->text().toInt());
        m_ui->linePop2->setStyleSheet("");
    }
    else {
        m_ui->linePop2->setStyleSheet("background-color: rgb(255, 0, 0);");
        valid = false;
    }
    // Size of elite for population 1
    if (validateIntEntry(m_ui->lineElitePop1->text(), 0, sysParams.getPopSizePop1())) {
        sysParams.setEliteSizePop1(m_ui->lineElitePop1->text().toInt());
        m_ui->lineElitePop1->setStyleSheet("");
    }
    else {
        m_ui->lineElitePop1->setStyleSheet("background-color: rgb(255, 0, 0);");
        valid = false;
    }
    // Size of elite for population 2
    if (validateIntEntry(m_ui->lineElitePop2->text(), 0, sysParams.getPopSizePop2())) {
        sysParams.setEliteSizePop2(m_ui->lineElitePop2->text().toInt());
        m_ui->lineElitePop2->setStyleSheet("");
    }
    else {
        m_ui->lineElitePop2->setStyleSheet("background-color: rgb(255, 0, 0);");
        valid = false;
    }
    // Crossover probabilty for population 1
    if (validateFloatEntry(m_ui->lineCxPop1->text(), 0, 1.0)) {
        sysParams.setCxProbPop1(m_ui->lineCxPop1->text().toFloat());
        m_ui->lineCxPop1->setStyleSheet("");
    }
    else {
        m_ui->lineCxPop1->setStyleSheet("background-color: rgb(255, 0, 0);");
        valid = false;
    }
    // Crossover probabilty for population 2
    if (validateFloatEntry(m_ui->lineCxPop2->text(), 0, 1.0)) {
        sysParams.setCxProbPop2(m_ui->lineCxPop2->text().toFloat());
        m_ui->lineCxPop2->setStyleSheet("");
    }
    else {
        m_ui->lineCxPop2->setStyleSheet("background-color: rgb(255, 0, 0);");
        valid = false;
    }
    // Individual mutation probability for population 1
    if (validateFloatEntry(m_ui->lineMutIndPop1->text(), 0, 1.0)) {
        sysParams.setMutFlipIndPop1(m_ui->lineMutIndPop1->text().toFloat());
        m_ui->lineMutIndPop1->setStyleSheet("");
    }
    else {
        m_ui->lineMutIndPop1->setStyleSheet("background-color: rgb(255, 0, 0);");
        valid = false;
    }
    // Individual mutation probability for population 2
    if (validateFloatEntry(m_ui->lineMutIndPop2->text(), 0, 1.0)) {
        sysParams.setMutFlipIndPop2(m_ui->lineMutIndPop2->text().toFloat());
        m_ui->lineMutIndPop2->setStyleSheet("");
    }
    else {
        m_ui->lineMutIndPop2->setStyleSheet("background-color: rgb(255, 0, 0);");
        valid = false;
    }
    // Bit mutation probabilty for population 1
    if (validateFloatEntry(m_ui->lineMutBitPop1->text(), 0, 1.0)) {
        sysParams.setMutFlipBitPop1(m_ui->lineMutBitPop1->text().toFloat());
        m_ui->lineMutBitPop1->setStyleSheet("");
    }
    else {
        m_ui->lineMutBitPop1->setStyleSheet("background-color: rgb(255, 0, 0);");
        valid = false;
    }
    // Bit mutation probabilty for population 2
    if (validateFloatEntry(m_ui->lineMutBitPop2->text(), 0, 1.0)) {
        sysParams.setMutFlipBitPop2(m_ui->lineMutBitPop2->text().toFloat());
        m_ui->lineMutBitPop2->setStyleSheet("");
    }
    else {
        m_ui->lineMutBitPop2->setStyleSheet("background-color: rgb(255, 0, 0);");
        valid = false;
    }
    // Sensitivity weight in fitness evaluation
    if (validateFloatEntry(m_ui->lineSensi->text(), 0, 1.0)) {
        sysParams.setSensiW(m_ui->lineSensi->text().toFloat());
        m_ui->lineSensi->setStyleSheet("");
    }
    else {
        m_ui->lineSensi->setStyleSheet("background-color: rgb(255, 0, 0);");
        valid = false;
    }
    // Specificity weight in fitness evaluation
    if (validateFloatEntry(m_ui->lineSpeci->text(), 0, 1.0)) {
        sysParams.setSpeciW(m_ui->lineSpeci->text().toFloat());
        m_ui->lineSpeci->setStyleSheet("");
    }
    else {
        m_ui->lineSpeci->setStyleSheet("background-color: rgb(255, 0, 0);");
        valid = false;
    }
    // Accuracy weight in fitness evaluation
    if (validateFloatEntry(m_ui->lineAccu->text(), 0, 1.0)) {
        sysParams.setAccuracyW(m_ui->lineAccu->text().toFloat());
        m_ui->lineAccu->setStyleSheet("");
    }
    else {
        m_ui->lineAccu->setStyleSheet("background-color: rgb(255, 0, 0);");
        valid = false;
    }
    // PPV weight in fitness evaluation
    if (validateFloatEntry(m_ui->linePpv->text(), 0, 1.0)) {
        sysParams.setPpvW(m_ui->linePpv->text().toFloat());
        m_ui->linePpv->setStyleSheet("");
    }
    else {
        m_ui->linePpv->setStyleSheet("background-color: rgb(255, 0, 0);");
        valid = false;
    }
    // RMSE weight in fitness evaluation
    if (validateFloatEntry(m_ui->lineRmse->text(), 0, 1.0)) {
        sysParams.setRmseW(m_ui->lineRmse->text().toFloat());
        m_ui->lineRmse->setStyleSheet("");
    }
    else {
        m_ui->lineRmse->setStyleSheet("background-color: rgb(255, 0, 0);");
        valid = false;
    }

    // RRSE weight in fitness evaluation
    if (validateFloatEntry(m_ui->lineRrse->text(), 0, 1.0)) {
        sysParams.setRrseW(m_ui->lineRrse->text().toFloat());
        m_ui->lineRrse->setStyleSheet("");
    }
    else {
        m_ui->lineRrse->setStyleSheet("background-color: rgb(255, 0, 0);");
        valid = false;
    }

    // Rae weight in fitness evaluation (Rae = relative absolute error)
    if (validateFloatEntry(m_ui->lineRae->text(), 0, 1.0)) {
        sysParams.setRaeW(m_ui->lineRae->text().toFloat());
        m_ui->lineRae->setStyleSheet("");
    }
    else {
        m_ui->lineRae->setStyleSheet("background-color: rgb(255, 0, 0);");
        valid = false;
    }

    // MXE
    if (validateFloatEntry(m_ui->lineMse->text(), 0, 1.0)) {
        sysParams.setMseW(m_ui->lineMse->text().toFloat());
        m_ui->lineMse->setStyleSheet("");
    }
    else {
        m_ui->lineMse->setStyleSheet("background-color: rgb(255, 0, 0);");
        valid = false;
    }

    // ADM
    if (validateFloatEntry(m_ui->lineDistance->text(), 0, 1.0)) {
        sysParams.setDistanceThresholdW(m_ui->lineDistance->text().toFloat());
        m_ui->lineDistance->setStyleSheet("");
    }
    else {
        m_ui->lineDistance->setStyleSheet("background-color: rgb(255, 0, 0);");
        valid = false;
    }

    // MDM
    if (validateFloatEntry(m_ui->lineMinDistance->text(), 0, 1.0)) {
        sysParams.setDistanceMinThresholdW(m_ui->lineMinDistance->text().toFloat());
        m_ui->lineMinDistance->setStyleSheet("");
    }
    else {
        m_ui->lineMinDistance->setStyleSheet("background-color: rgb(255, 0, 0);");
        valid = false;
    }

    // Size
    if (validateFloatEntry(m_ui->lineDontCare->text(), 0, 1.0)) {
        sysParams.setDontCareW(m_ui->lineDontCare->text().toFloat());
        m_ui->lineDontCare->setStyleSheet("");
    }
    else {
        m_ui->lineDontCare->setStyleSheet("background-color: rgb(255, 0, 0);");
        valid = false;
    }

	// Over learn
    if (validateFloatEntry(m_ui->lineOverLearn->text(), 0, 1.0)) {
        sysParams.setOverLearnW(m_ui->lineOverLearn->text().toFloat());
        m_ui->lineOverLearn->setStyleSheet("");
    }
    else {
        m_ui->lineOverLearn->setStyleSheet("background-color: rgb(255, 0, 0);");
        valid = false;
    }

    //  Threshold
    if (validateFloatEntry(m_ui->lineThresh->text(), 0, 1000000.0)) {
        sysParams.setThresholdVal(m_ui->cbThresh->currentIndex(), m_ui->lineThresh->text().toFloat());
        m_ui->lineThresh->setStyleSheet("");
    }
    else {
        m_ui->lineThresh->setStyleSheet("background-color: rgb(255, 0, 0);");
        valid = false;
    }

    sysParams.setNbCooperators(m_ui->spinBoxCooperators->value());

    // Set other internal parameters to their default value without user intervention
    sysParams.setFixedVars(false);

    return valid;
}

void EditParamsDialog::changeEvent(QEvent *e)
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
  * Validate an integer value.
  *
  * @param stringValue Value entered by the user
  * @param min Minimum value accepted
  * @param max Maximum value accepted
  */
bool EditParamsDialog::validateIntEntry(QString stringValue, int min, int max)
{
    // First verify that the user input is a number
    bool isNumber;
    stringValue.toInt(&isNumber, 10);
    if (!isNumber) {
        return false;
    }
    // Then verify that we have an integer
    if (stringValue.toFloat() != static_cast<int>(stringValue.toFloat())) {
        return false;
    }
    // Finally verify that the number is in the limits
    if (stringValue.toInt() < min || stringValue.toInt() > max) {
        return false;
    }
    return true;
}

/**
  * Validate a float value.
  *
  * @param stringValue Value entered by the user
  * @param min Minimum value accepted
  * @param max Maximum value accepted
  */
bool EditParamsDialog::validateFloatEntry(QString stringValue, float min, float max)
{
    // First verify that the user input is a number
    bool isNumber;
    stringValue.toFloat(&isNumber);
    if (!isNumber) {
        return false;
    }
    // Finally verify that the number is in the limits
    if (stringValue.toFloat() < min || stringValue.toFloat() > max) {
        return false;
    }
    return true;
}

/**
  * Slot called when the save path selector button is pressed.
  */
void EditParamsDialog::onBtPathPressed()
{
    QFileDialog pathDialog;
    connect(&pathDialog, SIGNAL(currentChanged(QString)), this, SLOT(changePath(QString)));
    pathDialog.setFileMode(QFileDialog::Directory);
    pathDialog.exec();
}

/**
  * Slot called when the path is edited.
  */
void EditParamsDialog::onLinePathEdited(QString path)
{
    SystemParameters& sysParams = SystemParameters::getInstance();

    sysParams.setSavePath(path+"/");
}

/**
  * SChange the save path in the system parameters.
  */
void EditParamsDialog::changePath(QString path)
{
    SystemParameters& sysParams = SystemParameters::getInstance();

    sysParams.setSavePath(path);
    m_ui->linePath->setText(path);
}

/**
  * Slot called when the accept button is pressed.
  */
void EditParamsDialog::accept()
{
    bool valid;

    valid = checkAndSetValues();

    if (valid) {
        *paramsLoaded = true;
        close();
    }
    else {
        ErrorDialog errDiag;
        errDiag.setError("Error : some parameters are not valid !");
        errDiag.setInfo("Please check values in red...");
        errDiag.exec();
    }
}

/**
  * Slot called when the threshold activation check box is clicked.
  */
void EditParamsDialog::onChbActivateThresh()
{
    SystemParameters& sysParams = SystemParameters::getInstance();

    m_ui->cbThresh->setEnabled(m_ui->chbThreshActive->isChecked());
    m_ui->lineThresh->setEnabled(m_ui->chbThreshActive->isChecked());
    sysParams.setThreshActivated(m_ui->chbThreshActive->isChecked());
}

void EditParamsDialog::on_buttonBox_accepted()
{

}
