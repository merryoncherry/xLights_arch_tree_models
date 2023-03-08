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

//(*Headers(VsaImportDialog)
#include <wx/button.h>
#include <wx/choice.h>
#include <wx/dialog.h>
#include <wx/gbsizer.h>
#include <wx/grid.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
//*)

class SequenceElements;
class xLightsFrame;
class VSAFile;
class EzGrid;

class VsaImportDialog: public wxDialog
{
	public:

		VsaImportDialog(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~VsaImportDialog();

		void Init(VSAFile* file, bool allModels);

		//(*Declarations(VsaImportDialog)
		EzGrid* ChannelMapGrid;
		wxButton* Button_Cancel;
		wxButton* Button_Ok;
		wxChoice* ChoiceSetModels;
		wxFlexGridSizer* FlexGridSizer2;
		wxFlexGridSizer* FlexGridSizer3;
		wxFlexGridSizer* SizerMap;
		wxGridBagSizer* GridBagSizer1;
		wxStaticText* StaticText1;
		wxStaticText* StaticText40;
		//*)

        SequenceElements *mSequenceElements;
        xLightsFrame * xlights;
        std::vector<std::string> modelNames;

        std::vector<std::string> trackNames;
        std::vector<size_t> trackIndex;
        std::vector<std::string> selectedModels;
        std::vector<std::string> selectedChannels;
        std::vector<size_t> selectedLayers;

	protected:

		//(*Identifiers(VsaImportDialog)
		static const long ID_STATICTEXT39;
		static const long ID_GRID1;
		static const long ID_STATICTEXT1;
		static const long ID_CHOICE_MODELS;
		static const long ID_BUTTON3;
		static const long ID_BUTTON4;
		static const long ID_BUTTON1;
		static const long ID_BUTTON2;
		//*)

	private:
        bool allowAddModels;
        bool _dirty;
        VSAFile* _file;
        size_t _num_tracks;
        void UpdateChannels(int row, const wxString& match_channel = wxEmptyString);

		//(*Handlers(VsaImportDialog)
		void LoadMapping(wxCommandEvent& event);
		void SaveMapping(wxCommandEvent& event);
		void OnButton_OkClick(wxCommandEvent& event);
		void OnButton_CancelClick(wxCommandEvent& event);
		void OnResize(wxSizeEvent& event);
		void OnChoiceSetModelsSelect(wxCommandEvent& event);
		void OnChannelMapGridCellChange(wxGridEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};
