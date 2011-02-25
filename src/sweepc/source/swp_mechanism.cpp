/*
  Author(s):      Matthew Celnik (msc37) and Markus Sander
  Project:        sweepc (population balance solver)
  Sourceforge:    http://sourceforge.net/projects/mopssuite

  Copyright (C) 2008 Matthew S Celnik.

  File purpose:
    Implementation of the Mechanism class declared in the
    swp_mechanism.h header file.

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

#include "swp_mechanism.h"
#include "swp_model_factory.h"
#include "swp_process_factory.h"
#include "swp_abf_model.h"
#include "rng.h"

#include "geometry1d.h"

#include <stdexcept>
#include <cassert>

#include "string_functions.h"
#include "swp_particle.h"
#include "swp_PAH_primary.h"


using namespace Sweep;
using namespace Sweep::Processes;
using namespace std;
using namespace Strings;


// CONSTRUCTORS AND DESTRUCTORS.

// Default constructor.
Mechanism::Mechanism(void)
: m_anydeferred(false), m_icoag(-1), m_termcount(0), m_processcount(0)
{
}

// Copy constructor.
Mechanism::Mechanism(const Mechanism &copy)
{
	*this = copy;
}

// Default destructor.
Mechanism::~Mechanism(void)
{
    releaseMem();
}


// OPERATOR OVERLOADS.

// Assignment operator.
Mechanism &Mechanism::operator=(const Mechanism &rhs)
{
    if (this != &rhs) {
        // Clear current mechanism from memory.
        releaseMem();

        // Copy base class.
        ParticleModel::operator=(rhs);

        // Easily copied data.
        m_anydeferred  = rhs.m_anydeferred;
        m_icoag        = rhs.m_icoag;
        m_termcount    = rhs.m_termcount;
        m_processcount = rhs.m_processcount;

        // Copy inceptions.
        for (IcnPtrVector::const_iterator i=rhs.m_inceptions.begin();
            i!=rhs.m_inceptions.end(); ++i) {
            m_inceptions.push_back((*i)->Clone());

            // Need to update the parent mechanism
            m_inceptions.back()->SetMechanism(*this);
        }

        // Copy particle processes.
        for (TransportPtrVector::const_iterator i=rhs.m_transports.begin();
            i!=rhs.m_transports.end(); ++i) {
            m_transports.push_back((*i)->Clone());

            // Need to update the parent mechanism
            m_transports.back()->SetMechanism(*this);
        }

        // Copy particle processes.
        for (PartProcPtrVector::const_iterator i=rhs.m_processes.begin();
            i!=rhs.m_processes.end(); ++i) {
            m_processes.push_back((*i)->Clone());

            // Need to update the parent mechanism
            m_processes.back()->SetMechanism(*this);
        }

        // Copy coagulation processes.
        for (CoagPtrVector::const_iterator i=rhs.m_coags.begin();
            i!=rhs.m_coags.end(); ++i) {
            m_coags.push_back((*i)->Clone());

            // Need to update the parent mechanism
            m_coags.back()->SetMechanism(*this);
        }


        // Copy process counters.
        m_proccount.assign(rhs.m_proccount.begin(), rhs.m_proccount.end());
        m_fictcount.assign(rhs.m_fictcount.begin(), rhs.m_fictcount.end());
    }
    return *this;
}


// ACTIVE-SITES MODELS.

// Returns the set of particle model ID used by this mechanism
const ActSites::ActSitesTypeSet &Mechanism::ActSiteModels(void) const
{
    return m_actsites;
}

// Returns true if the mechanism include the given model.
bool Mechanism::ContainsActSiteModel(ActSites::ActSitesType id) const
{
    return m_actsites.find(id) != m_actsites.end();
}

// Adds an active-sites model to the mechanism.
void Mechanism::AddActSitesModel(ActSites::ActSitesType id)
{
    m_actsites.insert(id);
}


// INCEPTIONS.

// Returns the vector of inceptions.
const IcnPtrVector &Mechanism::Inceptions(void) const
{
    return m_inceptions;
}

// Returns the inception with the given index.
const Inception *const Mechanism::Inceptions(unsigned int i) const
{
    if (i < m_inceptions.size()) {
        return m_inceptions[i];
    } else {
        return NULL;
    }
}



// Adds an inception to the mechanism.
void Mechanism::AddInception(Inception &icn)
{
    // Add the inception to the mechanism.
    m_inceptions.push_back(&icn);
    m_termcount += icn.TermCount();
    ++m_processcount;
    m_proccount.resize(m_termcount, 0);
    m_fictcount.resize(m_termcount, 0);

    // Set the inception to belong to this mechanism.
    icn.SetMechanism(*this);
}


// PARTICLE PROCESSES.

// Returns the vector of particle processes.
const PartProcPtrVector &Mechanism::Processes(void) const
{
    return m_processes;
}

// Returns the process with the given index.
const ParticleProcess *const Mechanism::Processes(unsigned int i) const
{
    if (i < m_processes.size()) {
        return m_processes[i];
    } else {
        return NULL;
    }
}

// Adds a process to the mechanism.
void Mechanism::AddProcess(ParticleProcess &p)
{
    // Add the process to the mechanism.
    m_processes.push_back(&p);
    m_termcount += p.TermCount();
    ++m_processcount;
    m_proccount.resize(m_termcount, 0);
    m_fictcount.resize(m_termcount, 0);

    // Check for any deferred.
    m_anydeferred = m_anydeferred || p.IsDeferred();

    // Set the process to belong to this mechanism.
    p.SetMechanism(*this);
}


// TRANSPORT PROCESSES.

/*!
 * Get the full list of transport processes, this is
 * a const function so the pointers in the vector should
 * not be used to modify the processes, even though this
 * is permitted by the language.
 *
 *\return   Vector of pointers to the transport processes
 */
const TransportPtrVector &Mechanism::Transports() const
{
    return m_transports;
}

/*!
 * Return a pointer to the ith transport process in the
 * mechanism, or null if there are fewer than i transport
 * process.
 *
 *\param[in]    i   Index of requested transport process in transport process array
 *
 *\return   Pointer to ith transport process
 */
const TransportProcess *const Mechanism::Transports(unsigned int i) const
{
    if (i < m_transports.size()) {
        return m_transports[i];
    } else {
        return NULL;
    }
}

/*!
 * Insert a transport process in the mechanism for use
 * in simulation.  The same transport process instance
 * cannot be added to another mechanism.
 *
 *\param[in]    p   Trnsport process to add to mechanism
 */
void Mechanism::AddTransport(TransportProcess &p)
{
    // Add the process to the mechanism.
    m_transports.push_back(&p);
    m_termcount += p.TermCount();
    ++m_processcount;
    m_proccount.resize(m_termcount, 0);
    m_fictcount.resize(m_termcount, 0);

    // Check for any deferred.
    m_anydeferred = m_anydeferred || p.IsDeferred();

    // Set the process to belong to this mechanism.
    p.SetMechanism(*this);
}

// COAGULATIONS.

/*!
 * @param[in,out]   coag    New coagulation process
 *
 * Ownership of the process will be taken by the mechanism.  The
 * process must be heap allocated so that delete can be called on
 * it.
 */
void Mechanism::AddCoagulation(Coagulation& coag)
{

    m_coags.push_back(&coag);
    m_termcount += coag.TermCount();
    ++m_processcount;

    m_termcount += coag.TermCount();
    m_proccount.resize(m_termcount, 0);
    m_fictcount.resize(m_termcount, 0);

    // Set the coagulation to belong to this mechanism.
    coag.SetMechanism(*this);
}


// PROCESS INFORMATION.

// Returns the number of processes (including
// inceptions) in the mechanism.
unsigned int Mechanism::ProcessCount(void) const
{
    return m_processcount;
}

// Returns the number of terms in all process rate expressions.
unsigned int Mechanism::TermCount(void) const {return m_termcount;}

// Returns true if the mechanism contains deferred (LPDA)
// processes otherwise false.
bool Mechanism::AnyDeferred(void) const {return m_anydeferred;}

// Checks all processes to see if any are deferred.
void Mechanism::CheckDeferred(void) const
{
    // Loop though all processes checking if any are deferred.
    m_anydeferred = false;
    PartProcPtrVector::const_iterator i;
    for (i=m_processes.begin(); i!=m_processes.end(); ++i) {
        if ((*i)->IsDeferred()) {
            // Set anydeferred flag is true.
            m_anydeferred = true;
            return;
        }
    }
}


// Returns a vector containing the names of all processes.
void Mechanism::GetProcessNames(std::vector<std::string> &names,
                                unsigned int start) const
{
    // Resize the output vector to hold all process names from
    // start index onwards.
    unsigned int N = start + m_processcount; //m_inceptions.size() + m_processes.size();
//    if (m_coag) ++N;
    names.resize(N, "");

    // Get iterator to first insertion point.
    vector<string>::iterator i = names.begin()+start;

    // Add inception names.
    for (unsigned int j=0; j!=m_inceptions.size(); ++j) {
        *i = m_inceptions[j]->Name(); ++i;
    }

    // Add particle process names.
    for (unsigned int j=0; j!=m_processes.size(); ++j) {
        *i = m_processes[j]->Name(); ++i;
    }

    // Add transport process names.
    for (TransportPtrVector::const_iterator it = m_transports.begin();
         it != m_transports.end(); ++it) {
        *i = (*it)->Name();
        ++i;
    }

    // Add coagulation name.
    for (CoagPtrVector::const_iterator it = m_coags.begin();
         it != m_coags.end(); ++it) {
        *i++ = (*it)->Name();
    }
}


// RATE CALCULATION.

// Get total rates of all processes.  Returns the sum of
// all rates.
real Mechanism::CalcRates(real t, const Cell &sys, const Geometry::LocalGeometry1d &local_geom, fvector &rates, bool scale) const
{
    // Ensure rates vector is the correct length, then set to zero.
    rates.resize(m_processcount+sys.InflowCount()+sys.OutflowCount(), 0.0);
    fill(rates.begin(), rates.end(), 0.0);

    real sum = 0.0;

    // Get rates of inception processes.
    sum += Inception::CalcRates(t, sys, m_inceptions, rates);

    // Query other processes for their rates.
    sum += ParticleProcess::CalcRates(t, sys, m_processes, rates, m_inceptions.size());

    // Query transport processes for their rates.
    sum += TransportProcess::CalcRates(t, sys, local_geom, m_transports, rates, m_inceptions.size() + m_processes.size());

    // Get coagulation rate.
    sum += Coagulation::CalcRates(t, sys, m_coags, rates, m_inceptions.size() + m_processes.size() + m_transports.size());

    // Get birth and death rates from the Cell.
    fvector::iterator i = rates.begin() + m_inceptions.size() + m_transports.size() + m_processes.size() + m_coags.size();
    const BirthPtrVector &inf = sys.Inflows();
    for (BirthPtrVector::const_iterator j=inf.begin(); j!=inf.end(); ++j) {
        *i = (*j)->Rate(t, sys);
        sum += *i;
        ++i;
    }
    const DeathPtrVector &out = sys.Outflows();
    for (DeathPtrVector::const_iterator j=out.begin(); j!=out.end(); ++j) {
        *i = (*j)->Rate(t, sys);
        sum += *i;
        ++i;
    }

    if (!scale) {
        // Need to return the rates to per unit vol.
        real invvol = 1.0 / sys.SampleVolume();
        for(i=rates.begin(); i!=rates.end(); ++i) {
            *i *= invvol;
        }
        sum *= invvol;
    }

    return sum;
}

// Get rates of all processes separated into different
// terms.  Rate terms are useful for subsequent particle
// selection by different properties for the same process.
// In particular this is used for the condensation and
// coagulation processes.  Returns the sum of all rates.
real Mechanism::CalcRateTerms(real t, const Cell &sys, const Geometry::LocalGeometry1d& local_geom, fvector &terms) const
{
    // Ensure rates vector is the correct length.
    terms.resize(m_termcount+sys.InflowCount()+sys.OutflowCount(), 0.0);
    fvector::iterator iterm = terms.begin();

    real sum = 0.0;

    // Get rates of inception processes.
    IcnPtrVector::const_iterator ii;
    for (ii=m_inceptions.begin(); ii!=m_inceptions.end(); ++ii) {
        sum += (*ii)->RateTerms(t, sys, iterm);
    }

    // Query other processes for their rates.
    if (sys.ParticleCount() > 0) {
        for(PartProcPtrVector::const_iterator i=m_processes.begin();
            (i!=m_processes.end()) && (iterm!=terms.end()); ++i) {
            sum += (*i)->RateTerms(t, sys, iterm);
        }
        for(TransportPtrVector::const_iterator i=m_transports.begin();
            (i!=m_transports.end()) && (iterm!=terms.end()); ++i) {
            sum += (*i)->RateTerms(t, sys, local_geom, iterm);
        }
    } else {
        // Fill vector with zeros.
        for(PartProcPtrVector::const_iterator i=m_processes.begin(); (i!=m_processes.end()) && (iterm!=terms.end()); ++i) {
            fill(iterm, iterm+(*i)->TermCount(), 0.0);
            iterm += (*i)->TermCount();
        }
        for(TransportPtrVector::const_iterator i=m_transports.begin();
            (i!=m_transports.end()) && (iterm!=terms.end()); ++i) {
            fill(iterm, iterm+(*i)->TermCount(), 0.0);
            iterm += (*i)->TermCount();
        }
    }

    // Coagulation
    sum += Coagulation::CalcRateTerms(t, sys, m_coags, iterm);

    // Get birth and death rates from the Cell.
    const BirthPtrVector &inf = sys.Inflows();
    for (BirthPtrVector::const_iterator j=inf.begin(); j!=inf.end(); ++j) {
        sum += (*j)->RateTerms(t, sys, iterm);
    }
    const DeathPtrVector &out = sys.Outflows();
    for (DeathPtrVector::const_iterator j=out.begin(); j!=out.end(); ++j) {
        sum += (*j)->RateTerms(t, sys, iterm);
    }

    return sum;
}

// Get total rates of non-deferred processes.  Returns the sum
// of all rates.
real Mechanism::CalcJumpRateTerms(real t, const Cell &sys, const Geometry::LocalGeometry1d& local_geom, fvector &terms) const
{
    // This routine only calculates the rates of those processes which are
    // not deferred.  The rate terms of deferred processes are returned
    // as zero.

    // Ensure vector is the correct length, then set to zero.
    terms.resize(m_termcount+sys.InflowCount()+sys.OutflowCount(), 0.0);
    fvector::iterator iterm = terms.begin();

    real sum = 0.0;

    // Get rates of inception processes.
    IcnPtrVector::const_iterator ii;
    for (ii=m_inceptions.begin(); ii!=m_inceptions.end(); ++ii) {
        sum += (*ii)->RateTerms(t, sys, iterm);
    }

    // Query other processes for their rates.
    if (sys.ParticleCount() > 0) {
        for(PartProcPtrVector::const_iterator i=m_processes.begin();
            (i!=m_processes.end()) && (iterm!=terms.end()); ++i) {
            if (!(*i)->IsDeferred()) {
                // Calculate rate if not deferred.
                sum += (*i)->RateTerms(t, sys, iterm);
            } else {
                // If process is deferred, then set rate to zero.
                for (unsigned int j=0; j!=(*i)->TermCount(); ++j) {*(iterm++)=0.0;}
            }
        }

        for(TransportPtrVector::const_iterator i=m_transports.begin();
            (i!=m_transports.end()) && (iterm!=terms.end()); ++i) {
            if (!(*i)->IsDeferred()) {
                // Calculate rate if not deferred.
                sum += (*i)->RateTerms(t, sys, local_geom, iterm);
            } else {
                // If process is deferred, then set rate to zero.
                for (unsigned int j=0; j!=(*i)->TermCount(); ++j) {*(iterm++)=0.0;}
            }
        }
    } else {
        // Fill vector with zeros.
        for(PartProcPtrVector::const_iterator i=m_processes.begin();
            (i!=m_processes.end()) && (iterm!=terms.end()); ++i) {
            fill(iterm, iterm+(*i)->TermCount(), 0.0);
            iterm += (*i)->TermCount();
        }
        for(TransportPtrVector::const_iterator i=m_transports.begin();
            (i!=m_transports.end()) && (iterm!=terms.end()); ++i) {
            fill(iterm, iterm+(*i)->TermCount(), 0.0);
            iterm += (*i)->TermCount();
        }
    }

    // Get coagulation rate.
    sum += Coagulation::CalcRateTerms(t, sys, m_coags, iterm);

    // Get birth and death rates from the Cell.
    const BirthPtrVector &inf = sys.Inflows();
    for (BirthPtrVector::const_iterator j=inf.begin(); j!=inf.end(); ++j) {
        sum += (*j)->RateTerms(t, sys, iterm);
    }
    const DeathPtrVector &out = sys.Outflows();
    for (DeathPtrVector::const_iterator j=out.begin(); j!=out.end(); ++j) {
        sum += (*j)->RateTerms(t, sys, iterm);
    }

    return sum;
}

/*!
 * LPDA allows some processes to be deferred and removed from the main simulation
 * loop in a kind of splitting.  This method calculates the combined rate of all
 * the processes that are deferred in this way.  The result is the expected number
 * of events per second in sys, the second argument.  The result will therefore
 * scale linearly with the sample volume of sys, if particle concentrations are
 * held constant.
 *
 *@param[in]    t           Time at which to calculate the rates
 *@param[in]    sys         System for which rates are to be calculated
 *@param[in]    local_geom  Information on location of surrounding cells
 *@param[out]   terms       Vector to fill with the terms contributing to the summed rate
 *
 *@return   The total rate of all deferred processes
 */
real Mechanism::CalcDeferredRateTerms(real t, const Cell &sys, const Geometry::LocalGeometry1d& local_geom, fvector &terms) const
{
    // This routine only calculates the rates of those processes which are
    // deferred.  The rate terms of non-deferred processes are returned
    // as zero.

    // Ensure vector is the correct length and full of zeros.
    fill(terms.begin(), terms.end(), 0.0);
    terms.resize(m_termcount+sys.InflowCount()+sys.OutflowCount(), 0.0);
    fvector::iterator iterm = terms.begin();

    real sum = 0.0;

    // Query other processes for their rates.
    if (sys.ParticleCount() > 0) {
        for(PartProcPtrVector::const_iterator i=m_processes.begin();
            (i!=m_processes.end()) && (iterm!=terms.end()); ++i) {
            if ((*i)->IsDeferred()) {
                // Calculate rate if not deferred.
                sum += (*i)->RateTerms(t, sys, iterm);
            }
        }

        for(TransportPtrVector::const_iterator i=m_transports.begin();
            (i!=m_transports.end()) && (iterm!=terms.end()); ++i) {
            if ((*i)->IsDeferred()) {
                // Calculate rate if not deferred.
                sum += (*i)->RateTerms(t, sys, local_geom, iterm);
            }
        }
    }

    return sum;
}

// Calculates the rates-of-change of the chemical species fractions,
// gas-phase temperature and density due to particle processes.
void Mechanism::CalcGasChangeRates(real t, const Cell &sys, fvector &rates) const
{
    // Resize vector to hold all species and set all rates to zero.
    rates.resize(m_species->size()+2, 0.0);
    fill(rates.begin(), rates.end(), 0.0);
    fvector::iterator idrho = rates.begin() + (rates.size()-1);

    // Precalculate parameters.
    real invVolNA = 1.0 / (sys.SampleVolume() * NA);

    // Inceptions and surface processes can affect the gas-phase chemistry
    // at the moment.

    // Loop over the contributions of all inception processes.
    for (IcnPtrVector::const_iterator i=m_inceptions.begin();
         i!=m_inceptions.end(); ++i) {

        // Calculate the inception rate.
        real rate = (*i)->Rate(t, sys);

        // Loop over all reactants, subtracting their contributions.
        for (Sprog::StoichMap::const_iterator j=(*i)->Reactants().begin();
             j!=(*i)->Reactants().end(); ++j) {
            real dc = rate * (real)j->second * invVolNA;
            rates[j->first] -= dc;
            *idrho -= dc;
        }

        // Loop over all products, adding their contributions.
        for (Sprog::StoichMap::const_iterator j=(*i)->Products().begin();
             j!=(*i)->Products().end(); ++j) {
            real dc = rate * (real)j->second * invVolNA;
            rates[j->first] += dc;
            *idrho += dc;
        }
    }

    // Loop over the contributions of all other processes (except coagulation and transport).
    for (PartProcPtrVector::const_iterator i=m_processes.begin();
         i!=m_processes.end(); ++i) {

        // Calculate the process rate.
        real rate = (*i)->Rate(t, sys);

        // Loop over all reactants, subtracting their contributions.
        for (Sprog::StoichMap::const_iterator j=(*i)->Reactants().begin();
             j!=(*i)->Reactants().end(); ++j) {
            real dc = rate * (real)j->second * invVolNA;
            rates[j->first] -= dc;
            *idrho -= dc;
        }

        // Loop over all products, adding their contributions.
        for (Sprog::StoichMap::const_iterator j=(*i)->Products().begin();
             j!=(*i)->Products().end(); ++j) {
            real dc = rate * (real)j->second * invVolNA;
            rates[j->first] += dc;
            *idrho += dc;
        }
    }

    // Now convert to changes in mole fractions.
    real invrho = 1.0 / sys.Density();
    for (unsigned int k=0; k!=m_species->size(); ++k) {
        rates[k] = (invrho * rates[k]) - (invrho * sys.MoleFraction(k) * (*idrho));
    }
}


// PERFORMING THE PROCESSES.

/*!
 * Performs the Process specified.  Process index could be
 * an inception, particle process or a coagulation event.
 *
 * \param[in]       i           Index of process to perform
 * \param[in]       t           Time at which event is to take place
 * \param[in,out]   sys         System in which event is to take place
 * \param[in]       local_geom  Information on surrounding cells for use with transport processes
 * \param[in,out]   rand_int    Pointer to function that generates uniform integers on a range
 * \param[in,out]   rand_u01    Pointer to function that generates U[0,1] deviates
 * \param[out]      out         Handle particles removed from the system by transport processes
 *
 * The support for transport processes may well no longer be needed, in that it is
 * rarely efficient to simulate such phenomena with stochastic jumps.
 */
void Mechanism::DoProcess(unsigned int i, real t, Cell &sys,
                          const Geometry::LocalGeometry1d& local_geom,
                          int (*rand_int)(int, int), real (*rand_u01)(),
                          Transport::TransportOutflow *out) const
{
    // Test for now
    assert(sys.ParticleModel() != NULL);

    // Work out to which process this term belongs.
    int j = i - m_inceptions.size();

    if (j < 0) {
        // This is an inception process.
        m_inceptions[i]->Perform(t, sys, local_geom, 0, rand_int, rand_u01, out);
        m_proccount[i] += 1;
    } else {
        // This is another process.
        for(PartProcPtrVector::const_iterator ip=m_processes.begin(); ip!=m_processes.end(); ++ip) {
            if (j < (int)(*ip)->TermCount()) {
                // Do the process.
                if ((*ip)->Perform(t, sys, local_geom, j, rand_int, rand_u01, out) == 0) {
                    m_proccount[i] += 1;
                } else {
                    m_fictcount[i] += 1;
                }
                return;
            } else {
                j -= (*ip)->TermCount();
            }
        }

        // See if this is a transport process.
        for(TransportPtrVector::const_iterator it = m_transports.begin(); it != m_transports.end(); ++it) {
            if (j < static_cast<int>((*it)->TermCount())) {
                // Do the process.
                if ((*it)->Perform(t, sys, local_geom, j, rand_int, rand_u01, out) == 0) {
                    m_proccount[i] += 1;
                } else {
                    m_fictcount[i] += 1;
                }
                return;
            } else {
                j -= (*it)->TermCount();
            }
        }

        // We are here because the process was neither an inception
        // nor a single particle process.  It is therefore either a
        // coagulation or a birth/death process.
        for(CoagPtrVector::const_iterator it = m_coags.begin(); it != m_coags.end(); ++it) {
            // Check if coagulation process.
            if (j < static_cast<int>((*it)->TermCount())) {
                // This is the coagulation process.
                if ((*it)->Perform(t, sys, local_geom, j, rand_int, rand_u01, out) == 0) {
                    m_proccount[i] += 1;
                } else {
                    m_fictcount[i] += 1;
                }
                return;
            } else {
                // This must be the birth/death process.
                j -= (*it)->TermCount();
            }
        }

        // The coagulation process is undefined, so it
        // must be either a birth or death process from
        // the cell.
        if ((j < (int)sys.InflowCount()) && (j>=0)) {
            // An inflow process.
            sys.Inflows(j)->SetMechanism(*this);
            sys.Inflows(j)->Perform(t, sys, local_geom, 0, rand_int, rand_u01, out);
        } else {
            j -= sys.InflowCount();
            if ((j < (int)sys.OutflowCount()) && (j>=0)) {
                // An outflow process.
                sys.Outflows(j)->SetMechanism(*this);
                sys.Outflows(j)->Perform(t, sys, local_geom, 0, rand_int, rand_u01, out);
            }
        }
    }
}


// LINEAR PROCESS DEFERMENT ALGORITHM.

// Performs linear update algorithm on the
// given system up to given time.
/*!
 * Performs linear process updates on all particles in a system.
 *
 *@param[in,out]    sys         System containing particles to update
 *@param[in]         t           Time upto which particles to be updated
 *@param[in,out]   rand_int    Pointer to function that generates uniform integers on a range
 *@param[in,out]    rand_u01    Pointer to function that generates U[0,1] deviates
 */
void Mechanism::LPDA(real t, Cell &sys, int (*rand_int)(int, int), real(*rand_u01)()) const
{
    // Check that there are particles to update and that there are
    // deferred processes to perform.
    if ((sys.ParticleCount() > 0) &&
        (m_anydeferred || (AggModel() == AggModels::PAH_ID))) {
        // Stop ensemble from doubling while updating particles.
        sys.Particles().FreezeDoubling();

        // Perform deferred processes on all particles individually.
        Ensemble::iterator i;
        for (i=sys.Particles().begin(); i!=sys.Particles().end(); ++i) {
            UpdateParticle(*(*i), sys, t, rand_u01);
        }

        // Perform deferred processes on secondary particles
        for(unsigned int k=0; k != sys.Particles().SecondaryCount(); ++k) {
            Particle* const sp = sys.Particles().SecondaryParticleAt(k);
            UpdateParticle(*sp, sys, t, rand_u01);

            // See if the particle has to move to the main population
            if(!isSecondary(*sp)) {
                sys.AddParticle(sp, 1.0 / sys.SecondarySampleVolume(), rand_int, rand_u01);
                sys.Particles().RemoveSecondaryParticle(k, false);

                // A different particle will now be at position k so the
                // check needs to be repeated (note the ++k at the end of the loop)
                --k;
            }
        }

        // Now remove any invalid particles and update the ensemble.
        sys.Particles().RemoveInvalids();

        // Start particle doubling again.  This will also double the ensemble
        // if too many particles have been removed.
        sys.Particles().UnfreezeDoubling();
    }
}


/*!
 * Performs linear process updates on a particle in the given system.
 *
 *@param[in,out]    sp          Particle to update
 *@param[in,out]    sys         System containing particle to update
 *@param[in]         t           Time upto which particle to be updated
 *@param[in,out]    rand_u01    Pointer to function that generates U[0,1] deviates
 */
void Mechanism::UpdateParticle(Particle &sp, Cell &sys, real t, real(*rand_u01)()) const
{
    // Deal with the growth of the PAHs
    if (AggModel() == AggModels::PAH_ID)
    {
        // If the agg model is PAH_ID then all the primary
        // particles must be PAHPrimary.
        AggModels::PAHPrimary *pah =
                dynamic_cast<AggModels::PAHPrimary*>(sp.Primary());

        // Look up new size of PAHs in database
        pah->UpdatePAHs(t, *this);
        pah->UpdateCache();
        pah->CheckCoalescence();
        if (sp.IsValid())
            sp.UpdateCache();

    }
    // If there are no deferred processes then stop right now.
    if (m_anydeferred) {
        PartProcPtrVector::const_iterator i;
        unsigned int num;
        real rate, dt;

        while ((sp.LastUpdateTime() < t) && sp.IsValid()) {
            // Calculate delta-t and update particle time.
            dt = t - sp.LastUpdateTime();
            sp.SetTime(t);

            // Loop through all processes, performing those
            // which are deferred.
            for (i=m_processes.begin(); i!=m_processes.end(); ++i) {
                if ((*i)->IsDeferred()) {
                    // Get the process rate x the time interval.
                    rate = (*i)->Rate(t, sys, sp) * dt;

                    // Use a Poission deviate to calculate number of
                    // times to perform the process.
                    num = ignpoi(rate, rand_u01);

                    if (num > 0) {
                        // Do the process to the particle.
                        (*i)->Perform(t, sys, sp, num);
                    }
                }
            }

            // Perform sintering update.
            if (m_sint_model.IsEnabled()) {
//				sp.UpdateFreeSurface();
			    sp.Sinter(dt, sys, m_sint_model);
				//sp.CreateTestTree();
				//	 sp.FindRoot()->CheckTree();
			    // cout << "check before sinter passed\n";

    			//	sp.FindRoot()->CheckTree();
				// cout << "check after sinter passed\n";
				//added to test the 3-d output
            }
        }

        // Check that the particle is still valid, only calculate
        // cache if it is.
        if (sp.IsValid())
            sp.UpdateCache();
    }
}

// READ/WRITE/COPY.

// Creates a copy of the mechanism.
Mechanism *const Mechanism::Clone(void) const
{
    return new Mechanism(*this);
}

// Writes the object to a binary stream.
void Mechanism::Serialize(std::ostream &out) const
{
    const unsigned int trueval  = 1;
    const unsigned int falseval = 0;

    if (out.good()) {
        // Output the version ID (=0 at the moment).
        const unsigned int version = 0;
        out.write((char*)&version, sizeof(version));

        // Write particle model base class.
        ParticleModel::Serialize(out);

        // Write if any processes are deferred.
        if (m_anydeferred) {
            out.write((char*)&trueval, sizeof(trueval));
        } else {
            out.write((char*)&falseval, sizeof(falseval));
        }

        // Write number of inceptions.
        unsigned int n = (unsigned int)m_inceptions.size();
        out.write((char*)&n, sizeof(n));

        // Write inceptions.
        for (IcnPtrVector::const_iterator i=m_inceptions.begin();
             i!=m_inceptions.end(); ++i) {
            ProcessFactory::Write(*(*i), out);
        }

        // Write number of particle processes.
        n = (unsigned int)m_processes.size();
        out.write((char*)&n, sizeof(n));

        // Write particle processes.
        for (PartProcPtrVector::const_iterator i=m_processes.begin();
             i!=m_processes.end(); ++i) {
            ProcessFactory::Write(*(*i), out);
        }

        //@TODO transport processes

        // Coagulation
        n = m_coags.size();
        out.write(reinterpret_cast<const char*>(&n), sizeof(n));
        for (CoagPtrVector::const_iterator i = m_coags.begin();
             i != m_coags.end(); ++i) {
            ProcessFactory::Write(*(*i), out);
        }

        // Write index of first coag process.
        int m = (int)m_icoag;
        out.write((char*)&m, sizeof(m));

        // Write term count.
        n = (unsigned int)m_termcount;
        out.write((char*)&n, sizeof(n));

        // Write process count.
        n = (unsigned int)m_processcount;
        out.write((char*)&n, sizeof(n));
    } else {
        throw invalid_argument("Output stream not ready "
                               "(Sweep, Mechanism::Serialize).");
    }
}

// Reads the object from a binary stream.
void Mechanism::Deserialize(std::istream &in)
{
    releaseMem();

    if (in.good()) {
        // Read the output version.  Currently there is only one
        // output version, so we don't do anything with this variable.
        // Still needs to be read though.
        unsigned int version = 0;
        in.read(reinterpret_cast<char*>(&version), sizeof(version));

        int m=0;
        unsigned int n=0;

        switch (version) {
            case 0:
                // Read ParticleModel base class.
                ParticleModel::Deserialize(in);

                // Read if any processes are deferred.
                in.read(reinterpret_cast<char*>(&n), sizeof(n));
                m_anydeferred = (n==1);

                // Read number of inceptions.
                in.read(reinterpret_cast<char*>(&n), sizeof(n));

                // Read inceptions.
                for (unsigned int i=0; i!=n; ++i) {
                    Inception *icn = ProcessFactory::ReadInception(in, *this);
                    icn->SetMechanism(*this);
                    m_inceptions.push_back(icn);
                }

                // Read number of particle processes.
                in.read(reinterpret_cast<char*>(&n), sizeof(n));

                // Read particle processes.
                for (unsigned int i=0; i!=n; ++i) {
                    ParticleProcess *p = ProcessFactory::ReadPartProcess(in, *this);
                    p->SetMechanism(*this);
                    m_processes.push_back(p);
                }

                //@TODO transport processes

                // Read coagulation process.
                in.read(reinterpret_cast<char*>(&n), sizeof(n));
                for(unsigned int i = 0; i != n; ++i) {
                    Coagulation *pCoag = ProcessFactory::ReadCoag(in, *this);
                    pCoag->SetMechanism(*this);
                    m_coags.push_back(pCoag);
                }

                // Read index of first coag process.
                in.read(reinterpret_cast<char*>(&m), sizeof(m));
                m_icoag = m;

                // Read term count.
                in.read(reinterpret_cast<char*>(&n), sizeof(n));
                m_termcount = n;

                // Read process count.
                in.read(reinterpret_cast<char*>(&n), sizeof(n));
                m_processcount = n;

                break;
            default:
                throw runtime_error("Serialized version number is invalid "
                                    "(Sweep, Mechanism::Deserialize).");
        }
    } else {
        throw invalid_argument("Input stream not ready "
                               "(Sweep, Mechanism::Deserialize).");
    }
}


// MEMORY MANAGEMENT.

// Clears the current mechanism from memory.
void Mechanism::releaseMem(void)
{
    // Clear base class.
    ParticleModel::releaseMem();

    // Delete inceptions.
    for (IcnPtrVector::iterator i=m_inceptions.begin();
         i!=m_inceptions.end(); ++i) {
        delete *i;
    }
    m_inceptions.clear();

    // Delete processes.
    for (PartProcPtrVector::iterator i=m_processes.begin();
         i!=m_processes.end(); ++i) {
        delete *i;
    }
    m_processes.clear();

    // Delete transport processes.
    for (TransportPtrVector::iterator i=m_transports.begin();
         i!=m_transports.end(); ++i) {
        delete *i;
    }
    m_transports.clear();

    // Delete coagulation processes.
    for (CoagPtrVector::iterator i = m_coags.begin();
         i != m_coags.end(); ++i) {
        delete *i;
    }
    m_coags.clear();
    m_icoag = -1;

    m_anydeferred = false;
    m_termcount = 0;
    m_processcount = 0;
    m_proccount.clear();
    m_fictcount.clear();
}