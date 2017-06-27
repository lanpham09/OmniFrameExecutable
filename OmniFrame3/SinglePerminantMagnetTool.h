#ifndef SINGLE_PERMINANT_MAGNET_TOOL_H
#define SINGLE_PERMINANT_MAGNET_TOOL_H

#include "Producers/magneticActuationMatrixProducer.h"
#include "Producers/MagneticToolProducer.h"
#include "Utilities/Magnetics.h"
#include "Utilities/Math.h"

class SinglePerminantMagnetTool: public magneticActuationMatrixProducer
{
    MagneticToolProducer* magTool;

public:
    SinglePerminantMagnetTool(  MagneticToolProducer* magTool );
    bool getMatrix( Eigen::MatrixXd& actuationMatrix, const std::vector<Math::Position>& omnimagnetLocations, actuationMatrixType type );
    std::vector< Math::MagneticTool > getMagneticToolList();

};

#endif // SINGLE_PERMINANT_MAGNET_TOOL_H
