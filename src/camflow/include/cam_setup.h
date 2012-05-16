/*!
 * \file   cam_setup.h
 * \author V. Janardhanan
 *
 * \brief Class to setup initial conditions.
 *
 *  Copyright (C) 2009 Vinod Janardhanan.
 *

 Licence:
    This file is part of "camflow".

    brush is free software; you can redistribute it and/or
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
    Prof Markus Kraft
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

#ifndef _CAM_SETUP_H
#define	_CAM_SETUP_H

#include <vector>

#include "cam_boundary.h"
#include "gpc.h"
#include "cam_params.h"
#include "cam_residual.h"
#include "cam_profile.h"
#include "cam_control.h"
#include "cam_converter.h"
#include "cam_soot.h"

namespace Camflow
{

    /*!
     *@brief    Class to setup initial conditions.
     *
     * Include a more detailed description here.
     */
    class CamSetup
    : public CamResidual
    {

        public:

            //! Structure containing inlet conditions.
            typedef struct{
                std::vector<doublereal> Species;
                doublereal FlowRate;
                doublereal Vel;
                doublereal rVelGrad;
                doublereal Dens;
                doublereal T;
                std::vector<doublereal> Dk;
                std::vector<doublereal> jk;
            } inletStruct;

            CamSetup
            (
                CamAdmin& ca,
                CamConfiguration& config,
                CamControl& cc,
                CamGeometry& cg,
                CamProfile& cp,
                CamSoot& cs,
                Sprog::Mechanism& mech
            );

            virtual ~CamSetup();

            //! Return the inlet species mass fractions for the given boundary.
            std::vector<doublereal> getInletMassFrac(CamBoundary &cb);

            //! Return the inlet temperature.
            doublereal getInletTemperature(CamBoundary &cb);

            //! Return the inlet flow rate.
            doublereal getInletFlowRate(CamBoundary &cb);

            //! Return the inlet velocity.
            doublereal getInletVelocity(CamBoundary &cb);

            //! Initialise the species vector.
            std::vector<doublereal> initSpecies(CamBoundary &cb);

            //! Initialize the species vector for a counter flow flame.
            std::vector<doublereal> initSpecies(CamBoundary &left, CamBoundary &right);

            //! Initialise the mass flow rate.
            std::vector<doublereal> initMassFlow(CamBoundary& cb);

            //! Initialise the temperature.
            std::vector<doublereal> initTemperature(CamBoundary& cb);

            //! Initialise the temperature field with a Gaussian profile.
            void initTempGauss(std::vector<doublereal> &soln);

            //! Store the inlet conditions.
            void storeInlet(CamBoundary &cb, inletStruct& ud_inlet);


        protected:

            CamProfile& profile_;

        }; // End CamSetup class declaration.

} // End Camflow namespace.

#endif	/* _CAM_SETUP_H */

