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

namespace xsStructuredLog
{
    bool setJsonLog(const char* fn);
    void closeJsonLog();

    void logStartup();
    void logScheduleActivate(const char *name);
    void logScheduleDeactivate(const char *name);
    void logPlayListStart(const char *name, const char *step);
    void logPlayListSuspend(const char *name, bool suspend);
    void logPlayListStop(const char *name);
    void logPlayListStepStart(const char *name);
    void logPlayListStepPause(const char* name, bool pause);
    void logPlayListStepSuspend(const char* name, bool suspend);
    void logPlayListStepRestart(const char* name);
    void logPlayListStepStop(const char* name);
    void logTimerTick();
    void logFrame();
    void logShutdown();
}
