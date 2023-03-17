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

    void logStartup()
    {
        std::lock_guard lk(xsLogMtx);
        if (!isJsonOpen)
            return;

        wxJSONValue val;
        val["type"] = wxJSONValue("startup");
        populateTimestamps(val);
        wxString vStr;
        wxJSONWriter().Write(val, vStr);
        jsonLogFile.Write(vStr);
    }

    void logShutdown()
    {
        std::lock_guard lk(xsLogMtx);
        if (!isJsonOpen)
            return;

        wxJSONValue val;
        val["type"] = "shutdown";
        populateTimestamps(val);
        wxString vStr;
        wxJSONWriter().Write(val, vStr);
        jsonLogFile.Write(vStr);
    }
}