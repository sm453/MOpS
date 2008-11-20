/*
  Author(s):      Matthew Celnik (msc37)
  Project:        mopsc (gas-phase chemistry solver).
  Sourceforge:    http://sourceforge.net/projects/mopssuite
  
  Copyright (C) 2008 Matthew S Celnik.

  File purpose:
    The StrangSolver class holds solves gas-phase chemistry equations coupled
    to the particle population balance (using Sweep) and a Strang operator
    splitting technique (see PP39).

  Licence:
    This file is part of "mops".

    mops is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
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

#ifndef MOPS_STRANG_SOLVER_H
#define MOPS_STRANG_SOLVER_H

#include "mops_params.h"
#include "mops_particle_solver.h"
#include "mops_reactor.h"
#include "mops_timeinterval.h"
#include "mops_mechanism.h"
#include "sweep.h"
#include <string>

namespace Mops
{
class StrangSolver : public ParticleSolver, Sweep::Solver
{
public:
    // Constructors.
    StrangSolver(void); // Default constructor.

    // Destructors.
    ~StrangSolver(void); // Default destructor.


    // SOLUTION.

    // Solves the coupled reactor using a Strang splitting algorithm
    // up to the stop time.  calls the output routine once at the
    // end of the function.  niter is ignored.
    virtual void Solve(
            Reactor &r,   // The reactor to solve.
            real tstop,   // The end time for the step.
            int nsteps,   // Number of internal steps to take.
            int niter,    // Number of internal iterations to take.
            OutFnPtr out, // Output function pointer.
            void *data    // Custom data object which will be passed as argument to out().
        );

    /*
    // Run the solver for the given reactor and the 
    // given time intervals.
    void SolveReactor(
        Reactor &r,              // Reactor object to solve.
        const timevector &times, // Vector of time intervals.
        unsigned int nruns = 1   // Number of runs to perform.
        );

    // Post-processes binary output files with the given file name
    // into CSV files.
    void PostProcess(
        const std::string &filename, // Filename to post-process.
        unsigned int nruns = 1       // Number of runs.
        ) const;
*/

private:
    // SIMULATION.

    // Performs n Strang splitting steps.
    void multiStrangStep(
        real dt,        // Splitting step size.
        unsigned int n, // Number of splitting steps.
        Reactor &r      // Reactor to solve.
        );
};
};

#endif
