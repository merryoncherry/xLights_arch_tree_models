#include <mutex>

#include <wx/time.h>

#include "TimeMgt.h"

namespace TimeMgt
{
    static std::mutex overridemtx;

    bool overridden = false;
    wxDateTime overrideTime; // Currently kept as UTC

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

        return wxDateTime::Now();
    }
    wxDateTime getSchedUNowWx()
    {
        {
            std::lock_guard lk(overridemtx);
            if (overridden)
                return overrideTime.ToTimezone(wxDateTime::TZ::Local);
        }

        return wxDateTime::UNow();
    }

    wxDateTime getSchedUNowWxUTC()
    {
        {
            std::lock_guard lk(overridemtx);
            if (overridden)
                return overrideTime;
        }

        return wxDateTime::UNow().ToUTC();
    }

    int64_t getSchedNowMs()
    {
        {
            std::lock_guard lk(overridemtx);
            if (overridden)
                return overrideTime.ToTimezone(wxDateTime::TZ::Local).GetValue().GetValue();
        }       
        return wxGetLocalTimeMillis().GetValue();
    }
    int64_t getSchedNowMsUTC()
    {
        {
            std::lock_guard lk(overridemtx);
            if (overridden)
                return overrideTime.GetValue().GetValue();
        }
        return wxGetLocalTimeMillis().GetValue();
    }

    wxDateTime getRTCNowWx()
    {
        return wxDateTime::Now();
    }
    wxDateTime getRTCUNowWx()
    {
        return wxDateTime::UNow();
    }
    int64_t getRTCNowMs()
    {
        return wxGetLocalTimeMillis().GetValue();
    }
    int64_t getRTCNowMsUTC()
    {
        return wxGetUTCTimeMillis().GetValue();
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
        overridden = true;
    }

    void clearTimeOverride()
    {
        std::lock_guard lk(overridemtx);
        overridden = false;
    }
}