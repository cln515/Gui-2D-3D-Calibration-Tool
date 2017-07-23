#include "Renderer.h"

    void Renderer::initialize()
    {
		cnt=0;
		bDrawCoordinates=false;
		vertSize=0;
		motionStart=0;
		motionEnd=0;
      glClearColor(0, 0, 0, 1);
	  pr.shaderSetup();
    }
    void Renderer::setViewport(int w, int h)
    {
      glViewport(0, 0, (GLint)w, (GLint)h);
	  w_=w;
	  h_=h;
	  
      //glOrtho(-w / 200.0, w / 200.0, -h / 200.0, h / 200.0, 1.0, 2.0);
    }

	void Renderer::setCameraMat(Matrix3d& R,Vector3d& T){
	Matrix4d camPara;
	camPara.block(0,0,3,3)=R;
	camPara.block(0,3,3,1)=T;
	cR=R;
	cT=T;
	camPara.block(3,0,1,4)<<0,0,0,1;
	pr.render_shader_viewset(camPara,h_);
	//	glMatrixMode(GL_PROJECTION);
 //     glLoadIdentity();
	//  gluPerspective(30.0, (double)w_ / (double)h_, 0.05, 300.0);	  
	//  
	//  //glFrustum( -1.0, 1.0, -1.0, 1.0, 5.0, 25.0 );
	//glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();
	//double m[16]={R(0,0),R(1,0),R(2,0),0,
	//	  R(0,1),R(1,1),R(2,1),0,
	//	  R(0,2),R(1,2),R(2,2),0,
	//	  T(0),T(1),T(2),1};
	//  
	// glMultMatrixd(m); 
	//	
	}

    void Renderer::draw()
    {

      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	  //glEnableClientState(GL_VERTEX_ARRAY);
      //glMatrixMode(GL_MODELVIEW);
	  glEnable(GL_DEPTH_TEST);
      // view
      //glLoadIdentity();
	  glClearColor(0.0f, 0.0f, 0.0f, 1.0f); 
	  pr.render_shader_main();
	  GLint view[4];
      glGetIntegerv(GL_VIEWPORT,view);
	  pr.shaderEnable(0);
	  glPointSize(12.0f);
	  glBegin(GL_POINTS);
	  for(auto itr=selectedPoint.begin();itr!=selectedPoint.end();itr++){
		  int i=itr->first;
			int r=i/16;
			int g=(i%16)/4;
			int b=(i%4);
			glColor3d(0.25+0.25*r,0.25+0.25*g,0.25+0.25*b);
	  
		  glVertex3d(itr->second(0) , itr->second(1),itr->second(2));	
	  }
	  glEnd();
	  /*glColor3d(1.0,1.0,1.0);
	  glVertexPointer(3,GL_FLOAT,0,verteces);
	  glDrawArrays(GL_POINTS,0,vertSize);
	  glColor3d(1.0,1.0,0.0);
	  glVertexPointer(3,GL_FLOAT,0,featurePoints);
	  glDrawArrays(GL_POINTS,0,featSize);
	  glDisableClientState(GL_VERTEX_ARRAY);*/
    }

	Vector3d Renderer::readPlyFile(string filePath){
		vector<string> fileNames;
		fileNames.push_back(filePath);
		plyData.release();
		plyData.readPlyFile(fileNames,fileNames.size());
		verteces=plyData.getVertecesPointer();
		vertSize=plyData.getVertexNumber();
		pr.setDataRGBfromReflectance(verteces,plyData.getFaces(),plyData.getReflectancePointer(),plyData.getVertexNumber(),plyData.getFaceNumber());
		return plyData.getCentroid();
	}

	int Renderer::get3Dcoordinate(double x,double y,Vector3d& depth){
		GLint view[4];
		glGetIntegerv(GL_VIEWPORT,view);
		GLfloat* depthArray=(GLfloat*)malloc(sizeof(GLfloat)*view[2]*view[3]);
		glReadPixels(view[0],view[1],view[2],view[3],GL_DEPTH_COMPONENT,GL_FLOAT,depthArray);
		int basex=(int)x;
		int basey=view[3]-1-(int)y;
		int idx=basex+basey*view[2];
		for(auto itr=selectedPoint.begin();itr!=selectedPoint.end();itr++){
			double phi,theta;
			Vector3d transedPoint=cR*itr->second+cT;
			omniTrans(transedPoint(0),transedPoint(1),transedPoint(2),phi,theta,view[3]);
			double dx=theta-x;
			double dy=phi-y;
			if(dx*dx+dy*dy<5*5){
				depth=itr->second;
				selectedPoint.erase(itr);
				return -1; 
			}
		}
		if(depthArray[idx]<1.0){
			double depth_=depthArray[idx];
			depth_*=(FAR_-NEAR_);
			depth_+=NEAR_;
			rev_omniTrans(x,y,view[2],view[3],depth);
			depth*=depth_;
			depth=cR.inverse()*(depth-cT);
			int i=0;
			while(selectedPoint.find(i)!=selectedPoint.end())i++;
			selectedPoint.insert(P_MAP::value_type(i,depth));
			return 1;
		}else return 0;

	
	
	};