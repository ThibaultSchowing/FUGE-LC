/**
  * @file   editparamsdialog.h
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
  * @class EditParamsDialog
  *
  * @brief This class implements the parameters edition dialog.
  */

#ifndef EDITPARAMSDIALOG_H
#define EDITPARAMSDIALOG_H

#include <iostream>
#include <cmath>

#include <QDialog>
#include <QVBoxLayout>
#include <QHelpEvent>
#include <QToolTip>
#include <QLineEdit>
#include <QFile>
#include <QFileDialog>
#include <QDebug>
#include <QTextStream>


#include "systemparameters.h"
#include "errordialog.h"

#define DEFAULTCONFIGNAME "defaultUserConfig.conf"

namespace Ui {
    class EditParamsDialog;
}

class EditParamsDialog : public QDialog {
    Q_OBJECT
    Q_DISABLE_COPY(EditParamsDialog)
public:
    explicit EditParamsDialog(QWidget *parent, bool* paramsLoaded, bool scriptLoaded);
    virtual ~EditParamsDialog();

protected:
    virtual void changeEvent(QEvent *e);

private:
    Ui::EditParamsDialog *m_ui;

    bool validateIntEntry(QString stringValue, int min, int max);
    bool validateFloatEntry(QString stringValue, float min, float max);
    void onParamsLoaded();
    bool* paramsLoaded;
    bool defaultValues;
    QVBoxLayout *vbox;

    //TODO: Implement a real config manager (Should not be in the UI)
    void saveConfig(QString filename);
    void loadConfig(QString filename);
    void setEnabledCoevParams(bool value);
    QString FormatParameterDisplay(const QString& str);

private slots:
    void accept();
    void displayFitFct();
    void changePath(QString path);
    bool checkAndSetValues();
    void onBtPathPressed();
    void onLinePathEdited(QString path);
    void onChbActivateThresh();
    void on_buttonBoxClose_accepted();
    void on_pushButton_SaveAsUserDefault_clicked();
    void on_pushButton_SaveAs_clicked();
    void on_pushButton_LoadFile_clicked();
    void on_pushButton_LoadUserDefault_clicked();
    void on_comboBox_LearningMethod_activated(int index);
    void on_comboBox_MembershipInitialization_activated(int index);
    void toggle_level_two(bool b);
    void toggle_level_three(bool b);
    void display_level_one();
    void display_level_two();
    void display_level_three();
};

#endif // EDITPARAMSDIALOG_H
