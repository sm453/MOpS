/*
  Author(s):      Matthew Celnik (msc37)
  Project:        sweep (population balance solver)

  File purpose:
    Definition of a surface reaction which includes terms for active sites.  The
    active sites concentration is provided by a function pointer, which must be
    provided before the process is used.

    This class inherits from the SurfaceReaction class, which defines particle 
    surface processes without an active site term.

    The original purpose of this process was to define the surface reactions on
    soot particles given by the ABF model.  These reactions involve reaction at
    active sites on the particle surface, the proportion of which is given by an
    empirical correlation.
*/

#ifndef SWEEP_ACTIVESITESRXN_H
#define SWEEP_ACTIVESITESRXN_H

#include "swpparams.h"
#include "swpcomponent.h"
#include "swpsurfacereaction.h"
#include "swpsystem.h"
#include <map>

using namespace std;

namespace Sweep
{
class ActiveSitesReaction : public Sweep::SurfaceReaction
{
public:
    /* Pointer to the function which returns the number of active
       sites per unit surface area of particles. */
    typedef real (*ActiveSitesFnPtr) (const real t, const System &sys, 
                                      const vector<real> &chem, const real T, 
                                      const real P, const vector<real> &sums);
protected:
    ActiveSitesFnPtr m_psitesfn;
public: // Default contructor and destructor.
    ActiveSitesReaction(void);
    ~ActiveSitesReaction(void);
public:
    /* Initialises the inception reaction. */
    void Initialise(const map<unsigned int, int> &reac, // Gas-phase reactants.
                    const map<unsigned int, int> &prod, // Gas-phase products.
                    const real a, const real n,         // Arrhenius rate parameters.
                    const real e,                       // ''.
                    const vector<real> &comp,           // Changes to component counts due to reaction.
                    const vector<real> &values,         // Changes to other values due to reaction.
                    const unsigned int pid,             // Index of particle property.
                    vector<Component*> &components,     // Reference to component vector used to define process.
                    ActiveSitesFnPtr pfn);              // Pointer to the active sites function.
    /* Sets the function used to calculate active sites density. */
    inline void SetActiveSitesFn(ActiveSitesFnPtr pfn) {m_psitesfn = pfn;};
    /* Returns the rate of the process for the given system. */
    real Rate(const real t, const System &sys) const;
    /* Calculates the process rate given the chemical conditions. */
    real Rate(const real t, const vector<real> &chem, const real T, 
              const real P, const vector<real> &sums, const System &sys) const;
    /* Returns the rate of the process for the given particle in
       the system. Process must be linear in particle number. */
    real Rate(const real t, const System &sys, const Particle &sp) const;
    /* Returns the rate of the process for the given particle in
       the system given the precalculated chemical conditions. Process
       must be linear in particle number. */
    real Rate(const real t, const vector<real> &chem, const real T, 
              const real P, const vector<real> &sums, const System &sys, 
              const Particle &sp) const;
};
};

#endif