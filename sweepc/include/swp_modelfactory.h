/*
  Author(s):      Matthew Celnik (msc37)
  Project:        sweep (population balance solver)

  File purpose:
    The ModelFactory is a factory class for sweep particle models.  It
    provides routines for creating, reading and writing particle model
    data objects.
*/

#ifndef SWEEP_MODELFACTORY_H
#define SWEEP_MODELFACTORY_H

#include "swp_params.h"
#include "swp_modeltype.h"
#include "swp_modeldata.h"
#include "swp_particledata.h"
#include "swp_coagmodeldata.h"
#include <iostream>

namespace Sweep
{
class ModelFactory
{
public:
    // MODEL DATA CREATION.

    // Creates a new model data object of the given type.
    static IModelData *const CreateData(
        ModelType id,        // Model ID.
        ParticleData &parent // Parent ParticleData object.
        );

    // Reads a model from a binary stream.  The first item read
    // is the model ID which tells the ModelFactory what type
    // of model to read.
    static IModelData *const Read(
        std::istream &in,    // Input stream.
        ParticleData &parent // Parent ParticleData object.
        );

    // Reads a coagulation model from a binary stream.  The first item read
    // is the model ID which tells the ModelFactory what type
    // of coagulation model to read.
    static CoagModelData *const ReadCoag(
        std::istream &in,    // Input stream.
        ParticleData &parent // Parent ParticleData object.
        );


    // STREAM OUTPUT.

    // Writes a model, along with its ID to an output stream.
    static void Write(
        const IModelData &model, // Model to write.
        std::ostream &out        // Output stream.
        );
};
};

#endif
