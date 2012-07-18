/*
  Author(s):      Matthew Celnik (msc37)
  Project:        sprog (gas-phase chemical kinetics).
  Sourceforge:    http://sourceforge.net/projects/mopssuite

  Copyright (C) 2008 Matthew S Celnik.

  File purpose:
    This files contains the definition of a structure for chemical reactions.  Also
    contains typedefs and other data structures related to chemical reactions.

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

#ifndef GPC_REACTION_H
#define GPC_REACTION_H

#include "gpc_params.h"
#include "gpc_species.h"
#include "gpc_stoich.h"
#include "gpc_rate_params.h"
#include <vector>
#include <string>
#include <iostream>

namespace Sprog
{
class Mechanism; // Forward declaration of mechanism.

namespace Kinetics
{
class Reaction
{
public:
    // Constructors.
    Reaction(void);                // Default constructor.
    Reaction(const Reaction &rxn); // Copy constructor.
    Reaction(std::istream &in);

    // Destructor.
    ~Reaction(void);

    // Operator overloads.
    Reaction &operator=(const Reaction &rxn);

    // REACTION NAME.

    // Returns the reaction name.
    const std::string &Name(void) const;

    // Sets the reaction name.
    void SetName(const std::string &name);


    // REVERSIBILITY.

    // Returns true if the reaction is reversible.
    bool IsReversible(void) const;

    // Sets the reaction to be reversible or not.
    void SetReversible(const bool isrev);


    // REACTANTS.

    // Returns the vector of integer stoichiometric reactant coefficients.
    const std::vector<Stoich> &Reactants(void) const;

    // Adds a reactant to the reaction.
    void AddReactant(const Stoich &reac);

    // Adds a reactant given the species name.
    void AddReactant(const std::string &name, real stoich);

    // Removes a reactant, given by name, from the reaction.
    void RemoveReactant(const std::string &name);

    // Returns the stoichiometry of the kth reactant.
    const Stoich Reactant(unsigned int k) const;

    // Returns the number of integer reactants.
    int ReactantCount() const;


    // PRODUCTS.

    // Returns the vector of integer stoichiometric product coefficients.
    const std::vector<Stoich> &Products(void) const;

    // Adds a product to the reaction.
    void AddProduct(const Stoich &prod);

    // Adds a product given the species name.
    void AddProduct(const std::string &name, real stoich);

    // Removes a product, given by name, from the reaction.
    void RemoveProduct(const std::string &name);

    // Returns the stoichiometry of the kth integer product.
    const Stoich Product(unsigned int k) const;

    // Returns the number of integer products.
    int ProductCount() const;


    // STOICHIOMETRY.

    // Returns the total stoichiometry of the reaction.
    real TotalStoich() const;

    // Returns the reactant stoichiometry of the reaction.
    real ReactantStoich() const;

    // Returns the product stoichiometry of the reaction.
    real ProductStoich() const;

    // FORWARD ARRHENIUS COEFFICIENTS.

    // Returns the forward Arrhenius rate parameters.
    const ARRHENIUS &Arrhenius(void) const;

    // Sets the forward Arrhenius rate parameters.
    void SetArrhenius(const ARRHENIUS &arr);


    // REVERSE ARRHENIUS COEFFICIENTS.

    // Returns a pointer to the reverse Arrhenius parameters.
    const ARRHENIUS *const RevArrhenius(void) const;

    // Sets the reverse Arrhenius parameters.
    void SetRevArrhenius(const ARRHENIUS &arr);


    // FORWARD LANDAU-TELLER PARAMETERS.

    // Returns a pointer to the forward Landau Teller coefficients.
    const LTCOEFFS *const LTCoeffs(void) const;

    // Sets the forward Landau Teller coefficients.
    void SetLTCoeffs(const LTCOEFFS &lt);


    // REVERSE LANDAU-TELLER PARAMETERS.

    // Returns a pointer to the reverse Landau Teller coefficients.
    const LTCOEFFS *const RevLTCoeffs(void) const;

    // Sets the reverse Landau Teller coefficients.
    void SetRevLTCoeffs(const LTCOEFFS &lt);


    // THIRD BODIES.

    // Returns true if this reaction uses third bodies.
    bool UseThirdBody() const;

    // Sets whether or not this reaction uses third bodies.
    void SetUseThirdBody(bool usetb);

    // Returns the vector of third-body coefficients.
    const std::vector<Stoich> &ThirdBodies(void) const;

    // Returns the coefficient for the ith third body.
    Stoich ThirdBody(unsigned int i) const;

    // Returns the number of third body coefficients
    // defined for this reaction.
    int ThirdBodyCount() const;

    // Adds a third body to the reaction.
    void AddThirdBody(const Stoich &tb);

    // Adds a third body to the reaction.
    void AddThirdBody(unsigned int sp, real coeff);

    // Adds a third body given the species name.
    void AddThirdBody(const std::string &name, real coeff);

    // Removes a third body, given by name, from the reaction.
    void RemoveThirdBody(const std::string &name);


    // LOW-PRESSURE LIMIT.

    // Returns the low pressure limit Arrhenius coefficients.
    const ARRHENIUS &LowPressureLimit(void) const;

    // Sets the low pressure limit Arrhenius coefficients.
    void SetLowPressureLimit(const ARRHENIUS &lowp);


    // FALL-OFF THIRD BODY.

    // Returns a pointer to the species used as a third
    // body for fall-off calculations.
    const Sprog::Species *const FallOffThirdBody(void) const;

    // Sets the species used as a third body for fall-off calculations.
    void SetFallOffThirdBody(int sp);

    // Sets the species used as a third body for fall-off calculations
    // given the species name.
    void SetFallOffThirdBody(const std::string &name);


    // FALL-OFF PARAMETERS.

    // Returns the fall-off type.
    FALLOFF_FORM FallOffType() const;

    // Returns the fall-off parameter type.
    const FALLOFF_PARAMS &FallOffParams(void) const;

    // Sets the fall-off type and parameters.
    void SetFallOffParams(
        const FALLOFF_FORM form,
        const real params[FALLOFF_PARAMS::MAX_FALLOFF_PARAMS]
        );


    // FALL-OFF FUNCTIONS.

    real FTROE3(real T, real logpr) const; // 3-parameter Troe fall-off form.
    real FTROE4(real T, real logpr) const; // 4-parameter Troe fall-off form.
    real FSRI(real T, real logpr) const;   // SRI fall-off form.
    //FallOffFnPtr FallOffFn() const;        // Custom fall-off function.


    // PARENT MECHANISM.

    // Returns a pointer to the parent mechanism.  The mechanism
    // is stored in order for the reaction to be aware of the
    // chemical species available.
    const Sprog::Mechanism *const Mechanism(void) const;

    // Sets the parent mechanism.  The mechanism
    // is stored in order for the reaction to be aware of the
    // chemical species available.
    void SetMechanism(Sprog::Mechanism &mech);


    // RATE CALCULATION.

    // Calculates the rate of progress of this reaction.
    real RateOfProgress(
        real density,        // Mixture molar density.
        const real *const x, // Species mole fractions.
        unsigned int n,      // Number of values in x array.
        real kforward,       // Forward rate constant.
        real kreverse        // Reverse rate constant.
        ) const;


    // READ/WRITE/COPY FUNCTIONS.

    // Creates a copy of the species object.
    Reaction *Clone(void) const;

    // Writes the element to a binary data stream.
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int /* file_version */)
    {
        ar.template register_type<ARRHENIUS>();
        ar.template register_type<Sprog::Stoichiometry>();
        ar.register_type(static_cast<LTCOEFFS *>(NULL));

        ar & m_name & m_reversible
           & m_reac & m_prod & m_dstoich & m_dreac & m_dprod
           & m_arrf & m_arrr & m_lt & m_revlt & m_usetb
           & m_thirdbodies & m_fotype & m_foparams & m_mech;
    }

    // Writes the element to a binary data stream.
    void Serialize(std::ostream &out) const;

    // Reads the element data from a binary data stream.
    void Deserialize(std::istream &in);

    // Prints a diagnostic output file containing all the
    // reaction data.  This is used to debug.
    void WriteDiagnostics(std::ostream &out) const;

    //! Indicates whether the pre-exponential should be converted to cgs
    bool ConvertPreexponential(void) const;

    //! Writes the reduced mechanism reactions to the output file
    void WriteReducedMechReacs(std::ostream &out, std::vector<std::string> RejectSpecies) const;

protected:
    // Reaction data.
    std::string m_name;                    // Reaction description.
    bool m_reversible;                     // Is the reaction reversible or not?
    std::vector<Stoich> m_reac, m_prod;    // reactant & product stoichiometry.
    real m_dstoich, m_dreac, m_dprod;      // Total stoichiometry changes.
    ARRHENIUS m_arrf, *m_arrr;             // Forward and reverse Arrhenius parameters.
    LTCOEFFS *m_lt, *m_revlt;              // Landau-Teller forward and reverse coefficients.

    // Third bodies.
    bool m_usetb; // Set to true if this reaction requires third bodies.
    std::vector<Stoich> m_thirdbodies; // Reaction third bodies and their coefficients.

    // Fall-off data.
    FALLOFF_FORM m_fotype;     // The type of the fall-off reaction.
    FALLOFF_PARAMS m_foparams; // Fall-off parameters.
    //FallOffFnPtr m_fofn;       // Custom fall-off function, if required.

    // Useful data to put reaction in context.
    Sprog::Mechanism *m_mech; // Parent mechanism.

    // MEMORY MANAGEMENT.

    // Releases all memory used by the reaction object.
    void releaseMemory(void);
};

// Inline function definitions.
#include "gpc_reaction_inl.h"

// A typedef for a STL vector of reactions.
typedef std::vector<Reaction> RxnVector;

// A typedef fo a STL vector of pointers to reactions.
typedef std::vector<Reaction*> RxnPtrVector;
};
};

#endif
