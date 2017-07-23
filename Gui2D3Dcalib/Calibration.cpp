#include "Calibration.h"


_6dof absolutePoseRansac(opengv::bearingVectors_t bearingVectors,opengv::points_t points){
//_6dof absolutePoseRansac(vector<Vector3d> bearingVectors,vector<Vector3d> points){
	opengv::absolute_pose::CentralAbsoluteAdapter adapter(bearingVectors,points);
	//ransac
	opengv::sac::Ransac<opengv::sac_problems::absolute_pose::AbsolutePoseSacProblem> ransac;
	std::shared_ptr<opengv::sac_problems::absolute_pose::AbsolutePoseSacProblem> absposeproblem_ptr(
	new opengv::sac_problems::absolute_pose::AbsolutePoseSacProblem(
		adapter,
		opengv::sac_problems::absolute_pose::AbsolutePoseSacProblem::GP3P));
	ransac.sac_model_ = absposeproblem_ptr;
	ransac.threshold_ = 1.0 - cos(atan(sqrt(16.0)*0.5/400.0));//reprojection error(sqrt(2.0)+0.5)pixel / focal length;
	//ransac.threshold_ = 7e-7;
	ransac.max_iterations_ = 250;
	//cout<<ransac.probability_<<endl;
	ransac.computeModel();
	Matrix3d cpRot=ransac.model_coefficients_.block(0,0,3,3);
	adapter.setR(cpRot);
	adapter.sett(ransac.model_coefficients_.col(3));
	opengv::transformation_t best_form = opengv::absolute_pose::optimize_nonlinear(adapter,ransac.inliers_);
	_6dof optimizedPara;
	cpRot=best_form.block(0,0,3,3);
	R2axisRot(cpRot,optimizedPara.rx,optimizedPara.ry,optimizedPara.rz);
	optimizedPara.x=best_form(0,3);
	optimizedPara.y=best_form(1,3);
	optimizedPara.z=best_form(2,3);

	

	return optimizedPara;
}