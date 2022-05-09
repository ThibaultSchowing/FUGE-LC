/**
  * @file   editparamsdialog.h
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

#ifndef EDITPARAMSDIALOG_H
#define EDITPARAMSDIALOG_H

#include <QtGui/QDialog>
#include <QVBoxLayout>

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

private slots:
    void accept();
    void displayFitFct();
    void changePath(QString path);
    bool checkAndSetValues();
    void onBtPathPressed();
    void onLinePathEdited(QString path);
    void onChbActivateThresh();
    void on_buttonBox_accepted();
};

#endif // EDITPARAMSDIALOG_H
