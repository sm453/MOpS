/*
  Author(s):      Vinod Janardhanan (vj231)
  Project:        flameLab (premix solver)
  Sourceforge:    http://sourceforge.net/projects/mopssuite
  
  Copyright (C) 2008 Vinod M Janardhanan.

  File purpose:
	This class contains information about the initial conditions.
	Inlet conditions at the fuel and oxidizer nozzle are manged in
	this class. The reactor object contains objects of intial conditions
  Licence:
    This file is part of "flameLab".

    flameLab is free software; you can redistribute it and/or
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

#ifndef FL_INITIAL_H
#define FL_INITIAL_H
#include "fl_params.h"
#include "gpc.h"
#include <map>
#include <vector>
namespace FlameLab{
	class InitialConditions{

	public:
		enum MassOrMole{
			Massfraction,
			Molefraction
		};

		enum{
			FUEL=0,
			OXIDIZER
		};

		InitialConditions(){}
		~InitialConditions(){}

		//set the axial velocity in m/s
		void setVelocity(real vel);

		// return the axial velocity in m/s
		real getVelocity() const;

		//set the flow rate in SI units
		void setFlowRate(real flr);

		//return the flow rate in Kg/m3 s
		real getFlowRate() const;

		// set the radial velocity gradient in /s
		void setRadialVelocityGrad(real vel);

		// return the radial velocity gradient in /s
		real& getRadialVelocityGrad();

		//set the temperature in K
		void setTemperature(real temp);

		// return the temperature in K
		real getTemperature() const;

		//set the density in Kg/m3
		void setDensity(real dens);

		//return the density in kg/m3
		real getDensity() const;

		//set the mass/mole fraction for a given species
		void setFraction(const std::string &name, real fraction);

		//return the user defined inlet species fractions mass/mole
		map<std::string,real> getFraction() const;

		//sets the mass fraction at a nozzle
		void setFraction(vector<real> fracs);

		//return the initial mass fractions
		vector<real>& getMassFractions();

		//sets wether the inlet conditions are in mass or mole fraction
		void setMassOrMole(int sp);
		int getMassOrMole() const;
		//bool operator==(MassOrMole sp);

		void setFuelMixture(Sprog::Thermo::Mixture mix);
		void setOxidizerMixture(Sprog::Thermo::Mixture mix);

		Sprog::Thermo::Mixture& getFuelMixture() ;
		Sprog::Thermo::Mixture& getOxidizerMixture() ;


	private:

		real velocity; 	// axial velocity
		real flowRate;		//inlet flow rate
		real radVelGrad;  // radial velocity gradient in case of counter flow diffusion
		real temperature; //inlet temperature
		real density;     //inlet density

		int mom;
		std::map<std::string,real> species;
		vector<real> massFracs;	
		//this is required since the default constructor for mixture is protected
		map<string,Sprog::Thermo::Mixture> nozzleMixture;
		map<string,Sprog::Thermo::Mixture>::iterator ni;		

		
	};
}

#endif
