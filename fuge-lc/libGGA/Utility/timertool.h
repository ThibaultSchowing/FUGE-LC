#ifndef TIMERTOOL_H
#define TIMERTOOL_H
#include <sys/time.h>
#include <Qt>

class TimerTool
{
public:
    static qreal getElapsedTime(struct timeval &startTime, struct timeval &endTime);
};

#endif // TIMERTOOL_H
