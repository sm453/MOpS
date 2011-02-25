/*
  Author(s):      Matthew Celnik (msc37)
  Project:        sweepc (population balance solver)
  Sourceforge:    http://sourceforge.net/projects/mopssuite

  Copyright (C) 2008 Matthew S Celnik.

  File purpose:
    Implementation of the Cell class declared in the
    swp_cell.h header file.

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

#include "swp_cell.h"
#include "swp_particle_model.h"
#include "swp_birth_process.h"
#include "swp_death_process.h"
#include <stdexcept>

using namespace Sweep;
using namespace std;

// CONSTRUCTORS AND DESTRUCTORS.

// Default constructor (private).
Cell::Cell(void)
: m_model(NULL), m_smpvol(1.0), m_fixed_chem(false)
{
}

// Default constructor (public).
Cell::Cell(const Sweep::ParticleModel &model)
: Sprog::Thermo::IdealGas(*model.Species()), m_ensemble(), m_model(&model),
  m_smpvol(1.0), m_fixed_chem(false)
{
}

// Copy constructor.
Cell::Cell(const Cell &copy)
{
    *this = copy;
}

// Stream-reading constructor.
Cell::Cell(std::istream &in, const Sweep::ParticleModel &model)
{
    Deserialize(in, model);
}

// Default destructor.
Cell::~Cell(void)
{
}


// OPERATOR OVERLOADS.

// Assignment operator.
Cell &Cell::operator=(const Sweep::Cell &rhs)
{
    if (this != &rhs) {
        Sprog::Thermo::IdealGas::operator=(rhs);
        m_ensemble   = rhs.m_ensemble;
        m_model      = rhs.m_model;
        m_smpvol     = rhs.m_smpvol;
        m_fixed_chem = rhs.m_fixed_chem;
        m_secondaryVol = rhs.m_secondaryVol;

    }
    return *this;
}

// Assignment operator - ideal gas.
Cell &Cell::operator=(const Sprog::Thermo::IdealGas &rhs)
{
    if (this != &rhs) {
        Sprog::Thermo::IdealGas::operator=(rhs);
    }
    return *this;
}

// THE GAS-PHASE INTERFACE.

// Returns the description of the gas-phase mixture.
const Sprog::Thermo::IdealGas &Cell::GasPhase(void) const
{
    return *this;
}

// Sets the gas-phase mixture.
void Cell::SetGasPhase(const Sprog::Thermo::IdealGas &gas)
{
    Sprog::Thermo::IdealGas::operator=(gas);
}

// Adjusts the concentration of the ith species.
void Cell::AdjustConc(unsigned int i, real dc)
{
   if (!m_fixed_chem) {
       unsigned int k;

        // Precalculate DC / density.
        real dc_rho = dc / Density();

        // New concentrations will be calculated by adjusting a copy of the existing data
        fvector newConcs = MoleFractions();

        // Calculate change to all mole fractions k < i.
        for (k=0; k<i; ++k) {
            newConcs[k] -= dc_rho * newConcs[k];
        }

        // Calculate change for ith species.
        newConcs[i] += dc_rho * (1.0 - newConcs[i]);

        // Calculate change for all mole fractions k > i.
        for (k=i+1; k < Species()->size(); ++k) {
            newConcs[k] -= dc_rho * newConcs[k];
        }

        // Set the new data
        SetFracs(newConcs);
    }
}

// Adjusts the concentration of all species.
void Cell::AdjustConcs(const fvector &dc)
{
    if (!m_fixed_chem) {
        // Calculate total change in density.
        real drho = 0.0;
        unsigned int k;
        for (k=0; k!=Species()->size(); ++k) {
            drho += dc[k];
        }

        // New concentrations will be calculated by adjusting a copy of the existing data
        fvector newConcs = MoleFractions();

        real xtot=0.;
        // Calculate changes to the mole fractions.
        const real invrho = 1.0 / Density();
        for (k=0; k!=Species()->size(); ++k) {
            newConcs[k] += (invrho * dc[k]) - (invrho * newConcs[k] * drho);
            if (newConcs[k]<0.) newConcs[k]=0;
                xtot+=newConcs[k];
        }

        if (xtot != 1.0) {
            for (unsigned int i=0; i!=Species()->size(); ++i) {
                newConcs[i] /= xtot;
            }
        }

        // Set the new data
        SetFracs(newConcs);
    }
}


// THE PARTICLE ENSEMBLE.

// Returns the particle ensemble.
Ensemble &Cell::Particles(void) {return m_ensemble;}
const Ensemble &Cell::Particles(void) const {return m_ensemble;}

// Returns the particle count.
unsigned int Cell::ParticleCount(void) const
{
    return m_ensemble.Count();
}

/*!
 *@param[in,out]    sp              Pointer to particle to add to ensemble.
 *@param[in]        stat_weight     Concentration of physical particles to be represented by added computational particles
 *@param[in,out]    rand_int        Pointer to function that generates uniform integers on a range
 *@param[in,out]    rand_u01        Pointer to function that generates U[0,1] deviates
 *
 * Ownership of the particle is taken by the ensemble, which will delete it when it is no
 * longer required (which may be immediately).
 *
 * Exactly matching the caller specified statistical weight is only possible if it is an integer multiple
 * of the statistical weight of one ensemble particle.  In general this condition will not be met and so
 * probabilistic methods are used to add computational particles with the mean of their combined
 * statistical weight equal to the value specified by the caller.
 */
void Cell::AddParticle(Particle* sp, real stat_weight,
                       int (*rand_int)(int, int), real(*rand_u01)()) {
    // Need to match caller specified weight with total weight of particles added to the ensemble
    unsigned int safetyCounter = 0;
    while(true) {
        real destinationWeight = 1.0 / SampleVolume();

        if(stat_weight >= destinationWeight) {
            // Insert one copy of the particle into the destination cell
            m_ensemble.Add(*(new Particle(*sp)), rand_int);
            stat_weight -= destinationWeight;

            // Avoid infinite loops
            if(++safetyCounter > 100000) {
                throw std::runtime_error("Failed to match particle weights in Sweep::Cell::AddParticle()");
            }
        }
        else
            break;
    }

     // Unfortunately we cannot quite conserve statistical weight, there will always be a bit
     // left over after the loop above.  This can only be handled in an average sense.
     if(rand_u01() < stat_weight * SampleVolume()) {
         m_ensemble.Add(*sp, rand_int);
     }
     else {
         // Ownership of the particle has not been passed on to an ensemble so the memory must be released
         delete sp;
     }
}

// Returns particle statistics.
void Cell::GetVitalStats(Stats::EnsembleStats &stats) const
{
    stats.Calculate(m_ensemble, 1.0/SampleVolume(), 1.0/SecondarySampleVolume());
}


// SCALING ROUTINES INCL. SAMPLE VOLUME.

// Returns the real system to stochastic system scaling factor.
real Cell::SampleVolume() const
{
    return m_smpvol * m_ensemble.Scaling();
}

//! Physical volume that would contain same number of secondary particles as the ensemble
real Cell::SecondarySampleVolume() const
{
    return m_secondaryVol * m_ensemble.SecondaryScaling();
}

/*!
 * Set the number density which the ensemble represents including the
 * effects of ensemble doublings and contractions.
 *
 *@param[in]    scale_factor    Ratio of new to old sample volumes
 *
 *@pre      scale_factor > 0
 */
void Cell::AdjustSampleVolume(real scale_factor)
{
    assert(scale_factor > 0);
    m_smpvol = SampleVolume() * scale_factor;
    m_secondaryVol = SecondarySampleVolume() * scale_factor;

    // The effects of ensemble rescalings are now incorporated in this sample
    // volume.
    m_ensemble.ResetScaling();
}

/**
 * Clear any particles and set the sample volume so that a full ensemble
 * (of m_ensemble.Capacity() particles) has the specified m0.
 *
 *@param[in]    m0              Particle number density for full ensemble (units \f$\mathrm{m}^{-3}\f$)
 *@param[in]    secondary_m0    Secondary particle number density for m_ensemble.Capacity() secondary particles (units \f$\mathrm{m}^{-3}\f$)
 */
void Cell::Reset(const real m0, const real secondary_m0)
{
    m_ensemble.Clear();
    m_ensemble.ResetScaling();

    if ((m_ensemble.Capacity() > 0) && (m0 > 0.0)) {
        m_smpvol = m_ensemble.Capacity() / m0;
        m_secondaryVol = m_ensemble.Capacity() / secondary_m0;
    }
    else {
        // The ensemble has not yet been initialised
        m_smpvol = 1.0;
        m_secondaryVol = 1.0;
    }

}


// FIXED/VARIABLE CHEMISTRY.

// Returns whether or not the chemical conditions are fixed.
bool Cell::FixedChem() const {return m_fixed_chem;}

// Sets whether or not the chemical conditions are fixed.
void Cell::SetFixedChem(bool fixed) {m_fixed_chem = fixed;}

// Set the chemical conditions to be variable.
void Cell::SetVariableChem(bool vari) {m_fixed_chem = !vari;}


// PARTICLE INFLOW PROCESSES.

// Returns the number of inflow processes defined
// for this Cell.
unsigned int Cell::InflowCount(void) const {return m_inflow.size();}

// Returns the vector of particle inflow processes.
const Processes::BirthPtrVector &Cell::Inflows(void) const {return m_inflow;}

// Returns the ith particle inflow process.
Processes::BirthProcess *const Cell::Inflows(unsigned int i) const
{
    if (i < m_inflow.size()) {
        return m_inflow[i];
    } else {
        return NULL;
    }
}

// Add an inflow process to the Cell. The process is copied.
void Cell::AddInflow(Processes::BirthProcess &inf)
{
    m_inflow.push_back(inf.Clone());
}


// PARTICLE OUTFLOW PROCESSES.

// Returns the number of outflow processes defined
// for this Cell.
unsigned int Cell::OutflowCount(void) const {return m_outflow.size();}

// Returns the vector of particle outflow processes.
const Processes::DeathPtrVector &Cell::Outflows(void) const {return m_outflow;}

// Returns the ith particle outflow process.
Processes::DeathProcess *const Cell::Outflows(unsigned int i) const
{
    if (i < m_outflow.size()) {
        return m_outflow[i];
    } else {
        return NULL;
    }
}


// Add an outflow process to the Cell. The process is copied.
void Cell::AddOutflow(Processes::DeathProcess &out)
{
    m_outflow.push_back(out.Clone());
}

// Add an outflow process with the given rate to the Cell.
void Cell::AddOutflow(real rate, const Sweep::Mechanism &mech)
{
    Processes::DeathProcess *death = new Processes::DeathProcess(mech);
    death->SetA(rate);
    m_outflow.push_back(death);
}


// READ/WRITE/COPY.

// Writes the object to a binary stream.
void Cell::Serialize(std::ostream &out) const
{
    if (out.good()) {
        // Output the version ID (=0 at the moment).
        const unsigned int version = 0;
        out.write((char*)&version, sizeof(version));

        // Output the base class.
        Sprog::Thermo::IdealGas::Serialize(out);

        // Output the sample volume.
        double v = (double)m_smpvol;
        out.write((char*)&v, sizeof(v));

        out.write(reinterpret_cast<const char*>(&m_secondaryVol), sizeof(m_secondaryVol));

        // Output if fixed chem.
        out.write((char*)&m_fixed_chem, sizeof(m_fixed_chem));

        // Output the ensemble.
        m_ensemble.Serialize(out);
    } else {
        throw invalid_argument("Output stream not ready "
                               "(Sweep, Cell::Serialize).");
    }
}

// Reads the object from a binary stream.
void Cell::Deserialize(std::istream &in, const Sweep::ParticleModel &model)
{
    if (in.good()) {
        // Read the output version.  Currently there is only one
        // output version, so we don't do anything with this variable.
        // Still needs to be read though.
        unsigned int version = 0;
        in.read(reinterpret_cast<char*>(&version), sizeof(version));

        double val = 0.0;

        switch (version) {
            case 0:
                // Read the base class.
                Sprog::Thermo::IdealGas::Deserialize(in);

                // Read the sample volume.
                in.read(reinterpret_cast<char*>(&val), sizeof(val));
                m_smpvol = (real)val;

                in.read(reinterpret_cast<char*>(&m_secondaryVol), sizeof(m_secondaryVol));

                // Read if fixed chem.
                in.read(reinterpret_cast<char*>(&m_fixed_chem), sizeof(m_fixed_chem));

                // Read the ensemble.
                m_ensemble.Deserialize(in, model);

                // Set the species.
                SetSpecies(*model.Species());

                break;
            default:
                throw runtime_error("Serialized version number is invalid "
                                    "(Sweep, Cell::Deserialize).");
        }
    } else {
        throw invalid_argument("Input stream not ready "
                               "(Sweep, Cell::Deserialize).");
    }
}