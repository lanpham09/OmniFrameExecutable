#ifndef SETUP_H
#define SETUP_H

//Include files purely for Omnimagnet Setup
#include "Devices/MultipleOmnimagnetSetup.h"
#include "Devices/MultipleOmnimagnetSystem.h"

#include "Producers/MagneticToolProducer.h"
#include "Producers/Configuration5DOF_FromFile.h"
#include "Producers/Configuration5DOF_Producer.h"
#include "Producers/ConstantPositionProducer.h"
#include "Producers/TransformedConfiguration5DOF_Producer.h"
#include "Producers/SinglePerminantMagnetTool.h"
#include "Producers/TwoOmnimagnet_SingularDirectionProducer.h"
#include "Producers/PositionProducer.h"

#include "Controls/DiscreteController.h"

class Setup
{
public:
    Setup();
    ~Setup();

    // Omnimagnets and System
    MultipleOmnimagnetSetup systemSetup;
};

#endif // SETUP_H
