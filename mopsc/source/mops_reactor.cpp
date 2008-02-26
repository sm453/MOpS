#include "mops_reactor.h"
#include "mops_mixture.h"

// CVODE includes.
#include "cvode\cvode.h"
#include "cvode\cvode_dense.h"
#include "nvector\nvector_serial.h"

#include <vector>
#include <math.h>

using namespace Mops;
using namespace std;

// CONSTRUCTORS AND DESTRUCTORS.

// Default constructor (protected).
Reactor::Reactor(void)
{
    init();
}

// Default constructor (public, requires mechanism).
Reactor::Reactor(const Mops::Mechanism &mech)
{
    init();
    SetMechanism(mech);
}

// Copy constructor.
Reactor::Reactor(const Mops::Reactor &copy)
{
    m_time    = copy.m_time;
    m_mix     = copy.m_mix->Clone(); // Need to clone mixture!
    m_mech    = copy.m_mech;
    m_emodel  = copy.m_emodel;
    m_odewk   = CVodeCreate(CV_BDF, CV_NEWTON);
    m_rtol    = copy.m_rtol;
    m_atol    = copy.m_atol;
    m_nsp     = copy.m_nsp;
    m_neq     = copy.m_neq;
    m_iT      = copy.m_iT;
    m_iDens   = copy.m_iDens;
    Initialise(m_time);
}

// Stream-reading constructor.
Reactor::Reactor(std::istream &in, const Mops::Mechanism &mech)
{
    init();
    Deserialize(in, mech);
    Initialise(m_time);
}

// Default destructor.
Reactor::~Reactor(void)
{
    releaseMemory();
}


// REACTOR SOLUTION.

// Returns the current reactor time.
real Reactor::Time() const
{
    return m_time;
}

// Sets the current reactor time.
void Reactor::SetTime(real t)
{
    m_time = t;
}

// Initialises the reactor to the given time.
void Reactor::Initialise(real time)
{
    m_time = time;
    
    // Fill solution and derivative vectors.
    for (unsigned int i=0; i<m_neq; i++) {
        m_deriv[i] = 0.0;
    }

    // Allocate CVODE stuff.
    CVodeMalloc(m_odewk, &rhsFn, time, 
                N_VMake_Serial(m_neq, m_mix->RawData()), 
                CV_SS, m_rtol, (void*)&m_atol);

    // Set up internal solution array.
    m_solvec = N_VNewEmpty_Serial(m_neq);

    // Set the f_data pointer to this Reactor object.
    CVodeSetFdata(m_odewk, (void*)this);

    // Set other parameters.
    CVodeSetMaxNumSteps(m_odewk, 2000);

    // Set CVDense as the linear system solver.
    CVDense(m_odewk, m_neq);
}

// Reset the solver.  Need to do this if the the reactor
// contents has been changed between calls to Solve().
void Reactor::ResetSolver(void)
{
    CVodeReInit(m_odewk, &rhsFn, m_time, 
                N_VMake_Serial(m_neq, m_mix->RawData()),
                CV_SS, m_rtol, (void*)&m_atol);
}

// Solves the reactor up to the given time.
void Reactor::Solve(real time)
{
    // Put the solution into an N_Vector data pointer.  This does not
    // involve copying the data.
    NV_DATA_S(m_solvec) = m_mix->RawData();
    CVodeSetStopTime(m_odewk, time);

    // Solve over time step.
    while (m_time < time) {
        CVode(m_odewk, time, m_solvec, &m_time, CV_NORMAL);
        m_mix->Normalise(); // This should not be required if CVODE solves correctly.
    }

    // Calculate derivatives at end point.
    if (m_emodel == ConstT) {
        RHS_ConstT(time, m_mix->RawData(), m_deriv);
    } else {
        RHS_Adiabatic(time, m_mix->RawData(), m_deriv);
    }
}


// REACTOR CONTENTS.

// Returns a pointer to the current reactor contents.
Mops::Mixture *const Reactor::Mixture() const
{
    return m_mix;
}

// Fills the reactor with the given mixture.  If the clearfirst flag
// is set then the current mixture is first deleted from memory.
void Reactor::Fill(Mops::Mixture &mix, bool clearfirst)
{
    if (m_mix!=NULL) {
        // First delete current mixture from memory.
        delete m_mix;
        m_mix = NULL;
    }

    // Set new mixture (also take ownership and responibility for its deletion).
    m_mix = &mix;

    // Ensure that the reactor and mixture are using the same
    // mechanism.
    m_mix->SetSpecies(m_mech->Species());
}


// REACTOR MECHANISM.

// Returns the current mechanism.
const Mops::Mechanism *const Reactor::Mechanism() const
{
    return m_mech;
}

// Sets the reactor mechanism.
void Reactor::SetMechanism(const Mops::Mechanism &mech)
{
    m_mech  = &mech;

    // Set up vector indices.
    m_nsp   = m_mech->SpeciesCount();
    m_neq   = m_nsp + 2;
    m_iT    = m_nsp;
    m_iDens = m_iT + 1;

    // Allocate the derivative array.
    if (m_deriv != NULL) delete [] m_deriv;
    m_deriv = new real[m_neq];
}


// ENERGY MODEL.

// Returns the current energy model.
Reactor::EnergyModel Reactor::EnergyEquation() const
{
    return m_emodel;
}

// Sets the energy model.
void Reactor::SetEnergyEquation(Reactor::EnergyModel model)
{
    m_emodel = model;
}


// EQUATION-OF-STATE MODEL.

// Sets the reactor to solve using a constant pressure assumption.
void Reactor::SetConstP(void)
{
    m_constv = false;
}

// Sets the reactor solve using a constant volume assumption.
void Reactor::SetConstV(void)
{
    m_constv = true;
}


// ERROR TOLERANCES.

real Reactor::ATOL() const
{
    return m_atol;
}

void Reactor::SetATOL(real atol)
{
    m_atol = atol;
}

real Reactor::RTOL() const
{
    return m_rtol;
}

void Reactor::SetRTOL(real rtol)
{
    m_rtol = rtol;
}


// READ/WRITE/COPY FUNCTIONS.

// Creates a copy of the reactor object.
Reactor* Reactor::Clone() const
{
    return new Reactor(*this);
}

// Writes the reactor to a binary data stream.
void Reactor::Serialize(std::ostream &out) const
{
    const unsigned int trueval  = 1;
    const unsigned int falseval = 0;
    
    if (out.good()) {
        // Output the version ID (=0 at the moment).
        const unsigned int version = 0;
        out.write((char*)&version, sizeof(version));

        // Output the time.
        double val = (double)m_time;
        out.write((char*)&val, sizeof(val));

        // Output the mixture.
        if (m_mix != NULL) {
            out.write((char*)&trueval, sizeof(trueval));
            m_mix->Serialize(out);
        } else {
            out.write((char*)&falseval, sizeof(falseval));
        }

        // Output the energy model.
        unsigned int n = (unsigned int)m_emodel;
        out.write((char*)&n, sizeof(n));

        // Output the EoS model.
        if (m_constv) {
            out.write((char*)&trueval, sizeof(trueval));
        } else {
            out.write((char*)&falseval, sizeof(falseval));
        }

        // Output the error tolerances.
        val = (double)m_atol;
        out.write((char*)&val, sizeof(val));
        val = (double)m_rtol;
        out.write((char*)&val, sizeof(val));

        // Output equation count + special indices.
        n = (unsigned int)m_nsp;
        out.write((char*)&n, sizeof(n));
        n = (unsigned int)m_neq;
        out.write((char*)&n, sizeof(n));
        n = (unsigned int)m_iT;
        out.write((char*)&n, sizeof(n));
        n = (unsigned int)m_iDens;
        out.write((char*)&n, sizeof(n));

        // Output derivatives array.
        if (m_deriv != NULL) {
            out.write((char*)&trueval, sizeof(trueval));
            for (unsigned int i=0; i<m_neq; i++) {
                val = (double)m_deriv[i];
                out.write((char*)&val, sizeof(val));
            }
        } else {
            out.write((char*)falseval, sizeof(falseval));
        }
    } else {
        throw invalid_argument("Output stream not ready (Mops, Reactor::Serialize).");
    }
}

// Reads the Reactor data from a binary data stream.
void Reactor::Deserialize(std::istream &in, const Mops::Mechanism &mech)
{
    // Clear current reactor data.
    if (m_mix != NULL) delete m_mix;
    if (m_deriv != NULL) delete [] m_deriv;
    init();

    if (in.good()) {
        // Read the output version.  Currently there is only one
        // output version, so we don't do anything with this variable.
        // Still needs to be read though.
        unsigned int version = 0;
        in.read(reinterpret_cast<char*>(&version), sizeof(version));

        double val = 0.0;
        unsigned int n = 0;

        switch (version) {
            case 0:
                // Read the time.
                in.read(reinterpret_cast<char*>(&val), sizeof(val));
                m_time = (real)val;

                // Read the mixture.
                in.read(reinterpret_cast<char*>(&n), sizeof(n));
                if (n == 1) {
                    m_mix = new Mops::Mixture(in, mech);
                }

                // Read the energy model.
                in.read(reinterpret_cast<char*>(&n), sizeof(n));
                m_emodel = (EnergyModel)n;

                // Read the EoS model.
                in.read(reinterpret_cast<char*>(&n), sizeof(n));
                m_constv = (n==1);

                // Read the error tolerances.
                in.read(reinterpret_cast<char*>(&val), sizeof(val));
                m_atol = (real)val;
                in.read(reinterpret_cast<char*>(&val), sizeof(val));
                m_rtol = (real)val;

                // Read equation count + special indices.
                in.read(reinterpret_cast<char*>(&m_nsp), sizeof(m_nsp));
                in.read(reinterpret_cast<char*>(&m_neq), sizeof(m_neq));
                in.read(reinterpret_cast<char*>(&m_iT), sizeof(m_iT));
                in.read(reinterpret_cast<char*>(&m_iDens), sizeof(m_iDens));

                // Store the mechanism and initialise CVODE.
                SetMechanism(mech);
                Initialise(m_time);

                // Read derivatives array.
                in.read(reinterpret_cast<char*>(&n), sizeof(n));
                if (n == 1) {
                    m_deriv = new real[m_neq];
                    for (unsigned int i=0; i<m_neq; i++) {
                        in.read(reinterpret_cast<char*>(&val), sizeof(val));
                        m_deriv[i] = (real)val;
                    }
                }

                break;
            default:
                throw runtime_error("Reactor serialized version number "
                                    "is invalid (Mops, Reactor::Deserialize).");
        }
    } else {
        throw invalid_argument("Input stream not ready (Mops, Reactor::Deserialize).");
    }
}

// Identifies the mixture type for serialisation.
Serial_ReactorType Reactor::SerialType() const
{
    return Serial_Reactor;
}


// RHS FUNCTION AND GOVERNING EQUATIONS.

// The right-hand side evaluator.  This function calculates the RHS of
// the reactor differential equations.  CVODE uses a void* pointer to
// allow the calling code to pass whatever information it wants to
// the RHS function.  In this case the void* pointer should be cast
// into a Reactor object.
int Reactor::rhsFn(double t,      // Independent variable.
                   N_Vector y,    // Solution array.
                   N_Vector ydot, // Derivatives of y wrt t.
                   void* reactor) // Pointer to reactor object.
{
    // Cast the Reactor object.
    Reactor *r = static_cast<Reactor*>(reactor);

    /*
    // Check values of y.
    real ytot = 0.0;
    for (int i=0; i<r->m_neq-2; i++) {
        ytot += NV_DATA_S(y)[i];
        if (NV_DATA_S(y)[i] < -r->m_atol) {
            return 1;
        }
        if (NV_DATA_S(y)[i] > 1.0+r->m_atol) {
            return 1;
        }
    }
    /*
    if (ytot > 1.0+r->m_atol) {
        return 2;
    }
    if (NV_DATA_S(y)[r->m_neq-2] < -r->m_atol) {
        return 3;
    }
    if (NV_DATA_S(y)[r->m_neq-1] < -r->m_atol) {
        return 4;
    }
    */

    // Get the RHS from the system model.
    if (r->m_emodel == ConstT) {
        r->RHS_ConstT(t, NV_DATA_S(y), NV_DATA_S(ydot));
    } else {
        r->RHS_Adiabatic(t, NV_DATA_S(y), NV_DATA_S(ydot));
    }

    return 0;
};

// Definition of RHS form for constant temperature energy equation.
void Reactor::RHS_ConstT(real t, const real *const y,  real *ydot)
{
    int i;
    fvector wdot;
    real wtot = 0.0;

    // Calculate molar production rates.
    wtot = m_mech->Reactions().GetMolarProdRates(y[m_iT], y[m_iDens], y, 
                                                 m_nsp, *m_mix, wdot);

    // Calculate mole fraction derivatives.
    for (i=0; i<m_neq-2; i++) {
        ydot[i] = (wdot[i] - (y[i]*wtot)) / y[m_iDens];
    }

    // Temperature derivative.
    ydot[m_iT]    = 0.0;

    // Density derivative.
    if (m_constv) {
        ydot[m_iDens] = wtot; // Constant volume.
    } else {
        ydot[m_iDens] = 0.0;  // Constant pressure.
    }
}

// Definition of RHS form for adiabatic energy equation.
void Reactor::RHS_Adiabatic(real t, const real *const y,  real *ydot)
{
    int i;
    fvector wdot, Cps, Hs;
    real wtot = 0.0, Cp = 0.0;

    // Calculate mixture thermodynamic properties.
    m_mix->CalcHs_RT(y[m_iT], Hs);
    Cp = m_mix->CalcBulkCp(y[m_iT], y, m_nsp, Cps) / Sprog::R;

    // Calculate molar production rates.
    wtot = m_mech->Reactions().GetMolarProdRates(y[m_iT], y[m_iDens], y, 
                                                 m_nsp, *m_mix, wdot);


    // Calculate mole fraction and temperature derivatives.
    ydot[m_iT] = 0.0;
    for (i=0; i<m_nsp; i++) {
        ydot[i] = (wdot[i] - (y[i]*wtot)) / y[m_iDens];
        ydot[m_iT] += wdot[i] * Hs[i];
    }

    // Complete temperature derivative.
    ydot[m_iT] *= - y[m_iT] / (Cp * y[m_iDens]);

    // Calculate density derivative.
    if (m_constv) {
        // Constant volume.
        ydot[m_iDens] = wtot;
    } else {
        // Constant pressure (Use EoS to calculate).
        ydot[m_iDens] = - y[m_iDens] * ydot[m_iT] / y[m_iT];
    }
}


// INITIALISATION.

// Initialises the reactor to a default state, this is called
// by the constructors.
void Reactor::init(void)
{
    m_time    = 0.0;
    m_mix     = NULL;
    m_mech    = NULL;
    m_deriv   = NULL;
    m_atol    = 1.0e-8;
    m_rtol    = 6.0e-4;
    m_neq     = 0;
    m_nsp     = 0;
    m_iT      = -1;
    m_iDens   = -1;
    m_constv  = false;

    // Init CVODE.
    m_odewk = CVodeCreate(CV_BDF, CV_NEWTON);
}

// Releases all object memory.
void Reactor::releaseMemory(void)
{
    if (m_mix != NULL) delete m_mix;
    m_mix = NULL;
    if (m_deriv != NULL) delete [] m_deriv;
    m_deriv = NULL;
    if (m_odewk != NULL) CVodeFree(&m_odewk);
    m_odewk = NULL;
}