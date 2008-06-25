/*
  Author(s):      Matthew Celnik (msc37)
  Project:        sweep (population balance solver)

  File purpose:
    The EnsembleStats class brings together vital statistics about a particle
    ensemble, including basic stats and the stats generated by all the
    particle models.
*/

#ifndef SWEEP_ENSEMBLESTATS_H
#define SWEEP_ENSEMBLESTATS_H

#include "swp_params.h"
#include "swp_model_stats.h"
#include "swp_particle_stats.h"
#include "swp_ensemble.h"
#include "swp_particle_model.h"
#include <vector>
#include <string>
#include <iostream>

namespace Sweep
{
class Mechanism;
namespace Stats
{
class EnsembleStats
{
public:
    // Constructors.
    EnsembleStats( // Default constructor.
        const Sweep::ParticleModel &model // Model used to define stats.
        );
    EnsembleStats(const EnsembleStats &copy); // Copy constructor.

    // Destructor.
    ~EnsembleStats(void);

    // Operators.
    EnsembleStats &operator=(const EnsembleStats &rhs);


    // IMPLEMENTATION.

    // Returns the number of stats for this model.
    unsigned int Count(void) const;

    // Calculates the model stats for a particle ensemble.
    void Calculate(
        const Ensemble &e, // Ensemble from which to get stats.
        real scale = 1.0   // Scaling factor to unit volume (summed stats).
        );

    // Returns a vector containing the stats.
    const fvector &Get(void) const;

    // Returns a vector containing the stats.
    void Get(
        fvector &stats,        // Output vector.
        unsigned int start = 0 // Optional start index for the first stat.
        ) const;

    // Returns a vector containing the stat names.
    const std::vector<std::string> &Names(void) const;

    // Adds to a vector containing stat names.
    void Names(
        std::vector<std::string> &names, // Output vector.
        unsigned int start = 0           // Optional start index for the first stat.
        ) const;


    // SUB-STATS INTERFACE.

    // Returns the basic stats object.
    const ParticleStats &BasicStats(void) const;


    // PARTICLE SIZE LISTS.

    // Returns the number of PSL output variables.
    unsigned int PSL_Count(void) const;

    // Returns a vector of PSL variable names.
    void PSL_Names(
        std::vector<std::string> &names, // Output vector.
        unsigned int start = 0           // Optional start index in vector.
        ) const;

    // Returns the particle size list (PSL) entry for particle i
    // in the given ensemble.
    void PSL(
        const Ensemble &ens, // Ensemble from which to get properties.
        unsigned int i,      // Index of particle in ensemble to get.
        real time,           // Current time.
        fvector &psl,        // Output vector.
        real scale = 1.0     // Scaling factor to unit volume.
        ) const;

    // Returns the PSL entry for the given particle.  The particle weight
    // is set to 1.0 because there is only one particle.
    void PSL(
        const Sweep::ParticleCache &sp, // Particle cache from which to get PSL data.
        real time,                      // Current flow time (used to calculate particle age).
        fvector &psl                    // Output vector.
        );

    // READ/WRITE/COPY.

    // Writes the object to a binary stream.
    void Serialize(std::ostream &out) const;

    // Reads the object from a binary stream.
    void Deserialize(
        std::istream &in,                 // Input stream.
        const Sweep::ParticleModel &model // Defining particle model.
        );

private:
    // Basic particle stats (from the ParticleData object).
    ParticleStats *m_basicstats;

    // Aggregation model stats.
    IModelStats *m_aggstats;

    // Map of model stats which are required to be collected.
    ModelStatsMap m_modelstats;

    // Cannot instantiate this class without knowing how many 
    // components and tracker variables there are for
    // each particle.
    EnsembleStats(void); // Default constructor.

    // Clears all memory associated with the EnsembleStats object.
    void releaseMem(void);
};
};
};

#endif
