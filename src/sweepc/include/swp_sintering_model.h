/*
  Author(s):      Matthew Celnik (msc37) and Markus Sander (ms785)
  Project:        sweep (population balance solver)
  Sourceforge:    http://sourceforge.net/projects/mopssuite
  
  Copyright (C) 2008 Matthew S Celnik.

  File purpose:
    The SinteringModel class defines the sintering model used by sweep.
    It assumes a basic expression describing the reduction in surface-area
    towards the spherical limit:

    dA/dt = (A - Asph) / tau

    where tau is the characteristic sintering time and is a function of
    particle temperature and primary particle size.

    Currently two physical models are programmed which determine the
    characteristic sintering time: viscous flow and grain-boundary diffusion.
    Viscous flow is suitable for silica, and GBD is suitable for titania.  They
    have the following functional forms:

    tau_vf  = K *     dp   * exp(E/RT)
    tau_gbd = K * T * dp^4 * exp(E/RT)

    where the parameters K and E are material and model dependent.

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

#ifndef SWEEP_SINTERING_H
#define SWEEP_SINTERING_H

#include "swp_params.h"
#include <iostream>

namespace Sweep
{
// Forward declare required classes.
class Cell;
class Particle;
class Primary;
class SubParticle;

namespace AggModels {
class PriPartPrimary;
};

namespace Processes
{
/*! Model for rate at which primary particles merge with their
 *  neighbours.
 */
class SinteringModel
{
public:
    // Sintering model types.
    enum SintType {
        ViscousFlow, // Viscous flow model (e.g. silica).
        GBD          // Grain-boundary diffusion (e.g. titania).
    };

    // Constructors.
    SinteringModel( ); // Default constructor.
    SinteringModel(const SinteringModel &copy); // Copy-constructor.
    SinteringModel(      // Stream-reading constructor.
        std::istream &in //  - Input stream.
        );        

    // Destructor.
    ~SinteringModel(void);

    // Operators.
    SinteringModel &operator=(const SinteringModel &rhs);


    // MODEL ENABLE/DISABLE.

    // Returns true is the model is enabled, otherwise false.
    bool IsEnabled(void) const;

    // Enables this sintering model.
    void Enable(void);

    // Disables this sintering model.
    void Disable(void);


    // PRE-EXPONENTIAL CONSTANT.

    // Returns the pre-exponential constant.
    real A(void) const;

    // Sets the pre-exponential constant.
    void SetA(real a);


    // ENERGY PARAMETER.

    // Returns the characteristic temperature (E) in Kelvin.
    real E(void) const;

    // Sets the characteristic temperature (E) in Kelvin.
    void SetE(real e);

    // MINIMUM PRIMARY PARTICLE DIAMETER PARAMETER.

    // Returns the minimum primary particle diameter in the system (Dpmin) in m.
    real Dpmin(void) const;

    // Sets the minimum primary particle diameter in the system (Dpmin) in m.
    void SetDpmin(real dpmin);

    // SINTERING MODEL TYPE.

    // Returns the sintering model type.
    SintType Type(void) const;

    // Sets the sintering model type.
    void SetType(SintType t);


    // CHARACTERISTIC SINTERING TIME.

    // Returns the characteristic sintering time for the
    // given particle.
    real SintTime(
        const Cell &sys,  // System to which the particle belongs (for T).
        const Particle &p // Particle for which to calculate time.
        ) const;

    // Returns the characteristic sintering time for the
    // given primary.
    real SintTime(
        const Cell &sys, // System to which the particle belongs (for T).
        const Primary &p // Particle for which to calculate time.
        ) const;

    // RATE CALCULATION.

    // Returns the rate of the process for the given particle.
    real Rate(
        real t,           // Time.
        const Cell &sys,  // System to which the particle belongs (for T).
        const Particle &p // Particle for which to calculate rate.
        ) const;

    // Returns the rate of the process for the given primary.
    real Rate(
        real t,          // Time.
        const Cell &sys, // System to which the particle belongs (for T).
        const Primary &p // Particle for which to calculate rate.
        ) const;

    // PERFORMING THE SINTERING.

    // Performs the process on the given particle. Must return 0
    // on success, otherwise negative.
    int Perform(
        real dt,    // Delta-Time.
        Cell &sys,  // System containing particle.
        Particle &p // Particle on which to perform sintering.
        ) const;

    // Performs the process on the given primary. Must return 0
    // on success, otherwise negative.
    int Perform(
        real dt,   // Delta-Time.
        Cell &sys, // System containing particle.
        Primary &p // Particle on which to perform sintering.
        ) const;


    // READ/WRITE/COPY.

    // Creates a copy of the sintering model.
    SinteringModel *const Clone(void) const;

    // Writes the object to a binary stream.
    void Serialize(std::ostream &out) const;

    // Reads the object from a binary stream.
    void Deserialize(
        std::istream &in // Input stream.
        );

private:
    // Model on/off flag.
    bool m_enable;

    // Pre-exponential rate constant.
    real m_A;

    // Characteristic temperature (energy) in Kelvin.
    real m_E;

    // Minimum primary particle diameter in the system (Dpmin) in m.
    real m_dpmin;

    // Sintering model type.
    SintType m_type;
};
};
};

#endif