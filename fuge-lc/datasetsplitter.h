#ifndef DATASETSPLITTER_H
#define DATASETSPLITTER_H

#include <QDialog>
#include <QGroupBox>

namespace Ui {
    class DatasetSplitter;
}

class DatasetSplitter : public QDialog
{
    Q_OBJECT

public:
    enum ValidatorType {
        HOLD_OUT,
        K_FOLD,
        NONE
    };

    explicit DatasetSplitter(QWidget *parent, QVector<quint32>* indexes, QVector<quint32>* updatedIndexes, ValidatorType* vt, const QList<QStringList>& listFile, QString datasetName);
    ~DatasetSplitter();

    static void generateIndexesHoldout(QVector<quint32>* indexes, const QList<QStringList>& listFile, QVector<quint32>* updatedIndexes, int train , int validate, int test);
    static void generateIndexesKFold(QVector<quint32>* indexes, const QList<QStringList>& listFile, QVector<quint32>* updatedIndexes, int nbPartitions);

private:
    Ui::DatasetSplitter *ui;
    QVector<QGroupBox*> validationGroups;
    QVector<quint32>* separatorIndexes;
    ValidatorType* vt;
    const QList<QStringList>& listFile;
    QVector<quint32>* updatedIndexes;
    QString datasetName;
    void treatHoldout();
    void treatKFold();
    static void defaultTreatDataset(QVector<quint32>* updatedIndexes, const QList<QStringList>& listFile);
    void displayError(const QString& title, const QString& message);
    int askYesNo(const QString& title, const QString& message);
    void writeInFile(const QString& path, int min, int max);

private slots:
    void onSelectedValidationType(int);
    void onOk();
    void checkValues();
};

#endif // DATASETSPLITTER_H
