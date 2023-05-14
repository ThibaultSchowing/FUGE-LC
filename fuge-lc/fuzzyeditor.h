/**
  * @file   fuzzyeditor.h
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
  * @class FuzzyEditor
  *
  * @brief This class implements a dialog allowing to completely modify a loaded fuzzy system.
  */

#ifndef FUZZYVIEWER_H
#define FUZZYVIEWER__H

#include <QDialog>
#include <QComboBox>
#include <QLineEdit>
#include <QGridLayout>

/*
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_array.h>
#include <qwt_legend.h>
*/
#include <QLineSeries>
#include <QChart>
#include <QChartView>
#include <QValueAxis>

#include "fuzzysystem.h"
#include "fuzzyrule.h"

namespace Ui {
    class FuzzyEditor;
}

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    using namespace QtCharts;
#endif



class FuzzyEditor : public QDialog {
    Q_OBJECT
    Q_DISABLE_COPY(FuzzyEditor)
public:
    explicit FuzzyEditor(QWidget *parent = 0, FuzzySystem* fSystem = NULL);
    virtual ~FuzzyEditor();

    void setSystemFile(QString fileName);
    void setDataFile(QList<QStringList>* data);

private slots:
    void onSelectVar();
    void onSelectSet();
    void onSliderChanged(int val);
    void onSaveFuzzy();
    void onOk();
    void onCancel();
    void onRulesChanged(int idx, int li, int col);
    void onPosChanged();
    void onVarSelected(int row);
    void onNbInSetsChanged(int idx);
    void onNbOutSetsChanged(int idx);
    void onSetNameChanged(QString newName, int li, int col);

protected:
    virtual void changeEvent(QEvent *e);

private:
    Ui::FuzzyEditor *m_ui;

    QChart* myPlot;
    QValueAxis* axisX;
    QValueAxis* axisY;
    QChartView* myPlotView;
    QVector<QLineSeries*> curves;
    QLineSeries* membCurve;
    /*
    QwtPlot *myPlot;
    QVector<QwtPlotCurve*> curves;
    QwtPlotCurve* membCurve;
    */
    FuzzySystem* fSystem;
    QString currentOpennedSystem;
    QList<QStringList>* listFile;
    QVector<FuzzyRule*> rulesVector;

    void displayRulesBox();
    void displayVars();
    void displaySets();
    void updateRule(int ruleNum);
    void updateCurves();
    void pruneInvalidAntecedents(int invalidSetNum);
    void pruneInvalidConsequents(int invalidSetNum);
};

/**
  * @class RefComboBox
  * @brief This class inherits a ComboBox while adding the position (row,column) information.
  */
class RefComboBox : public QComboBox
{
    Q_OBJECT

    private:
        int li;
        int col;

    public:
        RefComboBox(QWidget *parent = 0, int l = 0, int c = 0) : QComboBox(parent)
        {
            li = l;
            col = c;
            connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(onIdxChanged(int)));
        }
        virtual ~RefComboBox()
        {}

    public slots:
        void onIdxChanged(int idx)
        {
            emit idxChanged(idx, li, col);
        }

    signals:
        void idxChanged(int idx, int li, int col);

};

/**
  * @class RefLineEdit
  * @brief This class inherits a LineEdit while adding the position (row,column) information.
  */
class RefLineEdit : public QLineEdit
{
    Q_OBJECT

    private:
        int li;
        int col;

    public:
        RefLineEdit(QWidget *parent = 0, int l = 0, int c = 0) : QLineEdit(parent)
        {
            li = l;
            col = c;
            connect(this, SIGNAL(textChanged(QString)), this, SLOT(onTextChanged(QString)));
        }
        virtual ~RefLineEdit() {}

    public slots:
        void onTextChanged(QString text)
        {
            emit txtChanged(text, li, col);
        }

    signals:
        void txtChanged(QString txt, int li, int col);
};
#endif // FUZZYVIEWER__H
