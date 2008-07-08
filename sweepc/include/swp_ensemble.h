/*
  Author(s):      Matthew Celnik (msc37) and Peter Man (plwm2)
  Project:        sweep (population balance solver)
  Sourceforge:    http://sourceforge.net/projects/mopssuite
  
  Copyright (C) 2008 Matthew S Celnik.

  File purpose:
    A particle ensemble for Sweep.  The sweep particle ensemble uses a variable
    particle count algorithm and employs some scaling techniques to facilitate this.

    As most particle systems simulated involve an initial growth in particle count,
    followed by a coagulation phase in which particle count decreases (sometimes
    substantially) a particle doubling algorithm is used.  This algorithm copies
    one half of the population when the ensemble count falls below half capacity,
    thereby preventing the removal of all particles and hence statistically significant
    systematic error.

    The emsemble also uses a contraction algorithm if the ensemble requires more space for
    particles than is available.  This algorithm uniformly removes a particle from the
    ensemble to provide more space.

    To speed up particle selection the ensemble implements a binary tree.  Properties
    from the particle type are stored in the tree allowing for rapid selection by
    different properties.  The actual properties stored are not defined by the ensemble.

    The binary tree requires that the ensemble capacity must be a power of 2.

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

#ifndef SWEEP_ENSEMBLE_H
#define SWEEP_ENSEMBLE_H

#include "swp_params.h"
#include "swp_treenode.h"
#include "swp_particle.h"
#include "swp_particle_model.h"
#include "swp_particle_cache.h"
#include <cmath>
#include <iostream>

namespace Sweep
{
class Mechanism;

class Ensemble
{
public:
    // TYPEDEFS.

    // The type of particle in the ensemble.
    typedef Particle particle_type;

    // Iterator for the particles.
    typedef PartPtrVector::iterator iterator;

    // Constant iterator for the particles.
    typedef PartPtrVector::const_iterator const_iterator;

    // Constructors.
    Ensemble(void); // Default constructor.
    Ensemble(const Sweep::ParticleModel &model); // Initialising constructor (no particles).
    Ensemble(                             // Initialising constructor (incl. particles).
        unsigned int count,               //  - Capacity (max. number of particles).
        const Sweep::ParticleModel &model //  - Mechanism used to define particles.
        );  
    Ensemble(const Ensemble &copy); // Copy constructor.
    Ensemble(                            // Stream-reading constructor.
        std::istream &in,                //   - Input stream.
        const Sweep::ParticleModel &mech //   - Mechanism used to define particles.
        );

    // Destructor.
    ~Ensemble(void);

    // Operators.
    Ensemble &operator=(const Ensemble &rhs);
    

    // INITIALISATION.

    // Initialises the ensemble with the given capacity.
    void Initialise(
        unsigned int capacity,             // Max. number of particles.
        const Sweep::ParticleModel &model  // Model which defines the particles.
        );


    // THE PARTICLE MODEL.

    // Returns a pointer to the particle model to which this
    // ensemble subscribes.
    const Sweep::ParticleModel *const ParticleModel(void) const;


    // PARTICLE ADDITION AND REMOVAL.

    // Returns a pointer to the particle at index i.
    Particle *const At(unsigned int i);
    const Particle *const At(unsigned int i) const;

    // Adds the given particle to the ensemble.  Returns the new
    // particle's index in the ensemble.  The ensemble then takes
    // control of destruction of the particle.
    int Add(Particle &sp);

    // Removes the particle at the given index from the ensemble.
    void Remove(
        unsigned int i, // Index of particle to remove.
        bool fdel=true  // Set true to delete particle from memory as well, otherwise false.
        );

    // Removes invalid particles.
    void RemoveInvalids(void);

    // Replaces the particle at the given index with the given particle.
    void Replace (
        unsigned int i, // Index of particle to replace.
        Particle &sp    // Particle to insert.
        );

    // Removes all particles from the ensemble.
    void Clear();


    // ITERATOR FUNCTIONS.

    // Returns an iterator to the first particle.
    iterator begin();

    // Returns const_iterator to the first particle.
    const_iterator begin() const;

    // Returns an iterator to the last particle.
    iterator end();

    // Returns const_iterator to the last particle.
    const_iterator end() const;


    // SELECTING PARTICLES.

    // Select a particle uniformly from the ensemble and returns
    // its index. Returns negative on failure.
    int Select(void) const;

    // Randomly selects a particle, weighted by the given particle
    // property index.  The particle properties are those stored in
    // the ParticleData type. Returns particle index on success, otherwise
    // negative.
    int Select(ParticleCache::PropID id) const; 


    // Selects particle according to the particle property
    // specified by the given model and the given property id
    // of the model.
    int Select(
        SubModels::SubModelType model_id, // ID of the model for which to get the property.
        unsigned int wtid                 // Property ID within the model.
        ) const;

    // ENSEMBLE CAPACITY AND PARTICLE COUNT.

    // Returns the particle count.
    unsigned int Count(void) const;
    
    // Returns the ensemble capacity.
    unsigned int Capacity(void) const;


    // SCALING AND PARTICLE DOUBLING.

    // Returns the scaling factor due to particle operations.
    real Scaling() const;

    // Resets the scaling parameters.
    void ResetScaling();

    // Stops doubling algorithm.
    inline void FreezeDoubling();

    // Restarts doubling if it was off, and checks if the 
    // ensemble should be doubled.
    inline void UnfreezeDoubling();


    // GET SUMS OF PROPERTIES.

    // Returns the sums of all properties in the binary tree.
    const ParticleCache &GetSums(void) const;

    // Returns the sum of one particle property with the given index 
    // from the binary tree.
    real GetSum(
        ParticleCache::PropID id // ID of the ParticleData property.
        ) const;

    // Returns the sum of one particle property with the given index 
    // from the given model from the binary tree.
    real GetSum(
        SubModels::SubModelType model_id, // ID of model from which to get the property.
        unsigned int id                   // ID of the property within the model.
        ) const;


    // UPDATE ENSEMBLE.

    // Updates the ensemble tree from the given particle index.
    void Update(unsigned int i);

    // Updates the ensemble tree completely.
    void Update(void);


    // READ/WRITE/COPY.

    // Writes the object to a binary stream.
    void Serialize(std::ostream &out) const;

    // Reads the object from a binary stream.
    void Deserialize(
        std::istream &in,                // Input stream.
        const Sweep::ParticleModel &mech // Model used to define particles.
        );

private:
    // Particle model used to define particles.
    const Sweep::ParticleModel *m_model;

    // Vector of particles in the ensemble.
    PartPtrVector m_particles;

    // ENSEMBLE CAPACITY VARIABLES.
    unsigned int m_levels;   // Number of levels in the binary tree.
    unsigned int m_capacity; // The ensemble capacity (max. particle count).
    unsigned int m_halfcap;  // Half the ensemble capacity.
    unsigned int m_count;    // Number of particles currently in the ensemble.

    // ENSEMBLE SCALING VARIABLES.
    real m_scale;            // Scaling factor due to internal operations (doubling etc.).
    real m_contfactor;       // Contraction scaling factor, precalculated for speed.
    unsigned int m_ncont;    // Number of ensemble contractions that have occurred.

    // DOUBLING ALGORITHM VARIABLES.
    unsigned int m_ndble;      // Number of ensemble doublings that have occurred.
    bool m_dbleactive;         // Is doubling active or not.  Condition is to reach cutoff (see below).
    unsigned int m_dblecutoff; // Min. particle count at which doubling is activated.
    unsigned int m_dblelimit;  // Particle count below which ensemble is doubled (if active).
    unsigned int m_dbleslack;  // Slack space at end of ensemble after doubling operation.
    bool m_dbleon;             // Allows user to manually switch off/on doubling.  Does not affect activation criterion.

    // TREE.
    std::vector<TreeNode> m_tree;   // The binary tree nodes.
    mutable ParticleCache m_sums;


    // MEMORY MANAGEMENT.

    // Releases all memory resources used by the ensemble.
    void releaseMem(void);

    // Sets the ensemble to its initial condition.  Used in constructors.
    void init(void);


    // TREENODE ASSOCIATED FUNCTIONS.

    // Returns the index of the lowest binary tree node under which the 
    // given particle (by index) lies.
    int treeIndex(unsigned int i) const;

    // Returns true if particle index is a left branch, 
    // false if it is a right one.
    static bool isLeftBranch(unsigned int i); 

    // Recalculates a branch of the tree from the given node upwards.
    void ascendingRecalc(unsigned int i);

    // Performs the doubling algorithm.
    void dble();
};

// Include inline function definitions.
#include "swp_ensemble_inl.h"
};

#endif
