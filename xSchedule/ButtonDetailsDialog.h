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

 //(*Headers(ButtonDetailsDialog)
 #include <wx/button.h>
 #include <wx/choice.h>
 #include <wx/dialog.h>
 #include <wx/sizer.h>
 #include <wx/stattext.h>
 #include <wx/textctrl.h>
 //*)

class CommandManager;

class ButtonDetailsDialog: public wxDialog
{
    CommandManager* _commandManager;
    void ValidateWindow();

	public:

        std::string& _label;
        std::string& _parameter;
        std::string& _command;
        std::string& _color;
        char& _hotkey;

		ButtonDetailsDialog(wxWindow* parent, CommandManager* commandManager, std::string& label, std::string& command, std::string& color, std::string& parameter, char& hotkey, wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~ButtonDetailsDialog();

		//(*Declarations(ButtonDetailsDialog)
		wxButton* Button_Cancel;
		wxButton* Button_Ok;
		wxChoice* Choice1;
		wxChoice* Choice_Command;
		wxChoice* Choice_Hotkey;
		wxStaticText* StaticText1;
		wxStaticText* StaticText2;
		wxStaticText* StaticText3;
		wxStaticText* StaticText4;
		wxStaticText* StaticText5;
		wxTextCtrl* TextCtrl_Label;
		wxTextCtrl* TextCtrl_Parameters;
		//*)

	protected:

		//(*Identifiers(ButtonDetailsDialog)
		static const long ID_STATICTEXT1;
		static const long ID_TEXTCTRL1;
		static const long ID_STATICTEXT2;
		static const long ID_CHOICE1;
		static const long ID_STATICTEXT3;
		static const long ID_TEXTCTRL2;
		static const long ID_STATICTEXT4;
		static const long ID_CHOICE2;
		static const long ID_STATICTEXT5;
		static const long ID_CHOICE3;
		static const long ID_BUTTON1;
		static const long ID_BUTTON2;
		//*)

	private:

		//(*Handlers(ButtonDetailsDialog)
		void OnButton_OkClick(wxCommandEvent& event);
		void OnButton_CancelClick(wxCommandEvent& event);
		void OnTextCtrl_ParametersText(wxCommandEvent& event);
		void OnTextCtrl_LabelText(wxCommandEvent& event);
		void OnChoice_CommandSelect(wxCommandEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};
