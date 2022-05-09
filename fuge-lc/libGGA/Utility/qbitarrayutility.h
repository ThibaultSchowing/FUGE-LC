#ifndef QBITARRAYUTILITY_H
#define QBITARRAYUTILITY_H

#include "QBitArray"
#include "QString"

class QBitArrayUtility
{
public:
    QBitArrayUtility();
    static uint bitArray2Uint(QBitArray *array);
    static QString bitArray2String(QBitArray *array);
    static QBitArray *bitArrayInvert(QBitArray *array);
};

#endif // QBITARRAYUTILITY_H
