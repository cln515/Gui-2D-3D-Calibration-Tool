#include "Main.h"

wxBEGIN_EVENT_TABLE(MainGLCanvas, wxGLCanvas)
    EVT_PAINT(MainGLCanvas::OnPaint)
	//EVT_SIZE(MainGLCanvas::OnSize)
    EVT_MOUSE_EVENTS(MainGLCanvas::OnMouseEvent)
	EVT_ERASE_BACKGROUND(MainGLCanvas::OnEraseBackground)
	EVT_KEY_DOWN(MainGLCanvas::OnChar)
	EVT_KEY_UP(MainGLCanvas::OnCharUp)
	EVT_IDLE(MainGLCanvas::OnIdle)
wxEND_EVENT_TABLE()

//EVT_CHAR(MainGLCanvas::OnChar)
    
MainGLCanvas::MainGLCanvas(MyFrame*parent,
                           wxWindowID id,
                           int* gl_attrib)
						   : wxGLCanvas(parent, id, gl_attrib),isInitialized_(false),bReconstructionSet(false)
{
	//Connect(wxEVT_SIZE, wxSizeEventHandler(MainGLCanvas::OnSize));
	//Connect(wxEVT_PAINT, wxPaintEventHandler(MainGLCanvas::OnPaint));
	//Connect(wxEVT_MOUSE_);
	//Connect(wxEVT_ERASE_BACKGROUND, wxEraseEventHandler(MainGLCanvas::OnEraseBackground));
	m_owner=parent;
	m_glRC=new wxGLContext(this);
	bOriginCenter=false;
	bDataChange=false;
	bCtrlDown=false;
}

void MainGLCanvas::OnPaint(wxPaintEvent &event)
{
	wxPaintDC dc(this);
	SetCurrent(*m_glRC);
	const wxSize ClientSize = GetClientSize();
	if(!isInitialized_){
      renderer.initialize();
	  renderer.readPlyFile("C:\\Users\\ryoichi\\Documents\\Visual Studio 2012\\Projects\\Gui2D3Dcalib\\x64\\Release\\Seiken2_ds.ply");
      int w;
      int h;
	  GetClientSize(&w, &h);
      renderer.setViewport(w, h);
      isInitialized_=true;
	  R=Matrix3d::Identity();
	  //R=-R;//Initial status of OpenGL is left hand coordinates system
	  T<<0,0,0;
	}
	if(bDataChange){
		renderer.readPlyFile(newPlyFile);
		bDataChange=false;
		R=Matrix3d::Identity();
	  T<<0,0,0;
	}
	renderer.setCameraMat(R,T);
	renderer.draw();
	glFlush();
	SwapBuffers();
}

void MainGLCanvas::SetCameraParameter(_6dof camPara){
	R=axisRot2R(camPara.rx,camPara.ry,camPara.rz);
	T<<camPara.x,camPara.y,camPara.z;
	Matrix4d M,Mdst;
	M.block(0,0,3,3)=R;
	M.block(0,3,3,1)=T;
	M.block(3,0,1,4)<<0,0,0,1;
	Mdst=M.inverse();
	R=Mdst.block(0,0,3,3);
	T=Mdst.block(0,3,3,1);
}

void MainGLCanvas::GetCameraParameter(_6dof& camPara){
	Matrix4d M,Mdst;
//	R=axisRot2R(camPara.rx,camPara.ry,camPara.rz);
//	T<<camPara.x,camPara.y,camPara.z;
	
	M.block(0,0,3,3)=R;
	M.block(0,3,3,1)=T;
	M.block(3,0,1,4)<<0,0,0,1;
	Mdst=M.inverse();
	R2axisRot(Mdst.block(0,0,3,3),camPara.rx,camPara.ry,camPara.rz);
	camPara.x=T(0);camPara.y=T(1);camPara.z=T(2);
}

void MainGLCanvas::GetCameraParameter(Vector3d& camTrans,Matrix3d& camRot){
	Matrix4d M,Mdst;
//	R=axisRot2R(camPara.rx,camPara.ry,camPara.rz);
//	T<<camPara.x,camPara.y,camPara.z;
	
	M.block(0,0,3,3)=R;
	M.block(0,3,3,1)=T;
	M.block(3,0,1,4)<<0,0,0,1;
	Mdst=M.inverse();
	camTrans=Mdst.block(0,3,3,1);
	camRot=Mdst.block(0,0,3,3);
}

void MainGLCanvas::OnSize(int w,int h)
{
	if ( !IsShownOnScreen() )
        return;
    // This is normally only necessary if there is more than one wxGLCanvas
    // or more than one wxGLContext in the application.
    SetCurrent(*m_glRC);

    // It's up to the application code to update the OpenGL viewport settings.
    // This is OK here only because there is only one canvas that uses the
    // context. See the cube sample for that case that multiple canvases are
    // made current with one context.
	
	 //GetClientSize(&w, &h);
	 w=h*2;
	 SetClientSize(w,h);
      renderer.setViewport(w, h);
//    renderer.setViewport(event.GetSize().x, event.GetSize().y);
	
}

void MainGLCanvas::OnMouseEvent(wxMouseEvent& event){
	static int dragging = 0;
    static float last_x, last_y;

    // Allow default processing to happen, or else the canvas cannot gain focus
    // (for key events).
    event.Skip();

	if (event.LeftIsDown())//Trans
    {
        if (!dragging)
        {
            dragging = 1;
        }
        else
        {
            double m_trax = (event.GetX() - last_x)*0.01;
            double m_tray = (event.GetY() - last_y)*0.01;
			Vector3d dTx,dTy;
			dTx<<1,0,0;
			dTy<<0,1,0;
			
		/*	if(bOriginCenter){
				dTx=R*dTx;
				dTy=R*dTy;
			}*/

			T+=dTx*m_trax-dTy*m_tray;

            //Refresh(false);
        }
        last_x = event.GetX();
        last_y = event.GetY();
    }
	else if (event.RightIsDown())//Rot
    {
		
			if (!dragging)
			{
				dragging = 1;
				if(bCtrlDown){
					Vector3d depth;
					int ret;
					if((ret=renderer.get3Dcoordinate(event.GetX(),event.GetY(),depth))==1){
						stringstream ss;
						ss<<"hit!! ("<<depth(0)<<","<<depth(1)<<","<<depth(2)<<")";
						m_owner->SetStatusText(ss.str());
					}else if(ret==-1){
						stringstream ss;
						ss<<"erase!! ("<<depth(0)<<","<<depth(1)<<","<<depth(2)<<")";
						m_owner->SetStatusText(ss.str());
					}
					else{
						m_owner->SetStatusText("missing!!");
					}
				}
			}
			else
			{
				double m_roty = (event.GetX() - last_x)*0.01;
				double m_rotx = (event.GetY() - last_y)*0.01;
				Matrix3d dR=axisRot2R(-m_rotx,-m_roty,0);
				R=dR*R;
				if(!bOriginCenter)T=dR*T;//rotation by centering T
				//Refresh(false);
			}
		
        last_x = event.GetX();
        last_y = event.GetY();
	}else
    {
        dragging = 0;
    }
	if(event.GetWheelRotation()!=0){
			double m_traz = (event.GetWheelRotation())*0.01;
            Vector3d dTz;
			dTz<<0,0,-1;
			/*if(bOriginCenter){
				dTz=R*dTz;
			}*/
			T+=dTz*m_traz;
            //Refresh(false);
	}
};

void MainGLCanvas::OnCharUp(wxKeyEvent& event){
	    switch( event.GetKeyCode() )
    {

	case WXK_CONTROL:
		bCtrlDown=false;
		return;
		}
}

void MainGLCanvas::OnChar(wxKeyEvent& event)
{
	
    switch( event.GetKeyCode() )
    {
    case WXK_ESCAPE:
        wxTheApp->ExitMainLoop();
        return;
	case WXK_CONTROL:
		bCtrlDown=true;
		return;
    case WXK_LEFT:
        //m_yrot -= 15.0;
        break;

    case WXK_RIGHT:
        //m_yrot += 15.0;
        break;

    case WXK_UP:
        //m_xrot += 15.0;
        break;

    case WXK_DOWN:
        //m_xrot -= 15.0;
        break;

    case 's': case 'S':
        //g_smooth = !g_smooth;
        //if (g_smooth)
        //    glShadeModel(GL_SMOOTH);
        //else
        //    glShadeModel(GL_FLAT);
        break;

    case 'l': case 'L':
        //g_lighting = !g_lighting;
        //if (g_lighting)
        //    glEnable(GL_LIGHTING);
       // else
        //    glDisable(GL_LIGHTING);
        break;
    case 'o': case 'O':
		bOriginCenter=!bOriginCenter;
        break;
    case 'c': case 'C':
		renderer.bDrawCoordinates=!renderer.bDrawCoordinates;
        break;
    default:
        event.Skip();
        return;
    }

    //Refresh(false);
}

void MainGLCanvas::OnEraseBackground(wxEraseEvent& event)
{
	// Do nothing, to avoid flashing.
}

void MainGLCanvas::OnIdle(wxIdleEvent& event)
{
	event.RequestMore(true);
//	this->SetCurrent();
	this->Refresh();
}

void MainGLCanvas::ReadPlyFile(string fileName){
	
	newPlyFile=fileName;
	bDataChange=true;
}

P_MAP MainGLCanvas::Obtain3DPoint(){
	return renderer.get3DMap();
}