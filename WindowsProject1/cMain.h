#pragma once
#include "wx/wx.h"
#include <wx/txtstrm.h>
#include <wx/statline.h>
#include <wx/statbox.h>
#include <stdlib.h>     /* srand, rand */
#include <time.h>
#include <vector>
#include <map>
#include <math.h>
#include <list>
#include <wx/checkbox.h>

class cMain : public wxFrame
{
public:
	cMain();
	~cMain();

public:
	int iFieldWidth = 9;
	int iFieldHeight = 9;
	wxString buttonString = "";
	wxButton* arrayButton[81];
	bool solvePath = true;

	short AbstandX = 5;
	short AbstandY = 5;

	int buttonID = 0;
	short arrayField[9][9] = {
		0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0
	};
	bool givenField[9][9] = {
		0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0
	};
	wxColor origColorB;

	void OnButtonClicked(wxCommandEvent& evt);
	void OnKeyPress(wxKeyEvent& evt2);
	void SolveField(wxCommandEvent& evt3);
	void CreateField(wxCommandEvent& evt4); 
	bool Constraint(short i, short j, short fieldValue, short* colX, short* colY);
	bool SolveTest();
	short* SolveField(short arrField[9][9], short start);
	void OnEmpty(wxCommandEvent& evte);
	void solveSelection(wxCommandEvent& evt17);
	void OnExit(wxCommandEvent& evtu);
	void OnAbout(wxCommandEvent& evti);
};

