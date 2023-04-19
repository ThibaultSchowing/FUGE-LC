/**
  * @file   statsplot.h
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

#ifndef STATSPLOT_H
#define STATSPLOT_H

#include <QDialog>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QCloseEvent>
#include <QTime>

#include <QLineSeries>
#include <QChart>
#include <QChartView>
#include <QValueAxis>

/*
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_array.h>
#include <qwt_legend.h>*/

#include <iostream>
#include <unistd.h>

#include "ui_statsplot.h"
#include "systemparameters.h"
#include "coevstats.h"

namespace Ui {
    class StatsPlot;
}

using namespace QtCharts;

class StatsPlot : public QDialog {
    Q_OBJECT
    Q_DISABLE_COPY(StatsPlot)
public:
    explicit StatsPlot(QWidget *parent = 0);
    virtual ~StatsPlot();

    void setLogFileName(QString fileName);

public slots:
    void receiveData(QString name);
    void onClearStats();
    void onCloseStats();
    void onShowGraph();

protected:
    virtual void changeEvent(QEvent *e);

private:
    Ui::StatsPlot *m_ui;
    QVector<double>* xValsPop1;
    QVector<double>* xValsPop2;
    QVector<double>* yValsPop1;
    QVector<double>* yValsPop2;
    QVector<double>* yValsAvgPop1;
    QVector<double>* yValsAvgPop2;


    QChart* myPlot;
    QValueAxis* axisX;
    QValueAxis* axisY;
    QChartView* myPlotView;
    QLineSeries* fitMaxPop1Curve;
    QLineSeries* fitMaxPop2Curve;
    QLineSeries* fitAvgPop1Curve;
    QLineSeries* fitAvgPop2Curve;
    qint32 pop1Index = 0;
    qint32 pop2Index = 0;
    qint32 curMax = 0;
    /*
    QwtPlot *myPlot;
    QwtPlotCurve* fitMaxPop1Curve;
    QwtPlotCurve* fitMaxPop2Curve;
    QwtPlotCurve* fitAvgPop1Curve;
    QwtPlotCurve* fitAvgPop2Curve;
    QwtLegend* legend;*/

    QFile* fitLogFile;
    QString logFileName;
    QString bestSystemDesc;
    QTextStream* out;
    double bestFitPop1;
    double bestFitPop2;
    double bestDevPop1;
    double bestDevPop2;
    bool isShowed;

    void closeEvent(QCloseEvent *event);
};

#endif // STATSPLOT_H
