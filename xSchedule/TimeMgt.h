#pragma once

/***************************************************************
 * This source files comes from the xLights project
 * https://www.xlights.org
 * https://github.com/smeighan/xLights
 * See the github commit history for a record of contributing
 * developers.
 * Copyright claimed based on commit dates recorded in Github
 * License: https://github.com/smeighan/xLights/blob/master/License.txt
 **************************************************************/

// This file is a simple API for getting the current time.
// Why not just a bunch of now() calls all over?
//    That reads the current time clock, so in some cases it is what you want, but,
//    in other cases in xSchedule you want to know what time the event loop triggered,
//    and under unit test you want to be able to pretend that it is a certain time.
// So, for debug logging, use whatever, but for scheduling event processing, use this.

#include <stdint.h>
#include <wx/datetime.h>

namespace TimeMgt
{
    /// <summary>
    /// Gets the time according to the scheduler loop
    /// In production it is the real time when event processing started
    ///   to build frames, run logic, etc.
    /// In test, it might be a simulated time
    /// Local timezone
    /// Version without 'U' may not include milliseconds
    /// </summary>
    /// <returns>WX time</returns>
    wxDateTime getSchedNowWx();
    wxDateTime getSchedUNowWx();
    wxDateTime getSchedUNowWxUTC();
    int64_t getSchedNowMs();
    int64_t getSchedNowMsUTC();
    inline wxLongLong getSchedNowMsWxll()  // This is not an intended API and will get deleted
    {
        return wxLongLong(getSchedNowMs());
    }
    inline wxLongLong getSchedNowMsUTCWxll()   // This is not an intended API and will get deleted
    {
        return wxLongLong(getSchedNowMsUTC());
    }

    /// <summary>
    /// Get time from real-time clock
    /// Local timezone
    /// Version without 'U' may not include milliseconds
    /// </summary>
    /// <returns>WX RTC time</returns>
    wxDateTime getRTCNowWx();
    wxDateTime getRTCUNowWx();
    int64_t getRTCNowMs();
    int64_t getRTCNowMsUTC();

    /// <summary>
    /// Set the override time; this ought to include the millis and be in UTC
    /// </summary>
    /// <param name="t">time to set</param>
    void setTimeOverrideUTC(const wxDateTime& t);
    void setTimeOverrideUTC(int64_t millis);
    
    /// <summary>
    /// Clear off the time override
    /// </summary>
    void clearTimeOverride();

    /// <summary>
    /// Stash away the directivs to run a simulated time range (e.g. test purposes)
    ///   rather than the real system time
    /// </summary>
    /// <param name="start">Simulated run start time</param>
    /// <param name="end">Simulated run end time</param>
    /// <param name="accelerated">If true, run accelerated, rather than real time speed</param>
    void setRunTimeRange(const wxDateTime& start, const wxDateTime& end, bool accelerated = false);

    /// <summary>
    /// Check if current time or set time indicates a run should end
    /// </summary>
    /// <returns></returns>
    bool shouldEndRun();

    /// <summary>
    /// Is time mode to accelerated / simulated?
    /// </summary>
    bool isSimulatedTime();
    int64_t getSimulationEndUTC();

    /// <summary>
    /// Set the time between frame time ticks
    /// </summary>
    /// <param name="ms">Time until next tick, or negative to stop</param>
    void setNextFrameIn(int64_t ms, bool oneshot);
    void setNextSchedIn(int64_t ms, bool oneshot);
    int64_t getNextFrameIn(bool &oneshot);
    int64_t getNextFrameAt(bool &oneshot);
    int64_t getNextSchedIn(bool &oneshot);
    int64_t getNextSchedAt(bool &oneshot);
}