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

//(*Headers(PlayListItemImagePanel)
#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/filepicker.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
//*)

class PlayListItemImage;
class ImageFilePickerCtrl;

class PlayListItemImagePanel: public wxPanel
{
    PlayListItemImage* _image;

    void ValidateWindow();
    void SetWindowPositionText();

	public:

		PlayListItemImagePanel(wxWindow* parent, PlayListItemImage* Image,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~PlayListItemImagePanel();

		//(*Declarations(PlayListItemImagePanel)
		ImageFilePickerCtrl* FilePickerCtrl_ImageFile;
		wxButton* Button_PositionWindow;
		wxCheckBox* CheckBox_SuppressVirtualMatrix;
		wxCheckBox* CheckBox_Topmost;
		wxStaticText* StaticText1;
		wxStaticText* StaticText2;
		wxStaticText* StaticText3;
		wxStaticText* StaticText_Position;
		wxTextCtrl* TextCtrl_Delay;
		wxTextCtrl* TextCtrl_Duration;
		//*)

	protected:

		//(*Identifiers(PlayListItemImagePanel)
		static const long ID_STATICTEXT1;
		static const long ID_FILEPICKERCTRL1;
		static const long ID_BUTTON1;
		static const long ID_STATICTEXT4;
		static const long ID_CHECKBOX1;
		static const long ID_CHECKBOX2;
		static const long ID_STATICTEXT3;
		static const long ID_TEXTCTRL2;
		static const long ID_STATICTEXT2;
		static const long ID_TEXTCTRL1;
		//*)

	private:

		//(*Handlers(PlayListItemImagePanel)
		void OnButton_PositionWindowClick(wxCommandEvent& event);
		void OnFilePickerCtrl_ImageFileFileChanged(wxFileDirPickerEvent& event);
		void OnTextCtrl_DelayText(wxCommandEvent& event);
		void OnTextCtrl_DurationText(wxCommandEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};
