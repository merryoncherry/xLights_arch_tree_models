/***************************************************************
 * This source files comes from the xLights project
 * https://www.xlights.org
 * https://github.com/smeighan/xLights
 * See the github commit history for a record of contributing
 * developers.
 * Copyright claimed based on commit dates recorded in Github
 * License: https://github.com/smeighan/xLights/blob/master/License.txt
 **************************************************************/

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
    static int64_t overrideMs = 0;
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
        overrideMs = t.GetValue().GetValue();
        overridden = true;
    }
    void setTimeOverrideUTC(int64_t millis)
    {
        std::lock_guard lk(overridemtx);
        overrideMs = millis;
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
        if (accelerate) {
            // TODO: Should we set override time already?
            setTimeOverrideUTC(start.ToUTC().GetValue().GetValue());
        }
    }

    bool shouldEndRun()
    {
        if (!runTimeRange) {
            return false;
        }
        return getSchedUNowWx() >= rtrEnd;
    }

    static int64_t frameInterval;
    static int64_t frameAt;
    static bool frameOS;
    static int64_t schedInterval;
    static int64_t schedAt;
    static bool schedOS;

    bool isSimulatedTime()
    {
        std::lock_guard lk(overridemtx);
        return accelerated;
    }

    void setNextFrameIn(int64_t ms, bool oneshot)
    {
        std::lock_guard lk(overridemtx);
        frameInterval = ms;
        frameAt = ms <= 0 ? std::numeric_limits<int64_t>::min() : overrideMs + ms;
        frameOS = oneshot;
    }

    int64_t getNextFrameIn(bool& os)
    {
        std::lock_guard lk(overridemtx);
        os = frameOS;
        return frameInterval;
    }

    int64_t getNextFrameAt(bool& os)
    {
        std::lock_guard lk(overridemtx);
        os = frameOS;
        return frameAt;
    }

    void setNextSchedIn(int64_t ms, bool oneshot)
    {
        std::lock_guard lk(overridemtx);
        schedInterval = ms;
        schedAt = ms <= 0 ? std::numeric_limits<int64_t>::min() : overrideMs + ms;
        schedOS = oneshot;
    }

    int64_t getNextSchedIn(bool& os)
    {
        std::lock_guard lk(overridemtx);
        os = schedOS;
        return schedInterval;
    }

    int64_t getNextSchedAt(bool& os)
    {
        std::lock_guard lk(overridemtx);
        os = schedOS;
        return schedAt;
    }
}