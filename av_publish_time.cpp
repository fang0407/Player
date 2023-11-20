#include "av_publish_time.h"

AVPublishTime &AVPublishTime::GetInstance()
{
    static AVPublishTime instance;
    return instance;
}

void AVPublishTime::Rest()
{
}

AVPublishTime::AVPublishTime()
{
}

int64_t AVPublishTime::GetCurrentTimeMsec()
{
#ifdef _WIN32
        struct timeval tv;
        time_t clock;
        struct tm tm;
        SYSTEMTIME wtm;
        GetLocalTime(&wtm);
        tm.tm_year = wtm.wYear - 1900;
        tm.tm_mon = wtm.wMonth - 1;
        tm.tm_mday = wtm.wDay;
        tm.tm_hour = wtm.wHour;
        tm.tm_min = wtm.wMinute;
        tm.tm_sec = wtm.wSecond;
        tm.tm_isdst = -1;
        clock = mktime(&tm);
        tv.tv_sec = clock;
        tv.tv_usec = wtm.wMilliseconds * 1000;
        return ((unsigned long long)tv.tv_sec * 1000 + ( long)tv.tv_usec / 1000);
#else
        struct timeval tv;
        gettimeofday(&tv, NULL);
        return ((unsigned long long)tv.tv_sec * 1000 + (long)tv.tv_usec / 1000);
#endif
}

void AVPublishTime::SetClock(double pts)
{
    double time = GetCurrentTimeMsec() / 1000.0;
    pts_diff_ = pts - time;
}

double AVPublishTime::GetClock()
{
    double time = GetCurrentTimeMsec() / 1000.0;
    return pts_diff_ + time;
}
