/***************************************************************
 * This source files comes from the xLights project
 * https://www.xlights.org
 * https://github.com/xLightsSequencer/xLights
 * See the github commit history for a record of contributing
 * developers.
 * Copyright claimed based on commit dates recorded in Github
 * License: https://github.com/xLightsSequencer/xLights/blob/master/License.txt
 **************************************************************/

#include "CandleEffect.h"
#include "CandlePanel.h"

#include <map>

#include "../sequencer/Effect.h"
#include "../RenderBuffer.h"
#include "../UtilClasses.h"
#include "../AudioManager.h"
#include "../models/Model.h"
#include "../UtilFunctions.h"
#include "../Parallel.h"

#include "../../include/candle-16.xpm"
#include "../../include/candle-24.xpm"
#include "../../include/candle-32.xpm"
#include "../../include/candle-48.xpm"
#include "../../include/candle-64.xpm"

CandleEffect::CandleEffect(int id) : RenderableEffect(id, "Candle", candle_16, candle_24, candle_32, candle_48, candle_64)
{
    //ctor
}

CandleEffect::~CandleEffect()
{
    //dtor
}

std::list<std::string> CandleEffect::CheckEffectSettings(const SettingsMap& settings, AudioManager* media, Model* model, Effect* eff, bool renderCache)
{
    std::list<std::string> res = RenderableEffect::CheckEffectSettings(settings, media, model, eff, renderCache);

    if (media == nullptr && settings.GetBool("E_CHECKBOX_Candle_GrowWithMusic", false)) {
        res.push_back(wxString::Format("    WARN: Candle effect cant grow to music if there is no music. Model '%s', Start %s", model->GetName(), FORMATTIME(eff->GetStartTimeMS())).ToStdString());
    }

    return res;
}

xlEffectPanel *CandleEffect::CreatePanel(wxWindow *parent) {
    return new CandlePanel(parent);
}

struct CandleState
{
    wxByte flameprimer;
    wxByte flamer;
    wxByte wind;
    wxByte flameprimeg;
    wxByte flameg;
};

class CandleRenderCache : public EffectRenderStatePRNG
{
public:
    std::map<int, CandleState*> _states;
    int maxWid;

    CandleRenderCache(){};
    virtual ~CandleRenderCache()
    {
        while (_states.size() > 0) {
            int index = _states.begin()->first;
            CandleState* todelete = _states[index];
            _states.erase(index);
            delete todelete;
        }
    };
};

static CandleRenderCache* GetCache(RenderBuffer& buffer, int id)
{
    CandleRenderCache* cache = (CandleRenderCache*)buffer.infoCache[id];
    if (cache == nullptr) {
        cache = new CandleRenderCache();
        buffer.infoCache[id] = cache;
        cache->seedConsistently(buffer.curPeriod, buffer.BufferWi, buffer.BufferHt, buffer.GetModelName().c_str(), id);
    }
    return cache;
}

void CandleEffect::SetDefaultParameters()
{
    CandlePanel* fp = (CandlePanel*)panel;
    if (fp == nullptr) {
        return;
    }

    fp->BitmapButton_Candle_FlameAgilityVC->SetActive(false);
    fp->BitmapButton_Candle_WindBaselineVC->SetActive(false);
    fp->BitmapButton_Candle_WindVariabilityVC->SetActive(false);
    fp->BitmapButton_Candle_WindCalmnessVC->SetActive(false);

    SetSliderValue(fp->Slider_Candle_FlameAgility, 2);
    SetSliderValue(fp->Slider_Candle_WindBaseline, 30);
    SetSliderValue(fp->Slider_Candle_WindCalmness, 2);
    SetSliderValue(fp->Slider_Candle_WindVariability, 5);

    SetCheckBoxValue(fp->CheckBox_PerNode, false);
}

void CandleEffect::Update(EffectRenderStatePRNG* prng, wxByte& flameprime, wxByte& flame, wxByte& wind, size_t windVariability, size_t flameAgility, size_t windCalmness, size_t windBaseline)
{
    // We simulate a gust of wind by setting the wind var to a random value
    if (wxByte(prng->prnguniform() * 255.0) < windVariability) {
        wind = wxByte(prng->prnguniform() * 255.0);
    }

    // The wind constantly settles towards its baseline value
    if (wind > windBaseline) {
        wind--;
    }

    // The flame constantly gets brighter till the wind knocks it down
    if (flame < 255) {
        flame++;
    }

    // Depending on the wind strength and the calmness modifier we calculate the odds
    // of the wind knocking down the flame by setting it to random values
    if (wxByte(prng->prnguniform() * 255) < (wind >> windCalmness)) {
        flame = wxByte(prng->prnguniform() * 255);
    }

    // Real flames ook like they have inertia so we use this constant-aproach-rate filter
    // To lowpass the flame height
    if (flame > flameprime) {
        if (flameprime < (255 - flameAgility)) {
            flameprime += flameAgility;
        }
    } else {
        if (flameprime > flameAgility) {
            flameprime -= flameAgility;
        }
    }

    // How do we prevent jittering when the two are equal?
    // We don't. It adds to the realism.
}

void InitialiseState(CandleRenderCache *cache, int node, std::map<int, CandleState*>& states)
{
    if (states.find(node) == states.end()) {
        CandleState* state = new CandleState();
        states[node] = state;
    }

    states[node]->flamer = cache->prngint(255); // Could be 256 but 255 closely follows the original code
    states[node]->flameprimer = cache->prngint(255);

    states[node]->flameg = cache->prnguniform() * states[node]->flamer;
    states[node]->flameprimeg = cache->prnguniform() * states[node]->flameprimer;

    states[node]->wind = cache->prnguniform() * 255;
}

// 10 <= HeightPct <= 100
void CandleEffect::Render(Effect* effect, const SettingsMap& SettingsMap, RenderBuffer& buffer)
{
    float oset = buffer.GetEffectTimeIntervalPosition();
    int flameAgility = GetValueCurveInt("Candle_FlameAgility", 2, SettingsMap, oset, CANDLE_AGILITY_MIN, CANDLE_AGILITY_MAX, buffer.GetStartTimeMS(), buffer.GetEndTimeMS());
    int windCalmness = GetValueCurveInt("Candle_WindCalmness", 2, SettingsMap, oset, CANDLE_WINDCALMNESS_MIN, CANDLE_WINDCALMNESS_MAX, buffer.GetStartTimeMS(), buffer.GetEndTimeMS());
    int windVariability = GetValueCurveInt("Candle_WindVariability", 5, SettingsMap, oset, CANDLE_WINDVARIABILITY_MIN, CANDLE_WINDVARIABILITY_MAX, buffer.GetStartTimeMS(), buffer.GetEndTimeMS());
    int windBaseline = GetValueCurveInt("Candle_WindBaseline", 30, SettingsMap, oset, CANDLE_WINDBASELINE_MIN, CANDLE_WINDBASELINE_MAX, buffer.GetStartTimeMS(), buffer.GetEndTimeMS());
    bool perNode = SettingsMap.GetBool("CHECKBOX_PerNode", false);

    CandleRenderCache* cache = GetCache(buffer, id);
    std::map<int, CandleState*>& states = cache->_states;

    if (buffer.needToInit) {
        buffer.needToInit = false;

        if (perNode) {
            wxPoint maxBuffer = buffer.GetMaxBuffer(SettingsMap);
            int maxMWi = maxBuffer.x == -1 ? buffer.BufferWi : maxBuffer.x;
            int maxMHt = maxBuffer.y == -1 ? buffer.BufferHt : maxBuffer.y;
            cache->maxWid = maxMWi;
            for (size_t x = 0; x < maxMWi; ++x) {
                for (size_t y = 0; y < maxMHt; ++y) {
                    size_t index = y * maxMWi + x;
                    InitialiseState(cache, index, states);
                }
            }
        } else {
            InitialiseState(cache, 0, states);
        }
    }

    if (perNode) {
        int maxW = cache->maxWid;
        parallel_for(0, buffer.BufferHt, [&buffer, &states, maxW, windVariability, flameAgility, windCalmness, windBaseline, this](int y) {
            EffectRenderStatePRNG prng;
            prng.seedConsistently(buffer.curPeriod, buffer.BufferWi, buffer.BufferHt, buffer.GetModelName().c_str(), y);
            prng.prngnext();
            for (size_t x = 0; x < buffer.BufferWi; x++) {
                size_t index = y * maxW + x;
                if (index >= states.size()) {
                    // this should never happen
                    wxASSERT(false);
                } else {
                    CandleState* state = states[index];

                    Update(&prng, state->flameprimer, state->flamer, state->wind, windVariability, flameAgility, windCalmness, windBaseline);
                    Update(&prng, state->flameprimeg, state->flameg, state->wind, windVariability, flameAgility, windCalmness, windBaseline);

                    if (state->flameprimeg > state->flameprimer)
                        state->flameprimeg = state->flameprimer;
                    if (state->flameg > state->flamer)
                        state->flameprimeg = state->flameprimer;

                    //  Now play Candle
                    xlColor c = xlColor(state->flameprimer, state->flameprimeg / 2, 0);
                    buffer.SetPixel(x, y, c);
                }
            }
        });
    } else {
        CandleState* state = states[0];

        Update(cache, state->flameprimer, state->flamer, state->wind, windVariability, flameAgility, windCalmness, windBaseline);
        Update(cache, state->flameprimeg, state->flameg, state->wind, windVariability, flameAgility, windCalmness, windBaseline);

        if (state->flameprimeg > state->flameprimer)
            state->flameprimeg = state->flameprimer;
        if (state->flameg > state->flamer)
            state->flameprimeg = state->flameprimer;

        //  Now play Candle
        xlColor c = xlColor(state->flameprimer, state->flameprimeg / 2, 0);
        for (size_t y = 0; y < buffer.BufferHt; y++) {
            for (size_t x = 0; x < buffer.BufferWi; x++) {
                buffer.SetPixel(x, y, c);
            }
        }
    }
}
