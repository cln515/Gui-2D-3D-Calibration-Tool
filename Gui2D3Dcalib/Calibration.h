

#include <opengv/absolute_pose/methods.hpp>
#include <opengv/absolute_pose/CentralAbsoluteAdapter.hpp>
//#include <opengv/absolute_pose/NoncentralAbsoluteAdapter.hpp>
#include <opengv/sac/Ransac.hpp>
#include <opengv/sac_problems/absolute_pose/AbsolutePoseSacProblem.hpp>
#include <utility\utility.h>

#ifndef CALIB
#define CALIB

_6dof absolutePoseRansac(opengv::bearingVectors_t bearingVectors,opengv::points_t points);

#endif