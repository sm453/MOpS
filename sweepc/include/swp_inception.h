/*
  Author(s):      Matthew Celnik (msc37)
  Project:        sweep (population balance solver)

  File purpose:
    Definition of an inception process.  Inceptions are modelled as the coagulation
    of 2 gas-phase species.  The rate is given by the collision kernel of these species,
    therefore one must provide their masses and diameters.  Kernel parameters are
    calculated internally.
*/

#ifndef SWEEP_INCEPTION_H
#define SWEEP_INCEPTION_H

#include "swp_params.h"
#include "swp_process.h"
#include "swp_processtype.h"
#include "swp_particle.h"
#include "swp_cell.h"
#include <vector>

namespace Sweep
{
class Inception;
typedef std::vector<Inception*> IcnPtrVector;

class Inception : public Process
{
public: 
    // Constructors.
    Inception(void);                  // Default constructor.
    Inception(const Inception &copy); // Copy constructor.
    Inception(std::istream &in);      // Stream-reading constructor.

    // Destructors.
    ~Inception(void);

    // Operators.
    Inception &Inception::operator =(const Inception &rhs);


    // RATE CONSTANT.

    // Returns the rate constant.
    real A(void) const;

    // Sets the rate constant.
    void SetA(real a);


    // INCEPTION KERNEL.

    // Sets the coagulation kernel constants given incepting species
    // masses and diameters.
    void SetInceptingSpecies(
        real m1, // Mass of species 1.
        real m2, // Mass of species 2.
        real d1, // Collision diameter of species 1.
        real d2  // Collision diameter of species 2.
        );


    // PROPERTIES OF INCEPTED PARTICLES.

    // Returns the composition vector of the new particle.
    const fvector &ParticleComp(void) const;

    // Returns the amount of the ith component of the new particle.
    real ParticleComp(unsigned int i) const;

    // Sets the particle composition vector.
    void SetParticleComp(const fvector &comp);

    // Sets the amount of the ith component in the new particle.
    void SetParticleComp(unsigned int i, real comp);

    // Returns the tracker variable vector of the new particle.
    const fvector &ParticleTrackers(void) const;

    // Returns the value of the ith tracker variable of the
    // new particle.
    real ParticleTrackers(unsigned int i) const;

    // Sets the new particle tracker variable vector.
    void SetParticleTrackers(const fvector &track);

    // Sets the value of the ith tracker variable in the
    // new particle.
    void SetParticleTracker(unsigned int i, real track);


	// TOTAL RATE CALCULATIONS.

    // Returns rate of the process for the given system.
    real Rate(
        real t,         // Time.
        const Cell &sys // System for which to calculate rate.
        ) const;

	// Calculates the process rate using the given 
    // chemical conditions, rather than those conditions in the
    // given system.
    real Rate(
        real t,                   // Time.
        const Sprog::Thermo::IdealGas &gas, // Gas-phase conditions.
        const Cell &sys           // System for which to calculate rate.
        ) const;


    // Calculates the rate of multiple inceptions given a
    // vector of inceptions and an iterator to a vector of
    // reals for output.
    static real CalcRates(
        real t,                   // Time.
        const Cell &sys,          // System for which to calculate rates.
        const IcnPtrVector &icns, // Vector of inception processes.
        fvector &rates,           // Output rates vector.
        unsigned int start = 0    // Vector position to start at in vector rates.
        );


	// RATE TERM CALCULATIONS.

    // Returns the number of rate terms for this process.
    unsigned int TermCount(void) const;

    // Calculates the rate terms given an iterator to a real vector. The 
    // iterator is advanced to the position after the last term for this
    // process.  Returns the sum of all terms.
    real RateTerms(
        real t,                  // Time.
        const Cell &sys,         // System for which to calculate rate terms.
        fvector::iterator &iterm // Iterator to the first term.
        ) const;

    // Calculates the rate terms given an iterator to a real vector. The 
    // iterator is advanced to the position after the last term for this
    // process.  The given chemical conditions are used instead of those
    // in the given system object.
    real RateTerms(
        real t,                   // Time.
        const Sprog::Thermo::IdealGas &gas, // Gas-phase conditions.
        const Cell &sys,          // System for which to calculate rate terms.
        fvector::iterator &iterm  // Iterator to the first term.
        ) const;


	// PERFORMING THE PROCESS.

    // Performs the process on the given system.  The responsible rate term is given
    // by index.  Returns 0 on success, otherwise negative.
    int Perform(
        real t,            // Time.
        Cell &sys,         // System to update.
        unsigned int iterm // The process term responsible for this event.
        ) const;


    // READ/WRITE/COPY.

    // Creates a copy of the inception.
    Inception *const Clone(void) const;

    // Returns the process type.  Used to identify different
    // processes and for serialisation.
    ProcessType ID(void) const;

    // Writes the object to a binary stream.
    void Serialize(std::ostream &out) const;

    // Reads the object from a binary stream.
    void Deserialize(std::istream &in);

protected:
    // Rate parameters.
    real m_a;            // Rate constant.
    real m_kfm;          // Free-molecular kernel parameter.
    real m_ksf1, m_ksf2; // Slip-flow kernel parameters.

    // Properties of newly incepted particles.
    fvector m_newcomp; // Composition.
    fvector m_newvals; // Tracker variable values.

    // Free-molecular enhancement factor.  Currently hardcoded
    // for soot particles (m_efm = 2.2).
    static const real m_efm;

    // A faster rate calculation routine for Inception events only.  Requires all the
    // parameters that would otherwise be calculated by the routine to be passed as
    // arguments.
    real Rate(
        const fvector &fracs, // Gas-phase species mole fractions.
        real density,         // Gas-phase density.
        real sqrtT,           // Square-root of temperature.
        real T_mu,            // T / viscosity of air.
        real MFP,             // Gas mean free path.
        real vol              // Particle ensemble sample volume.
        ) const;
};
};

#endif
