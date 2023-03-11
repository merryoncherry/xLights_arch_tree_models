#include <mutex>

#include "TimeMgt.h"

namespace TimeMgt
{
    static std::mutex overridemtx;

    bool overridden = false;
    wxDateTime overrideTime;

    wxDateTime getSchedNowWx()
    {
        {
            std::lock_guard lk(overridemtx);
            if (overridden)
                return overrideTime;
        }

        return wxDateTime::Now();
    }

    wxDateTime getRTCNowWx()
    {
        return wxDateTime::Now();
    }
}