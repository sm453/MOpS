#include "gpc_params.h"
#include "gpc_mixture.h"
#include <vector>

using namespace Sprog;
using namespace Sprog::Thermo;
using namespace std;

// CONSTRUCTORS AND DESTRUCTORS.

// Default constructor (private).
Mixture::Mixture(void)
{
    m_data.clear();
    m_pT      = NULL;
    m_pdens   = NULL;
    m_species = NULL;
}

// Default constructor (public, requires species list).
Mixture::Mixture(const SpeciesPtrVector &sp)
{
    SetSpecies(sp);
}

// Copy constructor.
Mixture::Mixture(const Mixture &copy)
{
    *this = copy;
}


// Stream-reading constructor.
Mixture::Mixture(std::istream &in, const SpeciesPtrVector &sp)
{
    Deserialize(in);
    SetSpecies(sp);
}

// Default destructor.
Mixture::~Mixture(void)
{
    m_data.clear();
}


// OPERATOR OVERLOADING.

// Assignment operator.
Mixture &Mixture::operator=(const Mixture &mix)
{
    // Check for self assignment.
    if (this != &mix) {
        m_data.assign(mix.m_data.begin(), mix.m_data.end());
        m_species = mix.m_species;
        m_pT      = &m_data.at(m_species->size()-2);
        m_pdens   = &m_data.at(m_species->size()-1);
    }

    return *this;
}


// TEMPERATURE.

// Returns the mixture temperature.
real Mixture::Temperature() const
{
    return *m_pT;
}


// Set the mixture temperature.
void Mixture::SetTemperature(Sprog::real T)
{
    *m_pT = T;
}


// CONCENTRATIONS/FRACTIONS.

// Returns the vector of mixture species mole fractions.
const fvector &Mixture::MoleFractions() const
{
    return m_data;
}

// Returns a vector of species concentrations.
void Mixture::GetConcs(fvector &concs) const
{
    // Resize output vector.
    concs.resize(m_species->size());

    // Loop over all mole fractions and convert to concentrations.
    fvector::const_iterator i;
    fvector::iterator j;
    for (i=m_data.begin(),j=concs.begin(); i!=m_data.end()-2; i++,j++) {
        (*j) = (*i) * (*m_pdens);
    }
}

// Returns a vector of species mass fractions.
void Mixture::GetMassFractions(fvector &fracs) const
{
    // Clear output vector.
    fracs.resize(m_species->size());

    // Loop over all mole fractions and convert to mass fractions:
    //   y = x * wt / sum(x*wt)
    int i;
    real val, tot = 0.0;
    for (i=0; i<m_species->size(); i++) {
        val = m_data[i] * (*m_species)[i]->MolWt();
        fracs[i] = val;
        tot += val;
    }
    tot = 1.0 / tot;
    for (i=0; i<m_species->size(); i++) {
        fracs[i] *= tot;
    }
}

// Returns the mole fraction of species i.
real Mixture::MoleFraction(unsigned int i) const
{
    if (i < m_species->size()) {
        return m_data[i];
    } else {
        return 0.0;
    }
}

// Returns the molar concentration of species i.
real Mixture::MolarConc(unsigned int i) const
{
    if (i < m_species->size()) {
        return m_data[i] * (*m_pdens);
    } else {
        return 0.0;
    }
}

// Returns the mass fraction of species i.
real Mixture::MassFraction(unsigned int i) const
{
    if (i < m_species->size()) {
        // Get total x * W.
        real tot = 0.0;
        for (int j=0; j<m_species->size(); j++) {
            tot += m_data[j] * (*m_species)[j]->MolWt();
        }

        // Return mass fraction.
        return m_data[i] * (*m_species)[i]->MolWt() / tot;
    } else {
        return 0.0;
    }
}

// Sets the vector of species mole fractions.
void Mixture::SetFracs(const fvector &fracs)
{
    int i;
    real tot =0.0;

    // Set the mole fractions.
    for (i=0; i<m_species->size(); i++) {
        m_data[i] = fracs[i];
        tot += fracs[i];
    }

    // Ensure that the mole fractions are normalised.
    if (tot != 1.0) {
        for (i=0; i<m_species->size(); i++) {
            m_data[i] /= tot;
        }
    }
}

// Sets the species mole fractions from an array of values.
void Mixture::SetFracs(const Sprog::real fracs[], int n)
{
    // Check that the array of of sufficient length.
    if (n >= m_species->size()) {
        int i;
        real tot = 0.0;

        // Set the fractions.
        for (i=0; i<m_species->size(); i++) {
            m_data[i] = fracs[i];
            tot += fracs[i];
        }

        // Ensure that the mole fractions are normalised.
        if (tot != 1.0) {
            for (i=0; i<m_species->size(); i++) {
                m_data[i] /= tot;
            }
        }
    }
}

// Sets the species mole fractions using the supplied molar concentrations.
void Mixture::SetConcs(const fvector &concs)
{
    // Check that the concentration vector is of sufficient length.
    if (concs.size() >= m_species->size()) {
        int i;

        // Sum up the total concentration.
        *m_pdens = 0.0;
        for (i=0; i<m_species->size(); i++) {
            m_data[i] = concs[i];
            *m_pdens += concs[i];
        }

        // Convert values to mole fractions.
        real invdens = 1.0 / (*m_pdens);
        for (i=0; i<m_species->size(); i++) {
            m_data[i] *= invdens;
        }
    }
}

// Sets the species mole fractions using the supplied mass fractions.
void Mixture::SetMassFracs(const fvector &fracs)
{
    // Check that the mass fraction vector is of sufficient length.
    if (fracs.size() >= m_species->size()) {
        int i;
        real val = 0.0, tot = 0.0, totfrac = 0.0;

        // Check that the fractions are normalised by summing up
        // the total fractions, and dividing the values by this
        // sum in the next step.
        for (i=0; i<m_species->size(); i++) {
            totfrac += fracs[i];
        }

        // Convert to mole fractions:
        //   x = y / (wt * sum(y/wt))
        for (i=0; i<m_species->size(); i++) {
            val = fracs[i] / (totfrac * (*m_species)[i]->MolWt());
            m_data[i] = val;
            tot += val;
        }
        tot = 1.0 / tot;
        for (i=0; i<m_species->size(); i++) {
            m_data[i] *= tot;
        }

    }
}

// Checks the vector of mole fractions for validity by settings all negative
// values to zero, and by normalising the values so that they sum
// to one.
void Mixture::Normalise()
{
    real xtot = 0.0;

    for (int i=0; i<m_species->size(); i++) {
        if (m_data[i] < 0.0) m_data[i] = 0.0;
        xtot += m_data[i];
    }

    if (xtot != 1.0) {
        for (int i=0; i<m_species->size(); i++) {
            m_data[i] /= xtot;
        }
    }
}


// MIXTURE DENSITY.

// Returns the mixture molar density.
real Mixture::Density() const
{
    return *m_pdens;
}

// Returns the mixture mass density.
real Mixture::MassDensity() const
{
    int i;
    real rho = 0.0;

    // Calcualate mass density:
    //   rho_mass = rho_mole * sum(x * wt)
    for (i=0; i<m_species->size(); i++) {
        rho += m_data[i] * (*m_species)[i]->MolWt();
    }
    rho *= (*m_pdens);
    return rho;
}

// Sets the mixture molar density.
void Mixture::SetDensity(Sprog::real dens)
{
    *m_pdens = dens;
}

// Sets the molar density using the supplied mass density.
void Mixture::SetMassDensity(Sprog::real dens)
{
    int i;
    *m_pdens = 0.0;
    
    // Calcualate molar density:
    //   rho_mass = rho_mole * sum(x * wt)
    for (i=0; i<m_species->size(); i++) {
        *m_pdens += m_data[i] * (*m_species)[i]->MolWt();
    }
   *m_pdens = dens / (*m_pdens);
}


// MIXTURE CONTEXT.

// Returns a pointer to the vector of species used to define the mixture.
const SpeciesPtrVector *const Mixture::Species() const
{
    return m_species;
}

// Sets the vector of species used to define the mixture.
void Mixture::SetSpecies(const Sprog::SpeciesPtrVector &sp)
{
    m_species = &sp;
    m_data.resize(m_species->size()+2);
    m_pT    = &m_data.at(m_species->size());
    m_pdens = &m_data.at(m_species->size()+1);
}


// RAW DATA.

real *const Mixture::RawData()
{
    return &(m_data[0]);
}


// READ/WRITE/COPY FUNCTIONS.

// Creates a copy of the mixture object.
Mixture *const Mixture::Clone() const
{
    return new Mixture(*this);
}

// Writes the mixture to a binary data stream.
void Mixture::Serialize(std::ostream &out) const
{
    if (out.good()) {
        // Output the version ID (=0 at the moment).
        const unsigned int version = 0;
        out.write((char*)&version, sizeof(version));

        // Output the data vector size.
        unsigned int sz = m_data.size();
        out.write((char*)&sz, sizeof(sz));

        // Output all elements in the data vector.
        fvector::const_iterator i;
        for (i=m_data.begin(); i!=m_data.end(); i++) {
            out.write((char*)&(*i), sizeof(*i));
        }
    } else {
        throw invalid_argument("Output stream not ready (Sprog, Mixture::Serialize).");
    }
}

// Reads the mixture data from a binary data stream.
void Mixture::Deserialize(std::istream &in)
{
    if (in.good()) {
        // Read the output version.  Currently there is only one
        // output version, so we don't do anything with this variable.
        // Still needs to be read though.
        unsigned int version = 0;
        in.read(reinterpret_cast<char*>(&version), sizeof(version));

        switch (version) {
            case 0:
                // Read the data vector size.
                unsigned int sz;
                in.read(reinterpret_cast<char*>(&sz), sizeof(sz));

                // Fill the data vector.
                real val;
                m_data.reserve(sz);
                for (unsigned int i=0; i<sz; i++) {
                    in.read(reinterpret_cast<char*>(&val), sizeof(val));
                    m_data.push_back(val);
                }

                // The last two elements in the data vector are the temperature
                // and the density respectively.
                m_pT = &m_data[sz-2];
                m_pdens = &m_data[sz-1];

                // The mixture has no species associated it with right now.
                m_species = NULL;
                break;
            default:
                throw runtime_error("Mixture serialized version number "
                                    "is invalid (Sprog, Mixture::Deserialize).");
        }
    } else {
        throw invalid_argument("Input stream not ready (Sprog, Mixture::Deserialize).");
    }
}

// Identifies the mixture type for serialisation.
Serial_MixtureType Mixture::SerialType() const
{
    return Serial_Mixture;
}
