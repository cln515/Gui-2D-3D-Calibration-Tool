
#include "utility\utility.h"
#include "basicPly\BasicPly.h"
#include "PanoramaRenderer\PanoramaShaderRenderer.h"
#include <map>
#include <gl\GL.h>
#include <gl\GLU.h>

#ifndef MAIN_CANVAS
#define MAIN_CANVAS

typedef map<int,Vector3d> P_MAP;

class Renderer
{
  public:
	float* verteces;
	float* featurePoints;
	unsigned int vertSize,featSize,motionStart,motionEnd;
	int cnt;
	int w_,h_;
	bool bDrawCoordinates;

    void initialize();
    void setViewport(int w, int h);

	void setCameraMat(Matrix3d& R,Vector3d& T);

    void draw();
	Vector3d readPlyFile(string filePath);
	int get3Dcoordinate(double x,double y,Vector3d& depth);
	P_MAP get3DMap(){return selectedPoint;};
	void clearMap(){selectedPoint.clear();};
private:
	Matrix3d cR;
	Vector3d cT;
	P_MAP selectedPoint;
	BasicPly plyData;
	PanoramaRenderer pr;
};

#endif