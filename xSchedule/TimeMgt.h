#pragma once

// This file is a simple API for getting the current time.
// Why not just a bunch of now() calls all over?
//    That reads the current time clock, so in some cases it is what you want, but,
//    in other cases in xSchedule you want to know what time the event loop triggered,
//    and under unit test you want to be able to pretend that it is a certain time.
// So, for debug logging, use whatever, but for scheduling event processing, use this.

#include <wx/datetime.h>

namespace TimeMgt
{
    /// <summary>
    /// Gets the time according to the scheduler loop
    /// In production it is the real time when event processing started
    ///   to build frames, run logic, etc.
    /// In test, it might be a simulated time
    /// </summary>
    /// <returns>WX time</returns>
    wxDateTime getSchedNowWx();

    /// <summary>
    /// Get time from real-time clock
    /// </summary>
    /// <returns>WX RTC time</returns>
    wxDateTime getRTCNowWx();


}