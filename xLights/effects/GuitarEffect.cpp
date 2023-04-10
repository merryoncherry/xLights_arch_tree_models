/***************************************************************
 * This source files comes from the xLights project
 * https://www.xlights.org
 * https://github.com/smeighan/xLights
 * See the github commit history for a record of contributing
 * developers.
 * Copyright claimed based on commit dates recorded in Github
 * License: https://github.com/smeighan/xLights/blob/master/License.txt
 **************************************************************/

// I may still change the logic for choosing how to play each note. I am not convinced I have it right but i need feedback from someone using a real set of notes to give feedback on the choices.

#include <vector>

#include "../../include/guitar-16.xpm"
#include "../../include/guitar-64.xpm"

#include "GuitarEffect.h"
#include "GuitarPanel.h"
#include "../sequencer/Effect.h"
#include "../RenderBuffer.h"
#include "../UtilClasses.h"
#include "../xLightsXmlFile.h"
#include "../UtilFunctions.h"
#include "models/Model.h"

#include <string>
#include <list>

#include <log4cpp/Category.hh>

class GuitarTiming
{
public:
    uint32_t _startMS = 0;
    uint32_t _endMS = 0;
    std::list<std::pair<uint8_t, uint8_t>> _fingerPos;
    GuitarTiming(uint32_t start, uint32_t end)
    {
        _startMS = start;
        _endMS = end;
    }

    void AddFingerPos(uint8_t string, uint8_t pos)
    {
        _fingerPos.push_back({ string, pos });
    }

    int GetPositionCentre() const
    {
        int zeroCount = 0;
        int total = 0;
        for (auto v : _fingerPos) {
            total += v.second;
            if (v.second == 0)
                ++zeroCount;
        }

        if (_fingerPos.size() - zeroCount == 0)
            return 0;

        return total / (_fingerPos.size() - zeroCount);
    }

    int GetSpread() const
    {
        int min = -1;
        int max = -1;

        for (auto v : _fingerPos) {
            // we ignore zero as it requires no fingers
            if (v.second != 0) {
                if (min == -1 || v.second < min)
                    min = v.second;
                if (max == -1 || v.second > max)
                    max = v.second;
            }
        }
        return max - min;
    }
};

// These are the string notes when no finger is on the string
typedef struct GuitarNotes
{
    uint8_t string;
    uint8_t fret;
    uint8_t note;
} GuitarNotes;

std::vector<GuitarNotes> bass = {
    { 0, 0, 28 }, // E1
    { 1, 0, 33 }, // A1
    { 2, 0, 38 }, // D2
    { 3, 0, 43 }, // G2
};

std::vector<GuitarNotes>
    guitar = {
        { 0, 0, 40 }, // E2
        { 1, 0, 45 }, // A3
        { 2, 0, 50 }, // D3
        { 3, 0, 55 }, // G3
        { 4, 0, 59 }, // B4
        { 5, 0, 64 }, // E4
    };

std::vector<GuitarNotes>
    banjo = {
        { 0, 0, 50 }, // D3
        { 1, 0, 55 }, // G3
        { 2, 0, 59 }, // B4
        { 3, 0, 62 }, // D4
        { 0, 0, 64 }, // D4
    };

bool centresort(const GuitarTiming* first, const GuitarTiming* second)
{
    return first->GetPositionCentre() < second->GetPositionCentre();
}

class NoteTiming
{
    // zero is an open string. 1 is longest and deepest, maxFrets is shortest and highest
    int GetFretPos(uint8_t string, uint8_t note, const std::string& type, uint8_t maxFrets)
    {
        auto base = NoteTiming::GetBaseNotes(type);
        if (base == nullptr)
            return -1;

        // check we have enough strings
        if (string >= base->size())
            return -1;

        // too low for this string
        if (note < base->at(string).note)
            return -1;

        // too high for this string
        if (note > base->at(string).note + maxFrets)
            return -1;

        return note - base->at(string).note;
    }

public:
    uint32_t _startMS = 0;
    uint32_t _endMS = 0;
    std::list<uint8_t> _notes;
    std::list<GuitarTiming*> _possibleTimings;

    NoteTiming(const NoteTiming& nt)
    {
        _startMS = nt._startMS;
        _endMS = nt._endMS;
        _notes = nt._notes;
        for (auto it : nt._possibleTimings) {
            _possibleTimings.push_back(new GuitarTiming(*it));
        }
    }

    void SetGuitarTiming(GuitarTiming* gt)
    {
        ClearPossibleTimings();
        _possibleTimings.push_back(new GuitarTiming(*gt));
    }

    static const std::vector<GuitarNotes>* GetBaseNotes(const std::string& type)
    {
        if (type == "Guitar")
            return &guitar;
        else if (type == "Bass Guitar")
            return &bass;
        else
            return &banjo;
    }

    static size_t GetStrings(const std::string& type)
    {
        auto base = GetBaseNotes(type);
        if (base == nullptr)
            return 0;

        return base->size();
    }

    NoteTiming(uint32_t start, uint32_t end)
    {
        _startMS = start;
        _endMS = end;
    }

    NoteTiming(uint32_t start, uint32_t end, std::list<uint8_t> notes)
    {
        _startMS = start;
        _endMS = end;
        _notes = notes;
    }

    ~NoteTiming()
    {
        ClearPossibleTimings();
    }

    void ClearPossibleTimings()
    {
        while (_possibleTimings.size() > 0) {
            delete _possibleTimings.front();
            _possibleTimings.pop_front();
        }
    }

    int PickTimingClosestTo(int centre, int& lastString)
    {
        int ls = lastString;
        lastString = -1;

        if (_possibleTimings.size() == 0) {
            return -1;
        }

        if (_possibleTimings.size() == 1)
            return _possibleTimings.front()->GetPositionCentre();

        auto closest = _possibleTimings.begin();
        auto it = closest;
        auto closestOnDifferentString = closest;
        int diffOnDifferentString = 9999;
        bool different = false;
        int diff = std::abs((*closest)->GetPositionCentre() - centre);

        while (it != _possibleTimings.end()) {
            if (std::abs((*it)->GetPositionCentre() - centre) < diff) {
                closest = it;
                diff = std::abs((*it)->GetPositionCentre() - centre);
            }

            // if we were playing a single note and now we are playing a single note then try to use a different string
            if (std::abs((*it)->GetPositionCentre() - centre) < diffOnDifferentString && ls != -1 && (*it)->_fingerPos.size() == 1 && (*it)->_fingerPos.front().first != ls) {
                closestOnDifferentString = it;
                diffOnDifferentString = std::abs((*it)->GetPositionCentre() - centre);
                different = true;
            } else if (closestOnDifferentString == it && ls != -1 && (*it)->_fingerPos.size() == 1 && (*it)->_fingerPos.front().first != ls) {
                different = true;
            }

            ++it;
        }

        // Use this to check the closest on different string code
        ///if (different) {
        //    if ((*closestOnDifferentString)->_fingerPos.size() == 1 && ls == (*closestOnDifferentString)->_fingerPos.front().first) {
        //        int a = 0;
        //    }
        //} else {
        //    if ((*closest)->_fingerPos.size() == 1 && ls == (*closest)->_fingerPos.front().first) {
        //        int a = 0;
        //    }
        //}

        // Remove all timings but our selected one
        if (different) {
            while (_possibleTimings.begin() != closestOnDifferentString) {
                delete _possibleTimings.front();
                _possibleTimings.pop_front();
            }
        } else {
            while (_possibleTimings.begin() != closest) {
                delete _possibleTimings.front();
                _possibleTimings.pop_front();
            }
        }
        while (_possibleTimings.size() != 1) {
            auto todel = _possibleTimings.begin();
            ++todel;
            delete *todel;
            _possibleTimings.erase(todel);
        }

        if (_possibleTimings.front()->_fingerPos.size() == 1) {
            lastString = _possibleTimings.front()->_fingerPos.front().first;
        }

        return _possibleTimings.front()->GetPositionCentre();
    }

    void GeneratePossibleTimings(const std::string& type, uint8_t maxFrets)
    {
        static log4cpp::Category& logger_base = log4cpp::Category::getInstance(std::string("log_base"));

        // if there are no notes it may be because the timings were set using SnnPnn
        if (_notes.size() == 0)
            return;

        ClearPossibleTimings();

        _notes.sort();

        uint8_t strings = NoteTiming::GetStrings(type);

        std::list<uint8_t> firstNoteString;

        auto it = _notes.begin();
        while (it != _notes.end() && firstNoteString.size() == 0) {
            for (uint8_t i = 0; i < strings; ++i) {
                if (GetFretPos(i, *it, type, maxFrets) != -1) {
                    firstNoteString.push_back(i);
                }
            }
            ++it;
        }

        for (auto f : firstNoteString) {
            for (uint8_t j = f; j < strings; ++j) {
#ifdef FILTER_OUT_UPLAYABLE
                bool ok = true;
#endif
                auto fns = j;
                GuitarTiming* t = new GuitarTiming(_startMS, _endMS);
                for (auto n : _notes) {
#ifdef FILTER_OUT_UPLAYABLE
                    bool found = false;
#endif
                    for (uint8_t s = fns; s < strings; ++s) {
                        int fp = GetFretPos(s, n, type, maxFrets);
                        if (fp != -1) {
                            t->AddFingerPos(s, fp);
#ifdef FILTER_OUT_UPLAYABLE
                            found = true;
#endif
                            fns = s+1;
                            break;
                        }
                    }

#ifdef FILTER_OUT_UPLAYABLE
                    if (!found)
                        ok = false;
#endif
                }

                if (t->_fingerPos.size() == 0) {
                    delete t;
                } else {
#ifdef FILTER_OUT_UPLAYABLE
                    if (!ok)
                        delete t;
                    else
#endif
                        _possibleTimings.push_back(t);
                }
            }
        }

        bool allnotes = false;
        uint8_t max = 0;
        for (auto& it : _possibleTimings)
        {
            if (it->_fingerPos.size() == _notes.size())
                allnotes = true;
            if (it->_fingerPos.size() < max)
                max = it->_fingerPos.size();
        }

        if (allnotes)
        {
            // remove any possible timings not including all notes
            auto it = _possibleTimings.begin();
            while (it != _possibleTimings.end())
            {
                if ((*it)->_fingerPos.size() != _notes.size())
                {
                    it = _possibleTimings.erase(it);
                }
                else
                {
                    ++it;
                }
            }
        }
        else
        {
            // remove any possible timings not including the maximum notes
            auto it = _possibleTimings.begin();
            while (it != _possibleTimings.end()) {
                if ((*it)->_fingerPos.size() != max) {
                    it = _possibleTimings.erase(it);
                } else {
                    ++it;
                }
            }
        }

        if (max != _notes.size())
        {
            logger_base.debug("One or more notes not found on %s at %lu.", (const char*)type.c_str(), _startMS);
        }

        bool allZero = false;

        // prioritise centres that are lower (hence longer)
        _possibleTimings.sort(centresort);

        // remove the largest finger spreads until no more than 3 are left
        while (_possibleTimings.size() > 3 && !allZero) {
            allZero = true;

            auto max = _possibleTimings.begin();
            int maxSpread = (*max)->GetSpread();
            auto it = max;

            while (it != _possibleTimings.end()) {
                if ((*it)->GetSpread() != 0)
                    allZero = false;
                if ((*it)->GetSpread() >= maxSpread)
                    max = it;
                ++it;
            }

            // we only erase the maximum if they are not all zero
            if (!allZero)
                _possibleTimings.erase(max);
        }
    }
};

GuitarEffect::GuitarEffect(int id) :
    RenderableEffect(id, "Guitar", Guitar_16_xpm, Guitar_64_xpm, Guitar_64_xpm, Guitar_64_xpm, Guitar_64_xpm) 
{
    //ctor
	_panel = nullptr;
}

GuitarEffect::~GuitarEffect()
{
    //dtor
}

std::list<std::string> GuitarEffect::CheckEffectSettings(const SettingsMap& settings, AudioManager* media, Model* model, Effect* eff, bool renderCache)
{
    std::list<std::string> res;

    if (settings.Get("E_CHOICE_Guitar_MIDITrack_APPLYLAST", "") == "")
    {
        res.push_back(wxString::Format("    ERR: Guitar effect needs a timing track. Model '%s', Start %s", model->GetName(), FORMATTIME(eff->GetStartTimeMS())).ToStdString());
    }
    else
    {
        std::list<NoteTiming*> timings = LoadTimingTrack(settings.Get("E_CHOICE_Guitar_MIDITrack_APPLYLAST", ""), 50, "Guitar", 100, 6);
        if (timings.size() == 0)
        {
            res.push_back(wxString::Format("    ERR: Guitar effect timing track '%s' has no notes. Model '%s', Start %s", settings.Get("E_CHOICE_Guitar_MIDITrack_APPLYLAST", ""), model->GetName(), FORMATTIME(eff->GetStartTimeMS())).ToStdString());
        }

        while (timings.size() > 0)
        {
            delete timings.front();
            timings.pop_front();
        }
    }

    return res;
}

void GuitarEffect::SetPanelStatus(Model *cls)
{
    SetPanelTimingTracks();
}

void GuitarEffect::SetPanelTimingTracks()
{
    GuitarPanel *fp = (GuitarPanel*)panel;
    if (fp == nullptr)
    {
        return;
    }

    if (mSequenceElements == nullptr)
    {
        return;
    }

    // Load the names of the timing tracks
    std::string timingtracks = GetTimingTracks(1);
    wxCommandEvent event(EVT_SETTIMINGTRACKS);
    event.SetString(timingtracks);
    wxPostEvent(fp, event);
}

xlEffectPanel *GuitarEffect::CreatePanel(wxWindow *parent) {
    _panel = new GuitarPanel(parent);
	return _panel;
}

void GuitarEffect::SetDefaultParameters() {
    GuitarPanel *pp = (GuitarPanel*)panel;
    if (pp == nullptr) {
        return;
    }

    SetChoiceValue(pp->Choice_Guitar_Type, "Guitar");
    SetChoiceValue(pp->Choice_StringAppearance, "On");
    SetSliderValue(pp->Slider_MaxFrets, 19);
    SetPanelTimingTracks();
}

void GuitarEffect::RenameTimingTrack(std::string oldname, std::string newname, Effect* effect)
{
    wxString timing = effect->GetSettings().Get("E_CHOICE_Guitar_MIDITrack_APPLYLAST", "");

    if (timing.ToStdString() == oldname)
    {
        effect->GetSettings()["E_CHOICE_Guitar_MIDITrack_APPLYLAST"] = wxString(newname);
    }

    SetPanelTimingTracks();
}

void GuitarEffect::Render(Effect *effect, const SettingsMap &SettingsMap, RenderBuffer &buffer) {
    RenderGuitar(buffer,
                effect->GetParentEffectLayer()->GetParentElement()->GetSequenceElements(),
		        std::string(SettingsMap.Get("CHOICE_Guitar_Type", "Guitar")),
                std::string(SettingsMap.Get("CHOICE_Guitar_MIDITrack_APPLYLAST", "")),
				std::string(SettingsMap.Get("CHOICE_StringAppearance", "On")),
                SettingsMap.GetInt("SLIDER_MaxFrets", 19),
                 SettingsMap.GetBool("CHECKBOX_ShowStrings", false),
                 SettingsMap.GetBool("CHECKBOX_Fade", false),
                 SettingsMap.GetBool("CHECKBOX_Collapse", false)
        );
}

bool notesort(const NoteTiming* first, const NoteTiming* second)
{
    return first->_startMS < second->_startMS;
}

class GuitarCache : public EffectRenderCache
{
public:
	GuitarCache() { };
	virtual ~GuitarCache() { 
        ClearTimings();
    };

    void ClearTimings()
    {
        while (_notes.size() > 0) {
            delete _notes.front();
            _notes.pop_front();
        }
    }

    void SetTimings(std::list<NoteTiming*>& notes, const std::string& type, uint8_t maxFrets)
    {
        ClearTimings();
        for (auto& it : notes)
        {
            _notes.push_back(new NoteTiming(*it));
        }
        ConvertToGuitarTiming(type, maxFrets);
    }

    void ConvertToGuitarTiming(const std::string& type, uint8_t maxFrets)
    {
        // sort the notes
        _notes.sort(notesort);

        // go through each one generating possible finger positions ... up to a maximum of 3 prioritising smallest finger spreads
        for (auto& it : _notes) {
            it->GeneratePossibleTimings(type, maxFrets);
        }

        // choose the one that follows the prior with the least movement
        int centre = -1;
        int lastString = -1;
        uint32_t lastEnd = 0;
        for (auto& it : _notes) {

            // if there is a gap in the notes then using the same string again is ok
            if (it->_startMS > lastEnd)
            {
                lastString = -1;
            }

            // if there is no centre favour the longest string possible
            if (centre == -1)
                centre = 0;

            // pick the next hand location from the available options
            centre = it->PickTimingClosestTo(centre, lastString);

            lastEnd = it->_endMS;
        }
    }

    GuitarTiming* GetTimingAt(uint32_t ms)
    {
        for (const auto& it : _notes)
        {
            if (it->_startMS <= ms && it->_endMS > ms)
            {
                if (it->_possibleTimings.size() > 0)
                    return it->_possibleTimings.front();
                else
                    return nullptr;
            }
        }

        return nullptr;
    }

    std::list<NoteTiming*> _notes;
    std::string _MIDItrack;
};

//render Guitar fx during sequence:
void GuitarEffect::RenderGuitar(RenderBuffer& buffer, SequenceElements* elements, const std::string& type, const std::string& MIDITrack, const std::string& stringAppearance, int maxFrets, bool showStrings, bool fade, bool collapse)
{
    GuitarCache* cache = (GuitarCache*)buffer.infoCache[id];
    if (cache == nullptr) {
        cache = new GuitarCache();
        buffer.infoCache[id] = cache;
    }

    std::string& _MIDITrack = cache->_MIDItrack;

    uint8_t strings = NoteTiming::GetStrings(type);

    if (buffer.needToInit) {
        // just in case the timing tracks have changed
        SetPanelTimingTracks();

        buffer.needToInit = false;
        if (_MIDITrack != MIDITrack) {
            cache->ClearTimings();
            auto notes = LoadTimingTrack(MIDITrack, buffer.frameTimeInMs, type, maxFrets, strings);
            cache->SetTimings(notes, type, maxFrets);
            elements->AddRenderDependency(MIDITrack, buffer.cur_model);

            while (notes.size() > 0)
            {
                delete notes.front();
                notes.pop_front();
            }
        }

        _MIDITrack = MIDITrack;

        if (_MIDITrack != "") {
            elements->AddRenderDependency(_MIDITrack, buffer.cur_model);
        }
    }

    uint32_t time = buffer.curPeriod * buffer.frameTimeInMs;

    DrawGuitar(buffer, cache->GetTimingAt(time), stringAppearance, maxFrets, strings, showStrings, fade, collapse);
}

inline uint32_t FlipY(uint32_t y, uint32_t height)
{
    return height - y - 1;
}

void GuitarEffect::DrawGuitarOn(RenderBuffer& buffer, uint8_t string, uint8_t fretPos, uint32_t timePos, uint32_t of, uint8_t maxFrets, uint8_t strings, bool showStrings, bool fade, bool collapse)
{
    xlColor c;
    buffer.palette.GetColor(string, c);

    xlColor cc = c;

    float alpha = (float)(of - timePos) / (float)of;
    if (fade)
        c.alpha = 255.0 * alpha;

    float perString = (float)buffer.BufferHt / strings;

    uint32_t maxX = ((maxFrets - fretPos) * buffer.BufferWi) / maxFrets;
    if (showStrings) {
        for (uint32_t x = maxX; x < buffer.BufferWi; ++x) {
            buffer.SetPixel(x, FlipY(perString * string + perString / 2, buffer.BufferHt), cc);
        }
    }

    int centre = perString * string + perString / 2;
    int height = perString;
    if (collapse) {
        height *= alpha;
        // always draw the string so dont let the height get to zero
        if (height < 1)
            height = 1;
    }

    int start = centre - height / 2;

    for (uint32_t x = 0; x < maxX; ++x) {
        for (uint32_t y = start; y < start + height; ++y) {
            buffer.SetPixel(x, FlipY(y, buffer.BufferHt), c);
        }
    }
}

#define WAVE_RAMP 3.0

void GuitarEffect::DrawGuitarWave(RenderBuffer& buffer, uint8_t string, uint8_t fretPos, uint32_t timePos, uint32_t of, uint8_t maxFrets, uint8_t strings, bool showStrings, bool fade, bool collapse)
{
    xlColor c;
    buffer.palette.GetColor(string, c);

    xlColor cc = c;

    float alpha = (float)(of - timePos) / (float)of;
    if (fade)
        c.alpha = 255.0 * alpha;

    uint32_t cycles = (((maxFrets - fretPos) * buffer.BufferWi) / maxFrets) / 10;
    double perString = (float)buffer.BufferHt / strings;
    double maxX = ((maxFrets - fretPos) * buffer.BufferWi) / maxFrets;

    if (showStrings) {
        for (uint32_t x = maxX; x < buffer.BufferWi; ++x) {
            buffer.SetPixel(x, FlipY(perString * string + perString / 2, buffer.BufferHt), cc);
        }
    }

    for (uint32_t x = 0; x < maxX; ++x) {

        // this foces the wave to zero near the ends
        double maxY = perString;

        if (collapse)
            maxY *= alpha;

        if (x < WAVE_RAMP) {
            maxY *= ((double)x * 1.0 / WAVE_RAMP);
        } else if (x >= maxX - WAVE_RAMP) {
            maxY *= (WAVE_RAMP - (double)(maxX - x - 1)) * 1.0 / WAVE_RAMP;
        }

        uint32_t y = (maxY / 2.0) * sin((PI * 2.0 * cycles * (double)x) / maxX + timePos * 2) + perString / 2.0 + perString * string;
        buffer.SetPixel(x, FlipY(y, buffer.BufferHt), c);
    }
}

void GuitarEffect::DrawString(RenderBuffer& buffer, uint8_t string, uint8_t strings)
{
    xlColor c;
    buffer.palette.GetColor(string, c);

    double perString = (float)buffer.BufferHt / strings;
    for (uint32_t x = 0; x < buffer.BufferWi; ++x) {
        buffer.SetPixel(x, FlipY(perString * string + perString / 2, buffer.BufferHt), c);
    }
}

void GuitarEffect::DrawGuitar(RenderBuffer& buffer, GuitarTiming* pdata, const std::string& stringAppearance, uint8_t maxFrets, uint8_t strings, bool showStrings, bool fade, bool collapse)
{
    if (pdata != nullptr) {
        uint32_t pos = (buffer.curPeriod * buffer.frameTimeInMs - pdata->_startMS) / buffer.frameTimeInMs;
        uint32_t len = (pdata->_endMS - pdata->_startMS) / buffer.frameTimeInMs;

        if (stringAppearance == "On") {
            for (const auto& it : pdata->_fingerPos) {
                DrawGuitarOn(buffer, it.first, it.second, pos, len, maxFrets, strings, showStrings, fade, collapse);
            }
        } else if (stringAppearance == "Wave") {
            for (const auto& it : pdata->_fingerPos) {
                DrawGuitarWave(buffer, it.first, it.second, pos, len, maxFrets, strings, showStrings, fade, collapse);
            }
        }
    }

    if (showStrings) {
        for (uint8_t s = 0; s < strings; ++s) {
            bool found = false;
            if (pdata != nullptr) {
                for (const auto& it : pdata->_fingerPos) {
                    if (it.first == s) {
                        found = true;
                        break;
                    }
                }
            }
            if (!found)
                DrawString(buffer, s, strings);
        }
    }
}

std::vector<float> GuitarEffect::Parse(wxString& l)
{
	std::vector<float> res;
	wxString s = l;
	while (s.Len() != 0)
	{
		int end = s.First('\t');
		if (end > 0)
		{
			res.push_back(wxAtof(s.SubString(0, end - 1)));
			s = s.Right(s.Len() - end - 1);
		}
		else
		{
			res.push_back(wxAtof(s));
			s = "";
		}
	}

	return res;
}

std::list<std::string> GuitarEffect::ExtractNotes(const std::string& label)
{
    std::string n = label;
    std::transform(n.begin(), n.end(), n.begin(), ::toupper);

    std::list<std::string> res;

    std::string s = "";
    for (const auto& it : n)
    {
        if (it == ':' || it == ' ' || it == ';' || it == ',')
        {
            if (s != "")
            {
                res.push_back(s);
                s = "";
            }
        }
        else
        {
            if ((it >= 'A' && it <= 'G') ||
                (it == '#') ||
                (it == 'S') ||
                (it == 'P') ||
                (it >= '0' && it <= '9'))
            {
                s += it;
            }
        }
    }

    if (s != "")
    {
        res.push_back(s);
    }

    return res;
}

void GuitarEffect::ConvertStringPos(const std::string& note, uint8_t& string, uint8_t& pos)
{
    string = 0xFF;
    pos = 0xFF;

    std::string n = note;
    std::transform(n.begin(), n.end(), n.begin(), ::toupper);

    if (n[0] != 'S')
        return;

    string = 0;
    uint32_t index = 1;

    while (n[index] >= '0' && n[index] <= '9')
    {
        string *= 10;
        string += (uint8_t)(n[index] - '0');
        ++index;
    }

    if (n[index++] != 'P') {
        string = 0xFF;
        return;
    }

    pos = 0;
    while (n[index] >= '0' && n[index] <= '9') {
        pos *= 10;
        pos += (uint8_t)(n[index] - '0');
        ++index;
    }
}

int GuitarEffect::ConvertNote(const std::string& note)
{
    std::string n = note;
    int nletter;
    std::transform(n.begin(), n.end(), n.begin(), ::toupper);

    switch (n[0])
    {
    case 'S':
    case 'P':
        return -1;
    case 'A':
        nletter = 9;
        break;
    case 'B':
        nletter = 11;
        break;
    case 'C':
        nletter = 0;
        break;
    case 'D':
        nletter = 2;
        break;
    case 'E':
        nletter = 4;
        break;
    case 'F':
        nletter = 5;
        break;
    case 'G':
        nletter = 7;
        break;
    default:
        {
            int number = wxAtoi(n);
            if (number < 0) number = 0;
            if (number > 127) number = 127;
            return number;
        }
    }

    n = n.substr(1);
    int sharp = 0;
    if (n.find('#') != std::string::npos)
    {
        sharp = 1;
    }
    else if (n.find('B') != std::string::npos)
    {
        sharp = -1;
    }

    int octave = 4;

    if (n != "")
    {
        if (n[0] == '#')
        {
            n = n.substr(1);
        }
        else if (n[0] == 'B')
        {
            n = n.substr(1);
        }
    }

    if (n != "")
    {
        octave = wxAtoi(n);
    }

    int number = 12 + (octave * 12) + nletter + sharp;
    if (number < 0) number = 0;
    if (number > 127) number = 127;
    return number;
}

std::list<NoteTiming*> GuitarEffect::LoadTimingTrack(const std::string& track, int intervalMS, const std::string& type, uint8_t maxFrets, uint8_t strings)
{
    static log4cpp::Category& logger_Guitardata = log4cpp::Category::getInstance(std::string("log_Guitardata"));

    std::list<NoteTiming*> res;

    logger_Guitardata.debug("Loading timings from timing track " + track);

    if (mSequenceElements == nullptr) {
        logger_Guitardata.debug("No timing tracks found.");
        return res;
    }

    // Load the names of the timing tracks
    EffectLayer* el = GetTiming(track);

    if (el == nullptr) {
        logger_Guitardata.debug("Timing track not found.");
        return res;
    }

    for (int j = 0; j < el->GetEffectCount(); ++j) {
        std::list<uint8_t> notes;
        int starttime = el->GetEffect(j)->GetStartTimeMS();
        int endtime = el->GetEffect(j)->GetEndTimeMS();
        GuitarTiming t(starttime, endtime);
        std::string label = el->GetEffect(j)->GetEffectName();
        std::list<std::string> notelabels = ExtractNotes(label);
        for (const auto& s : notelabels) {
            uint8_t n = ConvertNote(s);
            if (n != 0xFF) {
                notes.push_back(n);
            } else {
                uint8_t string;
                uint8_t pos;
                ConvertStringPos(s, string, pos);
                if (string != 0xFF && string != 0 && pos != 0xFF && pos <= maxFrets) {
                    t.AddFingerPos(strings - (string - 1) - 1, pos);
                }
            }
        }
        if (notes.size() != 0)
            res.push_back(new NoteTiming(starttime, endtime, notes));
        else if (t._fingerPos.size() > 0) {
            NoteTiming* nt = new NoteTiming(starttime, endtime);
            nt->SetGuitarTiming(&t);
            res.push_back(nt);
        }
    }

    return res;
}
