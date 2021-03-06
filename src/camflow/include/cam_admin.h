/*
 * File:   cam_admin.h
 * Author: vinod (vj231@cam.ac.uk)
 *
 * Copyright (C) 2009 Vinod M Janardhanan
 *
 * File purpose:
 *  This class contains the implementation of process conditions
 *
 * Licence:
 *  This file is part of "Camflow".
 *
 *  Camflow is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * Contact:
 *  Dr Markus Kraft
 *  Dept of Chemical Engineering
 *  University of Cambridge
 *  New Museum Site
 *  Pembroke Street
 *  Cambridge
 *  CB2 3RA
 *  UK
 *
 *  Email   :   mk306@cam.ac.uk
 *  Website :   http://como.cheng.cam.ac.uk
 *
 * Created on January 17, 2009, 4:36 PM
 */

#ifndef _CAM_ADMIN_H
#define	_CAM_ADMIN_H

#include "cam_params.h"
#include "cam_boundary.h"
#include "comostrings.h"

namespace Camflow {

class CamAdmin
:
    public CamConc
{

        double T, T_wall;     //temperature in case of isothermal operation
        double pre;   //constant pressure system
        double strain; // strain rate
        double stepIgnition;    //step change of temperature for ignition
        int energyModel;
        int speciesOut;  //identifier for species output mass/mole
        CamBoundary left; //fuel
        CamBoundary right;//oxidizer
        static bool radiation;
        int restartType_;
        int flameletEquationType_;
        std::string restartFile_;
        std::string inputFileName_;

    public:

        enum RestartType{
            NONE,
            TEXT,
            BINARY
        };

        enum EnergyModel{
            ADIABATIC,
            ISOTHERMAL,
            USERDEFINED,
            NONISOTHERMAL
        };

        enum FlameletEquationType{
            SIMPLE,
            COMPLETE
        };

        //set the temperature
        //void setTemperature(double T_);

        //set the operating pressure
        void setPressure(double p_);

        //set the step for ignition temp
        void setIgnitionStep(double step);

        //set the wall temperature
        void setWallTemp(double Tw);

        //set the flamelet equation type
        void setFlameletEquationType(const std::string type);

        //set the energy model
        void setEnergyModel(int n);

        //set the energymodel
        void setEnergyModel(std::string model);

        //set left side boundary (normally for fuel inlet)
        void setLeftBoundary(CamBoundary &cb);

        //set right boundary (normally for oxidizer inlet)
        void setRightBoundary(CamBoundary &cb);

        //! Set radiation on/off.
        void setRadiationModel(bool radiation);

        //set the species output
        void setSpeciesOut(int n);

        void setRestartType(const std::string& restartType);

        void setRestartFile(const std::string& restartFile);

        void setInputFile(std::string inputFileName);
        const std::string& getInputFile() const;

        int getSpeciesOut() const;

        //return the temperature
        //double getTemperature() const;

        //return the wall temperature
        double getWallTemp() const;

        //return the pressure
        double getPressure()const;

        //return the ignition step
        double getIgnitionStep() const;

        //get the flamelet equation type
        int getFlameletEquationType() const;

        //return the energy model
        int getEnergyModel() const;

        //return the monitor option
        int getMonitor() const;

        //return fuel boundary
        Camflow::CamBoundary& getRightBoundary();

        //return oxidizer boundary
        Camflow::CamBoundary& getLeftBoundary();

        //! Is radiation model on or off.
        bool getRadiationModel() const;

        int getRestartType() const;

        const std::string& getRestartFile() const;

        //Evaluation of dimensionaless numbers
        double getNre(const double& hd, //hydraulic dia
                const double& u, //velocity
                const double& rho,//density
                const double& eta // viscosity
                );

        //return the prandtl number
        double getPrandtl(const double& eta, //viscosity
                const double& lambda,//thermal conductivity
                const double& cp //specific heat
                );

        //return the Graetz number
        double getGraetzInv(const double& x,//axial position
                    const double& dh,   //hydraulic dia
                    const double& Nre,  //Reynolds number
                    const double& Pr    //Prandtl number
                    );

        //return the nussult number
        double getNusselt(const double& gzInv);

        //return the heat transfer coefficient
        double getHeatTransferCoeff(const double& x,
                                        const double& vel,
                                        const double& hd,
                                        const double& rho,
                                        const double& eta,
                                        const double& lambda,
                                        const double& cp);

};

} // End Camflow namespace


#endif	/* _CAM_ADMIN_H */

