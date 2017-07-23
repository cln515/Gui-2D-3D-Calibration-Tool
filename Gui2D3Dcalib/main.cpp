/////////////////////////////////////////////////////////////////////////////
// Name:        minimal.cpp
// Purpose:     Minimal wxWidgets sample
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h".



#include "Main.h"

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// Define a new application type, each program should derive a class from wxApp
class MyApp : public wxApp
{
public:
    // override base class virtuals
    // ----------------------------

    // this one is called on application startup and is a good place for the app
    // initialization (doing it here and not in the ctor allows to have an error
    // return: if OnInit() returns false, the application terminates)
    virtual bool OnInit() wxOVERRIDE;
};



// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    // menu items
    Minimal_Quit = wxID_EXIT,
    ReadPly,
	ReadImage,
	ManualCalib,
	WriteCalibParameter,
	ReadCalibParameter, 
    // it is important for the id corresponding to the "About" command to have
    // this standard value as otherwise it won't be handled properly under Mac
    // (where it is special and put into the "Apple" menu)
    Minimal_About = wxID_ABOUT
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(Minimal_Quit,  MyFrame::OnQuit)
	EVT_MENU(ReadPly, MyFrame::OnReadPly)
	EVT_MENU(ReadImage, MyFrame::OnReadImage)
	EVT_MENU(ManualCalib, MyFrame::OnManualCalib)
	EVT_MENU(WriteCalibParameter, MyFrame::OnWriteCalibParameter)
	EVT_MENU(ReadCalibParameter, MyFrame::OnReadCalibParameter)
    EVT_MENU(Minimal_About, MyFrame::OnAbout)
	EVT_SIZE(MyFrame::OnSize)
wxEND_EVENT_TABLE()

// Create a new application object: this macro will allow wxWidgets to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also implements the accessor function
// wxGetApp() which will return the reference of the right type (i.e. MyApp and
// not wxApp)
wxIMPLEMENT_APP(MyApp);

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

// 'Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
    // call the base class initialization method, currently it only parses a
    // few common command-line options but it could be do more in the future
    if ( !wxApp::OnInit() )
        return false;

    // create the main application window
    MyFrame *frame = new MyFrame("3D Panorama View");

    // and show it (the frames, unlike simple controls, are not shown when
    // created initially)
    frame->Show(true);

    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned false here, the
    // application would exit immediately.
    return true;
}

// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------

// frame constructor
MyFrame::MyFrame(const wxString& title)
       : wxFrame(NULL, wxID_ANY, title)
{
    // set the frame icon
    SetIcon(wxICON(sample));

#if wxUSE_MENUS
    // create a menu bar
    wxMenu *fileMenu = new wxMenu;
    wxMenu *calibMenu = new wxMenu;

    // the "About" item should be in the help menu
    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(Minimal_About, "&About\tF1", "Show about dialog");
    fileMenu->Append(ReadPly, "Read Ply File", "Read Ply File");
    fileMenu->Append(ReadImage, "Read Image File", "Read Image File");
    fileMenu->Append(Minimal_Quit, "E&xit\tAlt-X", "Quit this program");


	calibMenu->Append(ManualCalib,"Manual Calibration","Compute Extrinsic Parameter");
	calibMenu->Append(wxID_SEPARATOR);
	calibMenu->Append(WriteCalibParameter,"Write Calibration Parameter","Write Calibration Parameter");
	calibMenu->Append(ReadCalibParameter,"Write Calibration Parameter","Write Calibration Parameter");
    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, "&File");
    menuBar->Append(calibMenu, "&Calibration");
    menuBar->Append(helpMenu, "&Help");

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);
	dFrame=new DrawFrame(wxT("Image Window"),
                                 wxDefaultPosition, wxSize(900, 450));;
	dFrame->Show(true);
	m_canvas=new MainGLCanvas(this, wxID_ANY,NULL);

#else // !wxUSE_MENUS
    // If menus are not available add a button to access the about box
    wxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
    wxButton* aboutBtn = new wxButton(this, wxID_ANY, "About...");
    aboutBtn->Bind(wxEVT_BUTTON, &MyFrame::OnAbout, this);
    sizer->Add(aboutBtn, wxSizerFlags().Center());
#endif // wxUSE_MENUS/!wxUSE_MENUS

#if wxUSE_STATUSBAR
    // create a status bar just for fun (by default with 1 pane only)
    CreateStatusBar(2);
    SetStatusText("");
#endif // wxUSE_STATUSBAR
}


// event handlers

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // true is to force the frame to close
    Close(true);
}

void MyFrame::OnReadPly(wxCommandEvent& WXUNUSED(event)){
	static wxString s_extDef;
    wxString path = wxFileSelector(
                                    wxT("Select the ply file to load"),
                                    wxEmptyString, wxEmptyString,
                                    s_extDef,
                                    wxString::Format
                                    (
                                        wxT("Plain text (*.ply)|*.ply"),
                                        wxFileSelectorDefaultWildcardStr,
                                        wxFileSelectorDefaultWildcardStr
                                    ),
                                    wxFD_OPEN|wxFD_CHANGE_DIR|wxFD_PREVIEW|wxFD_NO_FOLLOW,
                                    this
                                   );

    if ( !path )
        return;
	m_canvas->ReadPlyFile((string)path);
    
}

void MyFrame::OnSize(wxSizeEvent &event)
{
	if ( !IsShownOnScreen() )
        return;

    // It's up to the application code to update the OpenGL viewport settings.
    // This is OK here only because there is only one canvas that uses the
    // context. See the cube sample for that case that multiple canvases are
    // made current with one context.
	 int w;
     int h;
	 GetClientSize(&w, &h);
	 m_canvas->OnSize(w,h);
	 w=h*2;
	 SetClientSize(w,h);	

}

void MyFrame::OnReadImage(wxCommandEvent& WXUNUSED(event)){
	static wxString s_extDef;
    wxString path = wxFileSelector(
                                    wxT("Select the image file"),
                                    wxEmptyString, wxEmptyString,
                                    s_extDef,
                                    wxString::Format
                                    (
                                        wxT("All File|*.*"),
                                        wxFileSelectorDefaultWildcardStr,
                                        wxFileSelectorDefaultWildcardStr
                                    ),
                                    wxFD_OPEN|wxFD_CHANGE_DIR|wxFD_PREVIEW|wxFD_NO_FOLLOW,
                                    this
                                   );

    if ( !path )
        return;
	
	if(dFrame->m_canvas->ReadImage((string)path)){
		 SetStatusText("Success to read image!!");
	}else{
		 SetStatusText("Fail to read image!!");	
	};
	

}
void MyFrame::OnManualCalib(wxCommandEvent& WXUNUSED(event)){
	//get2d
	I_MAP imap=dFrame->getImap();
	cv::Size size=dFrame->m_canvas->getImageSize();
	//get3d
	P_MAP pmap=m_canvas->Obtain3DPoint();
	//make vector data
	opengv::bearingVectors_t bv;
	opengv::points_t pv;
	for(auto itr=imap.begin();itr!=imap.end();itr++){
		if(pmap.find(itr->first)!=pmap.end()){
			cv::Point2f p=cv::Point2f();
			Vector3d bve;
			rev_omniTrans(itr->second.x,itr->second.y,size.width,size.height,bve);
			bv.push_back(bve);
			pv.push_back(pmap.at(itr->first));
		}
	}
	//compute
	_6dof camPara=absolutePoseRansac(bv,pv);
	m_canvas->SetCameraParameter(camPara);
}


void MyFrame::OnWriteCalibParameter(wxCommandEvent& WXUNUSED(event)){
	wxFileDialog dialog(this,
                        wxT("Cpara File"),
						wxEmptyString,
                        wxEmptyString,
                        wxT("cpara files (*.cpara)|*.cpara"),
                        wxFD_SAVE|wxFD_OVERWRITE_PROMPT);

    dialog.SetFilterIndex(1);
    if (dialog.ShowModal() == wxID_OK)
	{
		Vector3d t;
		Matrix3d r;
		m_canvas->GetCameraParameter(t,r);//2d->3d
		Vector4d q=dcm2q(r);
		ofstream ofs((string)dialog.GetPath());
		ofs<<"Camera Parameter, Trans"<<endl
			<<t(0)<<" "<<t(1)<<" "<<t(2)<<endl
			<<"Rotation"<<endl
			<<q(0)<<" "<<q(1)<<" "<<q(2)<<" "<<q(3)<<endl;
		ofs.close();
	}
}

void MyFrame::OnReadCalibParameter(wxCommandEvent& WXUNUSED(event)){
	wxFileDialog dialog(this,
                        wxT("Cpara File"),
						wxEmptyString,
                        wxEmptyString,
                        wxT("cpara files (*.cpara)|*.cpara"),
						wxFD_OPEN);

    dialog.SetFilterIndex(1);
	if (dialog.ShowModal() == wxID_OK){
		Matrix4d m=readCPara((string)dialog.GetPath());		
		m_canvas->SetCameraParameter(m2_6dof(m));
	}
		
		//{
	//	Vector3d t;
	//	Matrix3d r;

	//	m_canvas->GetCameraParameter(t,r);//2d->3d
	//	Vector4d q=dcm2q(r);
	//	ofstream ofs((string)dialog.GetPath());
	//	ofs<<"Camera Parameter, Trans"<<endl
	//		<<t(0)<<" "<<t(1)<<" "<<t(2)<<endl
	//		<<"Rotation"<<endl
	//		<<q(0)<<" "<<q(1)<<" "<<q(2)<<" "<<q(3)<<endl;
	//	ofs.close();
	//}
}


void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox(wxString::Format
                 (
                    "Welcome to %s!\n"
                    "\n"
                    "This is the minimal wxWidgets sample\n"
                    "running under %s.",
                    wxVERSION_STRING,
                    wxGetOsDescription()
                 ),
                 "About wxWidgets minimal sample",
                 wxOK | wxICON_INFORMATION,
                 this);
}
