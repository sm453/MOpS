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
#include "swp_sprog_idealgas_wrapper.h"
#include "swp_fixed_mixture.h"

#include <stdexcept>

#include <boost/random/uniform_01.hpp>
#include <boost/random/mersenne_twister.hpp>

// aab64 OMP threads
typedef boost::mt19937 RandNumGen;
#include <omp.h>

using namespace std;

namespace Sweep {

// CONSTRUCTORS AND DESTRUCTORS.

// Default constructor (public).
Cell::Cell(const Sweep::ParticleModel &model, const bool const_gas)
: m_ensemble(), m_model(&model),
  m_smpvol(1.0), m_fixed_chem(false), 
  m_incepting_weight(1.0), m_incFactor(1.0), 
  m_notpsiflag(true), m_rateFactor(1.0), 
  m_cprop1(iUniform), m_cprop2(iUniform), 
  m_incepted(0.0), m_inceptions(0), m_inceptingcoagulations(0), 
  m_surface_rutiles(0), m_last_update_time(0), m_inceptions_tmp(0), m_inceptingcoagulations_tmp(0),
  m_diam_tmp(0), m_diam2_tmp(0), m_diam_1_tmp(0), m_diam_2_tmp(0), m_mass_1_2_tmp(0), m_diam2_mass_1_2_tmp(0),
  m_d0sum(0), m_disum(0), m_d0sum_sqrd(0), m_disum_sqrd(0),
  m_m0k(0), m_m1k(0), m_m2k(0), m_m3k(0), m_SGk(0), m_SGadjustment(0)
{
    if(const_gas)
        m_gas = new Sweep::FixedMixture(fvector(7 + model.Species()->size()), *model.Species());
    else
        m_gas = new Sweep::SprogIdealGasWrapper(*model.Species());
	assert(isValid());
}

// Copy constructor.
Cell::Cell(const Cell &copy)
: m_gas(copy.m_gas->Clone())
{
    *this = copy;
	assert(isValid());
}

// Stream-reading constructor.
Cell::Cell(std::istream &in, const Sweep::ParticleModel &model)
: m_gas(new Sweep::SprogIdealGasWrapper(*model.Species()))
{
    Deserialize(in, model);
	assert(isValid());
}

// Default destructor.
Cell::~Cell(void)
{
	assert(isValid());
    delete m_gas;
}


// OPERATOR OVERLOADS.

// Assignment operator.
Cell &Cell::operator=(const Sweep::Cell &rhs)
{
    if (this != &rhs) {
		//assert(rhs.isValid());
        delete m_gas;
        m_gas        = rhs.m_gas->Clone();
        m_ensemble   = rhs.m_ensemble;
        m_model      = rhs.m_model;
        m_smpvol     = rhs.m_smpvol;
        m_fixed_chem = rhs.m_fixed_chem;
        m_inflow     = rhs.m_inflow;
        m_outflow    = rhs.m_outflow;

	    // aab64 added variables
		// particle temperature
	    m_bulk_particle_temp = rhs.m_bulk_particle_temp;
	    // current process time step
	    m_proc_tau = rhs.m_proc_tau;
	    // flag for adiabatic operation
	    m_adiabatic_flag = rhs.m_adiabatic_flag;
		// incepting particle weight for bintree model
		m_incepting_weight = rhs.m_incepting_weight;
		// incepting composition adjustment factor
		m_incFactor = rhs.m_incFactor;
		// PSI flag
		m_notpsiflag = rhs.m_notpsiflag;
		// openmp rng
		m_prng = rhs.m_prng;
		// coagulation scaling for weighted events
		m_rateFactor = rhs.m_rateFactor;
		// coagulation properties for PSI
		m_cprop1 = rhs.m_cprop1;
		m_cprop2 = rhs.m_cprop2;
		// incepted tracker
		m_incepted = rhs.m_incepted;
		m_inceptions = rhs.m_inceptions;
		m_inceptingcoagulations = rhs.m_inceptingcoagulations;
		m_last_update_time = rhs.m_last_update_time;
		m_surface_rutiles = rhs.m_surface_rutiles;
		m_inceptions_tmp = rhs.m_inceptions_tmp;
		m_inceptingcoagulations_tmp = rhs.m_inceptingcoagulations_tmp;
		// incepting class averages
		m_diam_tmp = rhs.m_diam_tmp;
		m_diam2_tmp = rhs.m_diam2_tmp;
		m_diam_1_tmp = rhs.m_diam_1_tmp;
		m_diam_2_tmp = rhs.m_diam_2_tmp;
		m_mass_1_2_tmp = rhs.m_mass_1_2_tmp;
		m_diam2_mass_1_2_tmp = m_diam2_mass_1_2_tmp;
		// incepting class moments and contributing terms
		m_d0sum = rhs.m_d0sum;
		m_disum = rhs.m_disum;
		m_d0sum_sqrd = rhs.m_d0sum_sqrd;
		m_disum_sqrd = rhs.m_disum_sqrd;
		m_m0k = rhs.m_m0k;
		m_m1k = rhs.m_m1k;
		m_m2k = rhs.m_m2k;
		m_m3k = rhs.m_m3k;
		m_SGk = rhs.m_SGk;
		m_SGadjustment = rhs.m_SGadjustment;
		
	}
	assert(isValid());
    return *this;
}

/*!
 * @param os    Output stream
 * @param net   Cell object to print
 * @return      Output stream
 */
std::ostream& operator<<(
        std::ostream &os,
        const Sweep::Cell &c)
{
  os << "[Sweep::Cell]\n";
  if (&(c.Particles()) != NULL) os << " with " << c.Particles();
  os << " with [EnvInterface]," <<
          " T=" << c.GasPhase().Temperature() <<
          " P=" << c.GasPhase().Pressure() <<
          " SP0=" << c.GasPhase().SpeciesConcentration(0) <<
          " \n";
  assert(c.isValid());
  return os;
}


// THE PARTICLE ENSEMBLE.

// Returns the particle ensemble.
Ensemble &Cell::Particles(void) {return m_ensemble;}
const Ensemble &Cell::Particles(void) const {return m_ensemble;}

// Returns the particle count.
unsigned int Cell::ParticleCount(void) const
{
	assert(isValid());
    return m_ensemble.Count();
}

/**
 * Initialise the ensemble to hold particles of the type specified
 * by the model and containing the particular particles contained
 * in the range [particle_list_begin, particle_list_end) and set
 * the sample volume to achieve the statistical weight.
 *
 *
 *@param[in]        particle_list_begin     Iterator to first in range of particle pointers to insert
 *@param[in]        particle_list_end       Iterator to one past end of range of particle pointers to insert
 *@param[in]        statistical_weight      Number of physical particles represented by each computational particle
 *@param[in,out]    rng                     Random number generator
 */
void Cell::SetParticles(
        std::list<Particle*>::iterator particle_list_begin,
        std::list<Particle*>::iterator particle_list_end,
        double statistical_weight, rng_type &rng)
{
    assert(statistical_weight > 0);
    // This puts the particles into the ensemble and clears any scaling
    // stored inside the ensemble
    m_ensemble.SetParticles(particle_list_begin, particle_list_end, rng);

    m_smpvol = 1.0 / statistical_weight;
	assert(isValid());
}

/**
 * Initialise the ensemble to hold particles of the type specified
 * by the model and containing the particular particles contained
 * in the range [particle_list_begin, particle_list_end) and set
 * the sample volume to achieve the statistical weight.
 *
 *
 *@param[in]        particle_list_begin     Iterator to first in range of particle pointers to insert
 *@param[in]        particle_list_end       Iterator to one past end of range of particle pointers to insert
 *@param[in]        statistical_weight      Number of physical particles represented by each computational particle
 *
 *@pre  The length of the input sequence is at most m_ensemble.capacity()
 */
void Cell::SetParticles(
        std::list<Particle*>::iterator particle_list_begin,
        std::list<Particle*>::iterator particle_list_end,
        double statistical_weight)
{
    assert(statistical_weight > 0);
    // This puts the particles into the ensemble and clears any scaling
    // stored inside the ensemble
    if(std::distance(particle_list_begin, particle_list_end) > m_ensemble.Capacity())
        throw std::runtime_error("Attempt to set too many particles in Sweep::Cell::SetParticles");

    // The rng will not be used, so just provide a default constructed object
    rng_type rng;
    const rng_type rngCopy(rng);
    m_ensemble.SetParticles(particle_list_begin, particle_list_end, rng);
    assert(rngCopy == rng);

    m_smpvol = 1.0 / statistical_weight;
	assert(isValid());
}

// Returns particle statistics.
void Cell::GetVitalStats(Stats::EnsembleStats &stats) const
{
    stats.Calculate(m_ensemble, 1.0/SampleVolume());
}


// SCALING ROUTINES INCL. SAMPLE VOLUME.

// Returns the double system to stochastic system scaling factor.
double Cell::SampleVolume() const
{
    return m_smpvol * m_ensemble.Scaling();
}

/*!
 * Set the number density which the ensemble represents including the
 * effects of ensemble doublings and contractions.
 *
 *@param[in]    scale_factor    Ratio of new to old sample volumes
 *
 *@pre      scale_factor > 0
 */
void Cell::AdjustSampleVolume(double scale_factor)
{
    assert(scale_factor > 0);
    assert(scale_factor <= std::numeric_limits<double>::max());
    m_smpvol = SampleVolume() * scale_factor;

    // The effects of ensemble rescalings are now incorporated in this sample
    // volume.
    m_ensemble.ResetScaling();
	assert(isValid());
}

unsigned int Cell::NumOfStartingSpecies(const int index) const 
{
    // figure out how many starting PAHs are supposed in the particle ensemble
    // N = NA*Vsmpl*molar density*volume fraction of starting PAH.
    unsigned int m_amount = NA * SampleVolume() * GasPhase().SpeciesConcentration(index);
    return m_amount;
}

/**
 * Clear any particles and set the sample volume so that a full ensemble
 * (of m_ensemble.Capacity() particles) has the specified m0.
 *
 *@param[in]    m0              Particle number density for full ensemble (units \f$\mathrm{m}^{-3}\f$)
 */
void Cell::Reset(const double m0)
{
    assert(m0 > 0.0);
    m_ensemble.Clear();
    m_ensemble.ResetScaling();

    if ((m_ensemble.Capacity() > 0) && (m0 > 0.0)) {
        m_smpvol = m_ensemble.Capacity() / m0;
    }
    else {
        // The ensemble has not yet been initialised
        m_smpvol = 1.0;
    }
	assert(isValid());
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
void Cell::AddOutflow(double rate, const Sweep::Mechanism &mech)
{
    Processes::DeathProcess *death = new Processes::DeathProcess(mech);
    death->SetA(rate);
    m_outflow.push_back(death);
}

// aab64 Get thread specific rng
void Cell::Setprng(size_t seedval)
{
	size_t numthreads = omp_get_max_threads();
	m_prng.resize(numthreads);
	for (size_t i = 0; i < numthreads; i++)
	{
		m_prng[i].seed(i*seedval);
	}
}

// aab64 Get thread specific rng
RandNumGen Cell::Chooseprng(size_t threadid)
{
	return m_prng[threadid];
}

// aab64 Particle hybrid model
void Cell::AdjustIncepted(double adjustment) 
{ 
	m_incepted += adjustment; 
	m_ensemble.AdjustIncepted(adjustment);
}

// aab64 Set incepting class average properties using closed form moments for the diameter distribution
void Cell::SetDistParams(double mu, double sigma)
{
	double rhop = 4260.0; // titania
	double c1 = sigma * sigma * 0.5;
	double c2 = std::sqrt(6.0 / (rhop * PI));

	m_diam_tmp = exp(c1 + mu);
	m_diam2_tmp = exp(c1 * 4.0 + 2.0 * mu);
	m_diam_1_tmp = exp(c1 - mu);
	m_diam_2_tmp = exp(c1 * 4.0 - 2.0 * mu);
	m_mass_1_2_tmp = exp(c1 * 2.25 - 1.5 * mu) * c2;
	m_diam2_mass_1_2_tmp = exp(c1 * 0.25 + 0.5 * mu) * c2;
}

// aab64 Set the first moments of the diameter distribution
void Cell::SetMomentsk(double m0k, double m1k, double m2k, double m3k)
{
	m_m0k = m0k;
	m_m1k = m1k;
	m_m2k = m2k;
	m_m3k = m3k;
}

// aab64 Track increase in total diameter and total diameter squared
// due to inception events for moment calculation
void Cell::SetInceptionSums(double d0)
{
	m_d0sum += d0;
	m_d0sum_sqrd += (d0 * d0);
}

// aab64 Track increase in total diameter and total diameter squared
// due to coagulation events for moment calculation
void Cell::SetCoagulationSums(double di)
{
	m_disum += di;
	m_disum_sqrd += (di * di);
}

// aab64 Reset diameter counters
void Cell::ResetInceptionSums()
{
	m_d0sum = 0.0;
	m_d0sum_sqrd = 0.0;
}
void Cell::ResetCoagulationSums()
{
	m_disum = 0.0;
	m_disum_sqrd = 0.0;
}

// READ/WRITE/COPY.

/*!
 * Writes the object to a binary stream.
 *
 *@param[in,out]    out     output stream
 */
void Cell::Serialize(std::ostream &out) const
{
	assert(isValid());
    if (out.good()) {
        // Output the gas mixture
        // First check how the mixture is stored
        const SprogIdealGasWrapper *pSprogWrapper = dynamic_cast<const SprogIdealGasWrapper*>(m_gas);
        const FixedMixture *pFixedMix = dynamic_cast<const FixedMixture*>(m_gas);

        // Now serialise with a label to say what type of object has been serialised
        // so that it can be correctly deserialised.
        if(NULL != pSprogWrapper) {
            const int sprogWrapperID = 1;
            out.write(reinterpret_cast<const char *>(&sprogWrapperID), sizeof(sprogWrapperID));
            pSprogWrapper->Serialize(out);
        }
        else if(NULL != pFixedMix) {
            const int FixedMixID = 2;
            out.write(reinterpret_cast<const char *>(&FixedMixID), sizeof(FixedMixID));
            throw std::logic_error("Serialisation of Sweep::FixedMixture not supported in Sweep::Cell::Serialize");
        }
        else {
            throw std::logic_error("Unsupported gas phase mixture type in Sweep::Cell::Serialize");
        }

        // Output the sample volume.
        double v = (double)m_smpvol;
        out.write((char*)&v, sizeof(v));

        // Output if fixed chem.
        out.write((char*)&m_fixed_chem, sizeof(m_fixed_chem));

        // Output the ensemble.
        m_ensemble.Serialize(out);
    } else {
        throw invalid_argument("Output stream not ready "
                               "(Sweep, Cell::Serialize).");
    }
}

/*!
 * Read object state from a binary stream.
 *
 *@param[in,out]        in      Stream from which to read
 *@param[in]            model   Model for interpreting the particles in the cell
 *
 *@pre  m_gas is initialised
 */
void Cell::Deserialize(std::istream &in, const Sweep::ParticleModel &model)
{
    if (in.good()) {

        // Find out what kind of mixture to read, see Serialize() method for possible values
        int mixID;
        in.read(reinterpret_cast<char *>(&mixID), sizeof(mixID));

        switch(mixID) {
            case 1:
            {
                //SprogIdealGasWrapper
                Sweep::SprogIdealGasWrapper *pGas = new Sweep::SprogIdealGasWrapper(*model.Species());
                pGas->Implementation()->Deserialize(in);
                // Set the species, because the pointer gets reset in Sprog::Mixture::Deserialize
                pGas->Implementation()->SetSpecies(*model.Species());
                m_gas = pGas;
                break;
            }
            case 2:
                // FixedMixture - not currently supported
                throw std::logic_error("Deserialisation of Sweep::FixedMixture not supported in Sweep::Cell::Deserialize");
                break;
            default:
                throw std::logic_error("Unrecognised gas-phase mixture type in Sweep::Cell::Deserialize");
                break;
        }

        // Read the sample volume.
        in.read(reinterpret_cast<char*>(&m_smpvol), sizeof(m_smpvol));

        // Read if fixed chem.
        in.read(reinterpret_cast<char*>(&m_fixed_chem), sizeof(m_fixed_chem));

        // Read the ensemble.
        m_ensemble.Deserialize(in, model);

    } else {
        throw invalid_argument("Input stream not ready "
                               "(Sweep, Cell::Deserialize).");
    }
}

// aab64 check cell consistency, implemented to ensure gas phase mixture pointers are never NULL 
bool Cell::isValid() const {
	return m_gas != NULL; 
}

} // Sweep namespace
