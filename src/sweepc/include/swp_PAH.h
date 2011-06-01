 /*\author Markus Sander
 *  Copyright (C) 2011 Dongping Chen.
 *
 *  Project:        sweepc (population balance solver)
 *  Sourceforge:    http://sourceforge.net/projects/mopssuite
 *
 * \brief  Defines a primary including detailed PAH information
  Author(s):      Dongping Chen (dc516)
  Project:        sweep (population balance solver)
  Sourceforge:    http://sourceforge.net/projects/mopssuite

  Copyright (C) 2009 Markus Sander.

  File purpose:


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

#include "swp_kmc_pah_structure.h"

#ifndef SWEEP_PAH_H
#define SWEEP_PAH_H
	
namespace Sweep
{
//forward declaration 
namespace KMC_ARS{
class PAHStructure;}

namespace AggModels
{
//! PAH primary particle class
class PAH 
{
	friend class PAHPrimary;
public:
    //! Build a new primary with one molecule
    PAH();

    PAH(const PAH &copy); // Copy constructor.
	~PAH();
	PAH &operator=(const PAH &rhs);
	PAH *const Clone() const;
	//returen ID of PAH
	int ID() const;

private:
	        //! Number of Carbon atoms in the PAH
        unsigned int m_numcarbon;

        //! Index of the PAH story within its database
        //deleted by dongping 08 Apr. unsigned int ID;

        //! Simulated time at point when molecule created
        double time_created;

        //! Simulated time when molecule size was last updated
        double lastupdated;

		int PAH_ID;
        //! Amount of growth time to ignore (freezetime), deleted by dongping 02 May

		//! PAH structure provide by KMC PAH growth part add by dongping 08 Apr.//??
		Sweep::KMC_ARS::PAHStructure* m_pahstruct;
		

};
} //namespace AggModels
} //namespace Sweep

#endif

