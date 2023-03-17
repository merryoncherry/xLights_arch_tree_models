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
#include "TimeMgt.h"
#include "wxJSON/jsonval.h"
#include "wxJSON/jsonwriter.h"

namespace xsStructuredLog
{
    static std::mutex xsLogMtx;

    bool setJsonLog(const char* fn)
    {
        std::lock_guard lk(xsLogMtx);
        return true;
    }

    void closeJsonLog()
    {
        std::lock_guard lk(xsLogMtx);
    }
}