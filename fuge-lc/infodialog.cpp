/**
  * @file   infodialog.cpp
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
  * @class InfoDialog
  *
  * @brief This class implements the generic information dialog.
  */

#include "infodialog.h"
#include "ui_infodialog.h"

InfoDialog::InfoDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::InfoDialog)
{
    m_ui->setupUi(this);
    connect(m_ui->btNo, SIGNAL(clicked()), this, SLOT(notOk()));
    connect(m_ui->btYes, SIGNAL(clicked()), this, SLOT(ok()));
}

InfoDialog::~InfoDialog()
{
    delete m_ui;
}

void InfoDialog::changeEvent(QEvent *e)
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
  * Slot called when the user clicks the OK button.
  */
void InfoDialog::ok()
{
    accept();
}

/**
  * Slot called when the user clicks the CANCEL button.
  */
void InfoDialog::notOk()
{
    reject();
}

/**
  * Sets the information message that will be displayed.
  */
void InfoDialog::setInfo(QString info)
{
    m_ui->labelInfo->setText(info);
}

/**
  * Sets the question message that will be displayed.
  */
void InfoDialog::setQuestion(QString question)
{
    m_ui->labelQuestion->setText(question);
}
