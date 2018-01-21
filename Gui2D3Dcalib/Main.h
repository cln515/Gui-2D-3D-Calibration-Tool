#include "wx/wxprec.h"
#include "PanoramaRenderCanvas.h"
#include "ImageWindow.h"
#include "Calibration.h"
#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "utility\utility.h"
#include "NonLinearPoseSolver\NonLinearPoseSolver.h"
#include "image_utility\image_utility.h"
#include "Renderer.h"
#include <wx/glcanvas.h>
#include <gl\GLU.h>

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif



// ----------------------------------------------------------------------------
// resources
// ----------------------------------------------------------------------------

// the application icon (under Windows it is in resources and even
// though we could still include the XPM here it would be unused)
#ifndef wxHAS_IMAGES_IN_RESOURCES
    #include "../sample.xpm"
#endif

class MainGLCanvas;

// Define a new frame type: this is going to be our main frame
class MyFrame : public wxFrame
{
public:
    // ctor(s)
    MyFrame(const wxString& title);
	DrawFrame *dFrame;
	MainGLCanvas *m_canvas;
    // event handlers (these functions should _not_ be virtual)
    void OnQuit(wxCommandEvent& event);
	void OnReadPly(wxCommandEvent& event);
	void OnReadImage(wxCommandEvent& event);
	void OnManualCalib(wxCommandEvent& event);

	void OnWriteCalibParameter(wxCommandEvent& event);
	void OnReadCalibParameter(wxCommandEvent& event);

	void OnWriteCheckImage(wxCommandEvent& event);

	void OnSize(wxSizeEvent &event);
    void OnAbout(wxCommandEvent& event);

private:
	
    // any class wishing to process wxWidgets events must use this macro
    wxDECLARE_EVENT_TABLE();
};


class MainGLCanvas:public wxGLCanvas{
private:
	bool isInitialized_;
	Renderer renderer;
	MyFrame* m_owner;
	wxGLContext* m_glRC;
	Matrix3d R;
	Vector3d T;
	bool bReconstructionSet;
	bool bDataChange;
	string newPlyFile;
	bool bOriginCenter;
	bool bCtrlDown;
	GLfloat* reflectanceImage;
	vector<POINT_3D> w_p;
	vector<POINT_3D> i_p;
public:

	MainGLCanvas(MyFrame*parent,
                           wxWindowID id,
                           int* gl_attrib);
	void OnPaint(wxPaintEvent &event);
    void OnMouseEvent(wxMouseEvent& event);
	//void OnSize(wxSizeEvent &event);
	void OnSize(int w,int h);
	void OnChar(wxKeyEvent& event);
	void OnCharUp(wxKeyEvent& event);
	void OnIdle(wxIdleEvent& event);
	void OnEraseBackground(wxEraseEvent& event);
	void ReadPlyFile(string fileName);
	void SetCameraParameter(_6dof camPara);
	void GetCameraParameter(_6dof& camPara);
	void GetCameraParameter(Vector3d& camTrans,Matrix3d& camRot);
	BasicPly GetPlyData() {return renderer.getPlyData() ; };
	GLfloat* getReflecImage() {
		return reflectanceImage;
	};
	P_MAP Obtain3DPoint();
    wxDECLARE_EVENT_TABLE();

};