#include "cMain.h"

cMain::cMain() : wxFrame(nullptr, 9999, "Sudoku", wxPoint(30,30), wxSize(670,740))
{
	//Sudoku Button field generation
	wxFont font(28, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_NORMAL, false);

	for (int i = 0; i < iFieldWidth; i++)
	{
		if (i % 3 == 0)
		{
			AbstandX = AbstandX + 3;
		}

		for (int j = 0; j < iFieldHeight; j++)
		{
			if (j % 3 == 0)
			{
				AbstandY = AbstandY + 3;
			}

			arrayButton[i * iFieldWidth + j] = new wxButton(this, 10000 + (i * iFieldWidth + j), "", wxPoint(i * 70 + AbstandX, j * 70 + AbstandY), wxSize(70, 70));
			arrayButton[i * iFieldWidth + j]->SetFont(font);

			if (arrayField[j][i] != 0)
			{
				buttonString = wxString::Format(wxT("%d"), arrayField[j][i]);
				arrayButton[i * iFieldWidth + j]->SetLabel(buttonString);
				arrayButton[i * iFieldWidth + j]->Enable(false);
			}
		}

		AbstandY = 5;
	}
	Bind(wxEVT_BUTTON, &cMain::OnButtonClicked, this);

	//Menu
	wxMenu* menuSolve = new wxMenu;
	menuSolve->AppendRadioItem(5007, "&w/ Path", "You will see how it solves the Sudoku");
	menuSolve->AppendRadioItem(5008, "&w/o Path", "It will just solve it in the background");
	menuSolve->Append(5009, "&Solve", "Starts the solve algorithm");
	wxMenu* menuCreate = new wxMenu;
	menuCreate->Append(5010, "&Empty", "Creates an empty field and considers the placed digits");
	menuCreate->Append(5011, "&Normal", "Creates a partially filled field");
	wxMenu* menuFile = new wxMenu;
	menuFile->Append(wxID_ABOUT, "&About", "About this project");
	menuFile->AppendSeparator();
	menuFile->Append(wxID_EXIT);
	wxMenuBar* menuBar = new wxMenuBar;
	menuBar->Append(menuCreate, "&Create");
	menuBar->Append(menuSolve, "&Solve");
	menuBar->Append(menuFile, "&File");
	SetMenuBar(menuBar);
	CreateStatusBar();
	SetStatusText("Welcome to my Sudoku-Solver");
	Bind(wxEVT_MENU, &cMain::solveSelection, this, 5007);
	Bind(wxEVT_MENU, &cMain::solveSelection, this, 5008);
	Bind(wxEVT_MENU, &cMain::SolveField, this, 5009);
	Bind(wxEVT_MENU, &cMain::OnEmpty, this, 5010);
	Bind(wxEVT_MENU, &cMain::CreateField, this, 5011);
	Bind(wxEVT_MENU, &cMain::OnAbout, this, wxID_ABOUT);
	Bind(wxEVT_MENU, &cMain::OnExit, this, wxID_EXIT);

	origColorB = arrayButton[0]->GetBackgroundColour();
	for (int i = 0; i < 81; i++)
		arrayButton[i]->SetBackgroundColour(origColorB);

	SetDoubleBuffered(true);
}

cMain::~cMain()
{
	for (int i = 0; i < 81; i++)
	{
		delete arrayButton[i];
	}
}

void cMain::OnButtonClicked(wxCommandEvent& evt) //wxCommandEvent
{
	buttonID = evt.GetId();

	this->FindWindowById(9999)->SetFocus(); //sets the focus on the window; i can press keys!

	short tX = buttonID - 10000; //column 
	while (tX % iFieldWidth != 0)
		tX -= 1;
	tX /= iFieldWidth;
	short tY = buttonID - 10000; //row 
	while (tY - iFieldHeight >= 0)
		tY -= iFieldHeight;

	if (!givenField[tX][tY])
		Bind(wxEVT_KEY_DOWN, &cMain::OnKeyPress, this);

	for (int i = 0; i < 81; i++)
		if (arrayButton[i]->GetBackgroundColour() != origColorB)
			arrayButton[i]->SetBackgroundColour(origColorB); //wxNullColour

	wxColor color = wxColor(230, 230, 230, 255); //228, 238, 242, 255
	for (int i = 0; i < 9; i++)
		arrayButton[tX * iFieldWidth + i]->SetBackgroundColour(color);
	for (int j = 0; j < 9; j++)
		arrayButton[j * iFieldHeight + tY]->SetBackgroundColour(color);

	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
		{
			short tXB = floor(tX / 3) * 3 + i;
			short tYB = floor(tY / 3) * 3 + j;
			arrayButton[tXB * iFieldWidth + tYB]->SetBackgroundColour(color);
		}

	evt.Skip();
}

void cMain::OnKeyPress(wxKeyEvent& evt2)
{
	int keyCode = evt2.GetKeyCode();
	wxString keyCodeString = "";

	if (keyCode < 48 || keyCode > 57) return;
	else if(keyCode != 48)
	{ 
		keyCodeString = wxString::Format(wxT("%d"), keyCode-48);
	}

	Unbind(wxEVT_KEY_DOWN, &cMain::OnKeyPress, this);

	arrayButton[buttonID - 10000]->SetLabel(keyCodeString);

	for (int i = 0; i < 81; i++)
		if (arrayButton[i]->GetBackgroundColour() != origColorB)
			arrayButton[i]->SetBackgroundColour(origColorB);

	evt2.Skip();
}

void cMain::SolveField(wxCommandEvent& evt3)
{
	SetDoubleBuffered(false);

	short fieldValue = 0;
	bool collision = false;
	short blockX = 0;
	short blockY = 0;
	std::vector<short> fieldPathX;
	std::vector<short> fieldPathY;

	for (short i = 0; i < 9; i++) //columns (x)
	{
		for (short j = 0; j < 9; j++) //rows (y)
		{
			if (arrayField[i][j] == 0)
			{
				do
				{
					collision = false;
					fieldValue++;

					if (fieldValue == 10) //hier wird ein feld zurückgegangen
					{
						arrayField[i][j] = 0;
						//clear digit if With was selected
						if (solvePath == true)
						{
							arrayButton[j * iFieldWidth + i]->SetLabel("");
						}

						i = fieldPathX.back();
						j = fieldPathY.back();
						fieldValue = arrayField[i][j];
						fieldPathX.pop_back();
						fieldPathY.pop_back();
					}

					//x constraint detection:
					for (short h = 0; h < 9; h++)
					{
						if (fieldValue == arrayField[h][j]) // && fieldValue != arrayField[j][i]
						{
							collision = true;
							break;
						}
					}

					//y constraint detection:
					if (collision == false)
					{
						for (short h = 0; h < 9; h++)
						{
							if (fieldValue == arrayField[i][h]) // && fieldValue != arrayField[j][i]
							{
								collision = true;
								break;
							}
						}
					}

					//block constraint detection:
					if (collision == false)
					{
						for (short h = 0; h < 3; h++)
						{
							for (short k = 0; k < 3; k++)
							{
								blockX = floor(i / 3) * 3 + h;
								blockY = floor(j / 3) * 3 + k;
								if (fieldValue == arrayField[blockX][blockY])
								{
									collision = true;
									break;
								}
							}
						}
					}

					//place digit and track the path
					if (collision == false)
					{
						arrayField[i][j] = fieldValue;
						fieldPathX.push_back(i);
						fieldPathY.push_back(j);
						fieldValue = 0;

						//show digit if With was selected
						if (solvePath == true)
						{
							buttonString = wxString::Format(wxT("%d"), arrayField[i][j]);
							arrayButton[j * iFieldWidth + i]->SetLabel(buttonString);
						}
					}

				} while (collision == true);
			}
		}
	}

	if (solvePath == false)
	{
		//output of the field
		for (short i = 0; i < 9; i++)
		{
			for (short j = 0; j < 9; j++)
			{
				buttonString = wxString::Format(wxT("%d"), arrayField[i][j]);
				arrayButton[j * iFieldWidth + i]->SetLabel(buttonString);
			}
		}
	}

	SetDoubleBuffered(true);
	evt3.Skip();
}

void cMain::CreateField(wxCommandEvent& evt4)
{
	bool timesUp = false;
	time_t timeStart = time(NULL);

	do
	{
		//init the vars
		short fieldValue = 0;
		bool collision = false;
		short collisionPosX = 0;
		short collisionPosY = 0;
		for (short i = 0; i < 9; i++)
			for (short j = 0; j < 9; j++)
				arrayField[i][j] = 0;

		timesUp = false;
		timeStart = time(NULL);
		srand(time(NULL));

		//creates a full field
		for (short i = 0; i < 9; i++) //columns
		{
			for (short j = 0; j < 9; j++) //rows
			{
				fieldValue = rand() % 9 + 1;

				do
				{
					collision = Constraint(i, j, fieldValue, &collisionPosX, &collisionPosY);

					//place digit if no constraint active
					if (!collision)
					{
						arrayField[i][j] = fieldValue;
					}
					else //no digit can be placed
					{
						for (short d = 1; d <= 9; d++)
						{
							collision = Constraint(i, j, d, nullptr, nullptr);
							if (!collision)
							{
								arrayField[i][j] = d;
								break;
							}
						}
						if (collision) //all bool in arrayState are true
						{
							i = collisionPosX;
							j = collisionPosY;
							collision = false;
						}
					}
					if (timeStart + 0.5 < time(NULL))
					{
						timesUp = true;
						i = 9;
						j = 9;
						break;
					}
				} while (collision);
			}
		}
	} while (timesUp);

	//deletes cells, without which the game is still beatable
	short deleteCell = 0;
	bool multipleSolutions = false;
	short randX = 0;
	short randY = 0;
	for (short counter = 0; counter < 50; counter++) //50
	{
		do
		{
			randX = rand() % 9; //0-8
			randY = rand() % 9;
		} while (arrayField[randY][randX] == 0);

		deleteCell = arrayField[randY][randX];
		arrayField[randY][randX] = 0;
		multipleSolutions = SolveTest();

		if (multipleSolutions == false) //the sudoku cannot be solved
		{
			counter--;
			arrayField[randY][randX] = deleteCell;
		}
	}

	//output of the new field
	for (short i = 0; i < 9; i++)
	{
		for (short j = 0; j < 9; j++)
		{
			if (arrayField[j][i] != 0)
			{
				buttonString = wxString::Format(wxT("%d"), arrayField[j][i]);
				arrayButton[i * iFieldWidth + j]->SetLabel(buttonString);
				givenField[i][j] = 1;
			}
			else
			{
				arrayButton[i * iFieldWidth + j]->SetLabel("");
			}
		}
	}

	evt4.Skip();
}

bool cMain::Constraint(short i, short j, short fieldValue, short* colX, short* colY)
{
	//x constraint detection:
	for (int h = 0; h < 9; h++)
	{
		if (fieldValue == arrayField[i][h] && fieldValue != arrayField[i][j]) // && fieldValue != arrayField[j][i]
		{
			if (colX != nullptr)
			{
				*colX = i;
				*colY = h;
			}
			return true;
		}
	}

	//y constraint detection:
	for (int h = 0; h < 9; h++)
	{
		if (fieldValue == arrayField[h][j] && fieldValue != arrayField[i][j]) // && fieldValue != arrayField[j][i]
		{
			if (colX != nullptr)
			{
				*colX = h;
				*colY = j;
			}
			return true;
		}
	}

	//block constraint detection:
	for (int h = 0; h < 3; h++)
	{
		for (int k = 0; k < 3; k++)
		{
			int blockX = (int)floor(i / 3) * 3 + h;
			int blockY = (int)floor(j / 3) * 3 + k;
			if (fieldValue == arrayField[blockX][blockY] && fieldValue != arrayField[i][j])
			{
				if (colX != nullptr)
				{
					*colX = blockX;
					*colY = blockY;
				}
				return true;
			}
		}
	}

	return false;
}

bool cMain::SolveTest()
{
	short* arrayFieldTest1 = SolveField(arrayField, 0);
	short* arrayFieldTest2 = SolveField(arrayField, 10);

	//compare the two tests
	for (int i = 0; i < 9; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			if (arrayFieldTest1[i][&j] != arrayFieldTest2[i][&j])
			{
				return false;
			}
		}
	}

	return true;
}

short* cMain::SolveField(short arrField[9][9], short start)
{
	short fieldValue = 0;
	if (start == 10)
		fieldValue = 10;
	bool collision = false;
	int blockX = 0;
	int blockY = 0;
	std::vector<int> fieldPathX;
	std::vector<int> fieldPathY;
	for (int i = 0; i < 9; i++) //columns (x)
	{
		for (int j = 0; j < 9; j++) //rows (y)
		{
			if (arrField[i][j] == 0)
			{
				do
				{
					collision = false;
					if(start == 10) fieldValue--;
					else fieldValue++;

					if (fieldValue == start) //hier wird ein feld zurückgegangen
					{
						//arrField[i][j] = 0;

						i = fieldPathX.back();
						j = fieldPathY.back();
						fieldValue = arrField[i][j];
						fieldPathX.pop_back();
						fieldPathY.pop_back();
					}

					//x constraint detection:
					for (int h = 0; h < 9; h++)
					{
						if (fieldValue == arrField[h][j]) // && fieldValue != arrayField[j][i]
						{
							collision = true;
							break;
						}
					}

					//y constraint detection:
					if (collision == false)
					{
						for (int h = 0; h < 9; h++)
						{
							if (fieldValue == arrField[i][h]) // && fieldValue != arrayField[j][i]
							{
								collision = true;
								break;
							}
						}
					}

					//block constraint detection:
					if (collision == false)
					{
						for (int h = 0; h < 3; h++)
						{
							for (int k = 0; k < 3; k++)
							{
								blockX = floor(i / 3) * 3 + h;
								blockY = floor(j / 3) * 3 + k;
								if (fieldValue == arrField[blockX][blockY])
								{
									collision = true;
									break;
								}
							}
						}
					}

					//place digit and track the path
					if (collision == false)
					{
						//arrField[i][j] = fieldValue;
						fieldPathX.push_back(i);
						fieldPathY.push_back(j);
						fieldValue = 0;
					}

				} while (collision == true);
			}
		}
	}

	return (short*)arrField;
}

void cMain::OnEmpty(wxCommandEvent& evte)
{
	int arrayField[9][9] = { 
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

	for (int i = 0; i < iFieldWidth; i++)
	{
		for (int j = 0; j < iFieldHeight; j++)
		{
			arrayButton[i * iFieldWidth + j]->SetLabel("");
			arrayButton[i * iFieldWidth + j]->Enable(true);
		}
	}

	evte.Skip();
}
void cMain::solveSelection(wxCommandEvent& evt17)
{
	if (evt17.GetId() == 5007)
		solvePath = true;
	else
		solvePath = false;

	evt17.Skip();
}
void cMain::OnAbout(wxCommandEvent& evti)
{
	wxMessageBox("This project was started on 18.07.2021 by Martin Schneider. The goal was to build a \tSudoku-Creator and Solver.",
		"About this project", wxOK | wxICON_INFORMATION);
	evti.Skip();
}
void cMain::OnExit(wxCommandEvent& evtu)
{
	Close(true);
}
