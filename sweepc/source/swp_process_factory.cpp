/*
  Author(s):      Matthew Celnik (msc37)
  Project:        sweepc (population balance solver)
  Sourceforge:    http://sourceforge.net/projects/mopssuite
  
  Copyright (C) 2008 Matthew S Celnik.

  File purpose:
    Implementation of the ProcessFactory class declared in the
    swp_process_factory.h header file.

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

#include "swp_process_factory.h"
#include "swp_surface_reaction.h"
#include "swp_condensation.h"
#include "swp_actsites_reaction.h"
#include "swp_transcoag.h"
#include "swp_addcoag.h"
#include "swp_pah_inception.h"
#include "swp_dimer_inception.h"
#include "swp_arssc_inception.h"
#include "swp_arssc_reaction.h"
#include <stdexcept>

using namespace Sweep;
using namespace Sweep::Processes;
using namespace std;

// PROCESS CREATION.

// Creates a new process data object of the given type.
Process *const ProcessFactory::Create(ProcessType id, const Sweep::Mechanism &mech)
{
    switch (id) {
        case PAH_Inception_ID:
            return new PAHInception(mech);
        case Dimer_Inception_ID:
            return new DimerInception(mech);
        case Transition_Coagulation_ID:
            return new TransitionCoagulation(mech);
        case Additive_Coagulation_ID:
            return new AdditiveCoagulation(mech);
        case SurfaceReaction_ID:
            return new SurfaceReaction(mech);
        case Condensation_ID:
            return new Condensation(mech);
        case ActSiteRxn_ID:
            return new ActSiteReaction(mech);
        case ARSSC_Inception_ID:
            return new ARSSC_Inception(mech);
        case ARSSC_Reaction_ID:
            return new ARSSC_Reaction(mech);
        case ARSSC_Condensation_ID:
//            return new ARSSC_Condensation(mech);
        default:
            throw invalid_argument("Invalid process ID (Sweep, "
                                   "ProcessFactory::Create).");
    }
}


// STREAM INPUT.

// Reads a process from a binary stream.  The first item read
// is the process ID which tells the ModelFactory what type
// of process to read.
Process *const ProcessFactory::Read(std::istream &in, const Sweep::Mechanism &mech)
{
    if (in.good()) {
        Process *proc = NULL;

        // Read the process type from the input stream.
        unsigned int type;
        in.read((char*)&type, sizeof(type));

        // Read a process of this particular type.  This will throw
        // an exception if the type is invalid.
        switch ((ProcessType)type) {
            case Dimer_Inception_ID:
                proc = new DimerInception(in, mech);
                break;
            case PAH_Inception_ID:
                proc = new PAHInception(in, mech);
                break;
            case Transition_Coagulation_ID:
                proc = new TransitionCoagulation(in, mech);
                break;
            case Additive_Coagulation_ID:
                proc = new AdditiveCoagulation(in, mech);
                break;
            case SurfaceReaction_ID:
                proc = new SurfaceReaction(in, mech);
                break;
            case Condensation_ID:
                proc = new Condensation(in, mech);
                break;
            case ActSiteRxn_ID:
                //proc = new ActSiteReaction(in, mech);
                //break;
            case ARSSC_Inception_ID:
                //proc = new ARSSC_Inception(in, mech);
                //break;
            case ARSSC_Reaction_ID:
                //proc = new ARSSC_Reaction(in, mech);
                //break;
            case ARSSC_Condensation_ID:
//                proc = new ARSSC_Condensation(in, mech);
            default:
                throw runtime_error("Invalid process type read from "
                                    "input stream (Sweep, ProcessFactory::Read).");
        }

        return proc;
    } else {
        throw invalid_argument("Input stream not ready "
                               "(Sweep, ProcessFactory::Read).");
    }
}

// Reads an inception from a binary stream.  The first item read
// is the inception ID which tells the ModelFactory what type
// of inception to read.
Inception *const ProcessFactory::ReadInception(std::istream &in, 
                                               const Sweep::Mechanism &mech)
{
    if (in.good()) {
        Inception *proc = NULL;

        // Read the process type from the input stream.
        unsigned int type;
        in.read((char*)&type, sizeof(type));

        // Read an inception of this particular type.  This will throw
        // an exception if the type is invalid.
        switch ((ProcessType)type) {
            case Dimer_Inception_ID:
                proc = new DimerInception(in, mech);
                break;
            case PAH_Inception_ID:
                proc = new PAHInception(in, mech);
                break;
            case ARSSC_Inception_ID:
                proc = new ARSSC_Inception(in, mech);
            default:
                throw runtime_error("Invalid inception type read from "
                                    "input stream (Sweep, "
                                    "ProcessFactory::ReadInception).");
        }

        return proc;
    } else {
        throw invalid_argument("Input stream not ready "
                               "(Sweep, ProcessFactory::ReadInception).");
    }
}

// Reads a particle process from a binary stream.  The first item read
// is the process ID which tells the ModelFactory what type
// of process to read.
ParticleProcess *const ProcessFactory::ReadPartProcess(std::istream &in, 
                                                       const Sweep::Mechanism &mech)
{
    if (in.good()) {
        ParticleProcess *proc = NULL;

        // Read the process type from the input stream.
        unsigned int type;
        in.read((char*)&type, sizeof(type));

        // Read a process of this particular type.  This will throw
        // an exception if the type is invalid.
        switch ((ProcessType)type) {
            case SurfaceReaction_ID:
                proc = new SurfaceReaction(in, mech);
                break;
            case Condensation_ID:
                proc = new Condensation(in, mech);
                break;
            case ActSiteRxn_ID:
                return new ActSiteReaction(in, mech);
            case ARSSC_Reaction_ID:
                proc = new ARSSC_Reaction(in, mech);
                break;
            case ARSSC_Condensation_ID:
//                proc = new ARSSC_Condensation(in, mech);
                break;
            default:
                throw runtime_error("Invalid particle process type read from "
                                    "input stream (Sweep, "
                                    "ProcessFactory::ReadPartProcess).");
        }

        return proc;
    } else {
        throw invalid_argument("Input stream not ready "
                               "(Sweep, ProcessFactory::ReadPartProcess).");
    }
}

// Reads an coagulation from a binary stream.  The first item read
// is the coagulation ID which tells the ModelFactory what type
// of coagulation to read.
Coagulation *const ProcessFactory::ReadCoag(std::istream &in, 
                                            const Sweep::Mechanism &mech)
{
    if (in.good()) {
        Coagulation *proc = NULL;

        // Read the process type from the input stream.
        unsigned int type;
        in.read((char*)&type, sizeof(type));

        // Read an inception of this particular type.  This will throw
        // an exception if the type is invalid.
        switch ((ProcessType)type) {
            case Transition_Coagulation_ID:
                proc = new TransitionCoagulation(in, mech);
                break;
            case Additive_Coagulation_ID:
                proc = new AdditiveCoagulation(in, mech);
                break;
            default:
                throw runtime_error("Invalid coagulation type read from "
                                    "input stream (Sweep, "
                                    "ProcessFactory::ReadCoag).");
        }

        return proc;
    } else {
        throw invalid_argument("Input stream not ready "
                               "(Sweep, ProcessFactory::ReadInception).");
    }
}

// Reads a death process from a binary stream.  The first item read
// is the process ID which tells the ModelFactory what type
// of death process to read.
DeathProcess *const ProcessFactory::ReadDeath(std::istream &in, 
                                              const Sweep::Mechanism &mech)
{
    if (in.good()) {
        DeathProcess *proc = NULL;

        // Read the process type from the input stream.
        unsigned int type;
        in.read((char*)&type, sizeof(type));

        // Read an inception of this particular type.  This will throw
        // an exception if the type is invalid.
        switch ((ProcessType)type) {
            case Death_ID:
                proc = new DeathProcess(in, mech);
                break;
            default:
                throw runtime_error("Invalid death process type read from "
                                    "input stream (Sweep, "
                                    "ProcessFactory::ReadDeath).");
        }

        return proc;
    } else {
        throw invalid_argument("Input stream not ready "
                               "(Sweep, ProcessFactory::ReadInception).");
    }
}


// STREAM OUTPUT.

// Writes a process, along with its ID to an output stream.
void ProcessFactory::Write(const Process &proc, std::ostream &out)
{
    if (out.good()) {
        // Write the process Serial signature type to the stream.
        unsigned int type = (unsigned int)proc.ID();
        out.write((char*)&type, sizeof(type));

        // Serialize the process object.
        proc.Serialize(out);
    } else {
        throw invalid_argument("Output stream not ready "
                               "(Sweep, ProcessFactory::Write).");
    }
}
