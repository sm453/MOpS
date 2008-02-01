/*
  Author(s):      Matthew Celnik (msc37)
  Project:        sweep (population balance solver)

  File purpose:
    Definition of the coagulation process.  This coagulation process uses a transition
    kernel (REF).  It calculates the rates for the free molecular and slip-flow regimes.
*/

#ifndef SWEEP_COAGULATION_H
#define SWEEP_COAGULATION_H

#include "swpparams.h"
#include "swpprocess.h"
#include "swpcomponent.h"
#include "swpparticle1d.h"
#include <vector>

using namespace std;

namespace Sweep
{
class Coagulation : public Process
{
public:

	/* CONSTRUCTORS AND DESTRUCTOR */
	Coagulation(void);                    // Default constructor.
	Coagulation(const Coagulation & cg);  // Copy-constructor.
    ~Coagulation(void);                   // Destructor.

    /* Coagulation rate types.  These define how the rate is calculated and 
       how the particles are chosen. */
    static const unsigned int TYPE_COUNT = 6;
    const enum Type {SlipFlow1,SlipFlow2,SlipFlow3,SlipFlow4,FreeMol1,FreeMol2};
    /* Different types of kernel majorant types. */
    const enum MajorantType {None, FreeMol, SlipFlow};

	/* TOTAL RATE CALCULATION - OVERRIDING FUNCTIONS */
    real Rate(const real t, const System &sys) const;  // Returns the rate of the process for the given system.

    /* Calculates the process rate given the chemical conditions. */
    real Rate(const real t, const vector<real> &chem, const real T, 
              const real P, const vector<real> &sums, const System &sys) const;
    /* More efficient rate routine for coagulation only.  All parameters required to
       calculate rate passed as arguments. */
    inline real Rate(const vector<real> &sums, const real n, const real sqrtT, const real T_mu, 
              const real T_P, const real vol) const;

	/* SINGLE PARTICLE RATE CALCULATION - OVERRIDING FUNCTIONS */

    /*  Single particle rates are invalid for coagulation, so return negative. */
    inline real Rate(const real t, const System &sys, const Particle &sp) const {return -1.0;};
    /*  Single particle rates are invalid for coagulation, so return negative. */
    inline real Rate(const real t, const vector<real> &chem, const real T, 
                     const real P, const vector<real> &sums, const System &sys, 
                     const Particle &sp) const {return -1.0;};


	/* RATE TERM CALCULATION - OVERRIDING FUNCTIONS */
    /* Returns the number of rate terms for this process. */
    inline unsigned int TermCount(void) const {return TYPE_COUNT;};
    /* Calculates the rate terms give an iterator to a real vector.  The 
       iterator is advanced to the postition after the last term for this
       process. */
    void RateTerms(const real t, const System &sys, vector<real>::iterator &iterm) const;
    /* Calculates the rate terms give an iterator to a real vector.  The 
       iterator is advanced to the postition after the last term for this
       process.  The chemical conditions are precalculated to speed up the
       program. */
    void RateTerms(const real t, const vector<real> &chem, const real T, 
                   const real P, const vector<real> &sums, const System &sys, 
                   vector<real>::iterator &iterm) const;
   /* More efficient rate routine for coagulation only.  All parameters required to
       calculate rate terms passed as arguments. */
    inline void RateTerms(const vector<real> &sums, const real n, const real sqrtT, const real T_mu, 
                   const real T_P, const real vol, vector<real>::iterator &iterm) const;


	/* PERFORMING THE PROCESS */
	/* Performs the process on the given system. Must return 0
       on success, otherwise negative.*/
    int Perform(const real t, System &sys, const unsigned int iterm) const;
    /* Per particle Perform() routine invalid for coagulation. */
    inline int Perform(const real t, System &sys, Particle &sp, 
                       const unsigned int n) const {return -1;};


	/* PROPERTY GET/SETS */
    inline void SetDeferred(const bool defer) {m_defer=false;}


	/* COAGULATION KERNEL ROUTINES */
    real CoagKernel(const Particle *sp1, const Particle *sp2, 
                    const real T, const real P, const MajorantType maj) const;
    real FreeMolKernel(const Particle *sp1, const Particle *sp2, 
                       const real T, const real P, const bool maj) const;
    real SlipFlowKernel(const Particle *sp1, const Particle *sp2, 
                        const real T, const real P, const bool maj) const;

};

inline real Coagulation::Rate(const vector<real> &sums, const real n, const real sqrtT, const real T_mu, 
                              const real T_P, const real vol) const
{
    // Some prerequisites.
    real n_1 = n - 1.0;
    real a = CSF * T_mu;
    real b = a * KNUDSEN_K * T_P * 1.257;
    real c = CFM * sqrtT;

    // Summed particle properties required for coagulation rate.
    real d = Particle::CollisionDiameter(sums);
    real d2 = Particle::CollDiamSquared(sums);
    real d_1 = Particle::InvCollDiam(sums);
    real d_2 = Particle::InvCollDiamSquared(sums);
    real d2m_1_2 = Particle::CollDiamSqrdInvSqrtMass(sums);
    real m_1_2 = Particle::InvSqrtMass(sums);

    // Get individual terms.
    real *terms = new real[TYPE_COUNT];
    // Slip-flow.
    terms[0] = n * n_1 * a / vol;
    terms[1] = ((d * d_1) - n) * a / vol;
    terms[2] = d_1 * n_1 * b / vol;
    terms[3] = ((d * d_2) - d_1) * b / vol;
    // Free-molecular.
    terms[4] = CFMMAJ * n_1 * d2m_1_2  * c / vol;
    terms[5] = CFMMAJ * (m_1_2 * d2 - d2m_1_2) * c / vol;

    // Sum up total coagulation rates for different regimes.
    real sf = terms[0] + terms[1] + terms[2] + terms[3];
    real fm = terms[4] + terms[5];

    if ((sf>0.0) || (fm>0.0)) {
        // There is some coagulation.
        if (sf > fm) {
            // Use free-mol majorant.
            return fm;
        } else {
            // Use slip-flow majorant.
            return sf;
        }
    }
    return 0.0;
}

inline void Coagulation::RateTerms(const vector<real> &sums, const real n, const real sqrtT, 
                                   const real T_mu, const real T_P, const real vol,
                                   vector<real>::iterator &iterm) const
{
    // Some prerequisites.
    real n_1 = n - 1.0;
    real a = CSF * T_mu;
    real b = a * KNUDSEN_K * T_P * 1.257;
    real c = CFM * sqrtT;

    // Summed particle properties required for coagulation rate.
    real d = Particle::CollisionDiameter(sums);
    real d2 = Particle::CollDiamSquared(sums);
    real d_1 = Particle::InvCollDiam(sums);
    real d_2 = Particle::InvCollDiamSquared(sums);
    real d2m_1_2 = Particle::CollDiamSqrdInvSqrtMass(sums);
    real m_1_2 = Particle::InvSqrtMass(sums);

    // Slip-flow.
    *(iterm)   = n * n_1 * a / vol;
    *(iterm+1) = ((d * d_1) - n) * a / vol;
    *(iterm+2) = d_1 * n_1 * b / vol;
    *(iterm+3) = ((d * d_2) - d_1) * b / vol;
    // Free-molecular.
    *(iterm+4) = CFMMAJ * n_1 * d2m_1_2  * c / vol;
    *(iterm+5) = CFMMAJ * (m_1_2 * d2 - d2m_1_2) * c / vol;

    // Sum up total coagulation rates for different regimes.
    real sf = *(iterm) + *(iterm+1) + *(iterm+2) + *(iterm+3);
    real fm = *(iterm+4) + *(iterm+5);

    if ((sf>0.0) || (fm>0.0)) {
        // There is some coagulation.
        if (sf > fm) {
            // Use free-mol majorant.
            *(iterm) = 0.0;
            *(iterm+1) = 0.0;
            *(iterm+2) = 0.0;
            *(iterm+3) = 0.0;
        } else {
            // Use slip-flow majorant.
            *(iterm+4) = 0.0;
            *(iterm+5) = 0.0;
        }
    }

    iterm += TYPE_COUNT;
}

};

#endif
