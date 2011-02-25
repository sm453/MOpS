/*
  Author(s):      Matthew Celnik (msc37)
  Project:        sweep (population balance solver)
  Sourceforge:    http://sourceforge.net/projects/mopssuite

  Copyright (C) 2008 Matthew S Celnik.

  File purpose:
    A specialised ideal gas class for Sweep, which contains an ensemble
    with particles of type Particle.  The Cell class inherits the
    Sprog::IdealGas class and adds an additional layer of data to it
    to describe the particles within the cell.

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

#ifndef SWEEP_CELL_H
#define SWEEP_CELL_H

#include "swp_params.h"
#include "swp_particle_model.h"
#include "swp_ensemble.h"
#include "swp_ensemble_stats.h"
#include "swp_birth_process.h"
#include "swp_death_process.h"
#include "sprog.h"
#include <string>
#include <iostream>

namespace Sweep
{
// Forward declare Mechanism class.
class Mechanism;

class Cell : public Sprog::Thermo::IdealGas
{
public:
    // Constructors.
    Cell(const Sweep::ParticleModel &model); // Default constructor.
    Cell(const Cell &copy);                  // Copy constructor.
    Cell(                                 // Stream-reading constructor.
        std::istream &in,                 //   - Stream from which to read.
        const Sweep::ParticleModel &model //   - Model used to define particles.
        );

    // Destructor.
    virtual ~Cell(void);

    // Operators.
    Cell &operator=(const Cell &rhs);
    Cell &operator=(const Sprog::Thermo::IdealGas &rhs);

    // THE GAS-PHASE INTERFACE.

    // Returns the description of the gas-phase mixture.
    const Sprog::Thermo::IdealGas &GasPhase(void) const;

    // Sets the gas-phase mixture.
    void SetGasPhase(const Sprog::Thermo::IdealGas &gas);

    // Adjusts the concentration of the ith species.
    void AdjustConc(unsigned int i, real dc);

    // Adjusts the concentration of all species.
    void AdjustConcs(const fvector &dc);


    // THE PARTICLE ENSEMBLE.

    // Returns the particle ensemble.
    Sweep::Ensemble &Particles(void);
    const Sweep::Ensemble &Particles(void) const;

    // Returns the number of particles in the ensemble.
    unsigned int ParticleCount(void) const;

    // Returns particle statistics.
    void GetVitalStats(Stats::EnsembleStats &stats) const;

    //! Initialise with some particles
    template<class ForwardIterator> void SetParticles(
            ForwardIterator particle_list_begin,
            ForwardIterator particle_list_end,
            real statistical_weight);

    //! Initialise secondary particles
    template<class ForwardIterator> void SetSecondaryParticles(
            ForwardIterator particle_list_begin,
            ForwardIterator particle_list_end,
            real statistical_weight);

    //! Add particles to the ensemble with total statistical weight as specified
    void AddParticle(Particle* sp, real stat_weight,
                     int (*rand_int)(int, int), real(*rand_u01)());

    // THE PARTICLE MODEL.

    // Returns the particle model used to define particles in this
    // cell.
    const Sweep::ParticleModel *const ParticleModel(void) const {return m_model;}


    // SCALING ROUTINES INCL. SAMPLE VOLUME.

    //! Returns the real system to stochastic system scaling factor.
    real SampleVolume() const;

    //! Statistical weight of secondary particles in the member ensemble
    real SecondarySampleVolume() const;

    //! Multiply the sample volume by a scaling factor
    void AdjustSampleVolume(real scale_factor);

    //! Empty the cell and set the sample volume so a full particle ensemble would have the specified m0
    void Reset(real m0, real secondary_m0 = 1.0);

    // FIXED/VARIABLE CHEMISTRY.

    // Returns whether or not the chemical conditions are fixed.
    bool FixedChem() const;

    // Sets whether or not the chemical conditions are fixed.
    void SetFixedChem(bool fixed = true);

    // Set the chemical conditions to be variable.
    void SetVariableChem(bool vari = true);


    // PARTICLE INFLOW PROCESSES.

    // Returns the number of inflow processes defined
    // for this Cell.
    unsigned int InflowCount(void) const;

    // Returns the vector of particle inflow processes.
    const Processes::BirthPtrVector &Inflows(void) const;

    // Returns the ith particle inflow process.
    Processes::BirthProcess *const Inflows(unsigned int i) const;

    // Add an inflow process to the Cell. The process is copied.
    void AddInflow(Processes::BirthProcess &inf);


    // PARTICLE OUTFLOW PROCESSES.

    // Returns the number of outflow processes defined
    // for this Cell.
    unsigned int OutflowCount(void) const;

    // Returns the vector of particle outflow processes.
    const Processes::DeathPtrVector &Outflows(void) const;

    // Returns the ith particle outflow process.
    Processes::DeathProcess *const Outflows(unsigned int i) const;

    // Add an outflow process to the Cell. The process is copied.
    void AddOutflow(Processes::DeathProcess &out);

    // Add an outflow process with the given rate to the Cell.
    void AddOutflow(
        real rate, // Rate constant for outflow.
        const Sweep::Mechanism &mech // Mechanism which defines LPDA for outflow.
        );

    // READ/WRITE/COPY.

    // Writes the object to a binary stream.
    virtual void Serialize(std::ostream &out) const;

    // Reads the object from a binary stream.
    virtual void Deserialize(
        std::istream &in,                 // Input stream.
        const Sweep::ParticleModel &model // Model used to define particles.
        );

protected:
    // Default constructor is protected as it makes no
    // sense to define a mixture without knowledge of the
    // defining species.  This trait is brought over from Sprog.
    Cell();

private:
    // Particle ensemble.
    Sweep::Ensemble m_ensemble;

    // Particle model.
    const Sweep::ParticleModel *m_model;

    // The volume in which the ensemble represents
    // the complete real system.
    real m_smpvol;

    //! Physical volume that would initially contain same number of secondary particles as the ensemble
    real m_secondaryVol;

    // Flag determining whether or not the chemistry in this system is fixed.
    // If the chemical conditions are fixed, then they cannot be altered by
    // any particle processes.  Default is false.
    bool m_fixed_chem;

    // Particle inflow processes.  These are not stored in the
    // mechanism, but are used by the Mechanism class when
    // calculating rates.
    Processes::BirthPtrVector m_inflow;

    // Particle outflow processes.  These are not stored in the
    // mechanism, but are used by the Mechanism class when
    // calculating rates.
    Processes::DeathPtrVector m_outflow;
};

/**
 * Initialise the ensemble to hold particles of the type specified
 * by the model and containing the particular particles contained
 * in the range [particle_list_begin, particle_list_end) and set
 * the sample volume to achieve the statistical weight.
 *
 *@tparam   ForwardIterator     A model of a forward iterator
 *
 *@param[in]    particle_list_begin     Iterator to first in range of particle pointers to insert
 *@param[in]    particle_list_end       Iterator to one past end of range of particle pointers to insert
 *@param[in]    statistical_weight      Number of physical particles represented by each computational particle
 */
template<class ForwardIterator> void Cell::SetParticles(
        ForwardIterator particle_list_begin,
        ForwardIterator particle_list_end,
        real statistical_weight)
{
    // This puts the particles into the ensemble and clears any scaling
    // stored inside the ensemble
    m_ensemble.SetParticles(particle_list_begin, particle_list_end);

    m_smpvol = 1.0 / statistical_weight;
}

/**
 * Initialise the secondary population to hold particles of the type specified
 * by the model and containing the particular particles contained
 * in the range [particle_list_begin, particle_list_end).
 *
 *@tparam   ForwardIterator     A model of a forward iterator
 *
 *@param[in]    particle_list_begin     Iterator to first in range of particle pointers to insert
 *@param[in]    particle_list_end       Iterator to one past end of range of particle pointers to insert
 *@param[in]    statistical_weight      Number of physical particles represented by each computational particle
 */
template<class ForwardIterator> void Cell::SetSecondaryParticles(
        ForwardIterator particle_list_begin,
        ForwardIterator particle_list_end,
        real statistical_weight)
{
    // This puts the particles into the ensemble and clears any scaling
    // stored inside the ensemble
    m_ensemble.SetSecondaryParticles(particle_list_begin, particle_list_end);

    m_secondaryVol = 1.0 / statistical_weight;
}

} //namespace Sweep

#endif