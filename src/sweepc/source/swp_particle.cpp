/*
  Author(s):      Matthew Celnik (msc37)
  Project:        sweepc (population balance solver)
  Sourceforge:    http://sourceforge.net/projects/mopssuite
  
  Copyright (C) 2008 Matthew S Celnik.

  File purpose:
    Implementation of the Particle class declared in the
    swp_particle.h header file.

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

#include "swp_particle.h"
#include "swp_particle_model.h"
#include "swp_particle_image.h"
#include "string_functions.h"
#include <cmath>
#include <stdexcept>
#include <cassert>

using namespace Sweep;
using namespace std;

// CONSTRUCTORS AND DESTRUCTORS.

// Default constructor (protected).
Particle::Particle(void)
: m_Position(0.0)
, m_PositionTime(0.0)
, m_StatWeight(1.0)
, m_CoagCount(0)
{
}

// Initialising constructor.
Particle::Particle(real time, const Sweep::ParticleModel &model)
: SubParticle(time, model)
, m_Position(0.0)
, m_PositionTime(0.0)
, m_StatWeight(1.0)
, m_CoagCount(0)
{
}

/*!
 * @param[in]	time	Create time
 * @param[in]	weight	Statistical weight of new particle
 * @param[in]	model	Particle model that interprets contents of particle
 */
Particle::Particle(real time, real weight, const Sweep::ParticleModel &model)
: SubParticle(time, model)
, m_Position(0.0)
, m_PositionTime(0.0)
, m_StatWeight(weight)
, m_CoagCount(0)
{
}

// Initialising constructor (from Primary particle).
Particle::Particle(Sweep::AggModels::Primary &pri)
: SubParticle(pri)
, m_Position(0.0)
, m_PositionTime(0.0)
, m_StatWeight(1.0)
, m_CoagCount(0)
{
}

// Copy constructor.
Particle::Particle(const Sweep::Particle &copy)
{
    // Use assignment operator.
    *this = copy;
}

/*!
 * Read particle from binary stream.  This method must read the data in
 * the same way in which it is written in Serialize.
 *
 * @param[in,out]		in		Stream from which to read
 * @param[in]			model	Particle model defining interpretation of particle data
 *
 * @exception		invalid_argument	Stream not ready
 */
Particle::Particle(std::istream &in, const Sweep::ParticleModel &model)
: SubParticle(in, model)
{
    if(in.good()) {
        real val;
        in.read(reinterpret_cast<char*>(&val), sizeof(val));
        m_Position = val;

        in.read(reinterpret_cast<char*>(&m_PositionTime), sizeof(m_PositionTime));
        in.read(reinterpret_cast<char*>(&m_StatWeight), sizeof(m_StatWeight));
        in.read(reinterpret_cast<char*>(&m_CoagCount), sizeof(m_CoagCount));
    }
    else {
        throw std::invalid_argument("Input stream not ready \
        (Sweep, Particle::Deserialize).");
    }
}

// Default destructor.
Particle::~Particle()
{
    // Nothing special to destruct.
}

/*!
 * @param[in]		xml			XML node specifying the particle
 * @param[in]		model		Particle model that defines the interpretation of the particle data
 *
 * @return		Pointer to new particle constructed on the heap (caller must delete).
 *
 * @exception	runtime_error	Unrecognised component
 * @exception	runtime_error	Unrecognised tracker
 * @exception	runtime_error	Non-positive statistical weight
 */
Particle* Particle::createFromXMLNode(const CamXML::Element& xml, const Sweep::ParticleModel& model)
{
    // Read initial particle composition.
    vector<CamXML::Element*> subitems; 
    xml.GetChildren("component", subitems);
    fvector components(model.ComponentCount(), 0);
    
    for (vector<CamXML::Element*>::iterator j=subitems.begin(); j!=subitems.end(); ++j) {
        // Get component ID.
        string str = (*j)->GetAttributeValue("id");
        int id = model.ComponentIndex(str);

        if (id >= 0) {
            // Get component value (XML uses dx to match format for inception).
            str = (*j)->GetAttributeValue("dx");
            components[id] = Strings::cdble(str);
        } else {
            // Unknown component in mechanism.
            throw std::runtime_error(str + ": Component not found in mechanism \
                                       (Sweep, Particle::createFromXMLNode).");
        }
    }

    // Read initial tracker variable values.
    xml.GetChildren("track", subitems);
    fvector trackers(model.TrackerCount(), 0);
    
    for (vector<CamXML::Element*>::iterator j=subitems.begin(); j!=subitems.end(); j++) {
        // Get tracker ID.
        string str = (*j)->GetAttributeValue("id");
        int id = model.GetTrackerIndex(str);

        if (id >= 0) {
            // Get tracker value (XML uses dx to match format for inception).
            str = (*j)->GetAttributeValue("dx");
            trackers[id] = Strings::cdble(str);
        } else {
            // Unknown tracker variable in mechanism.
            throw std::runtime_error(str + ": Tracker variable not found in mechanism. \
                                       (Sweep, Particle::createFromXMLNode).");
        }
    }

    // Pointer to new particle
    // \TODO wrap in an auto_ptr for exception safety
    Particle* pNew = model.CreateParticle(0.0);

    // Read any statistical weight
    const CamXML::Element* const pWeightNode = xml.GetFirstChild("weight");
    if(pWeightNode != NULL) {
    	std::string str = pWeightNode->Data();
    	const real wt = Strings::cdble(str);
    	if(wt <= 0) {
    		throw std::runtime_error("Particle statistical weight must be >0, not " + str +
    				                 "(Sweep, Particle::createFromXMLNode).");
    	}
    	else
            pNew->setStatisticalWeight(wt);
    }
    
    // Read SurfVol specific parameters
    if (model.AggModel() == Sweep::AggModels::SurfVol_ID) {
        const CamXML::Element* const pSurfNode = xml.GetFirstChild("surf");
        if (pSurfNode != NULL) {
            std::string str = pSurfNode->Data();
            const real surf = Strings::cdble(str);
            if(surf <= 0) {
                throw std::runtime_error("Particle surface area must be >0, not " + str +
                                         "(Sweep, Particle::createFromXMLNode).");
            }
            else
                pNew->m_primary->SetSurfaceArea(surf);
        }
    }

    // Initialise the new particle.
    pNew->Primary()->SetComposition(components);
    pNew->Primary()->SetValues(trackers);
    pNew->UpdateCache();
    
    return pNew;
}


// Compound assignment (coagulation).
// OPERATOR OVERLOADING.

// Assignment operator.
Particle &Particle::operator=(const Sweep::Particle &rhs)
{
    if (this != &rhs) {
        SubParticle::operator=(rhs);
        m_Position = rhs.m_Position;
        m_PositionTime = rhs.m_PositionTime;
        m_StatWeight = rhs.m_StatWeight;
        m_CoagCount = rhs.m_CoagCount;
    }
    return *this;
}

/*!
 * Both position and the associated time must be updated together.  This is
 * because it makes no sense to specify a position without knowing when it
 * applies.
 *
 *@param[in]    x       New position of particle
 *@param[in]    t       Time at which new position is correct
 */
void Particle::setPositionAndTime(const real x, const real t) {
    m_Position = x;
    m_PositionTime = t;
}

/*!
 *@return   Number of coagulation events since counter was reset
 */
unsigned int Particle::getCoagCount() const {
    return m_CoagCount;
}

// READ/WRITE/COPY.

// Creates a clone of the particle.
Particle *const Particle::Clone() const
{
    return new Particle(*this);
}

/*!
 * Perform checks on the internal data structure.  This is mainly for
 * testing and checking purposes; it should not be called from performance
 * critical sections.
 * 
 * @return	true iff internal structures pass tests
 */
bool Particle::IsValid() const {
	return SubParticle::IsValid() && (m_StatWeight > 0);
}

void Particle::writeParticlePOVRAY(std::ofstream &out) const
{
    // First construct the image
    Sweep::Imaging::ParticleImage image;
    image.Construct(*(this), *(Primary()->ParticleModel()));

    // Write it to file
    image.WritePOVRAY(out);

    // Now we can delete the image
}

/*!
 * @param[in,out]	out		Stream to which binary representation should be written
 *
 * @exception		invalid_argument	Output stream not ready
 */
void Particle::Serialize(std::ostream &out) const
{
	assert(IsValid());

    if (out.good()) {
        // First serialise the parent class
        SubParticle::Serialize(out);

        // Output the data members in this class
        out.write((char*)&m_Position, sizeof(m_Position));
        out.write((char*)&m_PositionTime, sizeof(m_PositionTime));
        out.write((char*)&m_StatWeight, sizeof(m_StatWeight));
        out.write((char*)&m_CoagCount, sizeof(m_CoagCount));
    }
    else {
        throw std::invalid_argument("Output stream not ready \
                                    (Sweep, Particle::Serialize).");
    }
}
