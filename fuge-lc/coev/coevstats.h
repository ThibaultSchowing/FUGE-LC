/**
  * @file   coevstats.h
  * @author Jean-Philippe Meylan <jean-philippe.meylan_at_heig-vd.ch>
  * @author ReDS (Reconfigurable and embedded digital systems) <www.reds.ch>
  * @author HEIG-VD (Haute école d'ingénierie et de gestion) <www.heig-vd.ch>
  * @date   03.2010
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
  * @class CoevStats
  * @brief Singleton class which holds the coevolution statistics
  */

#ifndef COEVSTATS_H
#define COEVSTATS_H

#include <QObject>

class CoevStats : public QObject
{
    Q_OBJECT

private:
    CoevStats();
    ~CoevStats();
    CoevStats(const CoevStats&);
    CoevStats& operator=(const CoevStats&);

    double fitMaxPop1;
    double fitMinPop1;
    double fitAvgPop1;
    double fitStdPop1;
    double fitMaxPop2;
    double fitMinPop2;
    double fitAvgPop2;
    double fitStdPop2;
    float sensi;
    float speci;
    float accu;
    float rmse;
    float ppv;

    // MODIF - Bujard - 18.03.2010
    float rrse;
    float rae;
    float  mse;
    //float msePower;
    float distanceThreshold;
    float distanceMinThreshold;
    float dontCare;
    float overLearn;
    // FIN - MODIF - Bujard - 18.03.2010

    unsigned int sizePop1;
    unsigned int sizePop2;
    unsigned int genNumber;
    QString bestSystemDescription;

public:
    static CoevStats& getInstance()
    {
        static CoevStats instance;
        return instance;
    }

    inline void setFitMaxPop1(double value) {fitMaxPop1 = value;}
    inline void setFitMinPop1(double value) {fitMinPop1 = value;}
    inline void setFitAvgPop1(double value) {fitAvgPop1 = value;}
    inline void setFitStdPop1(double value) {fitStdPop1 = value;}
    inline void setFitMaxPop2(double value) {fitMaxPop2 = value;}
    inline void setFitMinPop2(double value) {fitMinPop2 = value;}
    inline void setFitAvgPop2(double value) {fitAvgPop2 = value;}
    inline void setFitStdPop2(double value) {fitStdPop2 = value;}
    inline void setSensi(float value) {sensi = value;}
    inline void setSpeci(float value) {speci = value;}
    inline void setAccu(float value) {accu = value;}
    inline void setRmse(float value) {rmse = value;}
    inline void setPpv(float value) {ppv = value;}
    // MODIF - Bujard - 18.03.2010
    inline void setRrse     (float value) {rrse       = value;}
    inline void setRae      (float value) {rae        = value;}
    inline void setMse ( float value) {mse = value;}
    inline void setDistanceThreshold (float value ) {distanceThreshold = value;}
    inline void setDistanceMinThreshold (float value ) {distanceMinThreshold = value;}
    inline void setDontCare (float value ){dontCare = value;}
    inline void setOverLearn (float value){overLearn = value;}
    // FIN - MODIF - Bujard - 18.03.2010
    inline void setSizePop1(unsigned int value) {sizePop1 = value;}
    inline void setSizePop2(unsigned int value) {sizePop2 = value;}
    inline void setBestSysDesc(QString desc) {bestSystemDescription = desc;}
    inline void setGenNumber(unsigned int value) {genNumber = value;}

    inline double getFitMaxPop1() {return fitMaxPop1;}
    inline double getFitMinPop1() {return fitMinPop1;}
    inline double getFitStdPop1() {return fitStdPop1;}
    inline double getFitAvgPop1() {return fitAvgPop1;}
    inline double getFitMaxPop2() {return fitMaxPop2;}
    inline double getFitMinPop2() {return fitMinPop2;}
    inline double getFitStdPop2() {return fitStdPop2;}
    inline double getFitAvgPop2() {return fitAvgPop2;}
    inline float getSensi() {return sensi;}
    inline float getSpeci() {return speci;}
    inline float getAccu() {return accu;}
    inline float getRmse() {return rmse;}
    inline float getPpv() {return ppv;}
    // MODIF - Bujard - 18.03.2010
    inline float getRrse     () {return rrse;    }
    inline float getRae      () {return rae;     }
    float getMse () {return mse;}
    //float getMsePower () {return msePower;}
    float getDistanceThreshold () {return distanceThreshold;}
    float getDistanceMinThreshold () {return distanceMinThreshold;}
    float getDontCare () {return dontCare;}
    float getOverLearn () {return overLearn;}
    // FIN - MODIF - Bujard - 18.03.2010
    inline double getSizePop1() {return sizePop1;}
    inline double getSizePop2() {return sizePop2;}
    inline QString* getBestSysDesc(void) {return &bestSystemDescription;}
    inline unsigned int getGenNumber() {return genNumber;}

    void transmitData(QString name);

signals:
    void transmit(QString name);
};

#endif // COEVSTATS_H
