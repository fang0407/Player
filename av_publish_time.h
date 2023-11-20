#ifndef AV_PUBLISH_TIME_H
#define AV_PUBLISH_TIME_H

#ifdef _WIN32
#include <winsock2.h>
#include <time.h>
#else
#include <sys/time.h>
#endif

class AVPublishTime
{
public:
    static AVPublishTime& GetInstance();

    void Rest();
    int64_t GetCurrentTimeMsec();
    void SetClock(double pts);
    double GetClock();

private:
    AVPublishTime();

private:
    double pts_diff_;

};

#endif // AV_PUBLISH_TIME_H
