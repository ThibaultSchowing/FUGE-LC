/**
 * @file randomgenerator.cpp
 * @author Yvan Da Silva <yvan.m.silva AT gmail.com>
 * @author IICT Institute for Information and Communication<www.iict.ch>
 * @author HEIG-VD (Haute école d'inénierie et de gestion) <www.heig-vd.ch>
 * @date 02.2012
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
 * @class RandomGenerator
 * @brief A random generator
 *
 * RandomGenerator is a random generator, thread safe.
 * It uses the Qt function qsrand and qrand to be multi-platform
 * compatible.
 * RangomGenerator works as a singleton and by so
 * no pointer of it should be kept on the user code.
 */

#include "randomgenerator.h"
#include <cstdlib>
QMutex RandomGenerator::mutex;
RandomGenerator* RandomGenerator::randomGenerator = NULL;


//auto_ptr<RandomGenerator::randomGenerator = QSharedPointer<RandomGenerator>(new MyObject);

const qreal RandomGenerator::RANDMAX_PLUSONE = (qreal)RAND_MAX+1;

RandomGenerator::RandomGenerator() :
    QThread()
{
    resetSeed();
}

RandomGenerator *RandomGenerator::getGeneratorInstance(){
    //Thread safety, automatically realeased at the end of the function.
    QMutexLocker locker(&RandomGenerator::mutex);

    if(randomGenerator == NULL)
        randomGenerator = new RandomGenerator();

    return randomGenerator;
}

qint32 RandomGenerator::random(qint32 min, qint32 max){
    if(min > max){
        return (qrand() / RANDMAX_PLUSONE) * (min-max+1);
    }else{
        return (qrand() / RANDMAX_PLUSONE) * (max+1-min);
    }
}

qreal RandomGenerator::randomReal(qreal min, qreal max){
    if(min > max){
        return ((qreal)qrand() / (qreal)RANDMAX_PLUSONE) * (min-max);
    }else{
        return ((qreal)qrand() / (qreal)RANDMAX_PLUSONE) * (max-min);
    }
}

qint32 RandomGenerator::randomNoRandMax(qint32 min, qint32 max){
    if(min < max){
        return qrand()%(max-min+1) + min;
    }else{
        return qrand()%(min-max+1) + max;
    }
}

void RandomGenerator::resetSeed(){
    qsrand(QTime::currentTime().msec());
}
