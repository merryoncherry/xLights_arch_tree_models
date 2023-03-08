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

 //(*Headers(RemapDialog)
 #include <wx/button.h>
 #include <wx/checkbox.h>
 #include <wx/dialog.h>
 #include <wx/sizer.h>
 #include <wx/spinctrl.h>
 #include <wx/stattext.h>
 #include <wx/textctrl.h>
 //*)

class OutputManager;

class RemapDialog: public wxDialog
{
    std::string& _from;
    std::string& _to;
    size_t& _channels;
    std::string& _description;
    bool& _enabled;
    OutputManager* _outputManager;

    void ValidateWindow();

    public:

		RemapDialog(wxWindow* parent, OutputManager* outputManager, std::string& startChannel, std::string& to, size_t& channels, std::string& description, bool& enabled, wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~RemapDialog();

		//(*Declarations(RemapDialog)
		wxButton* Button_Cancel;
		wxButton* Button_Ok;
		wxCheckBox* CheckBox_Enabled;
		wxSpinCtrl* SpinCtrl_Channels;
		wxStaticText* StaticText1;
		wxStaticText* StaticText2;
		wxStaticText* StaticText3;
		wxStaticText* StaticText4;
		wxStaticText* StaticText_From;
		wxStaticText* StaticText_To;
		wxTextCtrl* TextCtrl_Description;
		wxTextCtrl* TextCtrl_FromChannel;
		wxTextCtrl* TextCtrl_ToChannel;
		//*)

	protected:

		//(*Identifiers(RemapDialog)
		static const long ID_STATICTEXT1;
		static const long ID_TEXTCTRL2;
		static const long ID_STATICTEXT5;
		static const long ID_STATICTEXT2;
		static const long ID_TEXTCTRL3;
		static const long ID_STATICTEXT6;
		static const long ID_STATICTEXT3;
		static const long ID_SPINCTRL3;
		static const long ID_STATICTEXT4;
		static const long ID_TEXTCTRL1;
		static const long ID_CHECKBOX1;
		static const long ID_BUTTON1;
		static const long ID_BUTTON2;
		//*)

	private:

		//(*Handlers(RemapDialog)
		void OnButton_OkClick(wxCommandEvent& event);
		void OnButton_CancelClick(wxCommandEvent& event);
		void OnTextCtrl_FromChannelText(wxCommandEvent& event);
		void OnTextCtrl_ToChannelText(wxCommandEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};
