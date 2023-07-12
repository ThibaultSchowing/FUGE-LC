 #include "datasetsplitter.h"
#include "ui_datasetsplitter.h"
#include <algorithm>
#include <random>
#include <QFile>
#include <QFileInfo>
#include "errordialog.h"
#include "infodialog.h"
#include "projectmanager.h"

#define KFOLD_AMOUNT 10

DatasetSplitter::DatasetSplitter(QWidget *parent, QVector<quint32>* separatorIndexes, QVector<quint32>* updatedIndexes, ValidatorType* vt, const QList<QStringList>& listFile, QString datasetName) :
    QDialog(parent),
    ui(new Ui::DatasetSplitter),
    separatorIndexes(separatorIndexes), vt(vt), listFile(listFile), datasetName(datasetName), updatedIndexes(updatedIndexes)
{
    ui->setupUi(this);

    this->setWindowTitle("Validation set tool");

    ui->label_dataInfo->setText("<font color = green> Dataset loaded : " + datasetName + "<font>");
    ui->label_dataVars->setText(QString::number(listFile.at(0).size()-1) + " variables");
    ui->label_dataSamples->setText(QString::number(listFile.size()-1) + " samples");

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

/**
 * @brief Updates the displayed groupboxes
 * @param index Of the combobox
 */
void DatasetSplitter::onSelectedValidationType(int index) {
    if (index >= 0) {
        for (int i = 0; i < validationGroups.length(); ++i) {
            validationGroups.at(i)->setVisible(false);
        }

        validationGroups.at(index)->setVisible(true);
    }
}

/**
 * @brief handles closing the window by pressing ok
 */
void DatasetSplitter::onOk() {
    int selectedIndex = ui->cbox_validation_type->currentIndex();

    if (selectedIndex == 0) {
        treatHoldout();
    } else if (selectedIndex == 1) {
        treatKFold();
    }
}

/**
 * @brief Randomly shuffles the dataset indexes.
 * @param updatedIndexes QVector that contains the updated indexes
 * @param listFile Loaded dataset
 */
void DatasetSplitter::defaultTreatDataset(QVector<quint32>* updatedIndexes, const QList<QStringList>& listFile) {
    updatedIndexes->clear();
    for (int i = 0; i < listFile.length(); ++i) {
        updatedIndexes->push_back(i);
    }

    // treat the data, in this case we simply shuffle
    std::random_device rd;
    std::mt19937 generator(rd());

    std::shuffle(updatedIndexes->begin(), updatedIndexes->end(), generator);
}

/**
 * @brief Static method allowing to apply a hold-out dataset division
 * @param indexes QVector containing the splitting indexes
 * @param listFile Loaded dataset
 * @param updatedIndexes QVector containing the new indexes of the existing dataset
 * @param train
 * @param validate
 * @param test
 */
void DatasetSplitter::generateIndexesHoldout(QVector<quint32>* indexes, const QList<QStringList>& listFile, QVector<quint32>* updatedIndexes, int train , int validate, int test) {
    defaultTreatDataset(updatedIndexes, listFile);

    // generate the indexes
    indexes->clear();
    indexes->push_back(train * listFile.size() / 100);
    indexes->push_back((train + validate) * listFile.size() / 100);
}

/**
 * @brief Static method allowing to apply a K-Fold dataset division
 * @param indexes QVector containing the splitting indexes
 * @param listFile Loaded dataset
 * @param updatedIndexes QVector containing the new indexes of the existing dataset
 * @param nbPartitions Amount of partitions for K-Fold
 */
void DatasetSplitter::generateIndexesKFold(QVector<quint32>* indexes, const QList<QStringList>& listFile, QVector<quint32>* updatedIndexes, int nbPartitions) {
    defaultTreatDataset(updatedIndexes, listFile);

    // generate the indexes
    indexes->clear();
    for(int i = 1; i < nbPartitions; i++) {
        indexes->push_back(i * listFile.size() / nbPartitions);
    }
}

/**
 * @brief Applies the hold-out data set division using the GUI
 */
void DatasetSplitter::treatHoldout() {
    QString trainStr = ui->lineEdit_train->text();
    QString validateStr = ui->lineEdit_validation->text();
    QString testStr = ui->lineEdit_test->text();

    int train = trainStr.toInt();
    int validate = validateStr.toInt();
    int test = testStr.toInt();
    int total = train + validate + test;
    if (total != 100 || validate <= 0 || train <= 0 || test <= 0) {
        displayError("Bad values", "The cumulative entered values must be 100, and all individual values must be at least 1.");
        return;
    }

    *vt = HOLD_OUT;
    generateIndexesHoldout(separatorIndexes, listFile, updatedIndexes, train, validate, test);

    // write to file if the user wants
    if (askYesNo("Save to file", "Would you like to save the result in a different file?") == DialogCode::Accepted) {
        ProjectManager& pm = ProjectManager::getInstance();
        QString path = pm.getSavePath() == "./" ? pm.getDefaultFilePath() : pm.getPojectGeneratedDatasetPath();
        QString dsetName = QFileInfo(datasetName).fileName();
        dsetName.truncate(dsetName.length() - 4);
        qDebug() << dsetName;
        writeInFile(path + dsetName + "_train" + ".csv", 1, separatorIndexes->at(0));
        writeInFile(path + dsetName + "_validate" + ".csv", separatorIndexes->at(0), separatorIndexes->at(1));
        writeInFile(path + dsetName + "_test" + ".csv", separatorIndexes->at(1), listFile.length());
    }
}

/**
 * @brief Applies the k-fold data set divistion using the GUI
 */
void DatasetSplitter::treatKFold() {
    int nbPartitions = ui->cbox_partition_amount->currentIndex() + 2;
    *vt = K_FOLD;
    generateIndexesKFold(separatorIndexes, listFile, updatedIndexes, nbPartitions);

    // write to file if the user wants
    if (askYesNo("Save to file", "Would you like to save the result in a different file?") == DialogCode::Accepted) {
        ProjectManager& pm = ProjectManager::getInstance();
        QString path = pm.getSavePath() == "./" ? pm.getDefaultFilePath() : pm.getPojectGeneratedDatasetPath();
        QString dsetName = QFileInfo(datasetName).fileName();
        dsetName.truncate(dsetName.length() - 4);

        writeInFile(path + dsetName + "_fold_" + QString::number(1) + ".csv", 1, separatorIndexes->at(0));
        for (int i = 1; i < separatorIndexes->length(); i++) {
            writeInFile(path + dsetName + "_fold_" + QString::number(i + 1) + ".csv" , separatorIndexes->at(i - 1), separatorIndexes->at(i));
        }
        writeInFile(path + dsetName + "_fold_" + QString::number(separatorIndexes->length() + 1) + ".csv", separatorIndexes->last(), listFile.length());

        // remove extra folds that might have been generated in the past
        int j = separatorIndexes->length() + 2;
        while (QFile::remove(path + dsetName + "_fold_" + QString::number(j) + ".csv")) {
            j++;
        }
    }
}

/**
 * @brief Verifies if the values entered in the lineEdits for the hold-out validation are set properly
 */
void DatasetSplitter::checkValues() {
    QString trainStr = ui->lineEdit_train->text();
    QString validateStr = ui->lineEdit_validation->text();
    QString testStr = ui->lineEdit_test->text();

    // Normally, toInt takes a ref to a boolean. The value is set to true if the string is a number
    // When the conversion fails, the value is 0.
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

/**
 * @brief Displays an error through ErrorDialod
 */
void DatasetSplitter::displayError(const QString& title, const QString& message) {
    ErrorDialog errDiag;
    errDiag.setError(title);
    errDiag.setInfo(message);
    errDiag.exec();
}

/**
 * @brief Asks a question to the user through InfoDialog
 * @return Code of the execution, that should be treated with DialogCode::VALUE
 */
int DatasetSplitter::askYesNo(const QString& title, const QString& message) {
    InfoDialog infDiag;
    infDiag.setWindowTitle(title);
    infDiag.setQuestion(message);
    return infDiag.exec();
}

/**
 * @brief Writes the split dataset into a file using the updated index with [min, max[
 *        The data needs to be treated beforehands
 * @param path Path to write
 * @param min Starting index of the dataset
 * @param max 1 + Ending index of the dataset
 */
void DatasetSplitter::writeInFile(const QString& path, int min, int max) {
    QFile::remove(path);
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)){
        qDebug() << "Could not create the " << path << " file.";
    }else{
        QTextStream out(&file);
        for (int i = 0; i < listFile.at(0).length() - 1; ++i) {
            out << listFile.at(0).at(i) << ";";
        }
        out << listFile.at(0).at(listFile.at(0).length() - 1) << "\n";
        int index;
        for (int i = min; i < max; i++){
            index = updatedIndexes->at(i);
            for (int j = 0; j < listFile.at(index).length() - 1; ++j) {
                out << listFile.at(index).at(j) << ";";
            }
            out << listFile.at(index).at(listFile.at(index).length() - 1) << "\n";
        }
        file.close();
    }
}

