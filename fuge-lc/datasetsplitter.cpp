#include "datasetsplitter.h"
#include "ui_datasetsplitter.h"
#include <algorithm>
#include <random>
#include "errordialog.h"

#define KFOLD_AMOUNT 10

DatasetSplitter::DatasetSplitter(QWidget *parent, QVector<quint32>* indexes, ValidatorType* vt, QList<QStringList>* listFile, QString datasetName) :
    QDialog(parent),
    ui(new Ui::DatasetSplitter),
    indexes(indexes), vt(vt), listFile(listFile), datasetName(datasetName)
{
    ui->setupUi(this);

    ui->label_dataInfo->setText("<font color = green> Dataset loaded : " + datasetName + "<font>");
    ui->label_dataVars->setText(QString::number(listFile->at(0).size()-1) + " variables");
    ui->label_dataSamples->setText(QString::number(listFile->size()-1) + " samples");

    connect(ui->lineEdit_test, SIGNAL(textChanged(QString)), this, SLOT(checkValues()));
    connect(ui->lineEdit_train, SIGNAL(textChanged(QString)), this, SLOT(checkValues()));
    connect(ui->lineEdit_validation, SIGNAL(textChanged(QString)), this, SLOT(checkValues()));
    connect(ui->cbox_validation_type, SIGNAL(currentIndexChanged(int)), this, SLOT(onSelectedValidationType(int)));
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(onOk()));

    validationGroups.push_back(ui->groupBox_holdout);
    validationGroups.push_back(ui->groupBox_kfold);
    ui->cbox_validation_type->addItem(QString("Hold-out"));
    ui->cbox_validation_type->addItem(QString("K-Fold"));

    for (int i = 2; i <= KFOLD_AMOUNT; i++) {
        ui->cbox_partition_amount->addItem(QString::number(i));
    }
}

DatasetSplitter::~DatasetSplitter()
{
    delete ui;
}

void DatasetSplitter::onSelectedValidationType(int index) {
    if (index >= 0) {
        for (int i = 0; i < validationGroups.length(); ++i) {
            validationGroups.at(i)->setVisible(false);
        }

        validationGroups.at(index)->setVisible(true);
    }
}

void DatasetSplitter::generateIndexesHoldout(QVector<quint32>* indexes, QList<QStringList>* listFile, int train , int validate, int test) {
    // debug, should return errors in the future
    assert(train > 0);
    assert(validate > 0);
    assert(test > 0);
    assert(train + validate + test == 100);

    // treat the data, in this case we simply shuffle
    std::random_device rd;
    std::mt19937 generator(rd());

    std::shuffle(++listFile->begin(), listFile->end(), generator);

    // generate the indexes
    indexes->clear();
    indexes->push_back(train * listFile->size() / 100);
    indexes->push_back((train + validate) * listFile->size() / 100);

    // debug
    for(int i = 0; i < indexes->size(); i++) {
        qDebug() << indexes->at(i);
    }
}

void DatasetSplitter::generateIndexesKFold(QVector<quint32>* indexes, QList<QStringList>* listFile, int nbPartitions) {
    // treat the data, in this case we simply shuffle
    std::random_device rd;
    std::mt19937 generator(rd());
    std::shuffle(++listFile->begin(), listFile->end(), generator);

    // generate the indexes
    indexes->clear();
    for(int i = 1; i < nbPartitions; i++) {
        indexes->push_back(i * listFile->size() / nbPartitions);
    }

    // debug
    for(int i = 0; i < indexes->size(); i++) {
        qDebug() << indexes->at(i);
    }

}

void DatasetSplitter::onOk() {
    int selectedIndex = ui->cbox_validation_type->currentIndex();

    if (selectedIndex == 0) {
        treatHoldout();
    } else if (selectedIndex == 1) {
        treatKFold();
    }
}

void DatasetSplitter::treatHoldout() {
    QString trainStr = ui->lineEdit_train->text();
    QString validateStr = ui->lineEdit_validation->text();
    QString testStr = ui->lineEdit_test->text();

    int train = trainStr.toInt();
    int validate = validateStr.toInt();
    int test = testStr.toInt();
    int total = train + validate + test;
    if (total != 100 || validate <= 0 || train <= 0 || test <= 0) {
        ErrorDialog errDiag;
        errDiag.setError("Bad values");
        errDiag.setInfo("The cumulative entered values must be 100, and all individual values must be at least 1.");
        errDiag.exec();
        return;
    }

    *vt = HOLD_OUT;
    generateIndexesHoldout(indexes, listFile, train, validate, test);
}

void DatasetSplitter::treatKFold() {
    int nbPartitions = ui->cbox_partition_amount->currentIndex() + 2;
    *vt = K_FOLD;
    generateIndexesKFold(indexes, listFile, nbPartitions);
}

void DatasetSplitter::checkValues() {
    QString trainStr = ui->lineEdit_train->text();
    QString validateStr = ui->lineEdit_validation->text();
    QString testStr = ui->lineEdit_test->text();

    int train = trainStr.toInt();
    int validate = validateStr.toInt();
    int test = testStr.toInt();

    int total = train + validate + test;

    if (validate <= 0 || total != 100) {
        ui->lineEdit_validation->setStyleSheet("background-color: rgb(255, 0, 0);");
    }
    else {
        ui->lineEdit_validation->setStyleSheet("");
    }

    if (train <= 0 || total != 100) {
        ui->lineEdit_train->setStyleSheet("background-color: rgb(255, 0, 0);");
    }
    else {
        ui->lineEdit_train->setStyleSheet("");
    }

    if (test <= 0 || total != 100) {
        ui->lineEdit_test->setStyleSheet("background-color: rgb(255, 0, 0);");
    }
    else {
        ui->lineEdit_test->setStyleSheet("");
    }
}
