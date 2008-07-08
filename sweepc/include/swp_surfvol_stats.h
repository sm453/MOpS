/*
  Author(s):      Matthew Celnik (msc37)
  Project:        sweep (population balance solver)
  Sourceforge:    http://sourceforge.net/projects/mopssuite
  
  Copyright (C) 2008 Matthew S Celnik.

  File purpose:
    The SurfVolStats class is a specialization of the IModelStats
    interface which produces stats from the basic properties of the
    surface-volume model.

    The stats stored in this class are:

    1/2.    Total and average equiv. sphere surface area.
    3/4.    Total and average primary particle count (estimated).
    5.      Average primary particle diameter (estimated).

  Licence:
    This file is part of "sweepc".

    sweepc is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

  Contact:
    Dr Markus Kraft
    Dept of Chemical Engineering
    University of Cambridge
    New Museums Site
    Pembroke Street
    Cambridge
    CB2 3RA
    UK

    Email:       mk306@cam.ac.uk
    Website:     http://como.cheng.cam.ac.uk
*/

#ifndef SWEEP_SURFVOL_STATS_H
#define SWEEP_SURFVOL_STATS_H

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
class SurfVolStats : public IModelStats
{
public:
    // Constructors.
    SurfVolStats(void);  // Default constructor.
    SurfVolStats(const SurfVolStats &copy); // Copy constructor.
    SurfVolStats(                         // Stream-reading constructor.
        std::istream &in,                 // Input stream.
        const Sweep::ParticleModel &model // Defining particle model.
        );

    // Destructor.
    ~SurfVolStats(void);

    // Operators.
    SurfVolStats &operator=(const SurfVolStats &rhs);


    // IMPLEMENTATION.

    // Returns the number of stats for this model.
    unsigned int Count(void) const;

    // Returns the stats mask which informs whether stats should
    // be summed, averaged or both.
//    const std::vector<StatType> &Mask(void) const;

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


    // Returns the total equivalent-sphere surface area.
    real SphSurfaceArea(void) const;

    // Returns the avg. equivalent-sphere surface area.
    real AvgSphSurfaceArea(void) const;

    // Returns the total estimated primary particle count.
    real PriPartCount(void) const;

    // Returns the average estimated primary particle count.
    real AvgPriPartCount(void) const;

    // Returns the average estimated primary particle diameter.
    real AvgPriPartDiameter(void) const;


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


    // READ/WRITE/COPY.

    // Creates a copy of the object.
    SurfVolStats *const Clone(void) const;

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
    static const unsigned int STAT_COUNT = 5;
    enum StatIndices {iS=0, iPPN=2, iPPD=4};

    // PSL count and indices.
    static const unsigned int PSL_COUNT = 3;

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
};
};
};

#endif
