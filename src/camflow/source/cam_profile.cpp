
#include <map>
#include <vector>

#include "cam_params.h"

#include "array.h"

/*
 * File:   cam_profile.h
 * Author: vinod
 *
 * Created on January 18, 2009, 10:43 AM
 *File purpose:
 *  This class contains the implementation of initial profiles
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
 */
#include "cam_profile.h"
#include "comostrings.h"
#include <cmath>
using namespace Camflow;
using namespace Strings;

CamProfile::CamProfile(CamGeometry& cg)
:
mWidth(0.0),
mCenter(0.0),
fracType(0),
geom(cg),
flag_loadFracs(false),
flag_loadTemp(false)
{}

CamProfile::~CamProfile()
{
    //if (geom != NULL) delete geom;
}

/*
 *set products
 */
void CamProfile::setProductSpecies(std::map<std::string,double> spec){
    list_prdt = spec;
}
/*
 *set intermediates
 */
void CamProfile::setIntermediateSpecies(std::map<std::string,double> spec){
    list_intmd = spec;
}
/*
 *populate the product and intermediates
 */
void CamProfile::populateProducts(Mechanism& mech){
    if(list_prdt.size()>0)
        getmassFracs(list_prdt,mech,m_prdt);
}
void CamProfile::populateIntermdts(Mechanism& mech){
    if(list_intmd.size()>0)
        getmassFracs(list_intmd,mech,m_intmd);
}
/*
 *set the geometry object
 */
void CamProfile::setGeometryObj(CamGeometry& cg){
    geom = cg;
}
/*
 *set mixing center
 */
void CamProfile::setMixingCenter(double len){
    mCenter = len;
}
/*
 *set the mixing width
 */
void CamProfile::setMixingWidth(double len){
    mWidth = len;
}

/*
 *set start profile given 2 inletes
 */
void CamProfile::setStartprofile(CamBoundary& left, CamBoundary& right,
                                                            Mechanism& mech){
    /*
     *assign the oxidizer inlet species map to
     *product map
     */
    list_prdt = right.getInletSpecies();
    setStartProfile(left,mech);

}
/*
 *set the start profile
 */
void CamProfile::setStartProfile(CamBoundary& cb, Mechanism& mech){

    std::vector<double> m_in = cb.getInletMassfracs();
    std::vector<double> position = geom.getAxpos();
    int len = position.size();

    start.resize(len,mech.SpeciesCount());

    populateIntermdts(mech);
    populateProducts(mech);
    setGaussian(mech);

    if(mWidth != 0 && mCenter != 0 && m_prdt.size() != 0 && m_intmd.size() != 0){

        for(int i=0; i<len; i++){
            /*
             *sum the intermediates
             */
            double sumInter=0;
            for(unsigned int l=0; l<mech.SpeciesCount();l++){
                sumInter += start(i,l);
            }
            double factor = 1-sumInter;
            /*
             * scaling factor
             */
            double f_prdt, f_reac;
            if(position[i] <= (mCenter-mWidth/2.0)) {
                f_prdt = 0.0;
            }else{
                if(position[i] < (mCenter+mWidth/2.0)){
                    f_prdt = (1.0/mWidth)*(position[i]-mCenter)+0.5;
                }else{
                    f_prdt = 1.0;
                }
            }
            f_reac = 1-f_prdt;

            std::map<std::string, double>::iterator p;
            std::map<std::string,double> spec = cb.getInletSpecies();
            p = spec.begin();
            while(p!= spec.end()){
                int index = mech.FindSpecies(p->first);
                start(i,index) = factor*(f_prdt*m_prdt[index]+f_reac*m_in[index]);
                p++;
            }
            p=list_prdt.begin();
            while(p!=list_prdt.end()){
                int index = mech.FindSpecies(p->first);
                if(m_in[index]==0){
                    start(i,index) = factor*(f_prdt*m_prdt[index] + f_reac*m_in[index]);
                }
                p++;
            }

        }

    }else{
        for(int i=0; i<len; i++){
            for(unsigned int l=0; l<mech.SpeciesCount(); l++){
                start(i,l) = m_in[l];
            }
        }

    }

    // Overwrite the oxidiser inlet because otherwise we
    // will have machine zeros for intermediates.
    for(unsigned int l=0; l<mech.SpeciesCount(); l++){
        //start(0,l) = m_in[l];
    }

}
/*
 *set the gaussian
 */
void CamProfile::setGaussian(Mechanism& mech){
    std::vector<double> position = geom.getAxpos();
    size_t len = position.size();
    std::map<std::string, double>::iterator p;
    p = list_intmd.begin();
    while(p!=list_intmd.end()){
        size_t index = mech.FindSpecies(convertToCaps(trim(p->first)));
        double gWidth = -log(0.15*m_intmd[index])/pow(mWidth/2.0,2);
        for(size_t i=0; i<len; i++){
            start(i,index) = m_intmd[index]*exp(-gWidth*pow(position[i]-mCenter,2));
        }
        p++;

    }
}

/*
 *set the temperature profile based on a guassian distribution
 */

void CamProfile::setGaussTempProfile(std::vector<double>& vTemp){

    if(mWidth == 0.0 || mCenter == 0.0){
        throw CamError("Invalid mixing center and mixing width definition\n");
    }
    double dmax = 1.0;
    std::vector<double> position = geom.getAxpos();
    size_t len = position.size();
    vTemp.resize(len,0.0);
    double gWidth = -log(0.15*dmax)/pow(mWidth/2.0,2);
    for(size_t i=0; i<len; i++){
        double temp = exp(-gWidth*pow(position[i]-mCenter,2));
        vTemp[i] = temp*2000+300;
    }
}

/*
 *return the array
 */
Array2D& CamProfile::getStartProfile(){
    return start;
}
//return the species initial guesses
void CamProfile::getmassFracs(std::map<std::string,double>& spec, Mechanism& mech, std::vector<double>& frac){
    int index;
    frac.resize(mech.SpeciesCount(),0.0);
    std::vector<double> temp;
    temp.resize(mech.SpeciesCount(),0.0);
    std::map<std::string,double>::iterator p;
    p = spec.begin();
    while(p!=spec.end()){
        index = mech.FindSpecies(convertToCaps(trim(p->first)));
        if(index < 0)
           throw CamError("Species "+p->first +" not found in species list\n");
        else
            temp[index] = p->second;
        p++;
    }

    if(getFracType() == MASS){
        frac = temp;
    }else{
        CamConverter cc;
        cc.mole2mass(temp,frac,mech);
    }

}

void CamProfile::setUserTemp(double pos, double temp)
{
    flag_loadTemp = true;
    u_pos.push_back(pos);
    u_temp.push_back(temp);
}

void CamProfile::setUserFrac(double pos, double temp, std::string species)
{
    flag_loadFracs = true;
    u_species_pos.push_back(pos);
    u_frac.push_back(temp);
    u_species.push_back(species);
}

//return the user defined temperature
double CamProfile::getUserDefTemp(const double& pos)
{

    size_t len = u_pos.size();
    Utils::LinearInterpolator<double, double> mTempInterpolator(u_pos, u_temp);

    for (size_t i=0; i<len; ++i)
    {
        if(pos == u_pos[i])
        {
            return u_temp[i];
        }
        else if( (pos > u_pos[i]) && (pos < u_pos[i+1]) )
        {
            return mTempInterpolator.interpolate(pos);
        }
    }

}

double CamProfile::getUserDefFracs(const double& pos, const std::string species)
{

    std::vector<double> fracs,species_pos;
    size_t speciesIndex;
    size_t len = u_species_pos.size()/9;

    for (size_t i=0; i<u_species.size(); ++i)
    {
        if (species == u_species[i])
        {
            speciesIndex = i;
            break;
        }
    }

    for (size_t i=0; i<len; ++i)
    {
        fracs.push_back(u_frac[i+speciesIndex]);
        species_pos.push_back(u_species_pos[i]);
    }

    Utils::LinearInterpolator<double, double> mFracInterpolator(species_pos, fracs);

    for (size_t i=0; i<len; ++i)
    {
        if(pos == u_species_pos[i])
        {
            return fracs[i];
        }
        else if( (pos > u_species_pos[i]) && (pos < u_species_pos[i+1]) )
        {
            return mFracInterpolator.interpolate(pos);
        }
    }

}
std::vector<double>& CamProfile::getPosition(){
    return this->u_pos;
}
