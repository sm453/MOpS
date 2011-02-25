/*
  Author(s):      Matthew Celnik (msc37)
  Project:        sprog (gas-phase chemical kinetics).
  Sourceforge:    http://sourceforge.net/projects/mopssuite
  
  Copyright (C) 2008 Matthew S Celnik.

  File purpose:
    This files contains the definition of a structure for a set of chemical reactions, 
    including code to provide speed enhancements when working with many reactions.

  Licence:
    This file is part of "sprog".

    sprog is free software; you can redistribute it and/or
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

#ifndef GPC_REACTION_SET_H
#define GPC_REACTION_SET_H

#include "gpc_params.h"
#include "gpc_reaction.h"
#include "gpc_gasphase.h"
#include "gpc_mixture.h"
#include <vector>
#include <map>
#include <iostream>

namespace Sprog
{
class Mechanism; // Forward declare Mechanism class.

namespace Kinetics
{
//! Calculates reaction data
class ReactionSet
{
public:
    // Constructors.
    ReactionSet(void);                   // Default constructor.
    ReactionSet(const ReactionSet &rxn); // Copy constructor.
    ReactionSet(std::istream &in);       // Stream-reading constructor.

    // Destructor.
    ~ReactionSet(void);

    // Operator overloads.
    ReactionSet &operator=(const ReactionSet &rxns);
    ReactionSet &operator+=(const ReactionSet &rxns);
    const ReactionSet operator+(const ReactionSet &rxns) const;
    Reaction *const operator[](unsigned int i);
    const Reaction *const operator[](unsigned int i) const;
   

    // REACTIONS.

    // Returns the number of reactions in the set.
    unsigned int Count(void) const;

    // Returns the list of reactions.
    const RxnPtrVector &Reactions(void) const;

    // Returns a pointer to the ith reaction.  Returns NULL if i is invalid.
    const Reaction *const Reactions(unsigned int i) const;

    // Adds a reaction to the set.
    Reaction *const AddReaction(const Reaction &rxn);


    // TIDYING UP.

    // Clears all reactions from the set.
    void Clear(void);


    // SPECIES MOLAR PRODUCTION RATES.

    // Calculates the molar production rates of all species.  Returns
    // the total molar production rate.
    real GetMolarProdRates(
        const fvector &rop, // Rate of Progress of each reaction.
        fvector &wdot       // Return vector for molar prod. rates.
        ) const;

    // Calculates the molar production rates of all species.  Returns
    // the total molar production rate.
    real GetMolarProdRates(
        const Sprog::Thermo::GasPhase &gas, // The gas for which to calculate the rates.
        fvector &wdot                       // Return vector for molar prod. rates.
        ) const;

    // Calculates the molar production rates of all species.  Returns
    // the total molar production rate.
    real GetMolarProdRates(
        real T,              // The mixture temperature.
        real density,        // Mixture molar density.
        const real *const x, // Species mole fractions.
        unsigned int n,      // Number of values in x array.
        const Sprog::Thermo::ThermoInterface &thermo, // Thermodynamics interface.
        fvector &wdot        // Return vector for molar prod. rates.
        ) const;

	void GetMolarProdRates(Sprog::Thermo::Mixture &mix, fvector &wdot) const;



    // REACTION RATES OF PROGRESS.

    // Calculates the rate of progress of each reaction.
    void GetRatesOfProgress(
        const Sprog::Thermo::GasPhase &gas, // The gas for which to calculate the rates.
        const fvector &kforward,  // Forward rate constants of all reactions.
        const fvector &kreverse,  // Reverse rate constants of all reactions.
        fvector & rop             // Return vector for rates of progress.
        ) const;



    // Calculates the rate of progress of each reaction.
    void GetRatesOfProgress(
        real density,            // Mixture molar density.
        const real *const x,     // Species mole fractions.
        unsigned int n,          // Number of values in x array.
        const fvector &kforward, // Forward rate constants of all reactions.
        const fvector &kreverse, // Reverse rate constants of all reactions.
        fvector &rop             // Return vector for rates of progress.
        ) const;

    // Calculates the rate of progress of each reaction.
    void GetRatesOfProgress(
        real density,            // Mixture molar density.
        const real *const x,     // Species mole fractions.
        unsigned int n,          // Number of values in x array.
        const fvector &kforward, // Forward rate constants of all reactions.
        const fvector &kreverse, // Reverse rate constants of all reactions.
        fvector &rop,            // Return vector for rates of progress.
        fvector &rfwd,           // Return vector for forward rates of all reactions
        fvector &rrev            // Return vector for reverse rates of all reactions
        ) const;


    // Calculates the rate of progress of each reaction.
    void GetRatesOfProgress(
        const Sprog::Thermo::GasPhase &gas, // The gas for which to calculate the rates.
        fvector & rop                       // Return vector for rates of progress.
        ) const;

    // Calculates the rate of progress of each reaction.
    void GetRatesOfProgress(
        const Sprog::Thermo::GasPhase &gas, // The gas for which to calculate the rates.
        fvector & rop,                      // Return vector for rates of progress.
        fvector &rfwd,                      // Return vector for forward rates of all reactions
        fvector &rrev                       // Return vector for reverse rates of all reactions
        ) const;

    // Calculates the rate of progress of each reaction.
    void GetRatesOfProgress(
        real T,              // The mixture temperature.
        real density,        // Mixture molar density.
        const real *const x, // Species mole fractions.
        unsigned int n,      // Number of values in x array.
        const Sprog::Thermo::ThermoInterface &thermo, // Thermodynamics interface.
        fvector &rop         // Return vector for rates of progress.
        ) const;

    // RATE CONSTANTS.

    // Calculates the forward and reverse rate constants 
    // of all reactions given a mixture object.
    void GetRateConstants(
        const Sprog::Thermo::GasPhase &mix, // The mixture for which to calculate the rate constants.
        const fvector &Gs, // Dimensionless Gibbs free energy of each species (1/mol).
        fvector &kforward, // Return vector for forward rate constants.
        fvector &kreverse  // Return vector for reverse rate constants.
        ) const;

    // Calculates the forward and reverse rate constants 
    // of all reactions given mixture temperature, density
    // and species mole fractions.
    void GetRateConstants( 
        real T,              // The mixture temperature.
        real density,        // Mixture molar density.
        const real *const x, // Species mole fractions.
        unsigned int n,      // Number of values in x array.
        const fvector &Gs,   // Dimensionless Gibbs free energy of each species (1/mol).
        fvector &kforward,   // Return vector for forward rate constants.
        fvector &kreverse    // Return vector for reverse rate constants.
        ) const;

    // Calculates the forward and reverse rate constants 
    // of all reactions given a mixture object.
    void GetRateConstants(
        const Sprog::Thermo::GasPhase &mix, // The mixture for which to calculate the rate constants.
        fvector &kforward, // Return vector for forward rate constants.
        fvector &kreverse  // Return vector for reverse rate constants.
        ) const;

    // Calculates the forward and reverse rate constants 
    // of all reactions given mixture temperature, density
    // and species mole fractions.
    void GetRateConstants( 
        real T,              // The mixture temperature.
        real density,        // Mixture molar density.
        const real *const x, // Species mole fractions.
        unsigned int n,      // Number of values in x array.
        const Sprog::Thermo::ThermoInterface &thermo, // Thermodynamics interface.
        fvector &kforward,   // Return vector for forward rate constants.
        fvector &kreverse    // Return vector for reverse rate constants.
        ) const;


    // JACOBIAN EVALUATION.

    // Calculates the Jacobian matrix for a constant volume, adiabatic
    // homogeneous mixture. J[j][i] is the Jacobian entry for variable
    // i with respect to i: dFi/dYj.  It is assumed that the Jacobian
    // matrix array J has already been allocated for NSP+2 variables
    // (all species, temperature and density).
    void CalcJacobian(
        real T,           // The mixture temperature.
        real density,     // Mixture molar density.
        real *const x,    // Species mole fractions.
        unsigned int n,   // Number of values in x array.
        const Sprog::Thermo::ThermoInterface &thermo, // Thermodynamics interface.
        real pfac,        // Perturbation factor for calculating J entries.
        real **J,         // Jacobian matrix array.
        bool constV=true, // Is system constant volume or constant pressure?
        bool constT=false // Is system constant temperature or adiabatic?
        ) const;

//! Calculates domegai/dcj as opposed to above which calculates d(dxi/dt)/dxj
    void RateJacobian(
        real T,           // The mixture temperature.
        real density,     // Mixture molar density.
        real *const x,    // Species mole fractions.
        unsigned int n,   // Number of values in x array.
        const Sprog::Thermo::ThermoInterface &thermo, // Thermodynamics interface.
        real pfac,        // Perturbation factor for calculating J entries.
        real **J,         // Jacobian matrix array.
        bool constV=true, // Is system constant volume or constant pressure?
        bool constT=false // Is system constant temperature or adiabatic?
        ) const;

    // PARENT MECHANISM.

    // Returns a pointer to the parent mechanism.
    const Sprog::Mechanism *const Mechanism() const;

    // Sets the parent mechanism.
    void SetMechanism(Sprog::Mechanism &mech);


    // READ/WRITE/COPY FUNCTIONS.

    // Writes the reaction set to a binary data stream.
    void Serialize(std::ostream &out) const;

    // Reads the reaction set data from a binary data stream.
    void Deserialize(std::istream &in);

protected:
    // In the following map the key is the index in the vector of all reactions.
    typedef std::map<unsigned int,const Reaction*> RxnMap;

    // Reaction set data.
    RxnPtrVector m_rxns; // Vector of all reactions in the set.
    RxnMap m_rev_rxns;   // Map of reactions which have explicit reverse Arrhenius parameters.
    RxnMap m_tb_rxns;    // Map of third body reactions.
    RxnMap m_fo_rxns;    // Map of fall-off reactions.
    RxnMap m_lt_rxns;    // Map of reactions with Landau Teller parameters.
    RxnMap m_revlt_rxns; // Map of reactions with reverse Landau Teller parameters.


    // RATE CALCULATION.

    // Calculates the concentration-independent portions
    // of the rates constants.
    void calcRateConstantsT(
        real T,            // The mixture temperature.
        const fvector &Gs, // Dimensionless Gibbs free energy of each species (1/mol).
        fvector &kf,       // Return vector for forward rate constants.
        fvector &kr        // Return vector for reverse rate constants.
        ) const;

    // Calculates third-body concentrations for all reactions.  These
    // values will be multiplied by the rate constants, therefore if
    // a reaction does not have third-bodies the tbconcs is set to 1.0.
    void calcTB_Concs(
        real density,        // Mixture molar density.
        const real *const x, // Species mole fractions.
        unsigned int n,      // Number of values in x array.
        fvector &tbconcs     // Return vector of third-body concentrations.
        ) const;

    // Calculates the pressure-dependent fall-off terms in the rate
    // constant expressions.  This function multiplies the rate constants
    // by the fall-off terms.  This function may also change the values in
    // the tbconcs vector.
    void calcFallOffTerms(
        real T,              // The mixture temperature.
        real density,        // Mixture molar density.
        const real *const x, // Species mole fractions.
        unsigned int n,      // Number of values in x array.
        fvector &tbconcs,    // Vector of third-body concentrations (alterable).
        fvector &kf,         // Return vector for forward rate constants.
        fvector &kr          // Return vector for reverse rate constants.
        ) const;


    // MEMORY MANAGEMENT.

    // Clears all memory used by the set.
    void releaseMemory(void);

private:
    // Pointer to mechanism to which this ReactionSet belongs.
    Sprog::Mechanism *m_mech;
};
};
};

#endif