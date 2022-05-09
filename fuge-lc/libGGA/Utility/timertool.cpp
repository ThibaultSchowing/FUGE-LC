#include "timertool.h"

/**
  * Get time elapsed between two time values.
  * \param timeval start value.
  * \param timeval end value.
  * \return the time elapsed between the two times in ms.
  */
qreal TimerTool::getElapsedTime(struct timeval &startTime, struct timeval &endTime){
    qreal elapsedTime;
    elapsedTime = (endTime.tv_sec - startTime.tv_sec) * 1000.0;      // sec to ms
    elapsedTime += (endTime.tv_usec - startTime.tv_usec) / 1000.0;   // us to ms
    return elapsedTime;
}
