#include <mutex>

#include <wx/time.h>

#include "TimeMgt.h"

namespace TimeMgt
{
    static std::mutex overridemtx;

    // This is the time to return from sched calls below
    // It doesn't change with time unless we are told.
    // It may or may not be close to the real world time depending
    //   on whether we're just stabilizing the current time, or
    //   doing a test/simulation of another time interval
    static bool overridden = false;
    static wxDateTime overrideTime;

    // This is for pretending it is a different time
    //   than it really is... test/simulation/rehearsal purposes
    static bool runTimeRange = false;
    static wxDateTime rtrStart, rtrEnd;
    static wxDateTime startedAt;
    static wxLongLong timeDelta = 0;
    static bool accelerated;

    wxDateTime getSchedNowWx()
    {
        {
            std::lock_guard lk(overridemtx);
            if (overridden) {
                wxDateTime rv = overrideTime.ToTimezone(wxDateTime::TZ::Local);
                rv.SetMillisecond(0);
                return rv;
            }
        }

        return wxDateTime::Now().Subtract(timeDelta);
    }
    wxDateTime getSchedUNowWx()
    {
        {
            std::lock_guard lk(overridemtx);
            if (overridden)
                return overrideTime.ToTimezone(wxDateTime::TZ::Local);
        }

        return wxDateTime::UNow().Subtract(timeDelta);
    }

    wxDateTime getSchedUNowWxUTC()
    {
        {
            std::lock_guard lk(overridemtx);
            if (overridden)
                return overrideTime;
        }

        return wxDateTime::UNow().ToUTC().Subtract(timeDelta);
    }

    int64_t getSchedNowMs()
    {
        {
            std::lock_guard lk(overridemtx);
            if (overridden)
                return overrideTime.ToTimezone(wxDateTime::TZ::Local).GetValue().GetValue();
        }       
        return (wxGetLocalTimeMillis()-timeDelta).GetValue();
    }
    int64_t getSchedNowMsUTC()
    {
        {
            std::lock_guard lk(overridemtx);
            if (overridden)
                return overrideTime.GetValue().GetValue();
        }
        return (wxGetLocalTimeMillis()-timeDelta).GetValue();
    }

    wxDateTime getRTCNowWx()
    {
        return wxDateTime::Now().Subtract(timeDelta);
    }
    wxDateTime getRTCUNowWx()
    {
        return wxDateTime::UNow().Subtract(timeDelta);
    }
    int64_t getRTCNowMs()
    {
        return (wxGetLocalTimeMillis()-timeDelta).GetValue();
    }
    int64_t getRTCNowMsUTC()
    {
        return (wxGetUTCTimeMillis()-timeDelta).GetValue();
    }

    void setTimeOverrideUTC(const wxDateTime& t)
    {
        std::lock_guard lk(overridemtx);
        overrideTime = t;
        overridden = true;
    }
    void setTimeOverrideUTC(int64_t millis)
    {
        std::lock_guard lk(overridemtx);
        overrideTime.Set(time_t(millis/1000));
        overrideTime.SetMillisecond(millis % 1000);
        overrideTime = wxDateTime(wxLongLongNative(millis));
        overridden = true;
    }

    void clearTimeOverride()
    {
        std::lock_guard lk(overridemtx);
        overridden = false;
    }

    void setRunTimeRange(const wxDateTime& start, const wxDateTime& end, bool accelerate)
    {
        runTimeRange = true;
        rtrStart = start;
        rtrEnd = end;
        startedAt = wxDateTime::UNow();
        auto diff = startedAt - rtrStart;
        timeDelta = diff.GetValue();
        accelerated = accelerate;
    }

    bool shouldEndRun()
    {
        if (!runTimeRange) {
            return false;
        }
        return getSchedUNowWx() >= rtrEnd;
    }
}