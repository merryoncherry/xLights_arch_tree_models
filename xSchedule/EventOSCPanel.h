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

#include "EventPanel.h"

//(*Headers(EventOSCPanel)
#include <wx/choice.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
//*)

class EventOSCPanel: public EventPanel
{
	public:

		EventOSCPanel(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~EventOSCPanel();
        virtual bool ValidateWindow() override;
        virtual void Save(EventBase* event) override;
        virtual void Load(EventBase* event) override;

		//(*Declarations(EventOSCPanel)
		wxChoice* Choice_Parm1Type;
		wxChoice* Choice_Parm2Type;
		wxChoice* Choice_Parm3Type;
		wxStaticText* StaticText1;
		wxStaticText* StaticText2;
		wxStaticText* StaticText3;
		wxStaticText* StaticText4;
		wxTextCtrl* TextCtrl_Path;
		//*)

	protected:

		//(*Identifiers(EventOSCPanel)
		static const long ID_STATICTEXT1;
		static const long ID_TEXTCTRL1;
		static const long ID_STATICTEXT2;
		static const long ID_CHOICE1;
		static const long ID_STATICTEXT3;
		static const long ID_CHOICE2;
		static const long ID_STATICTEXT4;
		static const long ID_CHOICE3;
		//*)

	private:

		//(*Handlers(EventOSCPanel)
		void OnTextCtrl_PathText(wxCommandEvent& event);
		void OnChoice_Parm1TypeSelect(wxCommandEvent& event);
		void OnChoice_Parm2TypeSelect(wxCommandEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};
