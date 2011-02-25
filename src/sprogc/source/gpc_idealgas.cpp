/*
  Author(s):      Matthew Celnik (msc37)
  Project:        sprog (gas-phase chemical kinetics).
  Sourceforge:    http://sourceforge.net/projects/mopssuite
  
  Copyright (C) 2008 Matthew S Celnik.

  File purpose:
    Implementation of the IdealGas class declared in the
    gpc_idealgas.h header file.

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

#include "gpc_idealgas.h"
#include "gpc_params.h"
#include <math.h>

using namespace Sprog;
using namespace Sprog::Thermo;
using namespace std;


// CONSTRUCTORS AND DESTRUCTORS.

// Default constructor (private).
IdealGas::IdealGas(void)
{
}

// Default constructor (public, requires species vector).
IdealGas::IdealGas(const SpeciesPtrVector &sp) : GasPhase(sp)
{
}

// Copy constructor.
IdealGas::IdealGas(const Sprog::Thermo::IdealGas &copy)
{
    *this = copy;
}

// Stream-reading constructor.
IdealGas::IdealGas(std::istream &in, const SpeciesPtrVector &sp)
{
    Deserialize(in);
    SetSpecies(sp);
}

// Default destructor.
IdealGas::~IdealGas(void)
{
    // Nothing special to destruct here.
}


// OPERATOR OVERLOADS.

// Assignment operator (Mixture).
IdealGas &IdealGas::operator=(const Mixture &mix)
{
    if (this != &mix) {
        Mixture::operator=(mix);
    }
    return *this;
}

// Assignment operator (GasPhase).
IdealGas &IdealGas::operator=(const GasPhase &gas)
{
    if (this != &gas) {
        GasPhase::operator=(gas);
    }
    return *this;
}

// Assignment operator.
IdealGas &IdealGas::operator=(const IdealGas &gas)
{
    if (this != &gas) {
        GasPhase::operator=(gas);
    }
    return *this;
}


// EQUATION OF STATE FUNCTIONS.

// Calculates the mixture pressure.
real IdealGas::Pressure() const
{
    // P = rho R T.
    return Density() * R * Temperature();
}

// Sets the mixture density using the given pressure.
void IdealGas::SetPressure(Sprog::real p)
{
    SetDensity(p / (R * (Temperature())));
}


// THERMODYNAMIC PROPERTIES.

// INTERNAL ENERGY.

// Calculates molar internal energies of each species.
void IdealGas::CalcUs(real T, fvector &U) const
{
    // Calculate the enthalpies.
    CalcHs(T, U);

    // Convert to internal energies.
    fvector::iterator i;
    for (i=U.begin(); i!=U.end(); i++) {
        (*i) -= R * T;
    }
}

// Calculates molar internal energies of each species.
void IdealGas::CalcUs_RT(real T, fvector &U) const
{
    // Calculate the enthalpies.
    CalcHs_RT(T, U);

    // Convert to internal energies.
    vector<real>::iterator i;
    for (i=U.begin(); i!=U.end(); i++) {
        (*i) -= 1.0;
    }
}

// Calculates the bulk internal energy as well as the internal
// energies of each species.
real IdealGas::CalcBulkU(Sprog::real T, 
                         const Sprog::real *const x,
                         unsigned int n,
                         Sprog::fvector &U) const
{
    // Check that there are sufficient values in the x array.
    if (n >= Species()->size()) {
        // Calculate the species internal energies.
        CalcUs(T, U);

        // Calculate the bulk energy.
        real bulku = 0.0;
        for (unsigned int i=0; i!=Species()->size(); ++i) {
            bulku += U[i] * x[i];
        }

        return bulku;
    } else {
        return 0.0;
    }
}

// Calculates the dimensionless bulk internal energy as well as 
// the internal energies of each species.
real IdealGas::CalcBulkU_RT(Sprog::real T, 
                            const Sprog::real *const x,
                            unsigned int n,
                            Sprog::fvector &U) const
{
    // Check that there are sufficient values in the x array.
    if (n >= Species()->size()) {
        // Calculate the species internal energies.
        CalcUs_RT(T, U);

        // Calculate the bulk energy.
        real bulku = 0.0;
        for (unsigned int i=0; i!=Species()->size(); ++i) {
            bulku += U[i] * x[i];
        }

        return bulku;
    } else {
        return 0.0;
    }
}


// ENTHALPY.

// Calculates enthalpies of all species.
void IdealGas::CalcHs(real T, fvector &H) const
{
    int i, n;
    real t[Thermo::H_PARAM_COUNT];
    
    // Ensure output array has sufficient length.
    H.resize(Species()->size());

    // Precalculate temperature terms.
    t[0] = R * T;
    n = Thermo::H_PARAM_COUNT;
    for (i=1; i<n-1; i++) {
        t[i] = (real)i * t[i-1] * T / (real)(i+1);
    }
    t[n-1] = R;

    // Sum terms in polynomial for Hs.
    sumTerms(T, t, n, H);
}

// Calculates enthalpies of all species.
void IdealGas::CalcHs_RT(real T, fvector &H) const
{
    int i, n;
    real t[Thermo::H_PARAM_COUNT];

    // Ensure output array has sufficient length.
    H.resize(Species()->size());

    // Precalculate temperature terms.
    t[0] = 1.0;
    n = Thermo::H_PARAM_COUNT;
    for (i=1; i<n-1; i++) {
        t[i] = (real)i * t[i-1] * T / (real)(i+1);
    }
    t[n-1] = 1.0 / T;

    // Sum terms in polynomial for H.
    sumTerms(T, t, n, H);
}

// Calculates the bulk enthalpy and the enthalpies
// of each species at the given conditions.
real IdealGas::CalcBulkH(Sprog::real T, 
                         const Sprog::real *const x,
                         unsigned int n,
                         Sprog::fvector &H) const
{
    if (n >= Species()->size()) {
        // Get individual species enthalpies.
        CalcHs(T, H);
        
        // Sum to get bulk enthalpy.
        real bulkH = 0.0;
        for(unsigned int i=0; i!=Species()->size(); ++i) {
            bulkH += x[i] * H[i];
        }

        return bulkH;
    } else {
        return 0.0;
    }
}

// Calculates the dimensionless bulk enthalpy and the enthalpies
// of each species at the given conditions.
real IdealGas::CalcBulkH_RT(Sprog::real T, 
                            const Sprog::real *const x,
                            unsigned int n,
                            Sprog::fvector &H) const
{
    if (n >= Species()->size()) {
        // Get individual species enthalpies.
        CalcHs_RT(T, H);
        
        // Sum to get bulk enthalpy.
        real bulkH = 0.0;
        for(unsigned int i=0; i!=Species()->size(); ++i) {
            bulkH += x[i] * H[i];
        }

        return bulkH;
    } else {
        return 0.0;
    }
}


// ENTROPY.

// Calculates entropies of all species.
void IdealGas::CalcSs(real T, fvector &S) const
{
    int i, n;
    real t[Thermo::S_PARAM_COUNT];

    // Ensure output array has sufficient length.
    S.resize(Species()->size());

    // Precalculate temperature terms.
    n = Thermo::S_PARAM_COUNT;
    t[0] = R * log(T);
    t[1] = R * T;
    for (i=2; i!=n-2; ++i) {
        t[i] = (real)(i-1) * t[i-1] * T / (real)i;
    }
    t[n-2] = 0.0;
    t[n-1] = R;

    // Sum terms in polynomial for Ss.
    sumTerms(T, t, n, S);
}

// Calculates dimensionless entropies of all species.
void IdealGas::CalcSs_R(real T, fvector &S) const
{
    int i, n;
    real t[Thermo::S_PARAM_COUNT];

    // Ensure output array has sufficient length.
    S.resize(Species()->size());

    // Precalculate temperature terms.
    n = Thermo::S_PARAM_COUNT;
    t[0] = log(T);
    t[1] = T;
    for (i=2; i<n-2; i++) {
        t[i] = (real)(i-1) * t[i-1] * T / (real)i;
    }
    t[n-2] = 0.0;
    t[n-1] = 1.0;

    // Sum terms in polynomial for Ss.
    sumTerms(T, t, n, S);
}

// Calculates the bulk entropy and the entropies of
// each species.
real IdealGas::CalcBulkS(Sprog::real T, 
                         const Sprog::real *const x,
                         unsigned int n,
                         Sprog::fvector &S) const
{
    if (n >= Species()->size()) {
        // Get individual species entropies.
        CalcSs(T, S);
        
        // Sum to get bulk entropy.
        real bulkS = 0.0;
        for(unsigned int i=0; i!=Species()->size(); ++i) {
            bulkS += x[i] * S[i];
        }

        return bulkS;
    } else {
        return 0.0;
    }
}

// Calculates the dimensionless bulk entropy and the 
// entropies of each species.
real IdealGas::CalcBulkS_R(Sprog::real T, 
                           const Sprog::real *const x,
                           unsigned int n,
                           Sprog::fvector &S) const
{
    if (n >= Species()->size()) {
        // Get individual species entropies.
        CalcSs_R(T, S);
        
        // Sum to get bulk entropy.
        real bulkS = 0.0;
        for(unsigned int i=0; i!=Species()->size(); ++i) {
            bulkS += x[i] * S[i];
        }

        return bulkS;
    } else {
        return 0.0;
    }
}


// GIBBS FREE ENERGY.

// Calculates molar Gibbs free energies of each species.
void IdealGas::CalcGs(real T, fvector &G) const
{
    unsigned int i, n;
    real t[Thermo::S_PARAM_COUNT];

    // Ensure output array has sufficient length.
    G.resize(Species()->size());

    // Precalculate temperature terms.
    n = Thermo::S_PARAM_COUNT;
    t[1] = - R * T;
    t[0] = t[1] * (log(T) - 1.0);
    t[S_PARAM_COUNT-1] = t[1];
    t[1] *= 0.5 * T;
    for (i=2; i!=Thermo::CP_PARAM_COUNT; ++i) {
        t[i] = (real)(i-1) * t[i-1] * T / (real)(i+1);
    }
    t[H_PARAM_COUNT-1] = R;

    // Sum terms in polynomial for G.
    sumTerms(T, t, n, G);
}

// Calculates molar Gibbs free energies of each species.
void IdealGas::CalcGs_RT(real T, fvector &G) const
{
    unsigned int i, n;
    real t[Thermo::S_PARAM_COUNT];

    // Ensure output array has sufficient length.
    G.resize(Species()->size());

    // Precalculate temperature terms.
    t[0] = 1.0 - log(T);
    t[1] = - 0.5 * T;
    for (i=2; i!=Thermo::CP_PARAM_COUNT; ++i) {
        t[i] = (real)(i-1) * t[i-1] * T / (real)(i+1);
    }
    t[H_PARAM_COUNT-1] = 1.0 / T;
    t[S_PARAM_COUNT-1] = - 1.0;

    // Sum terms in polynomial for Cps.
    n = Thermo::S_PARAM_COUNT;
    sumTerms(T, t, n, G);
}

// Calculates the species' Gibbs free energies given the temperature and
// the species' enthalpies and entropies.
void IdealGas::CalcGs(Sprog::real T, 
                      const Sprog::fvector &H, 
                      const Sprog::fvector &S, 
                      Sprog::fvector &G) const
{
    for (unsigned int i=0; i!=Species()->size(); ++i) {
        G[i] = H[i] - (T * S[i]);
    }
}

// Calculates the dimensionless species' Gibbs free energies given 
// the temperature and the species' enthalpies and entropies.
void IdealGas::CalcGs_RT(Sprog::real T, 
                         const Sprog::fvector &H_RT, 
                         const Sprog::fvector &S_R, 
                         Sprog::fvector &G_RT) const
{
    for (unsigned int i=0; i!=Species()->size(); ++i) {
        G_RT[i] = H_RT[i] - S_R[i];
    }
}

// Calculates the bulk Gibbs free energy of the mixture and
// the Gibbs free energies of each species.
real IdealGas::CalcBulkG(Sprog::real T, 
                         const Sprog::real *const x, 
                         unsigned int n, 
                         Sprog::fvector &G) const
{
    // Check length of x array.
    if (n >= Species()->size()) {
        // Calculate species Gibbs free energies.
        CalcGs(T, G);

        // Calculate bulk Gibbs free energy.
        real bulkG = 0.0;
        for (unsigned int i=0; i!=Species()->size(); ++i) {
            bulkG += G[i] * x[i];
        }

        return bulkG;
    } else {
        return 0.0;
    }
}

// Calculates the dimensionless bulk Gibbs free energy of the mixture and
// the Gibbs free energies of each species.
real IdealGas::CalcBulkG_RT(Sprog::real T, 
                            const Sprog::real *const x, 
                            unsigned int n, 
                            Sprog::fvector &G) const
{
    // Check length of x array.
    if (n >= Species()->size()) {
        // Calculate species Gibbs free energies.
        CalcGs_RT(T, G);

        // Calculate bulk Gibbs free energy.
        real bulkG = 0.0;
        for (unsigned int i=0; i!=Species()->size(); ++i) {
            bulkG += G[i] * x[i];
        }

        return bulkG;
    } else {
        return 0.0;
    }
}


// CONSTANT PRESSURE HEAT CAPACITY.

// Calculates molar heat capacity at const. P of all species.
void IdealGas::CalcCps(real T, fvector &Cp) const
{
    int i, n;
    real t[Thermo::CP_PARAM_COUNT];

    // Ensure output array has sufficient length.
    Cp.resize(Species()->size());

    // Precalculate temperature terms.
    n = Thermo::CP_PARAM_COUNT;
    t[0] = R;
    for (i=1; i<n; i++) {
        t[i] = t[i-1] * T;
    }

    // Sum terms in polynomial for Cps.
    sumTerms(T, t, n, Cp);
}

// Calculates dimensionless molar heat capacity at const. P of all species.
void IdealGas::CalcCps_R(real T, fvector &Cp) const
{
    int i, n;
    real t[Thermo::CP_PARAM_COUNT];

    // Ensure output array has sufficient length.
    Cp.resize(Species()->size());

    // Precalculate temperature terms.
    n = Thermo::CP_PARAM_COUNT;
    t[0] = 1.0;
    for (i=1; i<n; i++) {
        t[i] = t[i-1] * T;
    }

    // Sum terms in polynomial for Cps.
    sumTerms(T, t, n, Cp);
}

// Calculates the mean molar heat capacity at const. P.
real IdealGas::CalcBulkCp(Sprog::real T, 
                          const Sprog::real *const x, 
                          unsigned int n, 
                          Sprog::fvector &Cp) const
{
    if (n >= Species()->size()) {
        // Get individual species heat capacities.
        CalcCps(T, Cp);
        
        // Sum to get bulk heat capacity.
        real bulkCp = 0.0;
        for(unsigned int i=0; i!=Species()->size(); ++i) {
            bulkCp += x[i] * Cp[i];
        }

        return bulkCp;
    } else {
        return 0.0;
    }
}

// Calculates the dimensionless mean molar heat capacity at const. P.
real IdealGas::CalcBulkCp_R(Sprog::real T, 
                            const Sprog::real *const x, 
                            unsigned int n, 
                            Sprog::fvector &Cp) const
{
    if (n >= Species()->size()) {
        // Get individual species heat capacities.
        CalcCps_R(T, Cp);
        
        // Sum to get bulk heat capacity.
        real bulkCp = 0.0;
        for(unsigned int i=0; i!=Species()->size(); ++i) {
            bulkCp += x[i] * Cp[i];
        }

        return bulkCp;
    } else {
        return 0.0;
    }
}


// CONSTANT VOLUME HEAT CAPACITY.

// Calculates molar heat capacity at const. V of all species.
void IdealGas::CalcCvs(real T, fvector &Cv) const
{

    // Calculate heat capacties at const. P.
    CalcCps(T, Cv);

    // Convert to const. V.
    fvector::iterator i;
    for(i=Cv.begin(); i!=Cv.end(); i++) {
        (*i) -= R;
    }
}

// Calculates molar heat capacity at const. V of all species.
void IdealGas::CalcCvs_R(real T, fvector &Cv) const
{

    // Calculate heat capacties at const. P.
    CalcCps_R(T, Cv);

    // Convert to const. V.
    fvector::iterator i;
    for(i=Cv.begin(); i!=Cv.end(); i++) {
        (*i) -= 1.0;
    }
}

// Calculates the mean molar heat capacity at const. V.
real IdealGas::CalcBulkCv(Sprog::real T, 
                          const Sprog::real *const x, 
                          unsigned int n, 
                          Sprog::fvector &Cv) const
{
    if (n >= Species()->size()) {
        // Get individual species heat capacities.
        CalcCvs(T, Cv);
        
        // Sum to get bulk heat capacity.
        real bulkCv = 0.0;
        for(unsigned int i=0; i!=Species()->size(); ++i) {
            bulkCv += x[i] * Cv[i];
        }

        return bulkCv;
    } else {
        return 0.0;
    }
}

// Calculates the dimensionless mean molar heat capacity at const. V.
real IdealGas::CalcBulkCv_R(Sprog::real T, 
                            const Sprog::real *const x, 
                            unsigned int n, 
                            Sprog::fvector &Cv) const
{
    if (n >= Species()->size()) {
        // Get individual species heat capacities.
        CalcCvs_R(T, Cv);
        
        // Sum to get bulk heat capacity.
        real bulkCv = 0.0;
        for(unsigned int i=0; i!=Species()->size(); ++i) {
            bulkCv += x[i] * Cv[i];
        }

        return bulkCv;
    } else {
        return 0.0;
    }
}


// MULTIPLE PROPERTIES.

// Calculates the species' enthalpies, entropies and constant pressure
// heat capacities using the given temperature.  This function can be 
// more efficient than calculating the properties individually.
void IdealGas::CalcCpHSs(real T,
                         fvector &Cp,
                         fvector &H,
                         fvector &S) const
{
    unsigned int i, k, nc, nh, ns;
    real tc[CP_PARAM_COUNT], th[H_PARAM_COUNT], ts[S_PARAM_COUNT];
    const THERMO_PARAMS *a;

    // Ensure output arrays have sufficient length.
    Cp.resize(Species()->size());
    H.resize(Species()->size());
    S.resize(Species()->size());

    // Precalculate temperature terms.
    nc = CP_PARAM_COUNT;
    nh = H_PARAM_COUNT;
    ns = S_PARAM_COUNT;

    tc[0] = R;
    th[0] = R;
    ts[0] = R * log(T);
    tc[1] = R * T;
    th[1] = R * T / 2.0;
    ts[1] = R * T;

    for (i=2; i<nc; i++) {
        tc[i] = tc[i-1] * T;
        th[i] = th[i-1] * T * (real)i / (real)(i+1);
        ts[i] = ts[i-1] * T * (real)(i-1) / (real)i;
    }
    th[nh-1] = R;
    ts[nh-1] = 0.0;
    ts[ns-1] = R;

    // Sum terms in polynomials.
    for (i=0; i!=Species()->size(); ++i) {
        // Set sums to 0 initially.
        Cp[i] = H[i] = S[i] = 0.0;

        // Get the thermo fitting parameters for this species
        // at this temperature.
        a = &(*Species())[i]->ThermoParams(T);

        // Calculate the thermo property for this species.
        for (k=0; k<nc; k++) {
            Cp[i] += a->Params[k] * tc[k];
            H[i]  += a->Params[k] * th[k];
            S[i]  += a->Params[k] * ts[k];
        }
        H[i] += a->Params[nh-1] * th[nh-1];
        S[i] += a->Params[nh-1] * ts[nh-1];
        S[i] += a->Params[ns-1] * ts[ns-1];
    }
}

// Calculates the species' enthalpies (H/RT), entropies (S/R) and constant pressure
// heat capacities (Cp/R) all in dimensionless units using the given 
// temperature.  This function can be more efficient than calculating 
// the properties individually.
void IdealGas::CalcCpHSs_RT(real T,
                            fvector &Cp,
                            fvector &H,
                            fvector &S) const
{
    unsigned int i, k, nc, nh, ns;
    real tc[CP_PARAM_COUNT], th[H_PARAM_COUNT], ts[S_PARAM_COUNT];
    const THERMO_PARAMS *a;

    // Ensure output arrays have sufficient length.
    Cp.resize(Species()->size());
    H.resize(Species()->size());
    S.resize(Species()->size());

    // Precalculate temperature terms.
    nc = CP_PARAM_COUNT;
    nh = H_PARAM_COUNT;
    ns = S_PARAM_COUNT;

    tc[0] = 1.0;
    th[0] = 1.0;
    ts[0] = log(T);
    tc[1] = T;
    th[1] = T / 2.0;
    ts[1] = T;

    for (i=2; i!=nc; ++i) {
        tc[i] = tc[i-1] * T;
        th[i] = th[i-1] * T * (real)i / (real)(i+1);
        ts[i] = ts[i-1] * T * (real)(i-1) / (real)i;
    }
    th[nh-1] = 1.0 / T;
    ts[nh-1] = 0.0;
    ts[ns-1] = 1.0;

    // Sum terms in polynomials.
    for (i=0; i!=Species()->size(); ++i) {
        // Set sums to 0 initially.
        Cp[i] = H[i] = S[i] = 0.0;
        
        // Get the thermo fitting parameters for this species
        // at this temperature.
        a = &(*Species())[i]->ThermoParams(T);

        // Calculate the thermo property for this species.
        for (k=0; k<nc; k++) {
            Cp[i] += a->Params[k] * tc[k];
            H[i]  += a->Params[k] * th[k];
            S[i]  += a->Params[k] * ts[k];
        }
        H[i] += a->Params[nh-1] * th[nh-1];
        S[i] += a->Params[nh-1] * ts[nh-1];
        S[i] += a->Params[ns-1] * ts[ns-1];
    }
}


// READ/WRITE/COPY FUNCTIONS.

// Creates a copy of the mixture object.
IdealGas *const IdealGas::Clone() const
{
    return new IdealGas(*this);
}

/*
// Writes the mixture to a binary data stream.
void IdealGas::Serialize(std::ostream &out) const
{
}

// Reads the mixture data from a binary data stream.
void IdealGas::Deserialize(std::istream &in)
{
}
*/

// Identifies the mixture type for serialisation.
Serial_MixtureType IdealGas::SerialType() const
{
    return Serial_IdealGas;
}


// PRIVATE FUNCTIONS.

// Calculates a polynomial fit of any thermo property given the
// temperature terms.  The polynomial coefficients are found per
// species.
void IdealGas::sumTerms(real T, real *t, int n, std::vector<real> &Xs) const
{
    unsigned int i;
    int k;
    const THERMO_PARAMS *a;

    for (i=0; i!=min(Species()->size(),Xs.size()); ++i) {
        // Set sums to zero initially.
        Xs[i] = 0.0;

        // Get the thermo fitting parameters for this species
        // at this temperature.
        a = &(*Species())[i]->ThermoParams(T);

        // Calculate the thermo property for this species.
        for (k=0; k!=n; ++k) {
            Xs[i] += a->Params[k] * t[k];
        }
    }
}







