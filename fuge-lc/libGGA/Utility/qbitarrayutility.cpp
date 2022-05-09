#include "qbitarrayutility.h"

QBitArrayUtility::QBitArrayUtility()
{
}

uint QBitArrayUtility::bitArray2Uint(QBitArray *array){
    uint value = 0;
    for ( int i = 0; i < array->size(); ++i ) {
        value <<= 1;
        value += (int)array->at(i);
    }
    return value;
}

QString QBitArrayUtility::bitArray2String(QBitArray *array)
{
    uint value = bitArray2Uint(array);
    QString str;
    str.setNum(value, 10);
    return str;
}

QBitArray* QBitArrayUtility::bitArrayInvert(QBitArray *array)
{
    for ( int i = 0; i < array->size(); ++i ) {
        array->toggleBit(i);
    }
    return array;
}
