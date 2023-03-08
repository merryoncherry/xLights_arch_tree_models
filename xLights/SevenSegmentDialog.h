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

//(*Headers(SevenSegmentDialog)
#include <wx/checkbox.h>
#include <wx/dialog.h>
#include <wx/sizer.h>
#include <wx/statbmp.h>
//*)

class SevenSegmentDialog: public wxDialog
{
	public:

		SevenSegmentDialog(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~SevenSegmentDialog();

		//(*Declarations(SevenSegmentDialog)
		wxCheckBox* CheckBox_Colon;
		wxCheckBox* CheckBox_Decimal;
		wxCheckBox* CheckBox_Hundreds;
		wxCheckBox* CheckBox_Ones;
		wxCheckBox* CheckBox_Tens;
		wxCheckBox* CheckBox_Thousands;
		wxStaticBitmap* StaticBitmap_7Segment;
		//*)

	protected:

		//(*Identifiers(SevenSegmentDialog)
		static const long ID_CHECKBOX1;
		static const long ID_CHECKBOX2;
		static const long ID_CHECKBOX4;
		static const long ID_CHECKBOX3;
		static const long ID_CHECKBOX5;
		static const long ID_CHECKBOX6;
		static const long ID_STATICBITMAP1;
		//*)

	private:

		//(*Handlers(SevenSegmentDialog)
		//*)

		DECLARE_EVENT_TABLE()
};
