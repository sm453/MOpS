#include "cam_sdr.h"
#include "camxml.h"
#include "stringFunctions.h"
#include "linear_interpolator.hpp"

using namespace Camflow;

ScalarDissipationRate::ScalarDissipationRate
(
    const std::string& inputFileName,
    const doublereal& stoichZ,
    const std::vector<doublereal>& mixFracCoords,
    const int n_TimePoints
)
:
  sdrType_(NONE),
  stoichZ_(stoichZ),
  mixFracCoords_(mixFracCoords)
{

    scalarDissipationRate_.resize(n_TimePoints);
    for (size_t t=0; t<n_TimePoints; ++t)
    {
        scalarDissipationRate_[t].resize(mixFracCoords.size());
    }
    sdrType_ = notFromCFD;
    readStrainRate(inputFileName);
    for (size_t i=0; i<mixFracCoords.size(); ++i)
    {
        scalarDissipationRate_[0][i] = calculate(mixFracCoords[i]);
    }

}

//! Destructor.
ScalarDissipationRate::~ScalarDissipationRate()
{}

void
ScalarDissipationRate::readStrainRate(const std::string& inputFileName)
{
    CamXML::Document doc;
    doc.Load(inputFileName);
    const CamXML::Element* root = doc.Root();
    CamXML::Element *subnode, *opNode;
    std::vector<CamXML::Element*> radiativeSpecies;
    std::vector<CamXML::Element*>::const_iterator p;

    opNode = root->GetFirstChild("op_condition");
    subnode = opNode->GetFirstChild("strain");
    if (subnode != NULL)
    {
        strainRate_ = IO::from_string<double>(subnode->Data());
        stoichSDR_ = scalarDissipationRate(stoichZ_);
    }
    else
    {
        subnode = opNode->GetFirstChild("sdr");
        if (subnode != NULL)
        {
            stoichSDR_ = IO::from_string<double>(subnode->Data());
            strainRate_ = strainRate(stoichZ_);
        }
        else
        {
            throw std::runtime_error
            (
                "No strain rate or stoich SDR read. Specify either\n"
                " <sdr>VALUE</sdr> or "
                " <strain>VALUE</strain> in <op_condition>."
            );
        }
    }

    std::cout << "Strain Rate = " << strainRate_
              << " | Stoich SDR = " << stoichSDR_
              << " | Stoich Mix. Frac. = " << stoichZ_
              << std::endl;
}

/*
 * Calculate the scalar dissipation rate profile. Method 1 in Carbonell(2009).
 */
doublereal
ScalarDissipationRate::calculate(const doublereal& mixtureFraction)
const
{
    CamMath cm;
    doublereal fZ = exp(-2*cm.SQR(cm.inverfc(2*mixtureFraction)));
    doublereal fZst = exp(-2*cm.SQR(cm.inverfc(2*stoichZ_)));

    /*Utils::LinearInterpolator<doublereal, doublereal> rhoInterpolate(reacGeom_.getAxpos(),m_rho);
    doublereal rhoStoich = rhoInterpolate.interpolate(stoichZ);

    doublereal phi = 0.75 *
                     (
                         cm.SQR(std::sqrt(m_rho[0]/m_rho[cell])+1.0)
                       / (2.0*std::sqrt(m_rho[0]/m_rho[cell])+1.0)
                     );
    doublereal phist = 0.75 *
                       (
                           cm.SQR(std::sqrt(m_rho[0]/rhoStoich)+1.0)
                         / (2.0*std::sqrt(m_rho[0]/rhoStoich)+1.0)
                       );*/

    return scalarDissipationRate(stoichZ_) * (fZ/fZst);// * (phi/phist);
}

/*!
 * Eq. 9.38 SummerSchool by N. Peters
 */
doublereal
ScalarDissipationRate::scalarDissipationRate(const doublereal& mixtureFraction)
const
{
    CamMath cm;
    doublereal fZ = exp(-2*cm.SQR(cm.inverfc(2*mixtureFraction)));
    return strainRate_*fZ/Sprog::PI;
}

doublereal
ScalarDissipationRate::strainRate(const doublereal& mixtureFraction)
const
{
    CamMath cm;
    doublereal fZ = exp(-2*cm.SQR(cm.inverfc(2*mixtureFraction)));
    return stoichSDR_*Sprog::PI/fZ;
}

void ScalarDissipationRate::setStrainRate(const doublereal strainRate)
{
    strainRate_ = strainRate;
    stoichSDR_ = scalarDissipationRate(stoichZ_);
    for (size_t i=0; i<mixFracCoords_.size(); ++i)
    {
        scalarDissipationRate_[0][i] = calculate(mixFracCoords_[i]);
    }
}

void ScalarDissipationRate::setSDRRate(const doublereal sdr)
{
    stoichSDR_ = sdr;
    strainRate_ = strainRate(stoichZ_);
    for (size_t i=0; i<mixFracCoords_.size(); ++i)
    {
        scalarDissipationRate_[0][i] = calculate(mixFracCoords_[i]);
    }
}


///**
// *  When the scalar dissipation rate has a time history
// *  use that during intergration
// */
void ScalarDissipationRate::setExternalScalarDissipationRate
(
    const std::vector<doublereal>& time,
    const std::vector<doublereal>& sdr
)
{

    sdrType_ = constant_fromCFD;
    v_stoichSDR = sdr;
    v_time = time;

    // Now calculate the analytic profile of sdr
    for (size_t t=0; t<sdr.size(); ++t)
    {
        stoichSDR_ = v_stoichSDR[t];
        strainRate_ = strainRate(stoichZ_);
        for (size_t z=0; z<mixFracCoords_.size(); ++z)
        {
            scalarDissipationRate_[t][z] = calculate(mixFracCoords_[z]);
        }
    }

}

///**
// *  When the scalar dissipation rate has a time history
// *  and has a profile with mixture fraction from the CFD.
// */
/*void ScalarDissipationRate::setExternalScalarDissipationRate
(
    const std::vector<doublereal>& time,
    const std::vector< std::vector<doublereal> >& sdr,
    const std::vector< std::vector<doublereal> >& Zcoords
)
{

    sdrType_ =  profile_fromCFD;
    profile_sdr = sdr;
    v_time = time;
    //mixFracCoords_ = Zcoords;


}*/

/**
 *  Interpolate and return the scalar dissipation rate
 */
/*doublereal
ScalarDissipationRate::getSDR(const doublereal time)
const
{

    Utils::LinearInterpolator<doublereal, doublereal> timeInterpolate(v_time, v_sdr);

    return timeInterpolate.interpolate(time);

}*/

///**
// *  Interpolate and return the scalar dissipation rate from a profile that varies through time.
// */
doublereal
ScalarDissipationRate::operator()
(
    const doublereal& Z,
    const doublereal& time
)
const
{

    if (sdrType_ == notFromCFD)
    {
        Utils::LinearInterpolator<doublereal, doublereal>
            interpolator(mixFracCoords_, scalarDissipationRate_[0]);
        return interpolator.interpolate(Z);
    }
    /*else if (sdrType_ == constant_fromCFD)
    {
        std::vector<doublereal> sdrTime, sdrInterpolated;
        std::vector<doublereal> cfdMixFracCoordsTime, cfdMixFracCoordsInterpolated;

        sdrInterpolated.clear();
        cfdMixFracCoordsInterpolated.clear();

        for (size_t i=0; i<mixFracCoords_.size(); ++i)
        {

            sdrTime.clear();
            sdrTime.push_back(profile_sdr[0][i]);
            sdrTime.push_back(profile_sdr[1][i]);

            cfdMixFracCoordsTime.clear();
            cfdMixFracCoordsTime.push_back(cfdMixFracCoords[0][i]);
            cfdMixFracCoordsTime.push_back(cfdMixFracCoords[1][i]);

            Utils::LinearInterpolator<doublereal, doublereal> timeInterpolate(v_time, sdrTime);
            doublereal sdrInterpolatedTime = timeInterpolate.interpolate(time);

            Utils::LinearInterpolator<doublereal, doublereal> time2Interpolate(v_time, cfdMixFracCoordsTime);
            doublereal cfdMixFracCoordsInterpolatedTime = time2Interpolate.interpolate(time);

            sdrInterpolated.push_back(sdrInterpolatedTime);
            cfdMixFracCoordsInterpolated.push_back(cfdMixFracCoordsInterpolatedTime);
        }

        Utils::LinearInterpolator<doublereal, doublereal> spaceInterpolate(cfdMixFracCoordsInterpolated, sdrInterpolated);

        return spaceInterpolate.interpolate(Z);
    }*/

}

