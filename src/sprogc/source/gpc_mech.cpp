/*
  Author(s):      Matthew Celnik (msc37)
  Project:        sprog (gas-phase chemical kinetics).
  Sourceforge:    http://sourceforge.net/projects/mopssuite
  
  Copyright (C) 2008 Matthew S Celnik.

  File purpose:
    Implementation of the Mechanism class declared in the
    gpc_mech.h header file.

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

#include "gpc_mech.h"
#include "gpc_unit_systems.h"
#include <string>
#include <math.h>
#include <stdexcept>
#include <fstream>
#include "string_functions.h"

using namespace Sprog;
using namespace std;
using namespace Strings;

// CONSTRUCTORS AND DESTRUCTORS.

// Default constructor.
Mechanism::Mechanism()
{
    m_units = SI;
    m_stoich_xref_valid = false;
    m_rxns.SetMechanism(*this);
}

// Copy constructor.
Mechanism::Mechanism(const Sprog::Mechanism &mech)
{
    *this = mech;
}

// Destructor.
Mechanism::~Mechanism()
{
    releaseMemory();
}


// OPERATOR OVERLOADING.

// Assignment operator
Mechanism &Mechanism::operator=(const Sprog::Mechanism &mech)
{
    // Check for self assignment!
    if (this != &mech) {
        // Clear current memory.
        releaseMemory();

        // Copy mechanism data.
        m_units = mech.m_units;

        // Copy new elements and species into mechanism.
        copyInElements(mech.m_elements);
        copyInSpecies(mech.m_species);

        // Copy reaction set and stoich cross-referencing.
        m_rxns = mech.m_rxns;
        m_stoich_xref.assign(mech.m_stoich_xref.begin(), mech.m_stoich_xref.end());
        m_stoich_xref_valid = mech.m_stoich_xref_valid;

        // Inform species of new elements vector and mechanism.
        SpeciesPtrVector::iterator sp;
        for (sp=m_species.begin(); sp!=m_species.end(); sp++) {
            (*sp)->SetMechanism(*this);
        }

        // Inform reactions of new species vector and mechanism.
        unsigned int i;
        for (i=0; i<m_rxns.Count(); i++) {
            m_rxns[i]->SetMechanism(*this);
        }
    }
    return *this;
}


// Clears the mechanism of all elements, species and reactions.
void Mechanism::Clear()
{
    releaseMemory();
}


// UNITS.

// Returns the current system of units of this mechanism.
UnitSystem Mechanism::Units() const
{
    return m_units;
}

// Sets the system of units used by this mechanism by converting all 
// element, species and reaction properties.
void Mechanism::SetUnits(Sprog::UnitSystem u)
{
    Kinetics::ARRHENIUS arr;

    // Is new system SI?
    if (u == SI) {
        // Is current system CGS?
        if (m_units == CGS) {
            // Convert elements' mol. weights.
            ElementPtrVector::iterator iel;
            for (iel=m_elements.begin(); iel!=m_elements.end(); iel++) {
                // Convert from g/mol to kg/mol.
                (*iel)->SetMolWt((*iel)->MolWt()*1.0e-3);
            }

            // Scale species' mol. weights.
            SpeciesPtrVector::iterator isp;
            for (isp=m_species.begin(); isp!=m_species.end(); isp++) {
                // Recalculate mol. weight.
                (*isp)->CalcMolWt();
            }

            // Scale reaction coefficients.
            for (unsigned int irxn=0; irxn!=m_rxns.Count(); ++irxn) {
                // Convert volumes from cm3 to m3, and convert energies from ergs/mol to
                // J/mol.

                // Forward rate coefficients.
                arr = m_rxns[irxn]->Arrhenius();
                arr.A *= pow(1.0e-6, m_rxns[irxn]->ReactantStoich()-1.0);
                arr.E *= 1.0e-7;
                m_rxns[irxn]->SetArrhenius(arr);

                // Reverse rate coefficients.
                if (m_rxns[irxn]->RevArrhenius() != NULL) {
                    arr = *(m_rxns[irxn]->RevArrhenius());
                    arr.A *= pow(1.0e-6, m_rxns[irxn]->ProductStoich()-1.0);
                    arr.E *= 1.0e-7;
                    m_rxns[irxn]->SetRevArrhenius(arr);
                }

                // Fall-off parameters.
                if (m_rxns[irxn]->FallOffType() != Kinetics::None) {
                    // Low-pressure limit.
                    arr = m_rxns[irxn]->LowPressureLimit();
                    // Note there is no -1 term here because there is also
                    // a third-body concentration.
                    arr.A *= pow(1.0e-6, m_rxns[irxn]->ReactantStoich());
                    arr.E *= 1.0e-7;
                    m_rxns[irxn]->SetLowPressureLimit(arr);
                } else {
                    // Third-body concentrations also need scaling.
                    if (m_rxns[irxn]->UseThirdBody()) {
                        // Forward rate coefficients.
                        arr = m_rxns[irxn]->Arrhenius();
                        arr.A *= 1.0e-6;
                        m_rxns[irxn]->SetArrhenius(arr);

                        // Reverse rate coefficients.
                        if (m_rxns[irxn]->RevArrhenius() != NULL) {
                            arr = *(m_rxns[irxn]->RevArrhenius());
                            arr.A *= 1.0e-6;
                            m_rxns[irxn]->SetRevArrhenius(arr);
                        }
                    }
                }
            }

            m_units = SI;
        }
    } else if (u == CGS) {
//        throw invalid_argument("Cannot currently convert mechanism to "
//                               "CGS units.  Consult your programmer.");
        m_units = CGS;
    }
}


// CHEMICAL ELEMENTS.

// Returns the number of chemical elements.
unsigned int Mechanism::ElementCount() const
{
    return m_elements.size();
}

// Returns the vector of chemical elements.
const ElementPtrVector &Mechanism::Elements() const
{
    return m_elements;
}

// Returns a pointer to the ith element.  NULL if i invalid.
const Sprog::Element *const Mechanism::Elements(unsigned int i) const
{
    if (i < m_elements.size()) {
        return m_elements[i];
    } else {
        return NULL;
    }
}

// Returns iterator to first element.
Mechanism::el_iterator Mechanism::ElBegin() {return m_elements.begin();}

// Returns const iterator to first element.
Mechanism::const_el_iterator Mechanism::ElBegin() const {return m_elements.begin();}

    // Returns iterator to position after last element.
Mechanism::el_iterator Mechanism::ElEnd() {return m_elements.end();}

// Returns const iterator to position after last element.
Mechanism::const_el_iterator Mechanism::ElEnd() const {return m_elements.end();}

// Adds a default element to the mechanism and returns it.
Element *const Mechanism::AddElement()
{
    // Add a new element.
    Element el;
    return AddElement(el);
}

// Copies the given element into the mechanism and returns a reference to
// the copy.
Element *const Mechanism::AddElement(const Sprog::Element &el)
{
    // First check current list for this element's name.  We
    // cannot have two identically named elements.
    int i = FindElement(el.Name());
    if (i >= 0) {
        // Element with this name already defined!
        return m_elements.at(i);
    }

    // Add the element.
    Element *elnew = el.Clone();
    m_elements.push_back(elnew);

    // Set up the element.
    elnew->SetMechanism(*this);

    // Return the element.
    return elnew;
}

// Returns index of element in list.  Returns -1 if not found.
int Mechanism::FindElement(const Sprog::Element &el) const
{
    // Loop over elements to find index.
    unsigned int i;
    for (i=0; i<m_elements.size(); i++) {
        if (el == *m_elements[i]) {
            // Found element!
            return i;
        }
    }

    // We are here because the element wasn't found.
    return -1;
}

// Returns index of element in list.  Returns -1 if not found.
int Mechanism::FindElement(const std::string &name) const
{
    // Loop over elements to find index.
    unsigned int i;
    for (i=0; i<m_elements.size(); i++) {
        if (*m_elements[i] == name) {
            // Found element!
            return i;
        }
    }

    // We are here because the element wasn't found.
    return -1;
}


// ELEMENT UPDATES.

// Updates the mechanism with changes to the given element.
void Mechanism::CheckElementChanges(const Element &el)
{
    // Find the index of the element in the list.
    int i = FindElement(el);
    if (i >= 0) {
        // Element found!

        // Loop over all species.  If a species contains the element, then
        // we must recalculate its mol. wt.
        SpeciesPtrVector::iterator sp;
        for (sp=m_species.begin(); sp!=m_species.end(); sp++) {
            if ((*sp)->ContainsElement(i)) {
                // Species contains element, recalculate mol. wt.
                (*sp)->CalcMolWt();
            }
        }
    }
}


// SPECIES.

// Returns the number of species in the mechanism.
unsigned int Mechanism::SpeciesCount(void) const
{
    return m_species.size();
}

// Returns the vector of chemical species.
const SpeciesPtrVector &Mechanism::Species() const
{
    return m_species;
}

// Returns a pointer to the ith species.  Returns NULL if i is invalid.
const Sprog::Species *const Mechanism::Species(unsigned int i) const
{
    if (i < m_species.size()) {
        return m_species[i];
    } else {
        return NULL;
    }
}

// Returns pointer to species with given name.  NULL if not found.
const Sprog::Species *const Mechanism::Species(const std::string &name) const
{
    int i = FindSpecies(name);
    if (i >= 0) {
        return m_species[i];
    } else {
        return NULL;
    }
}

// Returns iterator to first element.
Mechanism::sp_iterator Mechanism::SpBegin() {return m_species.begin();}

// Returns const iterator to first element.
Mechanism::const_sp_iterator Mechanism::SpBegin() const {return m_species.begin();}

    // Returns iterator to position after last element.
Mechanism::sp_iterator Mechanism::SpEnd() {return m_species.end();}

// Returns const iterator to position after last element.
Mechanism::const_sp_iterator Mechanism::SpEnd() const {return m_species.end();}

// Adds an empty species to the mechansism and returns a reference to it.
Species *const Mechanism::AddSpecies()
{
    // Adds species to vector.
    Sprog::Species sp;
    return AddSpecies(sp);
}

// Copies species into mechanism and returns a reference to
// the copy.
Species *const Mechanism::AddSpecies(const Sprog::Species &sp)
{
    // First check species vector to see if a species with the
    // same name is already defined.  We can only have one species
    // per name.
    int i = FindSpecies(sp.Name());
    if (i >= 0) {
        // A species with this name has been found.
        return m_species.at(i);
    }

    // Adds species to vector.
    Sprog::Species *spnew = sp.Clone();
    m_species.push_back(spnew);

    // Set up species.
    spnew->SetMechanism(*this);

    // Return species reference.
    return spnew;
}

// Returns index of species.  Returns -1 if not found.
int Mechanism::FindSpecies(const Sprog::Species &sp) const
{
    // Loop over species to find index.
    unsigned int i;
    for (i=0; i<m_species.size(); i++) {
        if (sp == *m_species[i]) {
            // Found species!
            return i;
        }
    }

    // We are here because the species wasn't found.
    return -1;
}

// Returns index of species.  Returns -1 if not found.
int Mechanism::FindSpecies(const std::string &name) const
{
    // Loop over species to find index.
    unsigned int i;
    for (i=0; i<m_species.size(); i++) {
        if (*m_species[i] == name) {
            // Found species!
            return i;
        }
    }

    // We are here because the species wasn't found.
    return -1;
}

// Returns a pointer to the species at the given index.  Returns NULL if not found.
Sprog::Species *const Mechanism::GetSpecies(const unsigned int i) const
{
    if (i < m_species.size()) {
        return m_species[i];
    } else {
        return NULL;
    }
}

// Returns a pointer to the species with the given name.  Returns NULL if not found.
Sprog::Species *const Mechanism::GetSpecies(const std::string &name) const
{
    int i = FindSpecies(name);
    if (i >= 0) {
        return m_species[i];
    } else {
        return NULL;
    }
}


// REACTIONS.

// Returns the number of reactions in the mechanism.
unsigned int Mechanism::ReactionCount(void) const
{
    return m_rxns.Count();
}

// Returns the set of chemical reactions.
const Sprog::Kinetics::ReactionSet &Mechanism::Reactions() const
{
    return m_rxns;
}

// Returns a pointer to the ith reaction. Returns NULL if i is invalid.
const Kinetics::Reaction *const Mechanism::Reactions(unsigned int i) const
{
    return m_rxns[i];
}
// Returns a pointer to the ith reaction. Returns NULL if i is invalid.
Kinetics::Reaction * Mechanism::GetReactions(unsigned int i)
{
    return m_rxns[i];
}

// Adds an empty reaction to the mechanism.
Sprog::Kinetics::Reaction *const Mechanism::AddReaction()
{
    return AddReaction(new Sprog::Kinetics::Reaction());
}

// Copies the reaction to the mechanism.
Sprog::Kinetics::Reaction *const Mechanism::AddReaction(const Sprog::Kinetics::Reaction *const rxn)
{
    Sprog::Kinetics::Reaction *prxn = m_rxns.AddReaction(*rxn);
    prxn->SetMechanism(*this);
    return prxn;
}


// STOICHIOMETRY CROSS REFERENCE.

// Builds the species-reaction stoichiometry cross-reference table.
void Mechanism::BuildStoichXRef()
{
    unsigned int i, j;
    int k;
    RxnStoichMap::iterator ij;
    real mu;

    // Clear current table.
    m_stoich_xref.clear();
    
    // Set up empty table.
    for (i=0; i!=m_species.size(); ++i) {
        m_stoich_xref.push_back(StoichXRef());
        m_stoich_xref[i].Species = i;
    }

    // Loop over all reactions.
    for (j=0; j!=m_rxns.Count(); ++j) {
        // Sum up integer reactant stoich.
        for (k=0; k!=m_rxns[j]->ReactantCount(); ++k) {
            // Get the species index and the stoichiometry.
            i  = m_rxns[j]->Reactant(k).Index();
            mu = (real)m_rxns[j]->Reactant(k).Mu();

            // Add up the contribution of this reaction to this species.
            ij = m_stoich_xref[i].RxnStoich.find(j);
            if (ij != m_stoich_xref[i].RxnStoich.end()) {
                (*ij).second -= mu;
            } else {
                m_stoich_xref[i].RxnStoich.insert(RxnStoichPair(j,-mu));
            }
        }

        // Sum up integer product stoich.
        for (k=0; k!=m_rxns[j]->ProductCount(); ++k) {
            // Get the species index and the stoichiometry.
            i  = m_rxns[j]->Product(k).Index();
            mu = (real)m_rxns[j]->Product(k).Mu();

            // Add up the contribution of this reaction to this species.
            ij = m_stoich_xref[i].RxnStoich.find(j);
            if (ij != m_stoich_xref[i].RxnStoich.end()) {
                (*ij).second += mu;
            } else {
                m_stoich_xref[i].RxnStoich.insert(RxnStoichPair(j,mu));
            }
        }

        // Sum up real reactant stoich.
        for (k=0; k!=m_rxns[j]->FReactantCount(); ++k) {
            // Get the species index and the stoichiometry.
            i  = m_rxns[j]->FReactant(k).Index();
            mu = m_rxns[j]->FReactant(k).Mu();

            // Add up the contribution of this reaction to this species.
            ij = m_stoich_xref[i].RxnStoich.find(j);
            if (ij != m_stoich_xref[i].RxnStoich.end()) {
                (*ij).second -= mu;
            } else {
                m_stoich_xref[i].RxnStoich.insert(RxnStoichPair(j,-mu));
            }
        }

        // Sum up real product stoich.
        for (k=0; k!=m_rxns[j]->FProductCount(); ++k) {
            // Get the species index and the stoichiometry.
            i  = m_rxns[j]->FProduct(k).Index();
            mu = m_rxns[j]->FProduct(k).Mu();

            // Add up the contribution of this reaction to this species.
            ij = m_stoich_xref[i].RxnStoich.find(j);
            if (ij != m_stoich_xref[i].RxnStoich.end()) {
                (*ij).second += mu;
            } else {
                m_stoich_xref[i].RxnStoich.insert(RxnStoichPair(j,mu));
            }
        }
    }

    m_stoich_xref_valid = true;
}

// Returns true if the stoichiometry xref map is valid, otherwise false.
bool Mechanism::IsStoichXRefValid()
{
    return m_stoich_xref_valid;
}

// Returns the stoichiometry for all reactions which
// involve the species with the given index.  Throws error if index is
// invalid.
const Sprog::RxnStoichMap &Mechanism::GetStoichXRef(unsigned int isp) const
{
    if (isp < m_species.size()) {
        return m_stoich_xref[isp].RxnStoich;
    }
    // Species index is invalid.
    throw invalid_argument("Invalid species index given when "
                           "finded RxnStoichMap (Sprog, Mechanism::GetStoichXRef.");
}


// READ/WRITE/COPY FUNCTIONS.

// Writes the mechanism to a binary data stream.
void Mechanism::Serialize(std::ostream &out) const
{
    if (out.good()) {
        // Write the serialize version to the stream.
        const unsigned int version = 0;
        out.write((char*)&version, sizeof(version));
        
        // Write the units system to the stream.
        unsigned int u = (unsigned int)m_units;
        out.write((char*)&u, sizeof(u));

        // Write the number of elements to the stream.
        u = m_elements.size();
        out.write((char*)&u, sizeof(u));

        // Write the elements to the stream.
        for (ElementPtrVector::const_iterator iel=m_elements.begin(); iel!=m_elements.end(); iel++) {
            (*iel)->Serialize(out);
        }

        // Write the number of species to the stream.
        u = m_species.size();
        out.write((char*)&u, sizeof(u));

        // Write the species to the stream.
        for (SpeciesPtrVector::const_iterator isp=m_species.begin(); isp!=m_species.end(); isp++) {
            (*isp)->Serialize(out);
        }

        // Write the reaction set to the stream.
        m_rxns.Serialize(out);

        // We don't need to write the stoich xref vector to the stream, as
        // this can be rebuilt when the mechanism is deserialised.

    } else {
        throw invalid_argument("Output stream not ready (Sprog, Mechanism::Serialize).");
    }
}

// Reads the mechanism data from a binary data stream.
void Mechanism::Deserialize(std::istream &in)
{
    // Clear the current mechanism.  We do this before checking
    // the stream condition to avoid confusion in the calling code.
    // Even if the possible exception is handled incorrectly, the
    // mechanism will still be empty.
    releaseMemory();

    if (in.good()) {
        // Read the serialized mechanism version.
        unsigned int version = 0;
        in.read(reinterpret_cast<char*>(&version), sizeof(version));

        unsigned int u = 0; // We'll need to to read unsigned ints.

        switch (version) {
            case 0:
                // Read the units.
                in.read(reinterpret_cast<char*>(&u), sizeof(u));
                m_units = (UnitSystem)u;

                // Read the number of elements and reserve memory.
                in.read(reinterpret_cast<char*>(&u), sizeof(u));
                m_elements.reserve(u);

                // Read the elements.
                try {
                    for (unsigned int i=0; i<u; i++) {
                        // Read the element from the stream using the
                        // appropriate constructor.
                        Element *el = new Element(in);
                        el->SetMechanism(*this);

                        // Add the element to the vector.
                        m_elements.push_back(el);
                    }
                } catch (exception &e) {
                    // Ensure the mechanism is cleared before throwing 
                    // the exception to the next layer up.
                    releaseMemory();
                    throw;
                }

                // Read the number of species and reserve memory.
                in.read(reinterpret_cast<char*>(&u), sizeof(u));
                m_species.reserve(u);

                // Read the species.
                try {
                    for (unsigned int i=0; i<u; i++) {
                        // Read the species from the stream using the
                        // appropriate constructor.
                        Sprog::Species *sp = new Sprog::Species(in);
                        sp->SetMechanism(*this);

                        // Add the species to the vector.
                        m_species.push_back(sp);
                    }
                } catch (exception &e) {
                    // Ensure the mechanism is cleared before throwing 
                    // the exception to the next layer up.
                    releaseMemory();
                    throw;
                }

                // Read the reaction set.
                try {
                    m_rxns.SetMechanism(*this);
                    m_rxns.Deserialize(in);
                } catch (exception &e) {
                    // Ensure the mechanism is cleared before throwing 
                    // the exception to the next layer up.
                    releaseMemory();
                    throw;
                }

                // Rebuild the stoich xref.
                BuildStoichXRef();

                break;
            default:
                throw runtime_error("Mechanism serialized version number "
                                    "is unsupported (Sprog, Mechanism::Deserialize).");
        }
    } else {
        throw invalid_argument("Input stream not ready (Sprog, Mechanism::Deserialize).");
    }
}


// COPYING ROUTINES.

// Copies elements from given array into this mechanism.
void Mechanism::copyInElements(const Sprog::ElementPtrVector &els)
{
    ElementPtrVector::const_iterator el;
    for (el=els.begin(); el!=els.end(); el++) {
        // Use Clone() function to create a copy of the element object.
        m_elements.push_back((*el)->Clone());
    }
}

// Copies species from given array into this mechanism.
void Mechanism::copyInSpecies(const Sprog::SpeciesPtrVector &sps)
{
    SpeciesPtrVector::const_iterator sp;
    for (sp=sps.begin(); sp!=sps.end(); sp++) {
        // Use Clone() function to create a copy of the species object.
        m_species.push_back((*sp)->Clone());
    }
}


// MEMORY MANAGEMENT.

// Clears memory used by the mechanism object.
void Mechanism::releaseMemory()
{
    // Clear elements.
    ElementPtrVector::iterator el;
    for (el=m_elements.begin(); el!=m_elements.end(); el++) {
        delete *el;
    }
    m_elements.clear();

    // Clear species.
    SpeciesPtrVector::iterator sp;
    for (sp=m_species.begin(); sp!=m_species.end(); sp++) {
        delete *sp;
    }
    m_species.clear();

    // Clear reactions.
    m_rxns.Clear();

    // Clear other variables.
    m_stoich_xref.clear();
}


// OUTPUT FUNCTIONS.

// Prints a diagnostic output file containing all the
// mechanism data.  This is used to debug.
void Mechanism::WriteDiagnostics(const std::string &filename) const
{
    string data = "";

    // Open the output file.
    ofstream fout;
    fout.open(filename.c_str());

    // Write the Elements to the file.
    data = "Elements:\n";
    fout.write(data.c_str(), data.length());
    for (unsigned int i=0; i!=m_elements.size(); ++i) {
        m_elements[i]->WriteDiagnostics(fout);
    }
    data = "End of elements.\n";
    fout.write(data.c_str(), data.length());

    // Write the Species to the file.
    data = "Species:\n";
    fout.write(data.c_str(), data.length());
    for (unsigned int i=0; i!=m_species.size(); ++i) {
        m_species[i]->WriteDiagnostics(fout);
    }
    data = "End of species.\n";
    fout.write(data.c_str(), data.length());

    // Write the reactions to the file.
    data = "Reactions:\n";
    fout.write(data.c_str(), data.length());
    for (unsigned int i=0; i!=m_rxns.Count(); ++i) {
        m_rxns.Reactions(i)->WriteDiagnostics(fout);
    }
    data = "End of reactions.\n";
    fout.write(data.c_str(), data.length());

    // Close the output file.
    fout.close();
}

/*!
@param[in]      filename            The name of the reduced mechanism output file.
@param[in]      RejectSpecies       The rejected species as specified by LOI reduction.
@param[in]      KeptSpecies         The kept species as specified by LOI reduction.
*/
void Mechanism::WriteReducedMech(const std::string &filename, std::vector<std::string> RejectSpecies) const {
    
    // Open the output file.
    ofstream fout;
    fout.open(filename.c_str());

    // Write the Elements to the file.
    fout << "ELEMENTS\n";
    for (unsigned int i = 0; i != m_elements.size(); ++i) {
        m_elements[i]->WriteElements(fout);
    }
    fout << "END\n\n";

    // Write the Species to the file.
    fout << "SPECIES\n";
    for (unsigned int i = 0; i != m_species.size(); ++i) { 
        if (m_species[i]->Name() != RejectSpecies[i])
            m_species[i]->WriteSpecies(fout);
    }
    fout << "END\n\n";

    // Write the reactions to the file.
    fout << "REAC\n";
    for (unsigned int i = 0; i != m_rxns.Count(); ++i) {
        m_rxns.Reactions(i)->WriteReducedMechReacs(fout, RejectSpecies);
    }
    fout << "END\n\n";
 
    // Close the output file.
    fout.close();
}

void Mechanism::setSpeciesTransport(std::map< string,vector<string> > &trMap, Sprog::Mechanism &mech) const{

	map< string,vector<string> >::iterator mi;
	vector<std::string> transData;
	unsigned int nSpecies = mech.SpeciesCount();

	//Transport::TransportData *td;

	for(unsigned int i =0; i!=nSpecies; i++){
		mi = trMap.find(mech.m_species[i]->Name());
		if( mi != trMap.end()){
			transData = mi->second;			
			mech.m_species[i]->setTransportData(transData);													
		}else{
			cout << "Species :" << mech.m_species[i]->Name() << "not found in transport data \n";
			//exit(1);
		}
	}
   
	
}