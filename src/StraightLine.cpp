#include "sisl_toolbox/StraightLine.hpp"
#include "sisl.h"

StraightLine::StraightLine(int type, int dimension, int order, Eigen::Vector3d startPoint, Eigen::Vector3d endPoint) 
    : Curve(dimension, order, type)
    {        
        // Generate a straight line from startPoint to endPoint
        s1602(&startPoint[0], &endPoint[0], Order(), Dimension(), startParameter_, &endParameter_, &curve_, &statusFlag_);

        // Pick parameters range of the line.
        s1363(curve_, &startParameter_, &endParameter_, &statusFlag_);

        // Pick curve length
        s1240(curve_, Epsge(), &length_, &statusFlag_);

        FromAbsToPos(startParameter_, startPoint_);
        FromAbsToPos(endParameter_, endPoint_);
    }