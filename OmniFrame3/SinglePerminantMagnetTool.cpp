#include "SinglePerminantMagnetTool.h"

SinglePerminantMagnetTool::SinglePerminantMagnetTool( MagneticToolProducer* magTool):
    magneticActuationMatrixProducer(  magTool->getFrame() )
{
    this->magTool = magTool;
}

bool SinglePerminantMagnetTool::getMatrix( Eigen::MatrixXd& actuationMatrix, const std::vector<Math::Position>& omnimagnetLocations, actuationMatrixType type )
{
    using namespace Magnetics;

    int numMag = omnimagnetLocations.size();
    int numCol = 3*numMag;
    switch(type)
    {
    case FORCE_ONLY:
        actuationMatrix.resize(3,numCol);
        break;
    case FIELD_ONLY:
        actuationMatrix.resize(3,numCol);
        break;
    case TORQUE_ONLY:
        actuationMatrix.resize(3,numCol);
        break;
    case WRENCH:
        actuationMatrix.resize(6,numCol);
        break;
    case WRENCH_AUGMENTED:
        actuationMatrix.resize(7,numCol);
        break;
    case FIELD_FORCE:
        actuationMatrix.resize(6,numCol);
        break;
    }

    //get magntic tool info
    Math::MagneticTool tool;
    bool returnValue = magTool->getMagneticTool(tool);

    for( int magNum = 0; magNum < numMag; magNum ++ )
    {
        int colNum = 3*magNum;
        switch(type)
        {
        case FORCE_ONLY:
            actuationMatrix.block<3,3>(0,colNum) = Magnetics::dipoleForceMatrix(tool.dipoleMoment,tool.position,omnimagnetLocations[magNum]);
            break;
        case FIELD_ONLY:
            actuationMatrix.block<3,3>(0,colNum) = Magnetics::dipoleMatrix(tool,omnimagnetLocations[magNum]);
            break;
        case TORQUE_ONLY:
            actuationMatrix.block<3,3>(0,colNum) = Magnetics::dipoleTorqueMatrix(tool.dipoleMoment,tool.position,omnimagnetLocations[magNum]);
            break;
        case WRENCH:
            actuationMatrix.block<6,3>(0,colNum) = Magnetics::dipoleWrenchMatrix(tool.dipoleMoment,tool.position,omnimagnetLocations[magNum]);
            break;
        case WRENCH_AUGMENTED:
            actuationMatrix.block<6,3>(0,colNum) = Magnetics::dipoleWrenchMatrix(tool.dipoleMoment,tool.position,omnimagnetLocations[magNum]);
            actuationMatrix.block<1,3>(6,colNum) = (tool.dipoleMoment.normalized().asVector().transpose()) * Magnetics::dipoleMatrix(tool, omnimagnetLocations[magNum]);
            break;
        case FIELD_FORCE:
            actuationMatrix.block<6,3>(0,colNum) = Magnetics::dipoleFieldForceMatrix(tool.dipoleMoment,tool.position,omnimagnetLocations[magNum]);
            break;
        }
    }


    return returnValue;
}


std::vector< Math::MagneticTool > SinglePerminantMagnetTool::getMagneticToolList()
{
    Math::MagneticTool tool;
    magTool->getMagneticTool(tool);
    std::vector< Math::MagneticTool> retValue = {tool};
    return retValue;
}
