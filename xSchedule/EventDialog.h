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

 //(*Headers(EventDialog)
 #include <wx/button.h>
 #include <wx/choice.h>
 #include <wx/choicebk.h>
 #include <wx/dialog.h>
 #include <wx/notebook.h>
 #include <wx/panel.h>
 #include <wx/sizer.h>
 #include <wx/splitter.h>
 #include <wx/stattext.h>
 #include <wx/textctrl.h>
 //*)

class OutputManager;
class EventBase;

class EventDialog: public wxDialog
{
    EventBase* _eventBase;

	public:

		EventDialog(wxWindow* parent, OutputManager* outputManager, EventBase* eventBase, wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~EventDialog();
        EventBase* GetEvent() const { return _eventBase; }
        void ValidateWindow();

		//(*Declarations(EventDialog)
		wxButton* Button_Cancel;
		wxButton* Button_Ok;
		wxChoice* Choice_Command;
		wxChoicebook* Choicebook_EventType;
		wxPanel* Panel2;
		wxPanel* Panel3;
		wxSplitterWindow* SplitterWindow1;
		wxStaticText* StaticText1;
		wxStaticText* StaticText2;
		wxStaticText* StaticText3;
		wxStaticText* StaticText4;
		wxStaticText* StaticText5;
		wxTextCtrl* TextCtrl_Name;
		wxTextCtrl* TextCtrl_P1;
		wxTextCtrl* TextCtrl_P2;
		wxTextCtrl* TextCtrl_P3;
		//*)

	protected:

		//(*Identifiers(EventDialog)
		static const long ID_CHOICEBOOK1;
		static const long ID_BUTTON1;
		static const long ID_BUTTON2;
		static const long ID_PANEL3;
		static const long ID_STATICTEXT5;
		static const long ID_TEXTCTRL4;
		static const long ID_STATICTEXT1;
		static const long ID_CHOICE1;
		static const long ID_STATICTEXT4;
		static const long ID_TEXTCTRL1;
		static const long ID_STATICTEXT3;
		static const long ID_TEXTCTRL2;
		static const long ID_STATICTEXT2;
		static const long ID_TEXTCTRL3;
		static const long ID_PANEL2;
		static const long ID_SPLITTERWINDOW1;
		//*)

	private:

		//(*Handlers(EventDialog)
		void OnButton_OkClick(wxCommandEvent& event);
		void OnButton_CancelClick(wxCommandEvent& event);
		void OnChoicebook_EventTypePageChanged(wxChoicebookEvent& event);
		void OnChoice_CommandSelect(wxCommandEvent& event);
		void OnTextCtrl_P1Text(wxCommandEvent& event);
		void OnTextCtrl_P2Text(wxCommandEvent& event);
		void OnTextCtrl_P3Text(wxCommandEvent& event);
		void OnTextCtrl_NameText(wxCommandEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};
