/**
  * @file   coevstats.cpp
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
#include "coevstats.h"

CoevStats::CoevStats() : QObject()
{
    fitMaxPop1 = -1.0;
    fitMinPop1 = -1.0;
    fitAvgPop1 = -1.0;
    fitStdPop1 = -1.0;
    fitMaxPop2 = -1.0;
    fitMinPop1 = -1.0;
    fitAvgPop1 = -1.0;
    fitStdPop1 = -1.0;
    sensi = 0.0;
    speci = 0.0;
    accu = 0.0;
    rmse = 0.0;
    ppv = 0.0;
    rrse = 0.0;
    rae = 0.0;
    mse = 0.0;
    //float msePower;
    distanceThreshold = 0.0;
    distanceMinThreshold = 0.0;
    dontCare = 0.0;
    overLearn = 0.0;
    sizePop1 = 0;
    sizePop2 = 0;
    genNumber = 0;
}

CoevStats::~CoevStats()
{
}

/**
  * Send a signal to the statistics plots when new data is available
  */
void CoevStats::transmitData(QString name)
{
    emit this->transmit(name);
}
