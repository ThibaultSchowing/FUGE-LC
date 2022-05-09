/**
  * @file   fuzzyeditor.cpp
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
  * @class FuzzyEditor
  *
  * @brief This class implements a dialog allowing to completely modify a loaded fuzzy system.
  */

#include <iostream>

#include <QFileDialog>
#include <QTextStream>
#include <QLabel>

#include "fuzzyeditor.h"
#include "fuzzy/fuzzyset.h"
#include "systemparameters.h"
#include "ui_fuzzyeditor.h"
#include "coev/coevstats.h"

#define FLOATTOINT 1000
#define MARGE 3
#define MAX_ANTE 7
#define MAX_IN_SETS 9
#define MAX_OUT_SETS 5

QColor colorTab[] = {Qt::red,Qt::blue,Qt::green,Qt::black,Qt::magenta,Qt::yellow,Qt::cyan};

/**
  * Constructor.
  *
  * @param parent Pointer to the parent widget
  * @param fSystem Fuzzy system to be modified
  */
FuzzyEditor::FuzzyEditor(QWidget *parent, FuzzySystem* fSystem) :
    QDialog(parent),
    m_ui(new Ui::FuzzyEditor)
{
    m_ui->setupUi(this);
    m_ui->btSave->setAutoDefault(false);
    m_ui->btOk->setAutoDefault(false);
    m_ui->btCancel->setAutoDefault(false);

    this->fSystem = fSystem;

    displayVars();
    displayRulesBox();

    // Add the set name edits to the rulesGrid array
    for (int i = 0; i < MAX_IN_SETS; i++) {
        QLabel* lbl = new QLabel("Set " + QString::number(i) + " name :", qobject_cast<QWidget*> (m_ui->gridSets));
        lbl->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        m_ui->gridSets->addWidget(lbl,i+1,0);
        RefLineEdit* lnEdit = new RefLineEdit(qobject_cast<QWidget*> (m_ui->gridSets), i+1, 1);
        lnEdit->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        m_ui->gridSets->addWidget(lnEdit,i+1,1);
        connect(lnEdit, SIGNAL(txtChanged(QString,int,int)), this, SLOT(onSetNameChanged(QString,int,int)));
    }
    QSpacerItem* spacer = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);
    m_ui->gridSets->addItem(spacer, MAX_IN_SETS+1, 1);
    QSpacerItem* spacer2 = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);
    m_ui->gridSets->addItem(spacer2, 0, 2);

    // Display the rules without the membership functions section
    QString desc = fSystem->getSystemDescritpion();
    int posText = desc.indexOf("Membership");
    desc.truncate(posText);
    m_ui->lblRules->setText(desc);

    myPlot = new QwtPlot((QWidget*) this);
    //legend = new QwtLegend();
    myPlot->setAxisTitle(QwtPlot::xBottom, "");
    myPlot->setAxisTitle(QwtPlot::yLeft, "");

    membCurve = new QwtPlotCurve("Membership functions");
    membCurve->attach(myPlot);
    //myPlot->insertLegend(legend, QwtPlot::TopLegend);

    m_ui->graphLayout->addWidget(myPlot);

    curves.resize(fSystem->getNbInSets());

    for (int i = 0; i < fSystem->getNbInVars(); i++) {
        m_ui->comboBox->addItem(fSystem->getInVar(i)->getName());
    }

    for (int i = 0; i < fSystem->getNbInSets(); i++) {
        QwtPlotCurve* curve = new QwtPlotCurve();
        m_ui->cbSets->addItem(fSystem->getInVar(m_ui->comboBox->currentIndex())->getSet(i)->getName());
        curves.replace(i, curve);
    }

    for (int i = 0; i < fSystem->getNbOutVars(); i++) {
        m_ui->comboBox->addItem(fSystem->getOutVar(i)->getName());
    }

    // Prepare the sets/var comboboxes
    for (int i = 0; i < MAX_IN_SETS; i++)
        m_ui->cbInSets->addItem(QString::number(i+1));
    m_ui->cbInSets->setCurrentIndex(fSystem->getNbInSets()-1);
    for (int i = 0; i < MAX_OUT_SETS; i++)
        m_ui->cbOutSets->addItem(QString::number(i+1));
    m_ui->cbOutSets->setCurrentIndex(fSystem->getNbOutSets()-1);

    // Set the current set position in the line edit
    m_ui->lnPos->setText(QString::number(((float) fSystem->getInVar(m_ui->comboBox->currentIndex())->getSet(m_ui->cbSets->currentIndex())->getPosition())));
    connect(m_ui->btSave, SIGNAL(clicked()), this, SLOT(onSaveFuzzy()));
    connect(m_ui->btOk, SIGNAL(clicked()), this, SLOT(onOk()));
    connect(m_ui->btCancel, SIGNAL(clicked()), this, SLOT(onCancel()));
    connect(m_ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onSelectVar()));
    connect(m_ui->cbSets, SIGNAL(currentIndexChanged(int)), this, SLOT(onSelectSet()));
    connect(m_ui->horizontalSlider, SIGNAL(valueChanged(int)), this, SLOT(onSliderChanged(int)));
    connect(m_ui->lnPos, SIGNAL(returnPressed()), this, SLOT(onPosChanged()));
    connect(m_ui->listVars, SIGNAL(currentRowChanged(int)), this, SLOT(onVarSelected(int)));
    connect(m_ui->cbInSets, SIGNAL(currentIndexChanged(int)), this, SLOT(onNbInSetsChanged(int)));
    connect(m_ui->cbOutSets, SIGNAL(currentIndexChanged(int)), this, SLOT(onNbOutSetsChanged(int)));
    m_ui->listVars->setCurrentRow(0);
    onSelectVar();
    onSelectSet();
    // Set the slider on current set position
    onSliderChanged(-1);

    displaySets();

    // Create the list widgets for the output variables sets edition
   /* for (int i = 0; i < fSystem->getNbOutVars(); i++) {
        QListWidget* lw = new QListWidget(this);
        QLabel* lbl = new QLabel(fSystem->getOutVar(i)->getName(), this);
        m_ui->gridOutVars->addWidget(lbl, 0, i);
        m_ui->gridOutVars->addWidget(lw, 1, i);
    }*/
}

FuzzyEditor::~FuzzyEditor()
{
    delete m_ui;
    for (int i = 0; i < curves.size(); i++) {
        delete curves.at(i);
    }
}

/**
  * Display all the variables in the list on the left of the window.
  */
void FuzzyEditor::displayVars()
{
    for (int i = 0; i < fSystem->getNbInVars(); i++) {
        m_ui->listVars->addItem(fSystem->getInVar(i)->getName());
    }
    for (int i = fSystem->getNbInVars(); i < fSystem->getNbInVars() + fSystem->getNbOutVars(); i++) {
        m_ui->listVars->addItem(fSystem->getOutVar(i-fSystem->getNbInVars())->getName());
        m_ui->listVars->item(i)->setTextColor(Qt::red);
    }
}

/**
  * Display the existing sets for the input variables.
  */
void FuzzyEditor::displaySets()
{
    // The number of sets displayed changes whether the current var is an input or an output
    // Input var
    if (m_ui->listVars->currentRow() < fSystem->getNbInVars()) {
        // Show only the requested fields
        int i;
        for (i = 1; i <= m_ui->cbInSets->currentText().toInt(); i++) {
            m_ui->gridSets->itemAtPosition(i,0)->widget()->show();
            qobject_cast<RefLineEdit*> (m_ui->gridSets->itemAtPosition(i,1)->widget())
                                        ->setText(fSystem->getInVar(m_ui->listVars->currentRow())
                                        ->getSet(i-1)->getName());
            m_ui->gridSets->itemAtPosition(i,1)->widget()->show();
        }
        // Hide the other ones
        for (; i <= MAX_IN_SETS; i++) {
            m_ui->gridSets->itemAtPosition(i,0)->widget()->hide();
            m_ui->gridSets->itemAtPosition(i,1)->widget()->hide();
        }
    }
    // Output var
    else {
        // Show only the requested fields
        int i;
        for (i = 1; i <= m_ui->cbOutSets->currentText().toInt(); i++) {
            m_ui->gridSets->itemAtPosition(i,0)->widget()->show();
            m_ui->gridSets->itemAtPosition(i,1)->widget()->show();
            qobject_cast<RefLineEdit*> (m_ui->gridSets->itemAtPosition(i,1)->widget())
                            ->setText(fSystem->getOutVar(m_ui->listVars->currentRow() - fSystem->getNbInVars())
                            ->getSet(i-1)->getName());
        }
        // Hide the other ones
        for (; i <= MAX_OUT_SETS; i++) {
            m_ui->gridSets->itemAtPosition(i,0)->widget()->hide();
            m_ui->gridSets->itemAtPosition(i,1)->widget()->hide();
        }
    }
}

/**
  * Display the rules of the fuzzy system in the rules tab.
  */
void FuzzyEditor::displayRulesBox()
{
    // INPUT VARS
    rulesVector.resize(fSystem->getNbRules());
    for (int i = 0; i < fSystem->getNbRules(); i++) {
        rulesVector.replace(i, fSystem->getRule(i));
        QLabel* lbl = new QLabel("IF", this);
        lbl->setStyleSheet("color: rgb(255, 0, 0);");
        lbl->setMaximumWidth(40);
        lbl->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        m_ui->rulesGrid->addWidget(lbl,i,0);
        for (int k = 0, index = 0; k < MAX_ANTE; k++, index+=4) {
            RefComboBox* cbBox = new RefComboBox(this,i,index+1);
            for (int z = 0; z < fSystem->getNbInVars(); z++) {
                cbBox->addItem(fSystem->getInVar(z)->getName());
            }
            cbBox->addItem("");
            if (k < rulesVector[i]->getNbInPairs())
                cbBox->setCurrentIndex(cbBox->findText(rulesVector[i]->getInVarAtPos(k)->getName()));
            else {
                cbBox->setCurrentIndex(cbBox->findText(""));
                cbBox->setStyleSheet("background-color: rgb(200, 200, 200);");
            }
            cbBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
            m_ui->rulesGrid->addWidget(cbBox,i,index+1);     
            connect(cbBox, SIGNAL(idxChanged(int, int, int)), this, SLOT(onRulesChanged(int, int, int)));

            QLabel* lblIs = new QLabel("is", this);
            lblIs->setMaximumWidth(20);
            lblIs->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
            m_ui->rulesGrid->addWidget(lblIs, i, index+2);

            RefComboBox* cbBox2 = new RefComboBox(this,i,index+3);
            for (int z = 0; z < fSystem->getNbInSets(); z++) {
                cbBox2->addItem(fSystem->getInVar(0)->getSet(z)->getName());
            }
            if (k < rulesVector[i]->getNbInPairs()) {
                cbBox2->setCurrentIndex(cbBox2->findText(rulesVector[i]->getInSetAtPos(k)->getName()));
            }
            else {
                cbBox2->setCurrentIndex(cbBox->findText(""));
                cbBox2->setStyleSheet("background-color: rgb(200, 200, 200);");
                cbBox2->setEnabled(false);
            }

            cbBox2->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
            m_ui->rulesGrid->addWidget(cbBox2,i,index+3);
            connect(cbBox2, SIGNAL(idxChanged(int, int, int)), this, SLOT(onRulesChanged(int, int, int)));

            if (k != MAX_ANTE-1) {
                QLabel* lblAnd = new QLabel("AND", this);
                lblAnd->setMaximumWidth(40);
                lblAnd->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
                m_ui->rulesGrid->addWidget(lblAnd, i, index+4);
                lblAnd->setStyleSheet("color: rgb(54, 139, 9);");
            }

        }
        // THEN
        QLabel* lblThen = new QLabel("THEN", this);
        lblThen->setMaximumWidth(40);
        lblThen->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        lblThen->setStyleSheet("color: rgb(255, 0, 0);");
        m_ui->rulesGrid->addWidget(lblThen, i, MAX_ANTE*4);
        // OUTPUT VARS
        for (int k = 0, index = MAX_ANTE*4; k < fSystem->getNbOutVars(); k++, index+=4) {
            RefComboBox* cbBox = new RefComboBox(this,i,index+1);
            for (int z = 0; z < fSystem->getNbOutVars(); z++) {
                cbBox->addItem(fSystem->getOutVar(z)->getName());
            }
            cbBox->addItem("");
            if (k < rulesVector[i]->getNbOutPairs())
                cbBox->setCurrentIndex(cbBox->findText(rulesVector[i]->getOutVarAtPos(k)->getName()));
            else {
                cbBox->setCurrentIndex(cbBox->findText(""));
                cbBox->setStyleSheet("background-color: rgb(200, 200, 200);");
            }
            cbBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
            m_ui->rulesGrid->addWidget(cbBox,i, index+1);
            connect(cbBox, SIGNAL(idxChanged(int, int, int)), this, SLOT(onRulesChanged(int, int, int)));

            QLabel* lblIs = new QLabel("is", this);
            lblIs->setMaximumWidth(20);
            lblIs->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
            m_ui->rulesGrid->addWidget(lblIs, i, index+2);

            RefComboBox* cbBox2 = new RefComboBox(this,i,index+3);
            for (int z = 0; z < fSystem->getNbOutSets(); z++) {
                cbBox2->addItem(fSystem->getOutVar(0)->getSet(z)->getName());
            }

            if (k < rulesVector[i]->getNbOutPairs())
                cbBox2->setCurrentIndex(cbBox2->findText(rulesVector[i]->getOutSetAtPos(k)->getName()));
            else {
                cbBox2->setCurrentIndex(cbBox->findText(""));
                cbBox2->setStyleSheet("background-color: rgb(200, 200, 200);");
                cbBox2->setEnabled(false);
            }
            cbBox2->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
            m_ui->rulesGrid->addWidget(cbBox2, i, index+3);
            connect(cbBox2, SIGNAL(idxChanged(int, int, int)), this, SLOT(onRulesChanged(int, int, int)));

            if (k != fSystem->getNbOutVars()-1) {
                QLabel* lblAnd = new QLabel("AND", this);
                lblAnd->setMaximumWidth(40);
                lblAnd->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
                m_ui->rulesGrid->addWidget(lblAnd, i, index+4);
                lblAnd->setStyleSheet("color: rgb(54, 139, 9);");
            }
        }
    }

    QLabel* lblElse = new QLabel("ELSE", this);
    lblElse->setMaximumWidth(40);
    lblElse->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    lblElse->setStyleSheet("color: rgb(255, 0, 0);");
    m_ui->rulesGrid->addWidget(lblElse, fSystem->getNbRules(), 0);
    //DEFAULT RULE
    for (int i = 0, index = 1; i < fSystem->getNbOutVars(); i++, index+=4) {
        RefComboBox* cbBox = new RefComboBox(this, fSystem->getNbRules()+1, index);
        cbBox->addItem(fSystem->getOutVar(i)->getName());
        cbBox->setCurrentIndex(cbBox->findText(fSystem->getOutVar(i)->getName()));
        m_ui->rulesGrid->addWidget(cbBox, fSystem->getNbRules()+1, index);
        connect(cbBox, SIGNAL(idxChanged(int, int, int)), this, SLOT(onRulesChanged(int, int, int)));

        QLabel* lblIs = new QLabel("is", this);
        lblIs->setMaximumWidth(20);
        cbBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        m_ui->rulesGrid->addWidget(lblIs, fSystem->getNbRules()+1, index+1);

        RefComboBox* cbBox2 = new RefComboBox(this,fSystem->getNbRules()+1,index+2);
        for (int z = 0; z < fSystem->getNbOutSets(); z++) {
            cbBox2->addItem(fSystem->getOutVar(0)->getSet(z)->getName());
        }
        cbBox2->setCurrentIndex(cbBox2->findText(fSystem->getOutVar(i)->getSet(fSystem->getDefaultRules().at(i))->getName()));
        cbBox2->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        m_ui->rulesGrid->addWidget(cbBox2, fSystem->getNbRules()+1, index+2);
        connect(cbBox2, SIGNAL(idxChanged(int, int, int)), this, SLOT(onRulesChanged(int, int, int)));
    }

}

/**
  * Slot called when a rule has been edited. Detect which part has been changed
  * and calls updateRule() to perform the modification on the fuzzy system.
  *
  * @param idx Index of the combox indicating which new set or variable has been selected.
  * @param li Row number indicating which rule has been changed
  * @param col Column number indicating whether this is a set or a variable.
  */
void FuzzyEditor::onRulesChanged(int idx, int li, int col)
{
    // Detect which ComboBox was modified in order to apply the corresponding changes
    // Check whether it is in a rule or in the default rule
    if (li < fSystem->getNbRules()) {
        // Check whether it is in input or output vars
        if (col < MAX_ANTE*4) {
            // Check whether it is a variable or a set
            if (col % 4 == 1) {
                // Take the corresponding action : disable the antecedent or update the rule
                // Disable the antecedent
                if (idx == fSystem->getNbInVars()) {
                    m_ui->rulesGrid->itemAtPosition(li,col)->widget()->setStyleSheet("background-color: rgb(200, 200, 200);");
                    m_ui->rulesGrid->itemAtPosition(li,col+2)->widget()->setStyleSheet("background-color: rgb(200, 200, 200);");
                    m_ui->rulesGrid->itemAtPosition(li,col+2)->widget()->setEnabled(false);
                    updateRule(li);
                }
                // Update the rule
                else {
                    m_ui->rulesGrid->itemAtPosition(li,col)->widget()->setStyleSheet("");
                    m_ui->rulesGrid->itemAtPosition(li,col+2)->widget()->setStyleSheet("");
                    m_ui->rulesGrid->itemAtPosition(li,col+2)->widget()->setEnabled(true);
                    qobject_cast<RefComboBox*>(m_ui->rulesGrid->itemAtPosition(li,col+2)->widget())->setCurrentIndex(0);
                    updateRule(li);

                }
            }
            // ComboBox is a set
            else {
                updateRule(li);
            }
        }
        // ComboBox in output vars
        else {
            // Check whether it is a variable or a set
            if (col % 4 == 1) {
                // Take the corresponding action : disable the antecedent or update the rule
                // Disable the antecedent
                if (idx == fSystem->getNbInVars()) {
                    m_ui->rulesGrid->itemAtPosition(li,col)->widget()->setStyleSheet("background-color: rgb(200, 200, 200);");
                    m_ui->rulesGrid->itemAtPosition(li,col+2)->widget()->setStyleSheet("background-color: rgb(200, 200, 200);");
                    m_ui->rulesGrid->itemAtPosition(li,col+2)->widget()->setEnabled(false);
                    updateRule(li);
                }
                // Update the rule
                else {
                    m_ui->rulesGrid->itemAtPosition(li,col)->widget()->setStyleSheet("");
                    m_ui->rulesGrid->itemAtPosition(li,col+2)->widget()->setStyleSheet("");
                    m_ui->rulesGrid->itemAtPosition(li,col+2)->widget()->setEnabled(true);
                    qobject_cast<RefComboBox*>(m_ui->rulesGrid->itemAtPosition(li,col+2)->widget())->setCurrentIndex(0);
                    updateRule(li);
                }
            }
            // ComboBox is a set
            else {
                updateRule(li);
            }
        }
    }
    // ComboBox in default rule
    else {
        // Check whether it is a variable or a set
        if (col % 4 == 1) {
            // Nothing to do...
        }
        // ComboBox is a set
        else {
            // Update the default rule set
            fSystem->updateDefaultRule(col/4, idx);
            // Update the rules text displayed
            fSystem->updateSystemDescription();
            // Display the rules without the membership functions section
            QString desc = fSystem->getSystemDescritpion();
            int posText = desc.indexOf("Membership");
            desc.truncate(posText);
            m_ui->lblRules->setText(desc);
        }
    }

    // Display the sets again to see the new name of the sets if it has changed
    displaySets();
}

/**
  * Apply the modification to the fuzzy system when a rule has been updated.
  *
  * @param ruleNum Index of the rule to be updated.
  */
void FuzzyEditor::updateRule(int ruleNum)
{
    QString cbTextVar;
    QString cbTextSet;
    QVector<FuzzyVariable*> inVars;
    QVector<FuzzyVariable*> outVars;
    QVector<int> inSets;
    QVector<int> outSets;

    // Read the rule
    // Read the input variables
    for (int i = 0; i < MAX_ANTE*4 ; i+=4) {
        cbTextVar = qobject_cast<RefComboBox*>(m_ui->rulesGrid->itemAtPosition(ruleNum, i+1)->widget())->currentText();
        cbTextSet = qobject_cast<RefComboBox*>(m_ui->rulesGrid->itemAtPosition(ruleNum, i+3)->widget())->currentText();
        if (!cbTextVar.isEmpty()) {
            inVars.append(fSystem->getInVarByName(cbTextVar));
            inSets.append(fSystem->getInVarByName(cbTextVar)->getSetIndexByName(cbTextSet));
        }
    }
    // Read the output variables
    for (int i = MAX_ANTE*4; i < (MAX_ANTE*4)+fSystem->getNbOutVars()*4; i+=4) {
        cbTextVar = qobject_cast<RefComboBox*>(m_ui->rulesGrid->itemAtPosition(ruleNum, i+1)->widget())->currentText();
        cbTextSet = qobject_cast<RefComboBox*>(m_ui->rulesGrid->itemAtPosition(ruleNum, i+3)->widget())->currentText();
        if (!cbTextVar.isEmpty()) {
            outVars.append(fSystem->getOutVarByName(cbTextVar));
            outSets.append(fSystem->getOutVarByName(cbTextVar)->getSetIndexByName(cbTextSet));
        }
    }
    // Create the rule
    FuzzyRule* rule = new FuzzyRule(inVars, inSets, outVars, outSets);

    // Update the rule in the system
    fSystem->replaceRule(ruleNum, rule);

    // Update the rules text displayed
    fSystem->updateSystemDescription();
    // Display the rules without the membership functions section
    QString desc = fSystem->getSystemDescritpion();
    int posText = desc.indexOf("Membership");
    desc.truncate(posText);
    m_ui->lblRules->setText(desc);
}

void FuzzyEditor::changeEvent(QEvent *e)
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
  * Slot called when the user select a variable in the membership functions tab.
  * The displayed curves are updated.
  */
void FuzzyEditor::onSelectVar()
{
    QVector<double> xVals;
    QVector<double> yVals;

    xVals.resize(3);
    yVals.resize(3);

    int varNum = m_ui->comboBox->currentIndex();

    // Update the sets combo box according to the actual set number
    if (fSystem->getVar(varNum)->getSetsCount() < m_ui->cbSets->count()) {
        for (int i = m_ui->cbSets->count()-1; i >= fSystem->getVar(varNum)->getSetsCount(); i--) {
            m_ui->cbSets->removeItem(i);
            curves.at(i)->detach();
        }
    }
    else if (fSystem->getVar(varNum)->getSetsCount() > m_ui->cbSets->count()){
        for (int i = m_ui->cbSets->count(); i < fSystem->getVar(varNum)->getSetsCount(); i++) {
             m_ui->cbSets->addItem(fSystem->getVar(m_ui->comboBox->currentIndex())->getSet(i)->getName());
             /// ICI créer les courbes
             QwtPlotCurve* curve = new QwtPlotCurve();
             curves.append(curve);
        }
    }

    // Update the plot
    for (int i = 0; i < fSystem->getVar(varNum)->getSetsCount(); i++) {
        xVals.clear();
        yVals.clear();
        xVals.resize(3);
        yVals.resize(3);
        if (i == 0) {
            xVals.replace(0, 0.0);
            yVals.replace(0, 1.0);
        }
        else {
            xVals.replace(0, fSystem->getVar(varNum)->getSet(i-1)->getPosition());
            yVals.replace(0, 0.0);
        }
        xVals.replace(1, fSystem->getVar(varNum)->getSet(i)->getPosition());
        yVals.replace(1, 1.0);
        if (i == fSystem->getVar(varNum)->getSetsCount() - 1) {
            xVals.replace(2, fSystem->getVar(varNum)->getSet(i)->getPosition()*1.2);
            yVals.replace(2, 1.0);
        }
        else {
            xVals.replace(2, fSystem->getVar(varNum)->getSet(i+1)->getPosition());
            yVals.replace(2, 0.0);
        }

        curves.at(i)->setPen(QPen (colorTab[i%(sizeof(colorTab)/sizeof(QColor))],3));
        curves.at(i)->setData(xVals, yVals);
        curves.at(i)->attach(myPlot);
    }
    myPlot->replot();

    // Update the sets combo boxes
    for (int i = 0; i < fSystem->getVar(varNum)->getSetsCount(); i++) {
        m_ui->cbSets->setItemText(i, fSystem->getVar(m_ui->comboBox->currentIndex())->getSet(i)->getName());
    }

    this->onSelectSet();
}

/**
  * Slot called when the user selects a set in the membership functions editor. The position of
  * the membership functions parameters displayed are updated.
  */
void FuzzyEditor::onSelectSet()
{
    // Update the limits of the slider
    int currentSet = m_ui->cbSets->currentIndex();

    //qDebug("var num = %d, cur index = %d", m_ui->comboBox->currentIndex(), currentSet);

    disconnect(m_ui->horizontalSlider, SIGNAL(valueChanged(int)), this, SLOT(onSliderChanged(int)));
    // First set
    if (currentSet == 0) {
        m_ui->horizontalSlider->setRange(0, (int) (fSystem->getVar(m_ui->comboBox->currentIndex())->getSet(currentSet+1)->getPosition()*FLOATTOINT)-MARGE);
    }
    // Last set
    else if (currentSet == m_ui->cbSets->count() - 1) {
        m_ui->horizontalSlider->setRange((int) (fSystem->getVar(m_ui->comboBox->currentIndex())->getSet(currentSet-1)->getPosition()*FLOATTOINT-MARGE),
                                                (int) (fSystem->getVar(m_ui->comboBox->currentIndex())->getSet(currentSet)->getPosition()*(FLOATTOINT*2)-MARGE));
    }
    // Internal sets
    else {
        m_ui->horizontalSlider->setRange((int) (fSystem->getVar(m_ui->comboBox->currentIndex())->getSet(currentSet-1)->getPosition()*FLOATTOINT-MARGE),
                                                (int) (fSystem->getVar(m_ui->comboBox->currentIndex())->getSet(currentSet+1)->getPosition()*FLOATTOINT-MARGE));
    }
    // Set the slider on current set position
    m_ui->horizontalSlider->setSliderPosition(fSystem->getVar(m_ui->comboBox->currentIndex())->getSet(currentSet)->getPosition()*FLOATTOINT-MARGE);
    // Set the line edit corresponding value
    m_ui->lnPos->setText(QString::number(fSystem->getVar(m_ui->comboBox->currentIndex())->getSet(currentSet)->getPosition()));
    connect(m_ui->horizontalSlider, SIGNAL(valueChanged(int)), this, SLOT(onSliderChanged(int)));
}

/**
  * Slot called when the user moves the slider.
  *
  * @param val New position of the slider.
  */
void FuzzyEditor::onSliderChanged(int val)
{
    if (val != -1) {
        fSystem->getVar(m_ui->comboBox->currentIndex())->getSet(m_ui->cbSets->currentIndex())->setPosition(((float) val)/FLOATTOINT);
        m_ui->lnPos->setText(QString::number(((float) val / FLOATTOINT)));
    }
    onSelectVar();
    myPlot->replot();
}

/**
  * Slot called when a new position value has been inserted in the line edit. The position of
  * the membership functions parameters displayed are updated.
  *
  * @param newPos New position value in the line edit.
  */
void FuzzyEditor::onPosChanged()
{
    float pos = m_ui->lnPos->text().toFloat();
    int currentSet = m_ui->cbSets->currentIndex();

    // First set
    if (currentSet == 0) {
        // Validate the value
        if (pos <= 0.0 || pos >= fSystem->getVar(m_ui->comboBox->currentIndex())->getSet(currentSet+1)->getPosition()) {
            m_ui->lnPos->setStyleSheet("background-color: rgb(255, 0, 0);");
        }
        else {
            fSystem->getVar(m_ui->comboBox->currentIndex())->getSet(m_ui->cbSets->currentIndex())->setPosition(pos);
            m_ui->lnPos->setStyleSheet("");
            disconnect(m_ui->horizontalSlider, SIGNAL(valueChanged(int)), this, SLOT(onSliderChanged(int)));
            m_ui->horizontalSlider->setSliderPosition(fSystem->getVar(m_ui->comboBox->currentIndex())->getSet(m_ui->cbSets->currentIndex())->getPosition()*FLOATTOINT);
            connect(m_ui->horizontalSlider, SIGNAL(valueChanged(int)), this, SLOT(onSliderChanged(int)));
        }
    }
    // Last set
    else if (currentSet == m_ui->cbSets->count() - 1) {
        // Validate the value
        if (pos <= fSystem->getVar(m_ui->comboBox->currentIndex())->getSet(currentSet-1)->getPosition() ||
            pos >= (fSystem->getVar(m_ui->comboBox->currentIndex())->getSet(currentSet)->getPosition())*2) {

            m_ui->lnPos->setStyleSheet("background-color: rgb(255, 0, 0);");
        }
        else {
            fSystem->getVar(m_ui->comboBox->currentIndex())->getSet(m_ui->cbSets->currentIndex())->setPosition(pos);
            m_ui->lnPos->setStyleSheet("");
            disconnect(m_ui->horizontalSlider, SIGNAL(valueChanged(int)), this, SLOT(onSliderChanged(int)));
            m_ui->horizontalSlider->setSliderPosition(fSystem->getVar(m_ui->comboBox->currentIndex())->getSet(m_ui->cbSets->currentIndex())->getPosition()*FLOATTOINT);
            connect(m_ui->horizontalSlider, SIGNAL(valueChanged(int)), this, SLOT(onSliderChanged(int)));
        }
    }
    // Internal sets
    else {
    // Validate the value
        if (pos <= fSystem->getVar(m_ui->comboBox->currentIndex())->getSet(currentSet-1)->getPosition() ||
            pos >= fSystem->getVar(m_ui->comboBox->currentIndex())->getSet(currentSet+1)->getPosition()) {

            m_ui->lnPos->setStyleSheet("background-color: rgb(255, 0, 0);");
        }
        else {
            fSystem->getVar(m_ui->comboBox->currentIndex())->getSet(m_ui->cbSets->currentIndex())->setPosition(pos);
            m_ui->lnPos->setStyleSheet("");
            disconnect(m_ui->horizontalSlider, SIGNAL(valueChanged(int)), this, SLOT(onSliderChanged(int)));
            m_ui->horizontalSlider->setSliderPosition(fSystem->getVar(m_ui->comboBox->currentIndex())->getSet(m_ui->cbSets->currentIndex())->getPosition()*FLOATTOINT);
            connect(m_ui->horizontalSlider, SIGNAL(valueChanged(int)), this, SLOT(onSliderChanged(int)));
        }
    }
    this->updateCurves();
}

/**
  * Slot called when the user clicks on the OK button. This closes the dialog
  * and keep the modifications.
  */
void FuzzyEditor::onOk()
{
    CoevStats& coevStats = CoevStats::getInstance();
    fSystem->saveToFile("temp.ffs", coevStats.getFitMaxPop1());
    accept();
}

/**
  * Slot called when the user clicks on the OK button. This closes the dialog
  * and drops the modifications.
  */
void FuzzyEditor::onCancel()
{
    fSystem->reset();
    fSystem->loadFromFile("temp.ffs");
    accept();
}

/**
  * Sets which fuzzy system will be modified.
  *
  * @param fileName Name of the fuzzy system file to be saved.
  */
void FuzzyEditor::setSystemFile(QString fileName)
{
    currentOpennedSystem = fileName;
}

/**
  * Sets which dataset corresponds to the currently edited fuzzy system.
  *
  * @param data Dataset file.
  */
void FuzzyEditor::setDataFile(QList<QStringList>* data)
{
    listFile = data;
}

/**
  * Slot called when the user chooses to save the fuzzy system.
  */
void FuzzyEditor::onSaveFuzzy()
{
    CoevStats& coevStats = CoevStats::getInstance();
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save fuzzy system"), ".ffs", "*.ffs");
    if (fileName != NULL) {
        fSystem->saveToFile(fileName, coevStats.getFitMaxPop1());
    }
}

/**
  * Slot called when the user selects a variable in the variable list on the left.
  * The displayed set names are updated.
  *
  * @param row Index of the variable selected
  */
void FuzzyEditor::onVarSelected(int row)
{

    // Check whether this is an input or an output and show the current sets count for the selected variable
    if (row < fSystem->getNbInVars()) {
        m_ui->lblVar->setText("Variable : " + fSystem->getInVar(row)->getName());
        m_ui->cbInSets->setCurrentIndex(fSystem->getInVar(row)->getSetsCount()-1);
    }
    else {
        m_ui->lblVar->setText("Variable : " + fSystem->getOutVar(row-fSystem->getNbInVars())->getName());
        m_ui->cbOutSets->setCurrentIndex(fSystem->getOutVar(row-fSystem->getNbInVars())->getSetsCount()-1);
    }
    // Display the new number of sets
    displaySets();
}

/**
  * Slot called when the user changes the number of sets per input variable.
  * The current rules are updated. If an antecedent refers to a set that has
  * been deleted, it is dropped. The mebership function curves are also updated.
  *
  * @param idx Index indicating the new number of input sets
  */
void FuzzyEditor::onNbInSetsChanged(int idx)
{
    SystemParameters& sysParams = SystemParameters::getInstance();

    // Delta between the new number of sets and the current number of sets
    int delta = idx - fSystem->getNbInSets() + 1;

    int nbInSets = idx+1;

    // Update the number of sets in the system and the parameters
    sysParams.setNbInSets(nbInSets);
    fSystem->setNbInSets(nbInSets);

    // Update the number of sets for all the input variables
    for (int i = 0; i < fSystem->getNbInVars(); i++) {
        int left = delta;
        while (left) {
            // We need to add sets
            if (left > 0) {
                FuzzySet* fSet = new FuzzySet("MF " + QString::number(idx-left+1), fSystem->getInVar(i)->getSet(idx-left)->getPosition()*2, idx-left+1);
                fSystem->getInVar(i)->addSet(fSet);
                left--;
            }
            // We need to remove sets AND FIRST the antecedents in the rules that use these sets
            else {
                pruneInvalidAntecedents(idx-delta);
                fSystem->getInVar(i)->removeLastSet();
                left++;
            }
        }
    }

    // Display the new number of sets
    displaySets();

    // Update the rulesBox display
    for (int i = 0; i < fSystem->getNbRules(); i++) {
        for (int k = 3, varIdx = 0; k < fSystem->getRule(i)->getNbInPairs()*4; k+=4, varIdx++) {
            int left = delta;
            while (left) {
                // Check wether we need to add or remove a set
                if (delta > 0) {
                    RefComboBox* cbSet = qobject_cast<RefComboBox*>(m_ui->rulesGrid->itemAtPosition(i, k)->widget());
                    cbSet->addItem(fSystem->getRule(i)->getInVarAtPos(varIdx)->getSet(idx-left+1)->getName());
                    left--;
                }
                // We remove a set
                else {
                    RefComboBox* cbSet = qobject_cast<RefComboBox*>(m_ui->rulesGrid->itemAtPosition(i, k)->widget());
                    cbSet->removeItem(cbSet->count()-1);
                    left++;
                }
            }
        }
    }

    // Update the membership functions plot
    updateCurves();
}

/**
  * Slot called when the user changes the number of sets per output variable.
  * The current rules are updated. If an antecedent refers to a set that has
  * been deleted, it is dropped. The mebership function curves are also updated.
  *
  * @param idx Index indicating the new number of output sets
  */
void FuzzyEditor::onNbOutSetsChanged(int idx)
{
    SystemParameters& sysParams = SystemParameters::getInstance();

    // Delta between the new number of sets and the current number of sets
    int delta = idx - fSystem->getNbOutSets() + 1;

    int nbOutSets = idx+1;

    // Update the number of sets in the system and the parameters
    sysParams.setNbOutSets(nbOutSets);
    fSystem->setNbOutSets(nbOutSets);

    // Update the number of sets for all the input variables
    for (int i = 0; i < fSystem->getNbOutVars(); i++) {
        int left = delta;
        while (left) {
            // We need to add sets
            if (left > 0) {
                FuzzySet* fSet = new FuzzySet("MF " + QString::number(idx-left+1), fSystem->getOutVar(i)->getSet(idx-left)->getPosition()*2, idx-left+1);
                fSystem->getOutVar(i)->addSet(fSet);
                left--;
            }
            // We need to remove sets AND FIRST the consequents in the rules that use these sets
            else {
                pruneInvalidConsequents(idx-delta);
                fSystem->getOutVar(i)->removeLastSet();
                left++;
            }
        }
    }

    // Display the new number of sets
    displaySets();

    // Update the rulesBox display
    // Rules
    for (int i = 0; i < fSystem->getNbRules(); i++) {
        int baseIdx = MAX_ANTE*4;
        for (int k = 3 + baseIdx, varIdx = 0; k < fSystem->getRule(i)->getNbOutPairs()*4 + baseIdx; k+=4, varIdx++) {

            int left = delta;
            while (left) {
                // Check wether we need to add or remove a set
                if (delta > 0) {
                    RefComboBox* cbSet = qobject_cast<RefComboBox*>(m_ui->rulesGrid->itemAtPosition(i, k)->widget());
                    cbSet->addItem(fSystem->getRule(i)->getOutVarAtPos(varIdx)->getSet(idx-left+1)->getName());
                    left--;
                }
                // We remove a set
                else {
                    RefComboBox* cbSet = qobject_cast<RefComboBox*>(m_ui->rulesGrid->itemAtPosition(i, k)->widget());
                    cbSet->removeItem(cbSet->count()-1);
                    left++;
                }
            }
        }
    }
    // Default rule
    for (int k = 3, index = 0; k < fSystem->getNbOutVars()*4; k+=4, index++) {
        // Clear the combo boxes
        RefComboBox* cbSetDef = qobject_cast<RefComboBox*>(m_ui->rulesGrid->itemAtPosition(fSystem->getNbRules()+1, k)->widget());
        cbSetDef->clear();
        for (int z = 0; z < nbOutSets; z++) {
            cbSetDef->addItem(fSystem->getOutVar(index)->getSet(z)->getName());
        }
    }
}

/**
  * Remove the antecedents referring to a removed set for an input variable.
  *
  * @param invalidSetNum Index of the removed set.
  */
void FuzzyEditor::pruneInvalidAntecedents(int invalidSetNum)
{
    QVector<FuzzyVariable*> inVars;
    QVector<FuzzyVariable*> outVars;
    QVector<int> inSets;
    QVector<int> outSets;

    // Scan all the rules
    for (int i = 0; i < fSystem->getNbRules(); i++) {
        inVars.clear();
        outVars.clear();
        inSets.clear();
        outSets.clear();
        // Scan all the antecedents
        for (int k = 0; k < fSystem->getRule(i)->getNbInPairs(); k++) {
            // Keep only valid antecedens
            if (fSystem->getRule(i)->getInSetAtPos(k)->getNumber() < invalidSetNum) {
                inVars.append(fSystem->getRule(i)->getInVarAtPos(k));
                inSets.append(fSystem->getRule(i)->getInSetAtPos(k)->getNumber());
            }
        }
        // We don't modifiy the consequents here
        for (int k = 0; k < fSystem->getRule(i)->getNbOutPairs(); k++) {
            outVars.append(fSystem->getRule(i)->getOutVarAtPos(k));
            outSets.append(fSystem->getRule(i)->getOutSetAtPos(k)->getNumber());
        }
        // Store the modified rule
        FuzzyRule* rule = new FuzzyRule(inVars, inSets, outVars, outSets);
        fSystem->replaceRule(i, rule);
    }
}

/**
  * Remove the antecedents referring to a removed set for an output variable.
  *
  * @param invalidSetNum Index of the removed set.
  */
void FuzzyEditor::pruneInvalidConsequents(int invalidSetNum)
{
    QVector<FuzzyVariable*> inVars;
    QVector<FuzzyVariable*> outVars;
    QVector<int> inSets;
    QVector<int> outSets;
    /*for (int i = 0; i < fSystem->getNbInSets(); i++) {
        m_ui->cbSets->setItemText(i, fSystem->getInVar(m_ui->comboBox->currentIndex())->getSet(i)->getName());
    }*/
    /// TODO tester le selectvar() ici et vérifier que la logique derrière soit correcte
    // Scan all the rules
    for (int i = 0; i < fSystem->getNbRules(); i++) {
        inVars.clear();
        outVars.clear();
        inSets.clear();
        outSets.clear();

        // We don't modifiy the antecedents here
        for (int k = 0; k < fSystem->getRule(i)->getNbInPairs(); k++) {
            inVars.append(fSystem->getRule(i)->getInVarAtPos(k));
            inSets.append(fSystem->getRule(i)->getInSetAtPos(k)->getNumber());
        }
        // Scan all the consequents
        for (int k = 0; k < fSystem->getRule(i)->getNbOutPairs(); k++) {
            // Keep only valid antecedens
            if (fSystem->getRule(i)->getOutSetAtPos(k)->getNumber() < invalidSetNum) {
                outVars.append(fSystem->getRule(i)->getOutVarAtPos(k));
                outSets.append(fSystem->getRule(i)->getOutSetAtPos(k)->getNumber());
            }
        }
        // Store the modified rule
        FuzzyRule* rule = new FuzzyRule(inVars, inSets, outVars, outSets);
        fSystem->replaceRule(i, rule);
    }
}

/**
  * Slot called when the name of a set has been changed. The rules are updated.
  *
  * @param newName New name of the set.
  * @param li Row number indicating which set has been renamed.
  */
void FuzzyEditor::onSetNameChanged(QString newName, int li, int)
{
     // Check wether we are modifying an input or an output var
    // Input var
    if (m_ui->listVars->currentRow() < fSystem->getNbInVars()) {
        fSystem->getInVar(m_ui->listVars->currentRow())->getSet(li-1)->setName(newName);
        // Update the rulesBox display
        for (int i = 0; i < fSystem->getNbRules(); i++) {
            for (int k = 3, varIdx = 0; k < fSystem->getRule(i)->getNbInPairs()*4; k+=4, varIdx++) {
                // Update the name of the set
                RefComboBox* cbSet = qobject_cast<RefComboBox*>(m_ui->rulesGrid->itemAtPosition(i, k)->widget());
                for (int z = 0; z < m_ui->cbInSets->currentIndex()+1; z++) {
                    cbSet->setItemText(li-1, newName);
                }
            }
        }
        // Update the set combo box in the memberships function editor
        for (int i = 0; i < fSystem->getNbInSets(); i++) {
            m_ui->cbSets->setItemText(i, fSystem->getInVar(m_ui->comboBox->currentIndex())->getSet(i)->getName());
        }
    }
    // Output var
    else {
        fSystem->getOutVar(m_ui->listVars->currentRow() - fSystem->getNbInVars())->getSet(li-1)->setName(newName);
        
        // Update the rulesBox display
        // Rules
        for (int i = 0; i < fSystem->getNbRules(); i++) {
            int baseIdx = MAX_ANTE*4;
            for (int k = 3 + baseIdx, varIdx = 0; k < fSystem->getRule(i)->getNbOutPairs()*4 + baseIdx; k+=4, varIdx++) {
                RefComboBox* cbSet = qobject_cast<RefComboBox*>(m_ui->rulesGrid->itemAtPosition(i, k)->widget());
                for (int z = 0; z < m_ui->cbOutSets->currentIndex()+1; z++) {
                    cbSet->setItemText(li-1, newName);
                }
            }
        }
        // Default rule
        for (int k = 3, index = 0; k < fSystem->getNbOutVars()*4; k+=4, index++) {
            // Clear the combo boxes
            RefComboBox* cbSetDef = qobject_cast<RefComboBox*>(m_ui->rulesGrid->itemAtPosition(fSystem->getNbRules()+1, k)->widget());
            cbSetDef->clear();
            for (int z = 0; z < m_ui->cbOutSets->currentIndex()+1; z++) {
                cbSetDef->addItem(fSystem->getOutVar(index)->getSet(z)->getName());
            }
        }
    }
    // Display the modified sets
    displaySets();
}

/**
  * Update the membership functions curves displayed.
  */
void FuzzyEditor::updateCurves()
{
    // Check whether we need to delete curves or add new ones
    int delta = fSystem->getNbInSets() - curves.size();
    // Add new curves
    if (delta > 0) {  
        for (int i = 1; delta > 0; delta--, i++) {
            QwtPlotCurve* curve = new QwtPlotCurve();
            curves.append(curve);
            // Update the sets combo box
            m_ui->cbSets->addItem(fSystem->getInVar(m_ui->comboBox->currentIndex())->getSet(i)->getName());
        }
    }
    // Delete curves
    else {
        for (; delta < 0; delta++) {
            delete curves.at(curves.size()-1);
            curves.remove(curves.size()-1);
            // Update the sets combo box
            m_ui->cbSets->removeItem(m_ui->cbSets->count()-1);
        }
    }

    onSelectVar();
}
