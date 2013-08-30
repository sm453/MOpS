/*
  Author(s):      Matthew Celnik (msc37)
  Project:        sweep (population balance solver)
  Sourceforge:    http://sourceforge.net/projects/mopssuite
  
  Copyright (C) 2008 Matthew S Celnik.

  File purpose:
    Definition of inline member functions of the Component class.

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

#ifndef SWEEP_COMPONENT_INL_H
#define SWEEP_COMPONENT_INL_H

#include "swp_params.h"
#include "swp_component.h"
#include <string>

// MOLECULAR WEIGHT

// Returns component molecular weight (kg/mol).
inline double Sweep::Component::MolWt() const {return m_molwt;};

// Sets the molecular weight (kg/mol).
inline void Sweep::Component::SetMolWt(const double molwt) {m_molwt = molwt;};

//! Returns component density (kg/m3).
inline double Sweep::Component::Density() const {return m_density;};

//! Sets the density (g/cm3).
inline void Sweep::Component::SetDensity(const double dens) {m_density = dens;};

// COMPONENT NAME.

// Returns component symbol or name.
inline const std::string &Sweep::Component::Name() const {return m_name;};

// Sets the symbol or name.
inline void Sweep::Component::SetName(const std::string &name) {m_name = name;};

#endif
