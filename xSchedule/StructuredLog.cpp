/***************************************************************
 * This source files comes from the xLights project
 * https://www.xlights.org
 * https://github.com/smeighan/xLights
 * See the github commit history for a record of contributing
 * developers.
 * Copyright claimed based on commit dates recorded in Github
 * License: https://github.com/smeighan/xLights/blob/master/License.txt
 **************************************************************/

#include <wx/wx.h>
#include <wx/config.h>

#include <mutex>
#include <log4cpp/Category.hh>

#include <wx/file.h>
#include "wxJSON/jsonwriter.h"
#include "TimeMgt.h"

namespace xsStructuredLog
{
    static std::mutex xsLogMtx;

    static bool isJsonOpen = false;
    static wxFile jsonLogFile;

    static void populateTimestamps(wxJSONValue& v)
    {
        v["actualTimeUTC"] = wxJSONValue(wxGetUTCTimeMillis().GetValue());
        v["timeLocal"] = wxJSONValue(TimeMgt::getRTCNowMs());
        v["timeSchedulerLocal"] = wxJSONValue(TimeMgt::getSchedNowMs());
    }

    bool setJsonLog(const char* fn)
    {
        std::lock_guard lk(xsLogMtx);

        if (isJsonOpen) {
            return false;
        }

        bool res = jsonLogFile.Open(fn, wxFile::write);
        if (!res) {
            static log4cpp::Category& logger_base = log4cpp::Category::getInstance(std::string("log_base"));
            int errn = jsonLogFile.GetLastError();
            logger_base.info("Unable to open JSON log file %s: %d", fn, errn);
            return false;
        }

        isJsonOpen = true;
        return true;
    }

    void closeJsonLog()
    {
        std::lock_guard lk(xsLogMtx);

        if (!isJsonOpen)
            return;

        jsonLogFile.Close();

        isJsonOpen = false;
    }

    static void makeLogEntry(wxJSONValue& val)
    {
        std::lock_guard lk(xsLogMtx);
        if (!isJsonOpen)
            return;

        populateTimestamps(val);
        wxString vStr;
        wxJSONWriter().Write(val, vStr);
        jsonLogFile.Write(vStr);
    }

    void logStartup()
    {
        wxJSONValue val;
        val["type"] = wxJSONValue(wxString("startup"));
        makeLogEntry(val);
    }

    void logShutdown()
    {
        wxJSONValue val;
        val["type"] = wxJSONValue(wxString("shutdown"));
        makeLogEntry(val);
    }

    void logScheduleActivate(const char *name)
    {
        wxJSONValue val;
        val["type"] = wxJSONValue(wxString("schedule_activate"));
        val["schedule"] = wxJSONValue(wxString(name));
        makeLogEntry(val);
    }

    void logScheduleDeactivate(const char* name)
    {
        wxJSONValue val;
        val["type"] = wxJSONValue(wxString("schedule_deactivate"));
        val["schedule"] = wxJSONValue(wxString(name));
        makeLogEntry(val);
    }

    void logPlayListStart(const char* name, const char *step)
    {
        wxJSONValue val;
        val["type"] = wxJSONValue(wxString("playlist_start"));
        val["playlist"] = wxJSONValue(wxString(name));
        val["step"] = wxJSONValue(wxString(step));
        makeLogEntry(val);
    }

    void logPlayListSuspend(const char* name, bool suspend)
    {
        wxJSONValue val;
        val["type"] = wxJSONValue(wxString(suspend ? "playlist_suspend" : "playlist_unsuspend"));
        val["playlist"] = wxJSONValue(wxString(name));
        makeLogEntry(val);
    }

    void logPlayListStop(const char* name)
    {
        wxJSONValue val;
        val["type"] = wxJSONValue(wxString("playlist_stop"));
        val["playlist"] = wxJSONValue(wxString(name));
        makeLogEntry(val);
    }

    void logPlayListStepStart(const char* name)
    {
        wxJSONValue val;
        val["type"] = wxJSONValue(wxString("playlist_step_start"));
        val["step"] = wxJSONValue(wxString(name));
        makeLogEntry(val);
    }

    void logPlayListStepRestart(const char* name)
    {
        wxJSONValue val;
        val["type"] = wxJSONValue(wxString("playlist_step_restart"));
        val["step"] = wxJSONValue(wxString(name));
        makeLogEntry(val);
    }

    void logPlayListStepStop(const char* name)
    {
        wxJSONValue val;
        val["type"] = wxJSONValue(wxString("playlist_step_stop"));
        val["step"] = wxJSONValue(wxString(name));
        makeLogEntry(val);
    }

    void logPlayListStepPause(const char* name, bool pause)
    {
        wxJSONValue val;
        val["type"] = wxJSONValue(wxString(pause ? "playlist_step_pause" : "playlist_step_unpause"));
        val["step"] = wxJSONValue(wxString(name));
        makeLogEntry(val);
    }

    void logPlayListStepSuspend(const char* name, bool suspend)
    {
        wxJSONValue val;
        val["type"] = wxJSONValue(wxString(suspend ? "playlist_step_suspend" : "playlist_step_unsuspend"));
        val["step"] = wxJSONValue(wxString(name));
        makeLogEntry(val);
    }
}