#include "cApp.h"
#include "cMain.h"

wxIMPLEMENT_APP(cApp);

cApp::cApp()
{

}

cApp::~cApp()
{

}

bool cApp::OnInit()
{
	frame = new cMain();

	frame->Show();

	return true;
}
