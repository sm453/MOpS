/*
  Author(s):      Matthew Celnik (msc37)
  Project:        sweep (population balance solver)

  File purpose:
    The PriPartStats class is a specialization of the IModelStats
    interface which produces stats from the basic properties of the
    simple primary-particle aggregation model.

    The stats stored in this class are:

    1/2.    Total and average primary particle count.
    3.      Average primary particle diameter.
    4/5.    Total and average equiv. sphere surface area.
    6/7.    Total and average primary particle surface area.
*/

#ifndef SWEEP_PRIPART_STATS_H
#define SWEEP_PRIPART_STATS_H

#include "swp_params.h"
#include "swp_model_stats.h"
#include "swp_ensemble.h"
#include "swp_particle_cache.h"
#include "swp_particle_model.h"
#include "swp_submodel_type.h"
#include <vector>
#include <string>
#include <iostream>

namespace Sweep
{
namespace Stats
{
class PriPartStats : public Sweep::IModelStats
{
public:
    // Constructors.
    PriPartStats(void);  // Default constructor.
    PriPartStats(const PriPartStats &copy); // Copy constructor.
    PriPartStats(                         // Stream-reading constructor.
        std::istream &in,                 // Input stream.
        const Sweep::ParticleModel &model // Defining particle model.
        );

    // Destructor.
    ~PriPartStats(void);

    // Operators.
    PriPartStats &operator=(const PriPartStats &rhs);


    // IMPLEMENTATION.

    // Returns the number of stats for this model.
    unsigned int Count(void) const;

    // Calculates the model stats for a single particle.
    void Calculate(const ParticleCache &data);

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


    // AVAILABLE BASIC STATS.

    // Returns the total estimated primary particle count.
    real PriPartCount(void) const;

    // Returns the average estimated primary particle count.
    real AvgPriPartCount(void) const;

    // Returns the average estimated primary particle diameter.
    real AvgPriPartDiameter(void) const;

    // Returns the total equivalent-sphere surface area.
    real SphSurfaceArea(void) const;

    // Returns the avg. equivalent-sphere surface area.
    real AvgSphSurfaceArea(void) const;

    // Returns the total primary-particle surface-area.
    real PriSurfaceArea(void) const;

    // Returns the average primary-particle surface-area.
    real AvgPriSurfaceArea(void) const;


    // PARTICLE SIZE LISTS.

    // Returns the number of PSL output variables.
    unsigned int PSL_Count(void) const;

    // Returns a vector of PSL variable names.
    void PSL_Names(
        std::vector<std::string> &names, // Vector in which to return names.
        unsigned int start = 0 // Optional start index for the first name.
        ) const;

    // Returns the particle size list (PSL) entry for particle i
    // in the given ensemble.
    void PSL(
        const Ensemble &ens,   // Ensemble from which to get properties.
        unsigned int i,        // Index of particle in ensemble to get.
        real time,             // The current time.
        fvector &psl,          // Output vector.
        unsigned int start = 0 // Optional start index for the first variable.
        ) const;

    // Returns the PSL entry for the given particle cache.
    void PSL(
        const Sweep::ParticleCache &sp, // Particle cache from which to get PSL data.
        real time,                      // Current flow time (used to calculate particle age).
        fvector &psl,                   // Output vector.
        unsigned int start = 0          // Optional start index for the first variable.
        ) const;


    // PRIMARY-PARTICLE SIZE LIST.

    // Returns the number of PPSL output variables.
    unsigned int PPSL_Count(void) const;

    // Returns a vector of PPSL (primary-particle size list) variable names.
    void PPSL_Names(
        std::vector<std::string> &names, // Vector in which to return names.
        unsigned int start = 0 // Optional start index for the first name.
        ) const;

    // Returns the primary-particle size list (PPSL) entry for particle
    // i in the given ensemble.
    void PPSL(
        const Ensemble &ens,        // Ensemble from which to get properties.
        unsigned int i,             // Index of particle in ensemble to get.
        real time,                  // The current time.
        std::vector<fvector> &ppsl, // Output vector.
        unsigned int start = 0      // Optional start index for the first variable.
        ) const;


    // READ/WRITE/COPY.

    // Creates a copy of the object.
    PriPartStats *const Clone(void) const;

    // Returns the model data type.  Used to identify different models
    // and for serialisation.
    unsigned int ID(void) const;

    // Writes the object to a binary stream.
    void Serialize(std::ostream &out) const;

    // Reads the object from a binary stream.
    void Deserialize(
        std::istream &in,                 // Input stream.
        const Sweep::ParticleModel &model // Defining particle model.
        );

private:
    // Stats count and indices.
    static const unsigned int STAT_COUNT = 7;
    enum StatIndices {iPPN=0, iPPD=2, iS=3, iPPS=5};

    // PSL count and indices.
    static const unsigned int PSL_COUNT  = 4;
    static const unsigned int PPSL_COUNT = 5;

    // The stats.
    fvector m_stats;

    // The stat names.
    static const std::string m_statnames[STAT_COUNT];
    std::vector<std::string> m_names;

    // The stat mask.
    static const IModelStats::StatType m_mask[STAT_COUNT];

    // The PSL names.
    static const std::string m_const_pslnames[PSL_COUNT];
    std::vector<std::string> m_pslnames;

    // The PPSL names.
    static const std::string m_const_ppslnames[PPSL_COUNT];
    std::vector<std::string> m_ppslnames;
};
};
};

#endif
