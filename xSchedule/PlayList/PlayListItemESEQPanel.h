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

 //(*Headers(PlayListItemESEQPanel)
 #include <wx/choice.h>
 #include <wx/filepicker.h>
 #include <wx/panel.h>
 #include <wx/sizer.h>
 #include <wx/spinctrl.h>
 #include <wx/stattext.h>
 #include <wx/textctrl.h>
 //*)

class PlayListItemESEQ;
class ESEQFilePickerCtrl;

class PlayListItemESEQPanel: public wxPanel
{
    PlayListItemESEQ* _ESEQ;
    void ValidateWindow();

	public:

		PlayListItemESEQPanel(wxWindow* parent, PlayListItemESEQ* ESEQ, wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~PlayListItemESEQPanel();

		//(*Declarations(PlayListItemESEQPanel)
		ESEQFilePickerCtrl* FilePickerCtrl_ESEQFile;
		wxChoice* Choice_BlendMode;
		wxSpinCtrl* SpinCtrl_Priority;
		wxStaticText* StaticText1;
		wxStaticText* StaticText2;
		wxStaticText* StaticText3;
		wxStaticText* StaticText4;
		wxTextCtrl* TextCtrl_Delay;
		//*)

	protected:

		//(*Identifiers(PlayListItemESEQPanel)
		static const long ID_STATICTEXT1;
		static const long ID_FILEPICKERCTRL1;
		static const long ID_STATICTEXT2;
		static const long ID_CHOICE1;
		static const long ID_STATICTEXT4;
		static const long ID_SPINCTRL1;
		static const long ID_STATICTEXT3;
		static const long ID_TEXTCTRL1;
		//*)

	private:

		//(*Handlers(PlayListItemESEQPanel)
		void OnTextCtrl_DelayText(wxCommandEvent& event);
		void OnFilePickerCtrl1FileChanged(wxFileDirPickerEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

