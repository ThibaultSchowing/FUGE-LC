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

    explicit DatasetSplitter(QWidget *parent, QVector<quint32>* indexes, ValidatorType* vt, QList<QStringList>* listFile, QString datasetName);
    ~DatasetSplitter();

    static void generateIndexesHoldout(QVector<quint32>* indexes, QList<QStringList>* listFile, int train , int validate, int test);
    static void generateIndexesKFold(QVector<quint32>* indexes, QList<QStringList>* listFile, int nbPartitions);

private:
    Ui::DatasetSplitter *ui;
    QVector<QGroupBox*> validationGroups;
    QVector<quint32>* indexes;
    ValidatorType* vt;
    QList<QStringList>* listFile;
    QString datasetName;
    void treatHoldout();
    void treatKFold();

private slots:
    void onSelectedValidationType(int);
    void onOk();
};

#endif // DATASETSPLITTER_H
