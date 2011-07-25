/*!
 * \file   cam_sdr.h
 * \author L. R. McGlashan
 *
 * \brief Class for calculating the scalar dissipation rate.
 *
 *  Copyright (C) 2011 L. R. McGlashan.
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

#ifndef _SDR_H
#define	_SDR_H

#include "array.h"
#include "cam_math.h"

namespace Camflow
{

/*! @brief SDR class.
 *
*/
class ScalarDissipationRate
{

    enum sdr
    {
        NONE,
        constant_fromStrainRate,
        profile_fromStrainRate,
        constant_fromCFD,
        profile_fromCFD
    };
    int sdrType_;

    const doublereal& stoichZ_;
    doublereal strainRate_;
    doublereal stoichSDR_;

    const std::vector<doublereal>& mixFracCoords_;

    //! The first index is 'time coordinate'.
    //! The second index is 'mixture fraction coordinate'.
    Array2D scalarDissipationRate_;

    std::vector<doublereal> v_sdr;   //scalar dissipation rate that has a time history
    std::vector<doublereal> v_time; //time profile of scalar dissipation rates
    std::vector< std::vector<doublereal> > profile_sdr; // SDR profile with a time history.

    void readStrainRate(const std::string& inputFileName);
    doublereal calculate(const doublereal& mixtureFraction) const;
    doublereal scalarDissipationRate(const doublereal& mixtureFraction) const;
    doublereal strainRate(const doublereal& mixtureFraction) const;

public:

    //! Default constructor.
    ScalarDissipationRate
    (
        const std::string& inputFileName,
        const doublereal& stoichZ,
        const std::vector<doublereal>& mixFracCoords,
        const int n_TimePoints
    );

    //! Destructor.
    ~ScalarDissipationRate();

    void setStrainRate(const doublereal strainRate);

    void setSDRRate(const doublereal sdr);

    doublereal operator[](int i) const
    {
        return scalarDissipationRate_(i,0);
    }

    inline const doublereal& getStoichSDR() const
    {
        return stoichSDR_;
    }

}; // End ScalarDissipationRate class declaration.

} // End Camflow namespace.

#endif	/* _SDR_H */
