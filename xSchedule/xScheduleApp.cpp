/***************************************************************
 * This source files comes from the xLights project
 * https://www.xlights.org
 * https://github.com/smeighan/xLights
 * See the github commit history for a record of contributing
 * developers.
 * Copyright claimed based on commit dates recorded in Github
 * License: https://github.com/smeighan/xLights/blob/master/License.txt
 **************************************************************/

#include "xScheduleApp.h"

//(*AppHeaders
#include "xScheduleMain.h"
#include <wx/image.h>
//*)

#include <log4cpp/Category.hh>
#include <log4cpp/Configurator.hh>
#include <log4cpp/PropertyConfigurator.hh>
#include <wx/file.h>
#include <wx/msgdlg.h>

#include <wx/cmdline.h>
#include <wx/confbase.h>
#include <wx/debugrpt.h>
#include <wx/filename.h>
#include <wx/snglinst.h>
#include <wx/stdpaths.h>

#include "ScheduleManager.h"
#include "TimeMgt.h"
#include "StructuredLog.h"
#include "../xLights/outputs/OutputManager.h"
#include "../xLights/xLightsVersion.h"

#ifdef _MSC_VER
#ifdef _DEBUG
#pragma comment(lib, "wxbase"WXWIDGETS_VERSION"ud.lib")
#pragma comment(lib, "wxbase"WXWIDGETS_VERSION"ud_net.lib")
#pragma comment(lib, "wxmsw"WXWIDGETS_VERSION"ud_core.lib")
#pragma comment(lib, "wxscintillad.lib")
#pragma comment(lib, "wxregexud.lib")
#pragma comment(lib, "wxbase"WXWIDGETS_VERSION"ud_xml.lib")
#pragma comment(lib, "wxtiffd.lib")
#pragma comment(lib, "wxjpegd.lib")
#pragma comment(lib, "wxpngd.lib")
#pragma comment(lib, "wxzlibd.lib")
#pragma comment(lib, "wxmsw"WXWIDGETS_VERSION"ud_qa.lib")
#pragma comment(lib, "wxexpatd.lib")
#pragma comment(lib, "log4cpplibd.lib")
//#pragma comment(lib, "log4cppd.lib")
#pragma comment(lib, "portmidid.lib")
#pragma comment(lib, "msvcprtd.lib")
#pragma comment(lib, "libzstdd_static_VS.lib")

#if !defined(SKIP_SMPTE)
#pragma comment(lib, "libltcd.lib")
#endif

#else
#pragma comment(lib, "wxbase"WXWIDGETS_VERSION"u.lib")
#pragma comment(lib, "wxbase"WXWIDGETS_VERSION"u_net.lib")
#pragma comment(lib, "wxmsw"WXWIDGETS_VERSION"u_core.lib")
#pragma comment(lib, "wxscintilla.lib")
#pragma comment(lib, "wxregexu.lib")
#pragma comment(lib, "wxbase"WXWIDGETS_VERSION"u_xml.lib")
#pragma comment(lib, "wxtiff.lib")
#pragma comment(lib, "wxjpeg.lib")
#pragma comment(lib, "wxpng.lib")
#pragma comment(lib, "wxzlib.lib")
#pragma comment(lib, "wxmsw"WXWIDGETS_VERSION"u_qa.lib")
#pragma comment(lib, "wxexpat.lib")
#pragma comment(lib, "libzstd_static_VS.lib")
#pragma comment(lib, "log4cpplib.lib")
//#pragma comment(lib, "log4cpp.lib")
#pragma comment(lib, "portmidi.lib")
#pragma comment(lib, "msvcprt.lib")
#if !defined(SKIP_SMPTE)
#pragma comment(lib, "libltc.lib")
#endif
#endif
#pragma comment(lib, "libcurl.dll.a")
#pragma comment(lib, "z.lib")
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "WS2_32.Lib")
#pragma comment(lib, "comdlg32.lib")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "Rpcrt4.lib")
#pragma comment(lib, "uuid.lib")
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "oleaut32.lib")
#pragma comment(lib, "odbc32.lib")
#pragma comment(lib, "odbccp32.lib")
#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "winspool.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "ImageHlp.Lib")
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "swresample.lib")
#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "swscale.lib")
#endif

IMPLEMENT_APP(xScheduleApp)

xScheduleApp::xScheduleApp() :
    xlBaseApp("xSchedule")
{
}

std::string DecodeOS(wxOperatingSystemId o)  {
    switch (o) {
    case wxOS_UNKNOWN:
        return "Call get get operating system failed.";
    case wxOS_MAC_OS:
        return "Apple Mac OS 8 / 9 / X with Mac paths.";
    case wxOS_MAC_OSX_DARWIN:
        return "Apple OS X with Unix paths.";
    case wxOS_MAC:
        return "An Apple Mac of some type.";
    case wxOS_WINDOWS_NT:
        return "Windows NT family(XP / Vista / 7 / 8 / 10).";
    case wxOS_WINDOWS:
        return "A Windows system of some type.";
    case wxOS_UNIX_LINUX:
        return "Linux.";
    case wxOS_UNIX_FREEBSD:
        return "FreeBSD.";
    case wxOS_UNIX_OPENBSD:
        return "OpenBSD.";
    case wxOS_UNIX_NETBSD:
        return "NetBSD.";
    case wxOS_UNIX_SOLARIS:
        return "Solaris.";
    case wxOS_UNIX_AIX:
        return "AIX.";
    case wxOS_UNIX_HPUX:
        return "HP / UX.";
    case wxOS_UNIX:
        return "Some flavour of Unix.";
    default:
        break;
    }

    return "Unknown Operating System.";
}

void DumpConfig()
{
    static log4cpp::Category &logger_base = log4cpp::Category::getInstance(std::string("log_base"));
    logger_base.info("Version: " + std::string(xlights_version_string.c_str()));
    logger_base.info("Bits: " + std::string(GetBitness().c_str()));
    logger_base.info("Build Date: " + std::string(xlights_build_date.c_str()));
    logger_base.info("Machine configuration:");
    wxMemorySize s = wxGetFreeMemory();
    if (s != -1)
    {
#if wxUSE_LONGLONG
        wxString msg = wxString::Format(_T("  Free Memory: %" wxLongLongFmtSpec "d."), s);
#else
        wxString msg = wxString::Format(_T("  Free Memory: %ld."), s);
#endif
        logger_base.info("%s", (const char *)msg.c_str());
    }
    logger_base.info("  Current directory: " + std::string(wxGetCwd().c_str()));
    logger_base.info("  Machine name: " + std::string(wxGetHostName().c_str()));
    logger_base.info("  OS: " + std::string(wxGetOsDescription().c_str()));
    int verMaj = -1;
    int verMin = -1;
    wxOperatingSystemId o = wxGetOsVersion(&verMaj, &verMin);
    logger_base.info("  OS: %s %d.%d", (const char *)DecodeOS(o).c_str(), verMaj, verMin);
    if (wxIsPlatform64Bit())
    {
        logger_base.info("      64 bit");
    }
    else
    {
        logger_base.info("      NOT 64 bit");
    }
    if (wxIsPlatformLittleEndian())
    {
        logger_base.info("      Little Endian");
    }
    else
    {
        logger_base.info("      Big Endian");
    }
#ifdef LINUX
    wxLinuxDistributionInfo l = wxGetLinuxDistributionInfo();
    logger_base.info("  " + std::string(l.Id.c_str()) \
        + " " + std::string(l.Release.c_str()) \
        + " " + std::string(l.CodeName.c_str()) \
        + " " + std::string(l.Description.c_str()));
#endif
}

void InitialiseLogging(bool fromMain)
{
    static bool loggingInitialised = false;

    if (!loggingInitialised)
    {

#ifdef __WXMSW__
        std::string initFileName = "xschedule.windows.properties";
#endif
#ifdef __LINUX__
        std::string initFileName = wxStandardPaths::Get().GetInstallPrefix() + "/bin/xschedule.linux.properties";
        if (!wxFile::Exists(initFileName)) {
            initFileName = wxStandardPaths::Get().GetInstallPrefix() + "/share/xLights/xschedule.linux.properties";
        }
#endif

        if (!wxFile::Exists(initFileName))
        {
#ifdef _MSC_VER
            // the app is not initialized so GUI is not available and no event loop.
            wxMessageBox(initFileName + " not found in " + wxGetCwd() + ". Logging disabled.");
#endif
        }
        else
        {
            try
            {
                log4cpp::PropertyConfigurator::configure(initFileName);
                static log4cpp::Category& logger_base = log4cpp::Category::getInstance(std::string("log_base"));

                wxDateTime now = TimeMgt::getRTCUNowWx();
                wxString ts = wxString::Format("%04d-%02d-%02d_%02d-%02d-%02d-%03d", now.GetYear(), now.GetMonth()+1, now.GetDay(), now.GetHour(), now.GetMinute(), now.GetSecond(), now.GetMillisecond());
                logger_base.info("Start Time: %s.", (const char*)ts.c_str());

                logger_base.info("Current Working Directory: " + wxGetCwd());
                logger_base.info("Log4cpp config read from %s.", (const char*)initFileName.c_str());

                auto categories = log4cpp::Category::getCurrentCategories();
                for (const auto& it : *categories)
                {
                    std::string apps = "";
                    auto appenders = it->getAllAppenders();
                    for (const auto& it2 : appenders)
                    {
                        if (apps != "") apps += ", ";
                        apps += it2->getName();
                    }

                    std::string levels = "";
                    if (it->isAlertEnabled()) levels += "ALERT ";
                    if (it->isCritEnabled()) levels += "CRIT ";
                    if (it->isDebugEnabled()) levels += "DEBUG ";
                    if (it->isEmergEnabled()) levels += "EMERG ";
                    if (it->isErrorEnabled()) levels += "ERROR ";
                    if (it->isFatalEnabled()) levels += "FATAL ";
                    if (it->isInfoEnabled()) levels += "INFO ";
                    if (it->isNoticeEnabled()) levels += "NOTICE ";
                    if (it->isWarnEnabled()) levels += "WARN ";

                    logger_base.info("    %s : %s", it->getName().c_str(), levels.c_str());
                    if (apps != "")
                    {
                        logger_base.info("         " + apps);
                    }
                }
            }
            catch (log4cpp::ConfigureFailure& e) {
                // ignore config failure ... but logging wont work
                printf("Log issue:  %s\n", e.what());
            }
            catch (const std::exception& ex) {
                printf("Log issue: %s\n", ex.what());
            }
        }
    }
}

void xScheduleApp::WipeSettings()
{
    static log4cpp::Category &logger_base = log4cpp::Category::getInstance(std::string("log_base"));
    logger_base.warn("------ Wiping settings ------");

    wxConfigBase* config = wxConfigBase::Get();
    config->DeleteAll();
}

int xScheduleApp::OnExit()
{
    if (_checker != nullptr)
    {
        delete _checker;
        _checker = nullptr;
    }

    static log4cpp::Category &logger_base = log4cpp::Category::getInstance(std::string("log_base"));
    xsStructuredLog::logShutdown();
    xsStructuredLog::closeJsonLog();
    logger_base.info("xSchedule exiting.");

    return 0;
}

bool xScheduleApp::OnInit()
{
    _checker = nullptr;

    // seed the random number generator
    srand(wxGetLocalTimeMillis().GetLo()); // TODO: PRNG strategy for xSchedule based on time it's supposed to be

    wxLog::SetLogLevel(wxLOG_FatalError);

    //curl_global_init(CURL_GLOBAL_SSL);

    InitialiseLogging(false);
    static log4cpp::Category& logger_base = log4cpp::Category::getInstance(std::string("log_base"));
    logger_base.info("******* OnInit: xSchedule started.");

#ifdef __WXMSW__
    logger_base.debug("xSchedule module handle 0x%llx", ::GetModuleHandle(nullptr));
    logger_base.debug("xSchedule wxTheApp 0x%llx", wxTheApp);
    MSWEnableDarkMode();
#endif
    DumpConfig();

    static const wxCmdLineEntryDesc cmdLineDesc[] =
    {
        { wxCMD_LINE_SWITCH, "h", "help", "displays help on the command line parameters", wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
        { wxCMD_LINE_OPTION, "s", "show", "specify show directory" },
        { wxCMD_LINE_OPTION, "p", "playlist", "specify the playlist to play" },
        { wxCMD_LINE_SWITCH, "w", "wipe", "wipe settings clean" },
        { wxCMD_LINE_OPTION, "t", "test", "test execute schedule over simulated time range (in local time)" },
        { wxCMD_LINE_OPTION, "j", "jsonlog", "log things that happen to json log file" },
        { wxCMD_LINE_OPTION, "e", "events", "read and replay events from json log file" },
        { wxCMD_LINE_OPTION, "f", "fseq", "write out xSchedule data to fseq file" },
        { wxCMD_LINE_NONE },
    };

    bool parmfound = false;
    bool wipeSettings = false;
    wxString showDir;
    wxString playlist;
    wxString testTimeRange, jsonLog, extEventsFile, fseqOutFile;
    wxCmdLineParser parser(cmdLineDesc, argc, argv);
    switch (parser.Parse()) {
    case -1:
        // help was given
        return false;
    case 0:
        if (parser.Found("w"))
        {
            parmfound = true;
            logger_base.info("-w: Wiping settings");
            WipeSettings();
            wipeSettings = true;
        }
        if (parser.Found("s", &showDir)) {
            parmfound = true;
            logger_base.info("-s: Show directory set to %s.", (const char*)showDir.c_str());
        }
        if (parser.Found("p", &playlist)) {
            parmfound = true;
            logger_base.info("-p: Playlist to play %s.", (const char*)playlist.c_str());
        }
        if (parser.Found("t", &testTimeRange)) {
            parmfound = true;
            logger_base.info("-t: Time range to test: %s.", (const char*)testTimeRange.c_str());
            auto st_end = wxSplit(testTimeRange, ';');
            if (st_end.size() != 2) {
                logger_base.info("Unrecognised date formats: %s.  Use start;end.", testTimeRange.c_str());
                wxMessageBox("Unrecognised date formats on command line.", _("Command Line Options"));
            } else {
                wxDateTime start, end;
                start.ParseFormat(st_end[0], "%Y-%m-%d %H:%M:%S");
                end.ParseFormat(st_end[1], "%Y-%m-%d %H:%M:%S");
                logger_base.info("Running time window %s - %s; not accelerated", start.Format("%Y-%m-%d %H:%M:%S").c_str(), end.Format("%Y-%m-%d %H:%M:%S").c_str());
                TimeMgt::setRunTimeRange(start, end, false);
            }
        }
        if (parser.Found("j", &jsonLog)) {
            parmfound = true;
            logger_base.info("-j: JSON Log: %s.", (const char*)jsonLog.c_str());
            if (!xsStructuredLog::setJsonLog(jsonLog.c_str())) {
                wxMessageBox("Unable to open JSON log; see log for details.", _("Command Line Log Options"));
            }
        }
        if (parser.Found("e", &extEventsFile)) {
            parmfound = true;
            logger_base.info("-e: Ext events file: %s.", (const char*)extEventsFile.c_str());
            // TODO
        }
        if (parser.Found("f", &fseqOutFile)) {
            parmfound = true;
            logger_base.info("-f: FSEQ file to write: %s.", (const char*)fseqOutFile.c_str());
            // TODO
        }
        if (!parmfound && parser.GetParamCount() > 0)
        {
            logger_base.info("Unrecognised command line parameter found.");
            wxMessageBox("Unrecognised command line parameter found.", _("Command Line Options")); //give positive feedback*/
        }
        break;
    default:
        wxMessageBox(_("Unrecognized command line parameters"), _("Command Line Error"));
        return false;
    }

    _checker = new wxSingleInstanceChecker();
    if (showDir == "")
    {
        if (_checker->IsAnotherRunning())
        {
            logger_base.info("Another instance of xSchedule is running.");
            delete _checker; // OnExit() won't be called if we return false
            _checker = nullptr;

            // Would be nice to switch focuse here to the existing instance ... but that doesnt work ... this only sees windows in this process
            //wxWindow* x = FindWindowByLabel(_("xLights Scheduler"));

            wxMessageBox("Another instance of xSchedule is already running. A second instance not allowed. Exiting.");

            return false;
        }
    }
    else
    {
        _checker->CreateDefault();
    }

    xsStructuredLog::logStartup(); // OnInit was called a bit before loading command line options...

    //(*AppInitialize
    bool wxsOK = true;
    wxInitAllImageHandlers();
    if (wxsOK)
    {
        xScheduleFrame* Frame = new xScheduleFrame(0, showDir, playlist);
        Frame->Show();
        SetTopWindow(Frame);
        if (wipeSettings) Frame->GetPluginManager().WipeSettings();
    }
    //*)
    return wxsOK;
}
