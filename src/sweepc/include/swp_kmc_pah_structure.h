/*!
  * \Author     Zakwan Zainuddin (zz260)
  * \file       swp_kmc_pah_structure.h
  *
  * \brief      Defines the data structure which holds information of PAH molecules
  *
  Project:      sweep (gas-phase chemistry solver).
  Sourceforge:  http://sourceforge.net/projects/mopssuite
  Copyright (C) 2010 Zakwan Zainuddin.

  File purpose:
    Holds the data structure which contains information on the molecular structure
    of PAH accounted by the kMC model. The structure is a directed graph connecting
    C atoms on the PAH perimeter, with the direction going clockwise along the edges.

  Licence:
    This file is part of "sweep".

    Sweep is free software; you can redistribute it and/or
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

#ifndef SWP_KMC_PAH_STRUCTURE_H
#define SWP_KMC_PAH_STRUCTURE_H

#include "swp_kmc_gasph.h"
#include "swp_kmc_structure_comp.h"
#include "swp_kmc_typedef.h"
#include "swp_kmc_jump_process.h" 
#include "swp_PAH.h"
#include <list>
#include <vector>
#include <map>
#include <set>

namespace Sweep{
    namespace KMC_ARS{
        class JumpProcess;
        typedef std::vector<Spointer> svector;
        class PAHStructure{
        public:
            friend class PAHProcess;
            // Constructors and destructors
            //! Default Constructor
            PAHStructure();
            //! Default Destructor
            ~PAHStructure();
			
			void setParent(Sweep::AggModels::PAH* parent);

			//! Overloaded Operators
			//PAHStructure &operator=(const PAHStructure &rhs);
			bool operator==(PAHStructure &rhs) const;
			bool operator!=(PAHStructure &rhs) const;

            //! Stores coordinates of all Carbon atoms (not according to order)
            std::set<cpair> m_cpositions;
			//! Initialise pah with pyrene (currently) or benzene
			void initialise(StartingStructure ss);
			PAHStructure* Clone() ;
			//! return number of carbon for particular PAH
			int numofC();
			//! check PAH have bridge or not
			bool havebridgeC();

			Sweep::AggModels::PAH* m_parent; // pointer to parent PAH
			
        protected:
            //! First and last Carbon atom in list
            Cpointer m_cfirst;
            Cpointer m_clast;
            //! Stores all principal PAH sites in order from m_cfirst-m_clast.
            std::list<Site> m_siteList;
			//! Stores iterators to the PAH sites according to their site type
            std::map<kmcSiteType, svector> m_siteMap;
			//! Stores total counts of carbon and hydrogen
            intpair m_counts;
            //! Stores number of rings
            int m_rings;
		private:
			//! Copy Constructor
			PAHStructure(const PAHStructure& copy);
        };
    }
}

#endif