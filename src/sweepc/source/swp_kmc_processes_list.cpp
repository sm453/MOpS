/*!
  * \Author     Zakwan Zainuddin (zz260)
  * \file       swp_kmc_processes_list.cpp
  *
  * \brief      implementation file for swp_kmc_processes_list.cpp
  *
  Project:      sweep (gas-phase chemistry solver).
  Sourceforge:  http://sourceforge.net/projects/mopssuite
  Copyright (C) 2010 Zakwan Zainuddin.

  File purpose:
    Defines namespace which contains all jump processes and related functions.

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

#include "swp_kmc_processes_list.h"
#include "swp_kmc_typedef.h"
#include "swp_kmc_pah_structure.h"
#include "swp_kmc_pah_process.h"
#include "swp_params.h"
#include <list>
#include <cmath>

using namespace Sweep;
using namespace Sweep::KMC_ARS;
using namespace Sweep::KMC_ARS::JumpProcessList;

typedef std::vector<Sweep::KMC_ARS::Reaction> rxnvector;
KMCGasPoint sp;
//! Returns a vector of jump processes
std::vector<JumpProcess*> JumpProcessList::obtainJumpProcess(const KMCGasPoint& gp){
    std::vector<JumpProcess*> temp;
    // Initialise all jump processes
    JumpProcess* j_G6R_AC = new G6R_AC; j_G6R_AC->initialise();
    JumpProcess* j_G6R_FE = new G6R_FE; j_G6R_FE->initialise();
    JumpProcess* j_L6_BY6 = new L6_BY6; j_L6_BY6->initialise();
    JumpProcess* j_PH_benz = new PH_benz; j_PH_benz->initialise();
    JumpProcess* j_D6R_FE3 = new D6R_FE3; j_D6R_FE3->initialise();
    JumpProcess* j_O6R_FE3_O2 = new O6R_FE3_O2; j_O6R_FE3_O2->initialise();
    JumpProcess* j_O6R_FE3_OH = new O6R_FE3_OH; j_O6R_FE3_OH->initialise();
    JumpProcess* j_O6R_FE_HACA_O2 = new O6R_FE_HACA_O2; j_O6R_FE_HACA_O2->initialise();
    JumpProcess* j_O6R_FE_HACA_OH = new O6R_FE_HACA_OH; j_O6R_FE_HACA_OH->initialise();
    JumpProcess* j_G5R_ZZ = new G5R_ZZ; j_G5R_ZZ->initialise();
    JumpProcess* j_D5R_R5 = new D5R_R5; j_D5R_R5->initialise();
    JumpProcess* j_C6R_AC_FE3 = new C6R_AC_FE3; j_C6R_AC_FE3->initialise();
    JumpProcess* j_C5R_RFE = new C5R_RFE; j_C5R_RFE->initialise();
    JumpProcess* j_C5R_RAC = new C5R_RAC; j_C5R_RAC->initialise();
    JumpProcess* j_M5R_RZZ = new M5R_RZZ; j_M5R_RZZ->initialise();
    JumpProcess* j_C6R_BY5_FE3 = new C6R_BY5_FE3; j_C6R_BY5_FE3->initialise();
    JumpProcess* j_C6R_BY5_FE3violi = new C6R_BY5_FE3violi; j_C6R_BY5_FE3violi->initialise();
    //JumpProcess* j_L5R_BY5 = new L5R_BY5; j_L5R_BY5->initialise();
    JumpProcess* j_M6R_BY5_FE3 = new M6R_BY5_FE3; j_M6R_BY5_FE3->initialise();
    // Jump Processes included in the model
    // (Comment out any process to be omitted):
    //--------------------------------------
    temp.push_back(j_G6R_AC); //        1- R6 Growth on AC [AR1]
    temp.push_back(j_G6R_FE); //        2- R6 Growth on FE [AR2]
    temp.push_back(j_L6_BY6); //        3- BY6 closure [AR14]
    temp.push_back(j_PH_benz); //       4- phenyl addition [AR15]
    temp.push_back(j_D6R_FE3); //       5- R6 Desorption at FE [AR8]
    temp.push_back(j_O6R_FE3_O2); //    6- R6 Oxidation at FE by O2 [AR10]
    temp.push_back(j_O6R_FE3_OH); //    7- R6 Oxidation at FE by OH [AR11]
    temp.push_back(j_O6R_FE_HACA_O2); // 8- R6 Oxidation at AC by O2 [AR12]
    temp.push_back(j_O6R_FE_HACA_OH); // 9- R6 Oxidation at AC by OH [AR13]
    temp.push_back(j_G5R_ZZ); //        10- R5 growth at ZZ [AR3]
    temp.push_back(j_D5R_R5); //        11- R5 desorption [AR7]
    temp.push_back(j_C6R_AC_FE3); //    12- R6 conversion to R5 [AR9]
    temp.push_back(j_C5R_RFE); //       13- R5 conversion to R6 on FE [AR5]
    temp.push_back(j_C5R_RAC); //       14- R5 conversion to R6 on AC [AR4]
    temp.push_back(j_M5R_RZZ); //       15- R5 migration to neighbouring ZZ [AR6]
    temp.push_back(j_C6R_BY5_FE3); //   16- R6 migration & conversion to R5 at BY5 [AR22]
    temp.push_back(j_C6R_BY5_FE3violi); //17- R6 migration & conversion to R5 at BY5 (violi) [AR24]
    //temp.push_back(j_L5R_BY5); //     18- BY5 closure [AR16]
    temp.push_back(j_M6R_BY5_FE3); // 19- R6 desorption at bay -> pyrene [AR21]
    //--------------------------------------
    return temp;
}
//std::vector<JumpProcess> process_vector = obtainJumpProcess();

//! Calculates jump rate for each jump process, returns total rate
real JumpProcessList::calculateRates(const KMCGasPoint& gp, 
                    PAHProcess& st, 
                    const real& t, 
                    std::vector<JumpProcess*>& jp,
                    rvector& rateV) {
    real temp=0;
    real pressure = gp.m_data[gp.P];
    // Choose suitable mechanism according to P
    if(pressure > 0.5 && pressure <= 5) { // mechanism at 1 atm 
        for(int i = 0; i!= (int) jp.size() ; i++) {
            (jp[i])->calculateElemRxnRate((jp[i])->getVec1(), gp);
            rateV[i] = (jp[i])->setRate1(gp, st/*, t*/);
            temp += rateV[i];
        }
    }else if(pressure > 0.01 && pressure <= 0.07) { // mechanism at 0.0267atm
        for(int i = 0; i!= (int) jp.size() ; i++) {
            (jp[i])->calculateElemRxnRate((jp[i])->getVec0p0267(), gp);
            rateV[i] = (jp[i])->setRate0p0267(gp, st/*, t*/);
            temp += rateV[i];
        }
    }else if(pressure > 0.07 && pressure <= 0.5) { // mechanism at 0.12atm
        for(int i = 0; i!= (int) jp.size() ; i++) {
            (jp[i])->calculateElemRxnRate((jp[i])->getVec0p12(), gp);
            rateV[i] = (jp[i])->setRate0p12(gp, st/*, t*/);
            temp += rateV[i];
        }
    }else std::cout<<"ERROR: No reaction mechanism for this pressure condition.\n";
    return temp;
}


//! Process list (rate calculations, energy units in kcal)
// For C++ jump process ID X, search for IDX
// For Matlab jump process X, search for ARX
// TEST: 4.563046 -> 4.56937799
//        11.51512 -> 11.53110048
//        13.354675 -> 13.37320574
//        38.463376 -> 38.51674641
// ************************************************************
// ID1- R6 growth on AC (AR1 on Matlab)
// ************************************************************

// Elementary rate constants, site type, process type and name
void G6R_AC::initialise() {
    // Adding elementary reactions
    // 0.0267 atm
    rxnvector& rxnV = m_rxnvector0p0267;
    addReaction(rxnV, Reaction(2.50e14, 0, 16.00, sp.H));      //0 - r1f
    addReaction(rxnV, Reaction(3.40e9, 0.88, 7.870, sp.H2));   //1 - r1b
    addReaction(rxnV, Reaction(2.10e13, 0, 4.56937799, sp.OH));  //2 - r2f
    addReaction(rxnV, Reaction(3.68e8, 1.139, 17.10, sp.H2O)); //3 - r2b
    addReaction(rxnV, Reaction(3.49e39, -7.77, 13.35468, sp.H));  //4 - r3f
    addReaction(rxnV, Reaction(1.87e7, 1.787, 3.262, sp.C2H2));   //5 - r4f
    //addReaction(rxnV, Reaction(2.20e12, 0, 7.5, sp.O2));          //6 - r5f
    // 0.12 atm
    rxnvector& rxnV2 = m_rxnvector0p12;
    addReaction(rxnV2, Reaction(2.50e14, 0, 16.00, sp.H));      //0 - r1f
    addReaction(rxnV2, Reaction(3.40e9, 0.88, 7.870, sp.H2));   //1 - r1b
    addReaction(rxnV2, Reaction(2.10e13, 0, 4.56937799, sp.OH));  //2 - r2f
    addReaction(rxnV2, Reaction(3.68e8, 1.139, 17.10, sp.H2O)); //3 - r2b
    addReaction(rxnV2, Reaction(2.18e35, -6.51, 11.53110048, sp.H));  //4 - r3f
    addReaction(rxnV2, Reaction(1.87e7, 1.787, 3.262, sp.C2H2));   //5 - r4f
    //addReaction(rxnV2, Reaction(9.7e3, 2.42, 38.46338, sp.O2));          //6 - r5f
    // 1 atm
    rxnvector& rxnV3 = m_rxnvector1;
    addReaction(rxnV3, Reaction(4.2e13, 0, 13.00, sp.H));      //0 - r1f
    addReaction(rxnV3, Reaction(3.9e12, 0, 11.00, sp.H2));   //1 - r1b
    addReaction(rxnV3, Reaction(1.0e10, 0.734, 1.43, sp.OH));  //2 - r2f
    addReaction(rxnV3, Reaction(3.68e8, 1.139, 17.10, sp.H2O)); //3 - r2b
    addReaction(rxnV3, Reaction(2.0e13, 0, 0, sp.H));  //4 - r3f
    addReaction(rxnV3, Reaction(8.0e7, 1.56, 3.8, sp.C2H2));   //5 - r4f
    //addReaction(rxnV3, Reaction(9.7e3, 2.42, 38.46338, sp.O2));          //6 - r5f

    m_sType = AC; // sitetype
    m_name = "G6R at AC"; // name of process
    m_ID = 1;
}
// Jump rate calculation
real G6R_AC::setRate0p0267(const KMCGasPoint& gp, PAHProcess& pah_st/*, const real& time_now*/) {
    // check if site count is zero
    real site_count = ((real)pah_st.getSiteCount(m_sType)); // Site count
    if(site_count==0) return m_rate=0;
    // calculate rate
    real r_denom = (m_r[1]+m_r[3]+m_r[4]+m_r[5]);
    real r_f; // radical fraction
    if(r_denom>0) {
        r_f = (m_r[0]+m_r[2])/r_denom; 
        r_f = r_f/(r_f+1.0);
    }
    else r_f=0;
    return m_rate = 2*m_r[5]*r_f* site_count; // Rate Equation
}
real G6R_AC::setRate0p12(const KMCGasPoint& gp, PAHProcess& pah_st/*, const real& time_now*/) {
    return setRate0p0267(gp, pah_st);
}
real G6R_AC::setRate1(const KMCGasPoint& gp, PAHProcess& pah_st/*, const real& time_now*/) {
    return setRate0p0267(gp, pah_st);
}
// 
// ************************************************************
// ID2- R6 growth on FE (AR2 on Matlab)
// ************************************************************

// Elementary rate constants, site type, process type and name
void G6R_FE::initialise() {
    // Adding elementary reactions
    // 0.0267 atm
    rxnvector& rxnV = m_rxnvector0p0267;
    addReaction(rxnV, Reaction(2.50e14, 0, 16.00, sp.H));      //0 - r1f
    addReaction(rxnV, Reaction(3.40e9, 0.88, 7.870, sp.H2));   //1 - r1b
    addReaction(rxnV, Reaction(2.10e13, 0, 4.569378, sp.OH));  //2 - r2f
    addReaction(rxnV, Reaction(3.68e8, 1.139, 17.10, sp.H2O)); //3 - r2b
    addReaction(rxnV, Reaction(8.02e19, -2.011, 1.968, sp.H));  //4 - r3f
    addReaction(rxnV, Reaction(7.50e26, -3.96, 17.10, sp.C2H2));   //5 - r4f1
    addReaction(rxnV, Reaction(4.40e49, -11.6, 19.30, sp.C2H2));   //6 - r4f2 (r4f = r4f1+r4f2)
    //addReaction(rxnV, Reaction(2.20e12, 0, 7.5, sp.O2));          //7 - r5f
    // 0.12 atm
    rxnvector& rxnV2 = m_rxnvector0p12;
    addReaction(rxnV2, Reaction(4.2e13, 0, 13.00, sp.H));      //0 - r1f
    addReaction(rxnV2, Reaction(3.40e9, 0.88, 7.870, sp.H2));   //1 - r1b
    addReaction(rxnV2, Reaction(2.10e13, 0, 4.56937799, sp.OH));  //2 - r2f
    addReaction(rxnV2, Reaction(3.68e8, 1.139, 17.10, sp.H2O)); //3 - r2b
    addReaction(rxnV2, Reaction(2.18e35, -6.51, 11.53110048, sp.H));  //4 - r3f
    addReaction(rxnV2, Reaction(9.1e24, -3.39, 20.4, sp.C2H2));   //5 - r4f1
    addReaction(rxnV2, Reaction(9.9e41, -9.26, 15.7, sp.C2H2));   //6 - r4f2 (r4f = r4f1+r4f2)
    //addReaction(rxnV2, Reaction(9.7e3, 2.42, 38.46338, sp.O2));          //7 - r5f
    // 1 atm
    rxnvector& rxnV3 = m_rxnvector1;
    addReaction(rxnV3, Reaction(4.2e13, 0, 13.00, sp.H));      //0 - r1f
    addReaction(rxnV3, Reaction(3.9e12, 0, 11.00, sp.H2));   //1 - r1b
    addReaction(rxnV3, Reaction(1.0e10, 0.734, 1.43, sp.OH));  //2 - r2f
    addReaction(rxnV3, Reaction(3.68e8, 1.139, 17.10, sp.H2O)); //3 - r2b
    addReaction(rxnV3, Reaction(2.0e13, 0, 0, sp.H));  //4 - r3f
    addReaction(rxnV3, Reaction(1.1e7, 1.71, 3.9, sp.C2H2));   //5 - r4f1
    addReaction(rxnV3, Reaction(3.3e33, -5.7, 25.5, sp.C2H2));   //6 - r4f2 (r4f = r4f1+r4f2)
    //addReaction(rxnV3, Reaction(9.7e3, 2.42, 38.46338, sp.O2));          //7 - r5f
    m_sType = FE; // sitetype
    m_name = "G6R at FE"; // name of process
    m_ID = 2;
}
// Jump rate calculation
real G6R_FE::setRate0p0267(const KMCGasPoint& gp, PAHProcess& pah_st/*, const real& time_now*/) {
    // check if site count is zero
    real site_count = ((real)pah_st.getSiteCount(m_sType)); // Site count
    if(site_count==0) return m_rate=0;
    
    
    real r_denom = (m_r[1]+m_r[3]+m_r[4]+m_r[5]+m_r[6]);
    real r_f; // radical fraction
    if(r_denom>0) {
        r_f = (m_r[0]+m_r[2])/r_denom; 
        r_f = r_f/(r_f+1.0);
    }
    else r_f=0;
    return m_rate = (m_r[5]+m_r[6])*r_f* site_count; // Rate Equation
}
real G6R_FE::setRate0p12(const KMCGasPoint& gp, PAHProcess& pah_st/*, const real& time_now*/) {
    return setRate0p0267(gp, pah_st);
}
real G6R_FE::setRate1(const KMCGasPoint& gp, PAHProcess& pah_st/*, const real& time_now*/) {
    return setRate0p0267(gp, pah_st);
}
// 
// ************************************************************
// ID3- BY6 closure reaction (AR14 on Matlab)
// ************************************************************

// Elementary rate constants, site type, process type and name
void L6_BY6::initialise() {
    // Adding elementary reactions
    // 0.0267 atm
    rxnvector& rxnV = m_rxnvector0p0267;
    addReaction(rxnV, Reaction(9.24e7, 1.5, 9.646, sp.H));      //0 - r1f
    addReaction(rxnV, Reaction(9.6e4, 1.96, 9.021, sp.H2));   //1 - r1b
    addReaction(rxnV, Reaction(2.1e13, 0, 4.56937799, sp.OH));  //2 - r2f
    addReaction(rxnV, Reaction(3.68e8, 1.139, 17.10, sp.H2O)); //3 - r2b
    addReaction(rxnV, Reaction(3.49e39, -7.77, 13.35468, sp.H));  //4 - r3f
    addReaction(rxnV, Reaction(1.11e11, .658, 23.99, sp.None));   //5 - r4f
    //addReaction(rxnV, Reaction(2.20e12, 0, 7.5, sp.O2));          //6 - r5f
    // 0.12 atm
    rxnvector& rxnV2 = m_rxnvector0p12;
    addReaction(rxnV2, Reaction(9.24e7, 1.5, 9.646, sp.H));      //0 - r1f
    addReaction(rxnV2, Reaction(9.6e4, 1.96, 9.021, sp.H2));   //1 - r1b
    addReaction(rxnV2, Reaction(2.1e13, 0, 4.56937799, sp.OH));  //2 - r2f
    addReaction(rxnV2, Reaction(3.68e8, 1.139, 17.10, sp.H2O)); //3 - r2b
    addReaction(rxnV2, Reaction(8.02e19, -2.011, 1.968, sp.H));  //4 - r3f
    addReaction(rxnV2, Reaction(2.22e11, .658, 23.99, sp.None));   //5 - r4f
    //addReaction(rxnV2, Reaction(9.7e3, 2.42, 38.46338, sp.O2));          //6 - r5f
    // 1 atm
    rxnvector& rxnV3 = m_rxnvector1;
    addReaction(rxnV3, Reaction(9.24e7, 1.5, 9.646, sp.H));      //0 - r1f
    addReaction(rxnV3, Reaction(9.6e4, 1.96, 9.021, sp.H2));   //1 - r1b
    addReaction(rxnV3, Reaction(2.1e13, 0, 4.56937799, sp.OH));  //2 - r2f
    addReaction(rxnV3, Reaction(3.68e8, 1.139, 17.10, sp.H2O)); //3 - r2b
    addReaction(rxnV3, Reaction(8.02e19, -2.011, 1.968, sp.H));  //4 - r3f
    addReaction(rxnV3, Reaction(1.11e11, .658, 23.99, sp.None));   //5 - r4f
    //addReaction(rxnV3, Reaction(9.7e3, 2.42, 38.46338, sp.O2));          //6 - r5f
    m_sType = BY6; // sitetype
    m_name = "BY6 closure"; // name of process
    m_ID = 3;
}
// Jump rate calculation
real L6_BY6::setRate0p0267(const KMCGasPoint& gp, PAHProcess& pah_st/*, const real& time_now*/) {
    // check if site count is zero
    real site_count = ((real)pah_st.getSiteCount(m_sType)); // Site count
    if(site_count==0) return m_rate=0;
    
    
    real r_denom = (m_r[1]+m_r[3]+m_r[4]+m_r[5]);
    real r_f; // radical fraction 
    if(r_denom>0) {
        r_f = (m_r[0]+m_r[2])/r_denom; 
        r_f = r_f/(r_f+1.0);
    }
    else r_f=0;
    return m_rate = 2*m_r[5]*r_f* site_count; // Rate Equation
}
real L6_BY6::setRate0p12(const KMCGasPoint& gp, PAHProcess& pah_st/*, const real& time_now*/) {
    return setRate0p0267(gp, pah_st);
}
real L6_BY6::setRate1(const KMCGasPoint& gp, PAHProcess& pah_st/*, const real& time_now*/) {
    return setRate0p0267(gp, pah_st);
}
// 
// ************************************************************
// ID4- phenyl addition (AR15 in Matlab)
// ************************************************************

// Elementary rate constants, site type, process type and name
void PH_benz::initialise() {
    // Adding elementary reactions
    // 0.0267 atm
    rxnvector& rxnV = m_rxnvector0p0267;
    addReaction(rxnV, Reaction(2.5e14, 0, 16.00, sp.H));      //0 - r1f
    addReaction(rxnV, Reaction(3.4e9, .88, 7.870, sp.H2));   //1 - r1b
    addReaction(rxnV, Reaction(2.1e13, 0, 4.56937799, sp.OH));  //2 - r2f
    addReaction(rxnV, Reaction(3.68e8, 1.139, 17.10, sp.H2O)); //3 - r2b
    addReaction(rxnV, Reaction(2.08e30, -4.98, 5.290, sp.H));  //4 - r3f
    addReaction(rxnV, Reaction(2e76, -18.4456, 46.93, sp.C6H6));   //5 - r4f
    //addReaction(rxnV, Reaction(2.20e12, 0, 7.5, sp.O2));          //6 - r5f
    // 0.12 atm
    rxnvector& rxnV2 = m_rxnvector0p12;
    addReaction(rxnV2, Reaction(4.2e13, 0, 13.00, sp.H));      //0 - r1f
    addReaction(rxnV2, Reaction(3.4e9, .88, 7.870, sp.H2));   //1 - r1b
    addReaction(rxnV2, Reaction(2.1e13, 0, 4.56937799, sp.OH));  //2 - r2f
    addReaction(rxnV2, Reaction(3.68e8, 1.139, 17.10, sp.H2O)); //3 - r2b
    addReaction(rxnV2, Reaction(2.18e35, -6.51, 11.53110048, sp.H));  //4 - r3f
    addReaction(rxnV2, Reaction(2.2e36, -8.21, 9.92, sp.C6H6));   //5 - r4f
    //addReaction(rxnV2, Reaction(9.7e3, 2.42, 38.46338, sp.O2));          //6 - r5f
    // 1 atm
    rxnvector& rxnV3 = m_rxnvector1;
    addReaction(rxnV3, Reaction(4.2e13, 0, 13.00, sp.H));      //0 - r1f
    addReaction(rxnV3, Reaction(3.9e12, 0, 11.00, sp.H2));   //1 - r1b
    addReaction(rxnV3, Reaction(1.0e10, .734, 1.43, sp.OH));  //2 - r2f
    addReaction(rxnV3, Reaction(3.68e8, 1.139, 17.10, sp.H2O)); //3 - r2b
    addReaction(rxnV3, Reaction(2.0e13, 0, 0, sp.H));  //4 - r3f
    addReaction(rxnV3, Reaction(1.9e76, -18.4043, 47.87, sp.C6H6));   //5 - r4f
    //addReaction(rxnV3, Reaction(9.7e3, 2.42, 38.46338, sp.O2));          //6 - r5f

    m_sType = benz; // sitetype
    m_name = "Phenyl addition"; // name of process
    m_ID = 4;
}
// Jump rate calculation
real PH_benz::setRate0p0267(const KMCGasPoint& gp, PAHProcess& pah_st/*, const real& time_now*/) {
    // check if site count is zero
    real site_count = ((real)pah_st.getSiteCount(m_sType)); // Site count
    if(site_count==0) return m_rate=0;
    
    
    real r_denom = (m_r[1]+m_r[3]+m_r[4]+m_r[5]);
    real r_f; // radical fraction 
    if(r_denom>0) {
        r_f = (m_r[0]+m_r[2])/r_denom; 
        r_f = r_f/(r_f+1.0);
    }
    else r_f=0;
    if((real)pah_st.getSiteCount(R5)!=0) site_count++;
    return m_rate = m_r[5]*r_f* site_count; // Rate Equation
}
real PH_benz::setRate0p12(const KMCGasPoint& gp, PAHProcess& pah_st/*, const real& time_now*/) {
    return setRate0p0267(gp, pah_st);
}
real PH_benz::setRate1(const KMCGasPoint& gp, PAHProcess& pah_st/*, const real& time_now*/) {
    return setRate0p0267(gp, pah_st);
}
// 
// ************************************************************
// ID5- R6 desorption at FE (AR8 in Matlab)
// ************************************************************

// Elementary rate constants, site type, process type and name
void D6R_FE3::initialise() {
    // Adding elementary reactions
    // 0.0267 atm
    rxnvector& rxnV = m_rxnvector0p0267;
    addReaction(rxnV, Reaction(3.23e7, 2.095, 15.84, sp.H));      //0 - r1f
    addReaction(rxnV, Reaction(3.4e9, .88, 7.870, sp.H2));   //1 - r1b
    addReaction(rxnV, Reaction(2.1e13, 0, 4.56937799, sp.OH));  //2 - r2f
    addReaction(rxnV, Reaction(3.68e8, 1.139, 17.10, sp.H2O)); //3 - r2b
    addReaction(rxnV, Reaction(3.49e39, -7.77, 13.37320574, sp.H));  //4 - r3f
    addReaction(rxnV, Reaction(1.5e11, 0.2, 42.57, sp.None));   //5 - r4f
    //addReaction(rxnV, Reaction(2.20e12, 0, 7.5, sp.O2));          //6 - r5f
    // 0.12 atm
    rxnvector& rxnV2 = m_rxnvector0p12;
    addReaction(rxnV2, Reaction(4.2e13, 0, 13.00, sp.H));      //0 - r1f
    addReaction(rxnV2, Reaction(3.4e9, .88, 7.870, sp.H2));   //1 - r1b
    addReaction(rxnV2, Reaction(2.1e13, 0, 4.56937799, sp.OH));  //2 - r2f
    addReaction(rxnV2, Reaction(3.68e8, 1.139, 17.10, sp.H2O)); //3 - r2b
    addReaction(rxnV2, Reaction(2.18e35, -6.51, 11.53110048, sp.H));  //4 - r3f
    addReaction(rxnV2, Reaction(2e11, 0.2, 42.57, sp.None));   //5 - r4f
    //addReaction(rxnV2, Reaction(9.7e3, 2.42, 38.46338, sp.O2));          //6 - r5f
    // 1 atm
    rxnvector& rxnV3 = m_rxnvector1;
    addReaction(rxnV3, Reaction(4.2e13, 0, 13.00, sp.H));      //0 - r1f
    addReaction(rxnV3, Reaction(3.9e12, 0, 11.00, sp.H2));   //1 - r1b
    addReaction(rxnV3, Reaction(1.0e10, 0.734, 1.43, sp.OH));  //2 - r2f
    addReaction(rxnV3, Reaction(3.68e8, 1.139, 17.10, sp.H2O)); //3 - r2b
    addReaction(rxnV3, Reaction(2.0e13, 0, 0, sp.H));  //4 - r3f
    addReaction(rxnV3, Reaction(1.3e11, 1.08, 70.42, sp.None));   //5 - r4f
    //addReaction(rxnV3, Reaction(9.7e3, 2.42, 38.46338, sp.O2));          //6 - r5f
    m_sType = FE3; // sitetype
    m_name = "R6 (FE3) desorption"; // name of process
    m_ID = 5;
}
// Jump rate calculation
real D6R_FE3::setRate0p0267(const KMCGasPoint& gp, PAHProcess& pah_st/*, const real& time_now*/) {
    // check if site count is zero
    real site_count = ((real)pah_st.getSiteCount(m_sType)); // Site count
    if(site_count==0) return m_rate=0;
    
    real r_denom = (m_r[1]+m_r[3]+m_r[4]+m_r[5]);
    real r_f; // radical fraction 
    if(r_denom>0) {
        r_f = (m_r[0]+m_r[2])/r_denom; 
        r_f = r_f/(r_f+1.0);
    }
    else r_f=0;
    return m_rate = m_r[5]*r_f* site_count; // Rate Equation
}
real D6R_FE3::setRate0p12(const KMCGasPoint& gp, PAHProcess& pah_st/*, const real& time_now*/) {
    return setRate0p0267(gp, pah_st);
}
real D6R_FE3::setRate1(const KMCGasPoint& gp, PAHProcess& pah_st/*, const real& time_now*/) {
    return setRate0p0267(gp, pah_st);
}
// ************************************************************
// ID6- R6 oxidation at FE by O2 (AR10 in Matlab)
// ************************************************************

// Elementary rate constants, site type, process type and name
void O6R_FE3_O2::initialise() {
    // Adding elementary reactions
    // 0.0267 atm
    rxnvector& rxnV = m_rxnvector0p0267;
    addReaction(rxnV, Reaction(3.23e7, 2.095, 15.84, sp.H));      //0 - r1f
    addReaction(rxnV, Reaction(3.4e9, .88, 7.870, sp.H2));   //1 - r1b
    addReaction(rxnV, Reaction(2.1e13, 0, 4.56937799, sp.OH));  //2 - r2f
    addReaction(rxnV, Reaction(3.68e8, 1.139, 17.10, sp.H2O)); //3 - r2b
    addReaction(rxnV, Reaction(3.49e39, -7.77, 13.37320574, sp.H));  //4 - r3f
    addReaction(rxnV, Reaction(9.7e3, 2.42, 38.51674641, sp.O2));   //5 - r5f(1)
    //addReaction(rxnV, Reaction(2.20e12, 0, 7.5, sp.O2));          //6 - r5f(2)
    // 0.12 atm
    rxnvector& rxnV2 = m_rxnvector0p12;
    addReaction(rxnV2, Reaction(4.2e13, 0, 13.00, sp.H));      //0 - r1f
    addReaction(rxnV2, Reaction(3.4e9, .88, 7.870, sp.H2));   //1 - r1b
    addReaction(rxnV2, Reaction(2.1e13, 0, 4.56937799, sp.OH));  //2 - r2f
    addReaction(rxnV2, Reaction(3.68e8, 1.139, 17.10, sp.H2O)); //3 - r2b
    addReaction(rxnV2, Reaction(2.18e35, -6.51, 11.53110048, sp.H));  //4 - r3f
    addReaction(rxnV2, Reaction(9.7e3, 2.42, 38.51674641, sp.O2));   //5 - r4f
    //addReaction(rxnV2, Reaction(9.7e3, 2.42, 38.51674641, sp.O2));          //6 - r5f(2)
    // 1 atm
    rxnvector& rxnV3 = m_rxnvector1;
    addReaction(rxnV3, Reaction(4.2e13, 0, 13.00, sp.H));      //0 - r1f
    addReaction(rxnV3, Reaction(3.9e12, 0, 11.00, sp.H2));   //1 - r1b
    addReaction(rxnV3, Reaction(1.0e10, 0.734, 1.43, sp.OH));  //2 - r2f
    addReaction(rxnV3, Reaction(3.68e8, 1.139, 17.10, sp.H2O)); //3 - r2b
    addReaction(rxnV3, Reaction(2.0e13, 0, 0, sp.H));  //4 - r3f
    addReaction(rxnV3, Reaction(9.7e3, 2.42, 38.46338, sp.O2));   //5 - r4f
    //addReaction(rxnV3, Reaction(9.7e3, 2.42, 38.46338, sp.O2));          //6 - r5f
    m_sType = FE3; // sitetype
    m_name = "R6 (FE3) oxidation by O2"; // name of process
    m_ID = 6;
}
// Jump rate calculation
real O6R_FE3_O2::setRate0p0267(const KMCGasPoint& gp, PAHProcess& pah_st/*, const real& time_now*/) {
    // check if site count is zero
    real site_count = ((real)pah_st.getSiteCount(m_sType)); // Site count
    if(site_count==0) return m_rate=0;
    
    real r_denom = (m_r[1]+m_r[3]+m_r[4]);
    real r_f; // radical fraction 
    if(r_denom>0) {
        r_f = (m_r[0]+m_r[2])/r_denom; 
        r_f = r_f/(r_f+1.0);
    }
    else r_f=0;
    return m_rate = m_r[5]*r_f* site_count; // Rate Equation
}
real O6R_FE3_O2::setRate0p12(const KMCGasPoint& gp, PAHProcess& pah_st/*, const real& time_now*/) {
    // check if site count is zero
    real site_count = ((real)pah_st.getSiteCount(m_sType)); // Site count
    if(site_count==0) return m_rate=0;
    
    real r_denom = (m_r[1]+m_r[3]+m_r[4]+m_r[5]);
    real r_f; // radical fraction 
    if(r_denom>0) {
        r_f = (m_r[0]+m_r[2])/r_denom; 
        r_f = r_f/(r_f+1.0);
    }
    else r_f=0;
    return m_rate = m_r[5]*r_f* site_count; // Rate Equation
}
real O6R_FE3_O2::setRate1(const KMCGasPoint& gp, PAHProcess& pah_st/*, const real& time_now*/) {
    return setRate0p12(gp, pah_st);
}
// ************************************************************
// ID7- R6 oxidation at FE by OH (AR11 in Matlab)
// ************************************************************

// Elementary rate constants, site type, process type and name
void O6R_FE3_OH::initialise() {
    // Adding elementary reactions
    // 0.0267atm
    rxnvector& rxnV = m_rxnvector0p0267;
    addReaction(rxnV, Reaction(3.23e7, 2.095, 15.84, sp.H));      //0 - r1f
    addReaction(rxnV, Reaction(3.4e9, .88, 7.870, sp.H2));   //1 - r1b
    addReaction(rxnV, Reaction(2.1e13, 0, 4.56937799, sp.OH));  //2 - r2f
    addReaction(rxnV, Reaction(3.68e8, 1.139, 17.10, sp.H2O)); //3 - r2b
    addReaction(rxnV, Reaction(3.49e39, -7.77, 13.37320574, sp.H));  //4 - r3f
    addReaction(rxnV, Reaction(1.3e11, 1.08, 70.42, sp.OH));   //5 - r4f
    addReaction(rxnV, Reaction(1.3e13, 0, 10.62, sp.OH));          //6 - r5f
    // 0.12atm
    rxnvector& rxnV2 = m_rxnvector0p12;
    //addReaction(rxnV2, Reaction(4.2e13, 0, 13, sp.H));      //0 - r1f
    //addReaction(rxnV2, Reaction(3.4e9, .88, 7.870, sp.H2));   //1 - r1b
    //addReaction(rxnV2, Reaction(2.1e13, 0, 4.56937799, sp.OH));  //2 - r2f
    //addReaction(rxnV2, Reaction(3.68e8, 1.139, 17.10, sp.H2O)); //3 - r2b
    //addReaction(rxnV2, Reaction(2.18e35, -6.51, 11.53110048, sp.H));  //4 - r3f
    addReaction(rxnV2, Reaction(1.3e13, 0, 10.62, sp.OH));   //5 - r4f
    //addReaction(rxnV2, Reaction(9.7e3, 2.42, 38.51674641, sp.O2));          //6 - r5f
    // 1 atm
    rxnvector& rxnV3 = m_rxnvector1;
    //addReaction(rxnV3, Reaction(4.2e13, 0, 13.00, sp.H));      //0 - r1f
    //addReaction(rxnV3, Reaction(3.9e12, 0, 11.00, sp.H2));   //1 - r1b
    //addReaction(rxnV3, Reaction(1.0e10, 0.734, 1.43, sp.OH));  //2 - r2f
    //addReaction(rxnV3, Reaction(3.68e8, 1.139, 17.10, sp.H2O)); //3 - r2b
    //addReaction(rxnV3, Reaction(2.0e13, 0, 0, sp.H));  //4 - r3f
    addReaction(rxnV3, Reaction(1.3e13, 0, 10.62, sp.OH));   //5 - r4f
    //addReaction(rxnV3, Reaction(9.7e3, 2.42, 38.46338, sp.O2));          //6 - r5f
    m_sType = FE3; // sitetype
    m_name = "R6 (FE3) oxidation by OH"; // name of process
    m_ID = 7;
}
// Jump rate calculation
real O6R_FE3_OH::setRate0p0267(const KMCGasPoint& gp, PAHProcess& pah_st/*, const real& time_now*/) {
    // check if site count is zero
    real site_count = ((real)pah_st.getSiteCount(m_sType)); // Site count
    if(site_count==0) return m_rate=0;
    
    
    real r_denom = (m_r[1]+m_r[3]+m_r[4]+m_r[5]);
    real r_f; // radical fraction 
    if(r_denom>0) {
        r_f = (m_r[0]+m_r[2])/r_denom; 
        r_f = r_f/(r_f+1.0);
    }
    else r_f=0;
    return m_rate = m_r[6]*r_f* site_count; // Rate Equation
}
real O6R_FE3_OH::setRate0p12(const KMCGasPoint& gp, PAHProcess& pah_st/*, const real& time_now*/) {
    // check if site count is zero
    real site_count = ((real)pah_st.getSiteCount(m_sType)); // Site count
    if(site_count==0) return m_rate=0;
    
    //real r_denom = (m_r[1]+m_r[3]+m_r[4]+m_r[5]);
    //real r_f; // radical fraction 
    //if(r_denom>0) {
    //    r_f = (m_r[0]+m_r[2])/r_denom; 
    //    r_f = r_f/(r_f+1.0);
    //}
    //else r_f=0;
    return m_rate = m_r[0]*site_count; // Rate Equation
}
real O6R_FE3_OH::setRate1(const KMCGasPoint& gp, PAHProcess& pah_st/*, const real& time_now*/) {
    return setRate0p12(gp, pah_st);
}
// ************************************************************
// ID8- R6 oxidation at AC by O2 (AR12 in Matlab)
// ************************************************************

// Elementary rate constants, site type, process type and name
void O6R_FE_HACA_O2::initialise() {
    // Adding elementary reactions
    // 0.0267atm
    rxnvector& rxnV = m_rxnvector0p0267;
    addReaction(rxnV, Reaction(3.23e7, 2.095, 15.84, sp.H));      //0 - r1f
    addReaction(rxnV, Reaction(3.4e9, .88, 7.870, sp.H2));   //1 - r1b
    addReaction(rxnV, Reaction(2.1e13, 0, 4.56937799, sp.OH));  //2 - r2f
    addReaction(rxnV, Reaction(3.68e8, 1.139, 17.10, sp.H2O)); //3 - r2b
    addReaction(rxnV, Reaction(3.49e39, -7.77, 13.37320574, sp.H));  //4 - r3f
    addReaction(rxnV, Reaction(9.7e3, 2.42, 38.51674641, sp.O2));   //5 - r5f(1)
    //addReaction(rxnV, Reaction(2.20e12, 0, 7.5, sp.O2));          //6 - r5f(2)
    // 0.12atm
    rxnvector& rxnV2 = m_rxnvector0p12;
    addReaction(rxnV2, Reaction(4.2e13, 0, 13.00, sp.H));      //0 - r1f
    addReaction(rxnV2, Reaction(3.4e9, .88, 7.870, sp.H2));   //1 - r1b
    addReaction(rxnV2, Reaction(2.1e13, 0, 4.56937799, sp.OH));  //2 - r2f
    addReaction(rxnV2, Reaction(3.68e8, 1.139, 17.10, sp.H2O)); //3 - r2b
    addReaction(rxnV2, Reaction(2.18e35, -6.51, 11.53110048, sp.H));  //4 - r3f
    addReaction(rxnV2, Reaction(9.7e3, 2.42, 38.51674641, sp.O2));   //5 - r4f
    //addReaction(rxnV2, Reaction(9.7e3, 2.42, 38.51674641, sp.O2));          //6 - r5f
    // 1 atm
    rxnvector& rxnV3 = m_rxnvector1;
    addReaction(rxnV3, Reaction(4.2e13, 0, 13.00, sp.H));      //0 - r1f
    addReaction(rxnV3, Reaction(3.9e12, 0, 11.00, sp.H2));   //1 - r1b
    addReaction(rxnV3, Reaction(1.0e10, 0.734, 1.43, sp.OH));  //2 - r2f
    addReaction(rxnV3, Reaction(3.68e8, 1.139, 17.10, sp.H2O)); //3 - r2b
    addReaction(rxnV3, Reaction(2.0e13, 0, 0, sp.H));  //4 - r3f
    addReaction(rxnV3, Reaction(9.7e3, 2.42, 38.46338, sp.O2));   //5 - r4f
    //addReaction(rxnV3, Reaction(9.7e3, 2.42, 38.46338, sp.O2));          //6 - r5f
    m_sType = FE_HACA; // sitetype
    m_name = "R6 (FE_HACA) oxidation by O2"; // name of process
    m_ID = 8;
}
// Jump rate calculation
real O6R_FE_HACA_O2::setRate0p0267(const KMCGasPoint& gp, PAHProcess& pah_st/*, const real& time_now*/) {
    // check if site count is zero
    real site_count = ((real)pah_st.getSiteCount(m_sType)); // Site count
    if(site_count==0) return m_rate=0;
    
    real r_denom = (m_r[1]+m_r[3]+m_r[4]);
    real r_f; // radical fraction 
    if(r_denom>0) {
        r_f = (m_r[0]+m_r[2])/r_denom; 
        r_f = r_f/(r_f+1.0);
    }
    else r_f=0;
    return m_rate = m_r[5]*r_f* site_count; // Rate Equation
}
real O6R_FE_HACA_O2::setRate0p12(const KMCGasPoint& gp, PAHProcess& pah_st/*, const real& time_now*/) {
    // check if site count is zero
    real site_count = ((real)pah_st.getSiteCount(m_sType)); // Site count
    if(site_count==0) return m_rate=0;
    
    real r_denom = (m_r[1]+m_r[3]+m_r[4]+m_r[5]);
    real r_f; // radical fraction 
    if(r_denom>0) {
        r_f = (m_r[0]+m_r[2])/r_denom; 
        r_f = r_f/(r_f+1.0);
    }
    else r_f=0;
    return m_rate = m_r[5]*r_f* site_count; // Rate Equation
}
real O6R_FE_HACA_O2::setRate1(const KMCGasPoint& gp, PAHProcess& pah_st/*, const real& time_now*/) {
    return setRate0p12(gp, pah_st);
}

// ************************************************************
// ID9- R6 oxidation at AC by OH (AR13 in Matlab)
// ************************************************************

// Elementary rate constants, site type, process type and name
void O6R_FE_HACA_OH::initialise() {
    // Adding elementary reactions
    // 0.0267 atm
    rxnvector& rxnV = m_rxnvector0p0267;
    addReaction(rxnV, Reaction(3.23e7, 2.095, 15.84, sp.H));      //0 - r1f
    addReaction(rxnV, Reaction(3.4e9, .88, 7.870, sp.H2));   //1 - r1b
    addReaction(rxnV, Reaction(2.1e13, 0, 4.56937799, sp.OH));  //2 - r2f
    addReaction(rxnV, Reaction(3.68e8, 1.139, 17.10, sp.H2O)); //3 - r2b
    addReaction(rxnV, Reaction(3.49e39, -7.77, 13.37320574, sp.H));  //4 - r3f
    addReaction(rxnV, Reaction(1.3e11, 1.08, 70.42, sp.OH));   //5 - r4f
    addReaction(rxnV, Reaction(1.3e13, 0, 10.62, sp.OH));          //6 - r5f
    // 0.12 atm
    rxnvector& rxnV2 = m_rxnvector0p12;
    //addReaction(rxnV2, Reaction(4.2e13, 0, 13, sp.H));      //0 - r1f
    //addReaction(rxnV2, Reaction(3.4e9, .88, 7.870, sp.H2));   //1 - r1b
    //addReaction(rxnV2, Reaction(2.1e13, 0, 4.56937799, sp.OH));  //2 - r2f
    //addReaction(rxnV2, Reaction(3.68e8, 1.139, 17.10, sp.H2O)); //3 - r2b
    //addReaction(rxnV2, Reaction(2.18e35, -6.51, 11.53110048, sp.H));  //4 - r3f
    addReaction(rxnV2, Reaction(1.3e13, 0, 10.62, sp.OH));   //5 - r4f
    //addReaction(rxnV2, Reaction(9.7e3, 2.42, 38.51674641, sp.OH));          //6 - r5f
    // 1 atm
    rxnvector& rxnV3 = m_rxnvector1;
    //addReaction(rxnV3, Reaction(4.2e13, 0, 13.00, sp.H));      //0 - r1f
    //addReaction(rxnV3, Reaction(3.9e12, 0, 11.00, sp.H2));   //1 - r1b
    //addReaction(rxnV3, Reaction(1.0e10, 0.734, 1.43, sp.OH));  //2 - r2f
    //addReaction(rxnV3, Reaction(3.68e8, 1.139, 17.10, sp.H2O)); //3 - r2b
    //addReaction(rxnV3, Reaction(2.0e13, 0, 0, sp.H));  //4 - r3f
    addReaction(rxnV3, Reaction(1.3e13, 0, 10.62, sp.OH));   //5 - r4f
    //addReaction(rxnV3, Reaction(9.7e3, 2.42, 38.46338, sp.O2));          //6 - r5f
    m_sType = FE_HACA; // sitetype
    m_name = "R6 (FE_HACA) oxidation by OH"; // name of process
    m_ID = 9;
}
// Jump rate calculation
real O6R_FE_HACA_OH::setRate0p0267(const KMCGasPoint& gp, PAHProcess& pah_st/*, const real& time_now*/) {
    // check if site count is zero
    real site_count = ((real)pah_st.getSiteCount(m_sType)); // Site count
    if(site_count==0) return m_rate=0;

    real r_denom = (m_r[1]+m_r[3]+m_r[4]+m_r[5]);
    real r_f; // radical fraction 
    if(r_denom>0) {
        r_f = (m_r[0]+m_r[2])/r_denom; 
        r_f = r_f/(r_f+1.0);
    }
    else r_f=0;
    return m_rate = m_r[6]*r_f* site_count; // Rate Equation
}
real O6R_FE_HACA_OH::setRate0p12(const KMCGasPoint& gp, PAHProcess& pah_st/*, const real& time_now*/) {
    // check if site count is zero
    real site_count = ((real)pah_st.getSiteCount(m_sType)); // Site count
    if(site_count==0) return m_rate=0;

    //real r_denom = (m_r[1]+m_r[3]+m_r[4]+m_r[5]);
    //real r_f; // radical fraction 
    //if(r_denom>0) {
    //    r_f = (m_r[0]+m_r[2])/r_denom; 
    //    r_f = r_f/(r_f+1.0);
    //}
    //else r_f=0;
    return m_rate = m_r[0]* site_count; // Rate Equation
}
real O6R_FE_HACA_OH::setRate1(const KMCGasPoint& gp, PAHProcess& pah_st/*, const real& time_now*/) {
    return setRate0p12(gp, pah_st);
}

// ************************************************************
// ID10- R5 growth on ZZ (AR3 in Matlab)
// ************************************************************

// Elementary rate constants, site type, process type and name
void G5R_ZZ::initialise() {
    // Adding elementary reactions
    // 0.0267 atm
    rxnvector& rxnV = m_rxnvector0p0267;
    addReaction(rxnV, Reaction(2.50e14, 0, 16.00, sp.H));      //0 - r1f
    addReaction(rxnV, Reaction(3.4e9, .88, 7.870, sp.H2));   //1 - r1b
    addReaction(rxnV, Reaction(2.1e13, 0, 4.56937799, sp.OH));  //2 - r2f
    addReaction(rxnV, Reaction(3.68e8, 1.139, 17.10, sp.H2O)); //3 - r2b
    addReaction(rxnV, Reaction(3.49e39, -7.77, 13.37320574, sp.H));  //4 - r3f
    addReaction(rxnV, Reaction(3.57e24, -3.176, 14.86, sp.C2H2));   //5 - r4f
    //addReaction(rxnV, Reaction(2.20e12, 0, 7.5, sp.O2));          //6 - r5f(2)
    // 0.12 atm
    rxnvector& rxnV2 = m_rxnvector0p12;
    addReaction(rxnV2, Reaction(4.2e13, 0, 13.00, sp.H));      //0 - r1f
    addReaction(rxnV2, Reaction(3.4e9, .88, 7.870, sp.H2));   //1 - r1b
    addReaction(rxnV2, Reaction(2.1e13, 0, 4.56937799, sp.OH));  //2 - r2f
    addReaction(rxnV2, Reaction(3.68e8, 1.139, 17.10, sp.H2O)); //3 - r2b
    addReaction(rxnV2, Reaction(2.18e35, -6.51, 11.53110048, sp.H));  //4 - r3f
    addReaction(rxnV2, Reaction(1.8e33, -5.91, 19.7, sp.C2H2));   //5 - r4f
    addReaction(rxnV2, Reaction(3.4e43, -9.56, 18.2, sp.C2H2));   //6 - r4f (T<800)
    //addReaction(rxnV2, Reaction(9.7e3, 2.42, 38.51674641, sp.O2));          //7 - r5f(2)
    // 1 atm
    rxnvector& rxnV3 = m_rxnvector1;
    addReaction(rxnV3, Reaction(4.2e13, 0, 13.00, sp.H));      //0 - r1f
    addReaction(rxnV3, Reaction(3.9e12, 0, 11.00, sp.H2));   //1 - r1b
    addReaction(rxnV3, Reaction(1.0e10, 0.734, 1.43, sp.OH));  //2 - r2f
    addReaction(rxnV3, Reaction(3.68e8, 1.139, 17.10, sp.H2O)); //3 - r2b
    addReaction(rxnV3, Reaction(2.0e13, 0, 0, sp.H));  //4 - r3f
    addReaction(rxnV3, Reaction(6.8e11, 0, 22.0, sp.C2H2));   //5 - r4f
    //addReaction(rxnV3, Reaction(9.7e3, 2.42, 38.46338, sp.O2));          //6 - r5f
    m_sType = ZZ; // sitetype
    m_name = "R5 growth on ZZ"; // name of process
    m_ID = 10;
}
// Jump rate calculation
real G5R_ZZ::setRate0p0267(const KMCGasPoint& gp, PAHProcess& pah_st/*, const real& time_now*/) {
    // check if site count is zero
    real site_count = ((real)pah_st.getSiteCount(m_sType)); // Site count
    if(site_count==0) return m_rate=0;
    
    real r_denom = (m_r[1]+m_r[3]+m_r[4]+m_r[5]);
    real r_f; // radical fraction 
    if(r_denom>0) {
        r_f = (m_r[0]+m_r[2])/r_denom; 
        r_f = r_f/(r_f+1.0);
    }
    else r_f=0;
    return m_rate = m_r[5]*r_f* site_count; // Rate Equation
}
real G5R_ZZ::setRate0p12(const KMCGasPoint& gp, PAHProcess& pah_st/*, const real& time_now*/) {
    // check if site count is zero
    real site_count = ((real)pah_st.getSiteCount(m_sType)); // Site count
    if(site_count==0) return m_rate=0;
    
    real r4f = m_r[5];
    if(gp.m_data[gp.T] < 800) r4f = m_r[6];
    real r_denom = (m_r[1]+m_r[3]+m_r[4]+r4f);
    real r_f; // radical fraction 
    if(r_denom>0) {
        r_f = (m_r[0]+m_r[2])/r_denom; 
        r_f = r_f/(r_f+1.0);
    }
    else r_f=0;
    return m_rate = r4f*r_f* site_count; // Rate Equation
}
real G5R_ZZ::setRate1(const KMCGasPoint& gp, PAHProcess& pah_st/*, const real& time_now*/) {
    return setRate0p0267(gp, pah_st);
}


// ************************************************************
// ID11- R5 desorption (AR7 in Matlab)
// ************************************************************

// Elementary rate constants, site type, process type and name
void D5R_R5::initialise() {
    // Adding elementary reactions
    // 0.0267 atm
    rxnvector& rxnV = m_rxnvector0p0267;
    addReaction(rxnV, Reaction(5.07e7, 1.93, 12.95, sp.H));      //0 - r1f
    addReaction(rxnV, Reaction(9.45e3, 2.56, 5.007, sp.H2));   //1 - r1b
    addReaction(rxnV, Reaction(2.1e13, 0, 4.56937799, sp.OH));  //2 - r2f
    addReaction(rxnV, Reaction(3.68e8, 1.139, 17.10, sp.H2O)); //3 - r2b
    //r3f to be calculated in setRate0p0267
    addReaction(rxnV, Reaction(1.60e14, 0, 42.42, sp.None));   //4 - r4f
    addReaction(rxnV, Reaction(1.4e30, -3.86, 3.320, sp.None)); // 5 - k_o: rate calculation
    addReaction(rxnV, Reaction(6.08e12, .27, 0.280, sp.None)); // 6 - k_inf: rate calculation
    //addReaction(rxnV, Reaction(2.20e12, 0, 7.5, sp.O2));          //7 - r5f(2)
    // 0.12 atm
    rxnvector& rxnV2 = m_rxnvector0p12;
    rxnV2 = rxnV;
    //addReaction(rxnV2, Reaction(5.07e7, 1.93, 12.95, sp.H));      //0 - r1f
    //addReaction(rxnV2, Reaction(9.45e3, 2.56, 5.007, sp.H2));   //1 - r1b
    //addReaction(rxnV2, Reaction(2.1e13, 0, 4.56937799, sp.OH));  //2 - r2f
    //addReaction(rxnV2, Reaction(3.68e8, 1.139, 17.10, sp.H2O)); //3 - r2b
    ////r3f to be calculated in setRate0p0267
    //addReaction(rxnV2, Reaction(1.60e14, 0, 42.42, sp.None));   //4 - r4f
    //addReaction(rxnV2, Reaction(1.4e30, -3.86, 3.320, sp.None)); // 5 - k_o: rate calculation
    //addReaction(rxnV2, Reaction(6.08e12, .27, 0.280, sp.None)); // 6 - k_inf: rate calculation
    //addReaction(rxnV2, Reaction(9.7e3, 2.42, 38.51674641, sp.O2));          //7 - r5f(2)
    // 1 atm
    rxnvector& rxnV3 = m_rxnvector1;
    addReaction(rxnV3, Reaction(5.06e7, 1.93, 12.95, sp.H));      //0 - r1f
    addReaction(rxnV3, Reaction(1.28e6, 1.93, 62.34, sp.H2));   //1 - r1b
    addReaction(rxnV3, Reaction(1.0e10, 0.734, 1.43, sp.OH));  //2 - r2f
    addReaction(rxnV3, Reaction(3.68e8, 1.139, 17.10, sp.H2O)); //3 - r2b
    addReaction(rxnV3, Reaction(6.08e12, 0, 0, sp.H));  //4 - r3f
    addReaction(rxnV3, Reaction(1.6e14, 0, 42.42, sp.None));   //5 - r4f
    //addReaction(rxnV3, Reaction(9.7e3, 2.42, 38.46338, sp.O2));          //6 - r5f
    m_sType = R5; // sitetype
    m_name = "R5 desorption"; // name of process
    m_ID = 11;
}
// Jump rate calculation
real D5R_R5::setRate0p0267(const KMCGasPoint& gp, PAHProcess& pah_st/*, const real& time_now*/) {
    // check if site count is zero
    real site_count = ((real)pah_st.getSiteCount(m_sType)); // Site count
    if(site_count==0) return m_rate=0;

    // Calculating r3f
    real T = gp.m_data[gp.T];
    real M = (2*gp.m_data[gp.H2])+(6*gp.m_data[gp.H2O])+(2*gp.m_data[gp.CH4])+(1.5*gp.m_data[gp.CO])+(2*gp.m_data[gp.CO2])+(3*gp.m_data[gp.C2H6]);
    real k_o = m_r[5];
    real k_inf = m_r[6];
    real F_cent = 0.218 * exp(-T/207.5) + 0.782*exp(-T/2663) + exp(-6095/T);
    real logF_cent = log(F_cent);
    real r_c = k_inf/(1+ (k_inf/(k_o*M)));
    real N0 = 0.75- 1.27*logF_cent;
    real C0 = -0.4 - 0.67*logF_cent;
    real logP_r = log(k_o*M/k_inf);
    real F = exp(logF_cent/(1+ pow(((logP_r+C0)/(N0-0.14*(logP_r+C0))),2)));
    r_c *= F;
    real r3f = r_c* gp.m_data[gp.H];

    real r_denom = (m_r[1]+m_r[3]+r3f+m_r[4]);
    real r_f; // radical fraction 
    if(r_denom>0) {
        r_f = (m_r[0]+m_r[2])/r_denom; 
        r_f = r_f/(r_f+1.0);
    }
    else r_f=0;
    return m_rate = m_r[4]*r_f* site_count; // Rate Equation
}
real D5R_R5::setRate0p12(const KMCGasPoint& gp, PAHProcess& pah_st/*, const real& time_now*/) {
    return setRate0p0267(gp, pah_st);
}
real D5R_R5::setRate1(const KMCGasPoint& gp, PAHProcess& pah_st/*, const real& time_now*/) {
    // check if site count is zero
    real site_count = ((real)pah_st.getSiteCount(m_sType)); // Site count
    if(site_count==0) return m_rate=0;
    
    real r_denom = (m_r[1]+m_r[3]+m_r[4]+m_r[5]);
    real r_f; // radical fraction 
    if(r_denom>0) {
        r_f = (m_r[0]+m_r[2])/r_denom; 
        r_f = r_f/(r_f+1.0);
    }
    else r_f=0;
    return m_rate = m_r[5]*r_f* site_count; // Rate Equation
}
// ************************************************************
// ID12- R6 conversion to R5 (AR9 in Matlab)
// ************************************************************

// Elementary rate constants, site type, process type and name
void C6R_AC_FE3::initialise() {
    // Adding elementary reactions
    // 0.0267 atm
    rxnvector& rxnV = m_rxnvector0p0267;
    addReaction(rxnV, Reaction(3.23e7, 2.095, 15.84, sp.H));      //0 - r1f
    addReaction(rxnV, Reaction(3.4e9, .88, 7.870, sp.H2));   //1 - r1b
    addReaction(rxnV, Reaction(2.1e13, 0, 4.56937799, sp.OH));  //2 - r2f
    addReaction(rxnV, Reaction(3.68e8, 1.139, 17.10, sp.H2O)); //3 - r2b
    addReaction(rxnV, Reaction(3.49e39, -7.77, 13.37320574, sp.H));  //4 - r3f
    addReaction(rxnV, Reaction(1.5e11, 0.2, 42.57, sp.None));   //5 - r4f
    //addReaction(rxnV, Reaction(2.20e12, 0, 7.5, sp.O2));          //6 - r5f(2)
    // 0.12 atm
    rxnvector& rxnV2 = m_rxnvector0p12;
    addReaction(rxnV2, Reaction(4.2e13, 00, 13.00, sp.H));      //0 - r1f
    addReaction(rxnV2, Reaction(3.4e9, .88, 7.870, sp.H2));   //1 - r1b
    addReaction(rxnV2, Reaction(2.1e13, 0, 4.56937799, sp.OH));  //2 - r2f
    addReaction(rxnV2, Reaction(3.68e8, 1.139, 17.10, sp.H2O)); //3 - r2b
    addReaction(rxnV2, Reaction(2.18e35, -6.51, 11.53110048, sp.H));  //4 - r3f
    addReaction(rxnV2, Reaction(2e11, 0.2, 42.57, sp.None));   //5 - r4f
    //addReaction(rxnV2, Reaction(9.7e3, 2.42, 38.51674641, sp.O2));          //6 - r5f(2)
    // 1 atm
    rxnvector& rxnV3 = m_rxnvector1;
    addReaction(rxnV3, Reaction(4.2e13, 00, 13.00, sp.H));      //0 - r1f
    addReaction(rxnV3, Reaction(3.9e12, 0, 11.00, sp.H2));   //1 - r1b
    addReaction(rxnV3, Reaction(1.0e10, 0.734, 1.43, sp.OH));  //2 - r2f
    addReaction(rxnV3, Reaction(3.68e8, 1.139, 17.10, sp.H2O)); //3 - r2b
    addReaction(rxnV3, Reaction(2.0e13, 0, 0, sp.H));  //4 - r3f
    addReaction(rxnV3, Reaction(1.3e11, 1.08, 70.42, sp.None));   //5 - r4f
    //addReaction(rxnV3, Reaction(9.7e3, 2.42, 38.51674641, sp.O2));          //6 - r5f(2)
    m_sType = AC_FE3; // sitetype
    m_name = "R6 conversion to R5"; // name of process
    m_ID = 12;
}
// Jump rate calculation
real C6R_AC_FE3::setRate0p0267(const KMCGasPoint& gp, PAHProcess& pah_st/*, const real& time_now*/) {
    // check if site count is zero
    real site_count = ((real)pah_st.getSiteCount(m_sType)); // Site count
    if(site_count==0) return m_rate=0;
    
    
    real r_denom = (m_r[1]+m_r[3]+m_r[4]+m_r[5]);
    real r_f; // radical fraction 
    if(r_denom>0) {
        r_f = (m_r[0]+m_r[2])/r_denom; 
        r_f = r_f/(r_f+1.0);
    }
    else r_f=0;
    return m_rate = m_r[5]*r_f* site_count; // Rate Equation
}
real C6R_AC_FE3::setRate0p12(const KMCGasPoint& gp, PAHProcess& pah_st/*, const real& time_now*/) {
    return setRate0p0267(gp, pah_st);
}
real C6R_AC_FE3::setRate1(const KMCGasPoint& gp, PAHProcess& pah_st/*, const real& time_now*/) {
    return setRate0p0267(gp, pah_st);
}

// ************************************************************
// ID13- R5 conversion to R6 on FE (AR5 in Matlab)
// ************************************************************

// Elementary rate constants, site type, process type and name
void C5R_RFE::initialise() {
    // Adding elementary reactions
    // 0.0267 atm
    rxnvector& rxnV = m_rxnvector0p0267;
    addReaction(rxnV, Reaction(2.50e14, 0, 16.00, sp.H));      //0 - r1f
    addReaction(rxnV, Reaction(3.4e9, .88, 7.870, sp.H2));   //1 - r1b
    addReaction(rxnV, Reaction(2.1e13, 0, 4.56937799, sp.OH));  //2 - r2f
    addReaction(rxnV, Reaction(3.68e8, 1.139, 17.10, sp.H2O)); //3 - r2b
    addReaction(rxnV, Reaction(3.49e39, -7.77, 13.37320574, sp.H));  //4 - r3f
    addReaction(rxnV, Reaction(7.50e26, -3.96, 17.10, sp.C2H2));   //5 - r4f1
    addReaction(rxnV, Reaction(4.40e49, -11.6, 19.30, sp.C2H2));   //6 - r4f2 (r4f = r4f1+r4f2)
    addReaction(rxnV, Reaction(8.42e8, 1.49, 0.990, sp.H));         //7 - for rate calc
    addReaction(rxnV, Reaction(6.28e37, -8.24, 44.67, sp.None));       //8 - for rate calc
    //addReaction(rxnV, Reaction(2.20e12, 0, 7.5, sp.O2));          //9 - r5f
    // 0.12 atm
    rxnvector& rxnV2 = m_rxnvector0p12;
    addReaction(rxnV2, Reaction(4.2e13, 0, 13.00, sp.H));      //0 - r1f
    addReaction(rxnV2, Reaction(3.4e9, .88, 7.870, sp.H2));   //1 - r1b
    addReaction(rxnV2, Reaction(2.1e13, 0, 4.56937799, sp.OH));  //2 - r2f
    addReaction(rxnV2, Reaction(3.68e8, 1.139, 17.10, sp.H2O)); //3 - r2b
    addReaction(rxnV2, Reaction(2.18e35, -6.51, 11.53110048, sp.H));  //4 - r3f
    addReaction(rxnV2, Reaction(9.1e24, -3.39, 20.4, sp.C2H2));   //5 - r4f1
    addReaction(rxnV2, Reaction(9.90e41, -9.26, 15.7, sp.C2H2));   //6 - r4f2 (r4f = r4f1+r4f2)
    addReaction(rxnV2, Reaction(8.42e8, 1.49, 0.990, sp.H));         //7 - for rate calc
    addReaction(rxnV2, Reaction(6.28e37, -8.24, 44.67, sp.None));       //8 - for rate calc
    //addReaction(rxnV2, Reaction(9.7e3, 2.42, 38.51674641, sp.O2));          //9 - r5f
    // 1 atm
    rxnvector& rxnV3 = m_rxnvector1;
    addReaction(rxnV3, Reaction(4.2e13, 0, 13.00, sp.H));      //0 - r1f
    addReaction(rxnV3, Reaction(3.9e12, 0, 11.00, sp.H2));   //1 - r1b
    addReaction(rxnV3, Reaction(1.0e10, 0.734, 1.43, sp.OH));  //2 - r2f
    addReaction(rxnV3, Reaction(3.68e8, 1.139, 17.10, sp.H2O)); //3 - r2b
    addReaction(rxnV3, Reaction(2.0e13, 0, 0, sp.H));  //4 - r3f
    addReaction(rxnV3, Reaction(1.1e7, 1.71, 3.9, sp.C2H2));   //5 - r4f1
    addReaction(rxnV3, Reaction(3.3e33, -5.7, 25.5, sp.C2H2));   //6 - r4f2 (r4f = r4f1+r4f2)
    //addReaction(rxnV3, Reaction(9.7e3, 2.42, 38.46338, sp.O2));          //7 - r5f
    m_sType = RFE; // sitetype
    m_name = "R5 conversion to R6 on FE"; // name of process
    m_ID = 13;
}
// Jump rate calculation
real C5R_RFE::setRate0p0267(const KMCGasPoint& gp, PAHProcess& pah_st/*, const real& time_now*/) {
    // check if site count is zero
    real site_count = ((real)pah_st.getSiteCount(m_sType)); // Site count
    if(site_count==0) return m_rate=0;
    // calculate rate
    real r_denom = (m_r[1]+m_r[3]+m_r[4]+m_r[5]+m_r[6]);
    real r_f; // radical fraction 
    if(r_denom>0) {
        r_f = (m_r[0]+m_r[2])/r_denom; 
        real f_R5H = m_r[7]/(m_r[8] + (m_r[0]/r_denom * (m_r[5]+m_r[6]))); 
        r_f *= f_R5H;
        r_f = r_f/(r_f+1.0);
    }
    else r_f=0;
    return m_rate = (m_r[5]+m_r[6])*r_f* site_count; // Rate Equation
}
real C5R_RFE::setRate0p12(const KMCGasPoint& gp, PAHProcess& pah_st/*, const real& time_now*/) {
    return setRate0p0267(gp, pah_st);
}
real C5R_RFE::setRate1(const KMCGasPoint& gp, PAHProcess& pah_st/*, const real& time_now*/) {
    // check if site count is zero
    real site_count = ((real)pah_st.getSiteCount(m_sType)); // Site count
    if(site_count==0) return m_rate=0;
    // calculate rate
    real r4f = m_r[5]+m_r[6];
    real r_denom = (m_r[1]+m_r[3]+m_r[4]+r4f);
    real r_f; // radical fraction 
    if(r_denom>0) {
        r_f = (m_r[0]+m_r[2])/r_denom; 
        real f_R5H = m_r[0]/(m_r[1] + ((m_r[0]/r_denom) * r4f)); 
        r_f = r_f*f_R5H;
        r_f = r_f/(r_f+1.0);
    }
    else r_f=0;
    return m_rate = r4f*r_f* site_count; // Rate Equation
}


// ************************************************************
// ID14- R5 conversion to R6 on AC (AR4 in Matlab)
// ************************************************************
// Elementary rate constants, site type, process type and name
void C5R_RAC::initialise() {
    // Adding elementary reactions
    // 0.0267 atm
    rxnvector& rxnV = m_rxnvector0p0267;
    addReaction(rxnV, Reaction(8.42e8, 1.49, 0.990, sp.H));      //0 - r1f
    addReaction(rxnV, Reaction(6.28e37, -8.24, 44.67, sp.None));   //1 - r1b
    addReaction(rxnV, Reaction(2.1e13, 0, 4.56937799, sp.OH));  //2 - r2f
    addReaction(rxnV, Reaction(3.68e8, 1.139, 17.10, sp.H2O)); //3 - r2b
    // r3f defined in setRate0p0267, r4f = 0
    //addReaction(rxnV, Reaction(2.20e12, 0, 7.5, sp.O2));          //4 - r5f
    // 0.12 atm
    rxnvector& rxnV2 = m_rxnvector0p12;
    addReaction(rxnV2, Reaction(8.42e8, 1.49, 0.990, sp.H));      //0 - r1f
    addReaction(rxnV2, Reaction(1.25e38, -8.20, 45.31, sp.None));   //1 - r1b
    addReaction(rxnV2, Reaction(2.1e13, 0, 4.56937799, sp.OH));  //2 - r2f
    addReaction(rxnV2, Reaction(3.68e8, 1.139, 17.10, sp.H2O)); //3 - r2b
    // r3f defined in setRate0p12, r4f = 0
    //addReaction(rxnV2, Reaction(9.7e3, 2.42, 38.51674641, sp.O2));          //4 - r5f
    // 1 atm
    rxnvector& rxnV3 = m_rxnvector1;
    addReaction(rxnV3, Reaction(8.41e8, 1.49, 0.980, sp.H));      //0 - r1f
    addReaction(rxnV3, Reaction(3.81e11, 0.49, 59.05, sp.None));   //1 - r1b
    addReaction(rxnV3, Reaction(1.0e10, .734, 1.43, sp.OH));  //2 - r2f
    addReaction(rxnV3, Reaction(3.68e8, 1.139, 17.10, sp.H2O)); //3 - r2b
    // r3f defined in setRate1, r4f = 0
    //addReaction(rxnV3, Reaction(9.7e3, 2.42, 38.51674641, sp.O2));          //4 - r5f
    m_sType = RAC; // sitetype
    m_name = "R5 conversion to R6 on AC"; // name of process
    m_ID = 14;
}
// Jump rate calculation
real C5R_RAC::setRate0p0267(const KMCGasPoint& gp, PAHProcess& pah_st/*, const real& time_now*/) {
    // check if site count is zero
    real site_count = ((real)pah_st.getSiteCount(m_sType)); // Site count
    if(site_count==0) return m_rate=0;
    // calculate rate
    real r3f = 7.297e8 * gp.m_data[gp.T] - 5.0641e11; 
    real r_denom = (m_r[1]+m_r[3]+r3f);
    real r_f; // radical fraction 
    if(r_denom>0) {
        r_f = (m_r[0]+m_r[2])/r_denom; 
        r_f = r_f/(r_f+1.0);
    }
    else r_f=0;
    return m_rate = 1.34e12*r_f* site_count; // Rate Equation
}
real C5R_RAC::setRate0p12(const KMCGasPoint& gp, PAHProcess& pah_st/*, const real& time_now*/) {
    // check if site count is zero
    real site_count = ((real)pah_st.getSiteCount(m_sType)); // Site count
    if(site_count==0) return m_rate=0;
    // calculate rate
    real r3f = 7.297e8 * gp.m_data[gp.T] - 5.0641e11; 
    real r_denom = (m_r[1]+m_r[3]+r3f);
    real r_f; // radical fraction 
    if(r_denom>0) {
        r_f = (m_r[0]+m_r[2])/r_denom; 
        r_f = r_f/(r_f+1.0);
    }
    else r_f=0;
    return m_rate = r3f*r_f* site_count; // Rate Equation
}
real C5R_RAC::setRate1(const KMCGasPoint& gp, PAHProcess& pah_st/*, const real& time_now*/) {
    // check if site count is zero
    real site_count = ((real)pah_st.getSiteCount(m_sType)); // Site count
    if(site_count==0) return m_rate=0;
    // calculate rate
    real r3f = 1.34e12; 
    real r_denom = (m_r[1]+m_r[3]+r3f);
    real r_f; // radical fraction 
    if(r_denom>0) {
        r_f = (m_r[0]+m_r[2])/r_denom; 
        r_f = r_f/(r_f+1.0);
    }
    else r_f=0;
    return m_rate = r3f*r_f* site_count; // Rate Equation
}


// ************************************************************
// ID15- R5 migration to neighbouring ZZ (AR6 in Matlab)
// ************************************************************

// Elementary rate constants, site type, process type and name
void M5R_RZZ::initialise() {
    // Adding elementary reactions
    // 0.0267 atm
    rxnvector& rxnV = m_rxnvector0p0267;
    addReaction(rxnV, Reaction(8.42e8, 1.49, 0.990, sp.H));      //0 - r1f
    addReaction(rxnV, Reaction(6.28e37, -8.24, 44.67, sp.None));   //1 - r1b
    addReaction(rxnV, Reaction(2.1e13, 0, 4.56937799, sp.OH));  //2 - r2f
    addReaction(rxnV, Reaction(3.68e8, 1.139, 17.10, sp.H2O)); //3 - r2b
    // r3f defined in setRate0p0267, r4f = 0
    //addReaction(rxnV, Reaction(2.20e12, 0, 7.5, sp.O2));          //4 - r5f
    // 0.12 atm
    rxnvector& rxnV2 = m_rxnvector0p12;
    addReaction(rxnV2, Reaction(8.42e8, 1.49, 0.990, sp.H));      //0 - r1f
    addReaction(rxnV2, Reaction(1.25e38, -8.20, 45.31, sp.None));   //1 - r1b
    addReaction(rxnV2, Reaction(2.1e13, 0, 4.56937799, sp.OH));  //2 - r2f
    addReaction(rxnV2, Reaction(3.68e8, 1.139, 17.10, sp.H2O)); //3 - r2b
    // r3f defined in setRate0p12, r4f = 0
    //addReaction(rxnV2, Reaction(2.20e12, 0, 7.5, sp.O2));          //4 - r5f
    // 1 atm
    rxnvector& rxnV3 = m_rxnvector1;
    addReaction(rxnV3, Reaction(8.41e8, 1.49, 0.980, sp.H));      //0 - r1f
    addReaction(rxnV3, Reaction(3.81e11, .49, 59.05, sp.None));   //1 - r1b
    addReaction(rxnV3, Reaction(1.0e10, 0.734, 1.43, sp.OH));  //2 - r2f
    addReaction(rxnV3, Reaction(3.68e8, 1.139, 17.10, sp.H2O)); //3 - r2b
    // r3f defined in setRate1, r4f = 0
    //addReaction(rxnV3, Reaction(2.20e12, 0, 7.5, sp.O2));          //4 - r5f
    m_sType = RZZ; // sitetype
    m_name = "R5 migration to neighbouring ZZ"; // name of process
    m_ID = 15;
}
// Jump rate calculation
real M5R_RZZ::setRate0p0267(const KMCGasPoint& gp, PAHProcess& pah_st/*, const real& time_now*/) {
    // check if site count is zero
    real site_count = ((real)pah_st.getSiteCount(m_sType)); // Site count
    if(site_count==0) return m_rate=0;
    // calculate rate
    real r3f = 7.297e8 * gp.m_data[gp.T] - 5.0641e11; 
    real r_denom = (m_r[1]+m_r[3]+r3f);
    real r_f; // radical fraction 
    if(r_denom>0) {
        r_f = (m_r[0]+m_r[2])/r_denom; 
        r_f = r_f/(r_f+1.0);
    }
    else r_f=0;
    return m_rate = 1.34e12*r_f* site_count; // Rate Equation
}
real M5R_RZZ::setRate0p12(const KMCGasPoint& gp, PAHProcess& pah_st/*, const real& time_now*/) {
    // check if site count is zero
    real site_count = ((real)pah_st.getSiteCount(m_sType)); // Site count
    if(site_count==0) return m_rate=0;
    // calculate rate
    real r3f = 7.297e8 * gp.m_data[gp.T] - 5.0641e11; 
    real r_denom = (m_r[1]+m_r[3]+r3f);
    real r_f; // radical fraction 
    if(r_denom>0) {
        r_f = (m_r[0]+m_r[2])/r_denom; 
        r_f = r_f/(r_f+1.0);
    }
    else r_f=0;
    return m_rate = r3f*r_f* site_count; // Rate Equation
}
real M5R_RZZ::setRate1(const KMCGasPoint& gp, PAHProcess& pah_st/*, const real& time_now*/) {
    // check if site count is zero
    real site_count = ((real)pah_st.getSiteCount(m_sType)); // Site count
    if(site_count==0) return m_rate=0;
    // calculate rate
    real r3f = 1.34e12;
    real r_denom = (m_r[1]+m_r[3]+r3f);
    real r_f; // radical fraction 
    if(r_denom>0) {
        r_f = (m_r[0]+m_r[2])/r_denom; 
        r_f = r_f/(r_f+1.0);
    }
    else r_f=0;
    return m_rate = r3f*r_f* site_count; // Rate Equation
}


// ************************************************************
// ID16- R6 migration & conversion to R5 at BY5 (pyrene+R5; pathway 1; AR22 in Matlab)
// ************************************************************

// Elementary rate constants, site type, process type and name
void C6R_BY5_FE3::initialise() {
    // Adding elementary reactions
    // 0.0267atm
    rxnvector& rxnV = m_rxnvector0p0267;
    addReaction(rxnV, Reaction(7.81e7, 1.772, 10.333, sp.H));      //0 - r1f
    addReaction(rxnV, Reaction(1.85e6, 1.86, 11.04, sp.H2));   //1 - r1b
    addReaction(rxnV, Reaction(2.1e13, 0, 4.56937799, sp.OH));  //2 - r2f
    addReaction(rxnV, Reaction(3.68e8, 1.139, 17.10, sp.H2O)); //3 - r2b
    addReaction(rxnV, Reaction(3.49e39, -7.77, 13.37320574, sp.H)); //4 - r3f
    addReaction(rxnV, Reaction(2.3e9, 1.6031, 61.85, sp.None)); //5 - r4f
    //addReaction(rxnV, Reaction(2.20e12, 0, 7.5, sp.O2));          //6 - r5f
    // 0.12 atm
    rxnvector& rxnV2 = m_rxnvector0p12;
    rxnV2 = rxnV;
    // 1 atm
    rxnvector& rxnV3 = m_rxnvector1;
    rxnV3 = rxnV;

    m_sType = BY5_FE3; // sitetype
    m_name = "R6 migration & conversion to R5 at BY5 (pathway 1)"; // name of process
    m_ID = 16;
}
// Jump rate calculation
real C6R_BY5_FE3::setRate0p0267(const KMCGasPoint& gp, PAHProcess& pah_st/*, const real& time_now*/) {
    // check if site count is zero
    real site_count = ((real)pah_st.getSiteCount(m_sType)); // Site count
    if(site_count==0) return m_rate=0;
    // calculate rate
    real r_denom = (m_r[1]+m_r[3]+m_r[4]+m_r[5]);
    real r_f; // radical fraction 
    if(r_denom>0) {
        r_f = (m_r[0]+m_r[2])/r_denom; 
        r_f = r_f/(r_f+1.0);
    }
    else r_f=0;
    return m_rate = m_r[5]*r_f* site_count; // Rate Equation
}
real C6R_BY5_FE3::setRate0p12(const KMCGasPoint& gp, PAHProcess& pah_st/*, const real& time_now*/) {
    return setRate0p0267(gp, pah_st);
}
real C6R_BY5_FE3::setRate1(const KMCGasPoint& gp, PAHProcess& pah_st/*, const real& time_now*/) {
    return setRate0p0267(gp, pah_st);
}

// ************************************************************
// ID17- R6 migration & conversion to R5 at BY5 (pyrene+R5; pathway 2-violi; AR24 in Matlab)
// ************************************************************

// Elementary rate constants, site type, process type and name
void C6R_BY5_FE3violi::initialise() {
    // Adding elementary reactions
    // 0.0267 atm
    rxnvector& rxnV = m_rxnvector0p0267;
    addReaction(rxnV, Reaction(7.25e7, 1.76, 9.69, sp.H));      //0 - r1f
    addReaction(rxnV, Reaction(1.85e6, 1.86, 11.04, sp.H2));   //1 - r1b
    addReaction(rxnV, Reaction(2.1e13, 0, 4.56937799, sp.OH));  //2 - r2f
    addReaction(rxnV, Reaction(3.68e8, 1.139, 17.10, sp.H2O)); //3 - r2b
    addReaction(rxnV, Reaction(3.49e39, -7.77, 13.37320574, sp.H)); //4 - r3f
    addReaction(rxnV, Reaction(1.23e10, 1.41, 85.2, sp.None)); //5 - r4f
    //addReaction(rxnV, Reaction(2.20e12, 0, 7.5, sp.O2));          //6
    // 0.12 atm
    rxnvector& rxnV2 = m_rxnvector0p12;
    rxnV2 = rxnV;
    // 1 atm
    rxnvector& rxnV3 = m_rxnvector1;
    rxnV3 = rxnV;

    m_sType = BY5_FE3; // sitetype
    m_name = "R6 migration & conversion to R5 at BY5 (pathway 2; violi)"; // name of process
    m_ID = 17;
}
// Jump rate calculation
real C6R_BY5_FE3violi::setRate0p0267(const KMCGasPoint& gp, PAHProcess& pah_st/*, const real& time_now*/) {
    // check if site count is zero
    real site_count = ((real)pah_st.getSiteCount(m_sType)); // Site count
    if(site_count==0) return m_rate=0;
    // calculate rate
    real r_denom = (m_r[1]+m_r[3]+m_r[4]+m_r[5]);
    real r_f; // radical fraction 
    if(r_denom>0) {
        r_f = (m_r[0]+m_r[2])/r_denom; 
        r_f = r_f/(r_f+1.0);
    }
    else r_f=0;
    return m_rate = m_r[5]*r_f* site_count; // Rate Equation
}
real C6R_BY5_FE3violi::setRate0p12(const KMCGasPoint& gp, PAHProcess& pah_st/*, const real& time_now*/) {
    return setRate0p0267(gp, pah_st);
}
real C6R_BY5_FE3violi::setRate1(const KMCGasPoint& gp, PAHProcess& pah_st/*, const real& time_now*/) {
    return setRate0p0267(gp, pah_st);
}
// ************************************************************
// ID18- BY5 closure (AR16 in Matlab)
// ************************************************************

// Elementary rate constants, site type, process type and name
bool violi = false;
void L5R_BY5::initialise() {
    // Adding elementary reactions
    // 0.0267 atm
    rxnvector rxnV = m_rxnvector0p0267;
    addReaction(rxnV, Reaction(1.73e7, 1.7453, 8.732, sp.H));      //0 - r1f
    addReaction(rxnV, Reaction(3.40e9, .88, 7.870, sp.H2));   //1 - r1b
    addReaction(rxnV, Reaction(2.1e13, 0, 4.56937799, sp.OH));  //2 - r2f
    addReaction(rxnV, Reaction(3.68e8, 1.139, 17.10, sp.H2O)); //3 - r2b
    addReaction(rxnV, Reaction(3.49e39, -7.77, 13.37320574, sp.H)); //4 - r3f
    if(violi) {
        addReaction(rxnV, Reaction(3.86e11, .21, 17.4, sp.None)); //5 - r4f - violi(2005) mechanism, include reverse.
    }
    else {
        addReaction(rxnV, Reaction(1.6451e12, .3239, 15.088227, sp.None)); // 5 - r4f - AR's work
        addReaction(rxnV, Reaction(9.9866e13, 0, 22.57, sp.None)); //6
        addReaction(rxnV, Reaction(5.75e10, .93, 30.4, sp.None));//7 - for rate calculation
    }
    //addReaction(rxnV, Reaction(2.20e12, 0, 7.5, sp.O2));          //6
    // 0.12 atm
    rxnvector rxnV2 = m_rxnvector0p12;
    addReaction(rxnV2, Reaction(1.73e7, 1.7453, 8.732, sp.H));      //0 - r1f
    addReaction(rxnV2, Reaction(3.40e9, .88, 7.870, sp.H2));   //1 - r1b
    addReaction(rxnV2, Reaction(2.1e13, 0, 4.56937799, sp.OH));  //2 - r2f
    addReaction(rxnV2, Reaction(3.68e8, 1.139, 17.10, sp.H2O)); //3 - r2b
    addReaction(rxnV2, Reaction(2.18e35, -6.51, 11.53110048, sp.H)); //4 - r3f
    if(violi)
        addReaction(rxnV2, Reaction(7.72e11, .21, 17.4, sp.None)); //5 - r4f - violi(2005) mechanism, include reverse.
    else {
        addReaction(rxnV2, Reaction(3.2902e12, .3239, 15.088227, sp.None)); // 5 - r4f - AR's work
        addReaction(rxnV2, Reaction(9.9866e13, 0, 22.560985, sp.None)); //6
        addReaction(rxnV2, Reaction(5.75e10, .93, 30.4, sp.None));//7 - for rate calculation
    }
    //addReaction(rxnV2, Reaction(9.7e3, 2.42, 38.51674641, sp.O2));          //6
    // 1 atm
    rxnvector rxnV3 = m_rxnvector1;
    addReaction(rxnV3, Reaction(7.25e7, 1.76, 9.69, sp.H));      //0 - r1f
    addReaction(rxnV3, Reaction(3.40e9, .88, 7.870, sp.H2));   //1 - r1b
    addReaction(rxnV3, Reaction(2.1e13, 0, 4.56937799, sp.OH));  //2 - r2f
    addReaction(rxnV3, Reaction(3.68e8, 1.139, 17.10, sp.H2O)); //3 - r2b
    addReaction(rxnV3, Reaction(3.49e39, -7.77, 13.37320574, sp.H)); //4 - r3f
    if(violi)
        addReaction(rxnV3, Reaction(3.86e11, .21, 17.4, sp.None)); //5 - r4f - violi(2005) mechanism, include reverse.
    else {
        addReaction(rxnV3, Reaction(1.6451e12, .3239, 15.088227, sp.None)); // 5 - r4f - AR's work
        addReaction(rxnV3, Reaction(9.9866e13, 0, 22.560985, sp.None)); //6
        addReaction(rxnV3, Reaction(5.75e10, .93, 30.4, sp.None));//7 - for rate calculation
    }
    //addReaction(rxnV3, Reaction(9.7e3, 2.42, 38.51674641, sp.O2));          //6

    m_sType = BY5; // sitetype
    m_name = "BY5 closure"; // name of process
    m_ID = 18;
}
// Jump rate calculation
real L5R_BY5::setRate0p0267(const KMCGasPoint& gp, PAHProcess& pah_st/*, const real& time_now*/) {
    // check if site count is zero
    real site_count = ((real)pah_st.getSiteCount(m_sType)); // Site count
    if(site_count==0) return m_rate=0;
    // calculate rate
    real r_denom = (m_r[1]+m_r[3]+m_r[4]+m_r[5]);
    real r_f; // radical fraction 
    if(r_denom>0) {
        if(violi)
            r_f = (m_r[0]+m_r[2])/r_denom; 
        else
            r_f = (m_r[0]+m_r[2])/(r_denom+((m_r[6]/m_r[7])*(m_r[1]+m_r[4]))); 
        r_f = r_f/(r_f+1.0);
    }
    else r_f=0;
    return m_rate = 2*m_r[5]*r_f* site_count; // Rate Equation
}
real L5R_BY5::setRate0p12(const KMCGasPoint& gp, PAHProcess& pah_st/*, const real& time_now*/) {
    return setRate0p0267(gp, pah_st);
}
real L5R_BY5::setRate1(const KMCGasPoint& gp, PAHProcess& pah_st/*, const real& time_now*/) {
    return setRate0p0267(gp, pah_st);
}
// ************************************************************
// ID19- R6 desorption at bay -> pyrene (AR21 in Matlab)
// ************************************************************

// Elementary rate constants, site type, process type and name
void M6R_BY5_FE3::initialise() {
    // Adding elementary reactions
    // 0.0267 atm
    rxnvector& rxnV = m_rxnvector0p0267;
    addReaction(rxnV, Reaction(7.81e7, 1.772, 10.333, sp.H));      //0 - r1f
    addReaction(rxnV, Reaction(1.85e6, 1.86, 11.04, sp.H2));   //1 - r1b
    addReaction(rxnV, Reaction(2.1e13, 0, 4.56937799, sp.OH));  //2 - r2f
    addReaction(rxnV, Reaction(3.68e8, 1.139, 17.10, sp.H2O)); //3 - r2b
    addReaction(rxnV, Reaction(3.49e39, -7.77, 13.37320574, sp.H)); //4 - r3f
    addReaction(rxnV, Reaction(2.3e9, 1.6031, 61.85, sp.None)); //5 - r4f
    //addReaction(rxnV, Reaction(2.20e12, 0, 7.5, sp.O2));          //6 - r5f
    // 0.12 atm
    rxnvector& rxnV2 = m_rxnvector0p12;
    rxnV2 = rxnV;
    // 1 atm
    rxnvector& rxnV3 = m_rxnvector1;
    rxnV3 = rxnV;

    m_sType = BY5_FE3; // sitetype
    m_name = "R6 desorption at bay -> pyrene"; // name of process
    m_ID = 19;
}
// Jump rate calculation
real M6R_BY5_FE3::setRate0p0267(const KMCGasPoint& gp, PAHProcess& pah_st/*, const real& time_now*/) {
    // check if site count is zero
    real site_count = ((real)pah_st.getSiteCount(m_sType)); // Site count
    if(site_count==0) return m_rate=0;
    // calculate rate
    real r_denom = (m_r[1]+m_r[3]+m_r[4]+m_r[5]);
    real r_f; // radical fraction 
    if(r_denom>0) {
        r_f = (m_r[0]+m_r[2])/r_denom; 
        r_f = r_f/(r_f+1.0);
    }
    else r_f=0;
    return m_rate = m_r[5]*r_f* site_count; // Rate Equation
}
real M6R_BY5_FE3::setRate0p12(const KMCGasPoint& gp, PAHProcess& pah_st/*, const real& time_now*/) {
    return setRate0p0267(gp, pah_st);
}
real M6R_BY5_FE3::setRate1(const KMCGasPoint& gp, PAHProcess& pah_st/*, const real& time_now*/) {
    return setRate0p0267(gp, pah_st);
}