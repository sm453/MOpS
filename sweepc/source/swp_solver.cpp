/*
  Author(s):      Matthew Celnik (msc37)
  Project:        sweepc (population balance solver)
  Sourceforge:    http://sourceforge.net/projects/mopssuite
  
  Copyright (C) 2008 Matthew S Celnik.

  File purpose:
    Implementation of the Solver class declared in the
    swp_solver.h header file.

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

#include "swp_solver.h"
#include "rng.h"
#include <stdlib.h>
#include <cmath>
#include "string_functions.h"
#include <stdexcept>
#include <ctime> 
using namespace Sweep;
using namespace std;
using namespace Strings;

// CONSTRUCTORS AND DESTRUCTORS.

// Default constructor.
Solver::Solver(void)
: m_maxdt(0.0), m_splitratio(1.0e9)
{
    // Seed the random number generator.
    //srnd(123);
	srnd(time(0));
}

// Default destructor.
Solver::~Solver(void)
{
    // Nothing special to destruct.
}


// RUNNING THE SOLVER.

// Performs stochastic stepping algorithm up to specified stop time using
// the given mechanism to define the stochastic processes.  Updates given
// system accordingly.  On error returns <0, otherwise returns 0.
int Solver::Run(real &t, real tstop, Cell &sys, const Mechanism &mech)
{ 
    int err = 0;
    real tsplit, dtg, dt, jrate;
    static fvector rates(mech.TermCount(), 0.0);
    // Global maximum time step.
    dtg     = tstop - t;
    m_maxdt = dtg / 3.0;
    m_tstop = tstop;
	/*if (t>0.00044)
	{
		fvector dc(mech.Species()->size(), 0.0);
		sys.AdjustConcs(dc);
		cout <<"setting all species to 0";
	}*/


/*
	//introduced by ms785 to test the sintering of one single particle
	jrate=10000000000000000;
	rates[0]=10000000000000000;
	dt = timeStep(t, sys, mech, rates, jrate);
	dt = timeStep(t, sys, mech, rates, jrate);
	dt = timeStep(t, sys, mech, rates, jrate);
	dt = timeStep(t, sys, mech, rates, jrate);
	Ensemble::iterator i,j,k;
	i=sys.Particles().begin();
	j=i+1;
	k=j+1;
	(*(*i)).Coagulate(*(*k));
	(*(*i)).Coagulate(*(*j));
	for (int l=0;l<100000;l++)
	{
		(*(*i)).Sinter(1e-4, sys, mech.SintModel());
		(*(*i)).UpdateCache();
		ofstream out;
		string fname = "tree" + cstr(l) + ".inp";
		out.open(fname.c_str());
		ParticleCache::PropID id=ParticleCache::iFS;
		(*(*i)).printSubtree(out,id);
		out.close();

		fname = "subpart1" + cstr(t) + ".3d";
	    out.open(fname.c_str());
		Sweep::Imaging::ParticleImage img;
		img.Construct(*(*i));
		img.Write3dout(out,0,0,0);
		out.close();
	}*/
	//mech.m_inceptions[0]->Perform(t, sys, 0);

//	end ms785


    // Loop over time until we reach the stop time.
    while (t < tstop)
    {
        if (mech.AnyDeferred() && (sys.ParticleCount() > 0)) {
            // Get the process jump rates (and the total rate).
            jrate = mech.CalcJumpRateTerms(t, sys, rates);

            // Calculate split end time.
            tsplit = calcSplitTime(t, tstop, jrate, sys.ParticleCount(), dtg);
        } else {
            // There are no deferred processes, therefore there
            // is no need to perform LPDA splitting steps.
            tsplit = tstop;
        }

        // Perform stochastic jump processes.
        while (t < tsplit) {
            jrate = mech.CalcJumpRateTerms(t, sys, rates);
            dt = timeStep(t, sys, mech, rates, jrate);
            if (dt >= 0.0) {
                t+=dt; t = min(t, tstop);
            } else {
                return -1;
            }
        }

        // Perform Linear Process Deferment Algorithm to
        // update all deferred processes.
        if (mech.AnyDeferred()) {
            mech.LPDA(t, sys);	
        }
		mech.output(sys, t);
	}

    return err;
}


// TIME STEPPING ROUTINES.

// Calculates the splitting end time after which all particles
// shallbe updated using LPDA.
real Solver::calcSplitTime(real t, real tstop, real jrate, 
                           unsigned int n, real maxdt) const
{
    // Calculate the splitting time step, ensuring that it is
    // not longer than the maximum allowable time.
    real tsplit = n * m_splitratio / (jrate + 1.0);
    tsplit = min(tsplit, maxdt);

    // Now put the split end time into tsplit, again
    // checking that it is not beyond the stop time.
    return min(tsplit+t, tstop);
}

// Performs a single stochastic event on the ensemble from the given
// mechanism.  Returns the length of the time step if successful,
// otherwise returns negative.
real Solver::timeStep(real t, Cell &sys, const Mechanism &mech, 
                      const fvector &rates, real jrate)
{
    // The purpose of this routine is to perform a single stochastic jump process.  This
    // involves summing the total rate of all processes, generating a waiting time,
    // selecting a process and performing that process.

    int i;
    real dt;

    // Calculate exponentially distributed time step size.
    if (jrate > 0.0) {
        dt = rnd();
        dt = - log(dt) / jrate;
    } else {
        // Avoid divide by zero.
        dt = 1.0e+30;
    }

    // Truncate if step is too long and select a process
    // to perform.
    if (dt > m_maxdt) {
        dt = m_maxdt;
        i = -1;
    } else {
        if (t+dt <= m_tstop) {
            i = chooseProcess(rates);
        } else {
            i = -1;
        }
    }

    // Perform process.
    if (i >= 0) {
        mech.DoProcess(i, t+dt, sys);
/*        if (j==0) {
            m_processcounter[i]++;
        } else if (j==1) {
            m_ficticiouscounter[i]++;
        } else {
            // Return invalid time step on error.
            dt = -dt;
        }*/
    }

    return dt;
}

// Selects a process using a DIV algorithm and the process rates
// as weights.
int Solver::chooseProcess(const fvector &rates)
{
    // This routine implements a DIV algorithm to select a process from a 
    // discrete list of processes when each process's rate is given.

    // Add together all process rates.
    fvector::const_iterator i;
    real sum = 0.0;
    for (i=rates.begin(); i!=rates.end(); ++i) {
        sum += *i;
    }

    // Generate a uniform random number.
    real r = rnd() * sum ;

    // Select a process using DIV algorithm.
    int j=-1;
    i = rates.begin();
    while((r > 0.0) && (i != rates.end())) {
        r -= *i;
        ++i; ++j;
    }
    return j;
};
