/*
  Author(s):      Vinod Janardhanan (vj231)
  Project:        flameLab (premix solver)
  Sourceforge:    http://sourceforge.net/projects/mopssuite
  
  Copyright (C) 2008 Vinod M Janardhanan.

  File purpose:
	This class manages the reading of input file flame.xml
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
#include "fl_io.h"
#include "string_functions.h"
#include "fl_error_handler.h"
#include "fl_initial.h"
#include "gpc.h"
#include<cstring>
#include<iostream>
using namespace FlameLab;
using namespace Strings;
// main routine for reading the input file
void FlameLabIO::readInput(const std::string &fileName, FlameLab::Reactor &reac, 
						   FlameLab::SolverControl &solver){

	CamXML::Document doc;
	const CamXML::Element *root;

	if(doc.Load(fileName) == 0){
		root = doc.Root();
		readGeometry(reac, *root);
		readOPConditions(reac, *root);
		readInlet(reac, *root);
		readSolverControl(solver, *root);
		readInitialGuess(reac,*root);
		readMonitor(*root);

	}
}
// read geometry related properties
void FlameLabIO::readGeometry(FlameLab::Reactor &reac, const CamXML::Element &node){

	CamXML::Element *subnode, *reactorNode;
	const CamXML::Attribute *attr;
	std::string attrValue;
	reactorNode = node.GetFirstChild("reactor");
	if(reactorNode != NULL){
		attr = reactorNode->GetAttribute("model");
		if(attr != NULL){
			attrValue = attr->GetValue();
			// get the reactor model and set the reactor model
			if(!attrValue.compare("premix")) reac.setReactorModel(Reactor::PremixFlame);
			if(!attrValue.compare("cdflame")) reac.setReactorModel(Reactor::CDflmae);
			if(!attrValue.compare("plug")) reac.setReactorModel(Reactor::Plug);

			// get the reactor length and set the reactor length
			subnode = reactorNode->GetFirstChild("length");
			if( subnode != NULL) {
				attr = subnode->GetAttribute("unit");
				attrValue = attr->GetValue();
				if(!attrValue.compare("cm"))reac.setLength(cdble(subnode->Data())/100.0);
				if(!attrValue.compare("m")) reac.setLength(cdble(subnode->Data()));
				if(!attrValue.compare("in"))reac.setLength(cdble(subnode->Data())*0.0254);
				if(! (attrValue.compare("cm") ||attrValue.compare("m") || attrValue.compare("in")) ){

					throw ErrorHandler("Supported units for length are cm, m, and in\n", 101);
				}
			}else{
				
				throw ErrorHandler("Missing length parameter\n", 102);
			}
			// get the max number of computational cells and set the number of cells
			subnode = reactorNode->GetFirstChild("ncells");
			if( subnode != NULL){
				reac.setnCells((unsigned int)cdble(subnode->Data()));
			}else{
				reac.setnCells(50);
			}

			subnode = reactorNode->GetFirstChild("aspect_ratio");
			if(subnode != NULL){
				reac.setAspectRatio(cdble(subnode->Data()));
			}else{
				reac.setAspectRatio(1.0);
			}
			subnode = reactorNode->GetFirstChild("strain_rate");
			if(subnode != NULL){
				reac.setStrainRate(cdble(subnode->Data()));
			}else{
				reac.setStrainRate(0.0);
			}

		
						
		}else{
			throw ErrorHandler("Undefined reactor model\nUse either premix or cdflame\n",100);
			
		}

	}


	
}

// read operating conditions
void  FlameLabIO::readOPConditions(FlameLab::Reactor &reac, const CamXML::Element &node){

	const CamXML::Attribute *attr, *attr1;
	CamXML::Element *subnode, *opNode;
	std::string attrValue;

	opNode = node.GetFirstChild("op_condition");
	if( opNode != NULL){
		subnode = opNode->GetFirstChild("temperature");
		if( subnode != NULL){
			attr = subnode->GetAttribute("condition");
			if( attr != NULL){
				attrValue = attr->GetValue();
				if(!attrValue.compare("isothermal")) {					
					reac.setEnergModel(reac.Isothermal);
					reac.setTemperature(cdble(subnode->Data()));
				}else if(!attrValue.compare("adiabatic")){
					reac.setEnergModel(reac.Adiabatic);
					reac.setTemperature(cdble(subnode->Data()));
				}else if(!convertToCaps(attrValue).compare("USERDEFINED")){					
					reac.setEnergModel(reac.UserDefined);
				}
				
			}
		}
		subnode = opNode->GetFirstChild("pressure");
		if(subnode != NULL){
			attr = subnode->GetAttribute("unit");
			attr1 = subnode->GetAttribute("mode");
			attrValue = attr->GetValue();
			if(!convertToCaps(attrValue).compare("PA")){
				reac.setPressure(cdble(subnode->Data()));
			}else if(!convertToCaps(attrValue).compare("ATM")){
				reac.setPressure(cdble(subnode->Data())*101325.0);
			}else{
				throw ErrorHandler("Unsupported pressure units\nSupported units are Pa and atm\n",107);
			}
		}
		// get the diffuson setting
		subnode = opNode->GetFirstChild("diffusion");
		if(subnode != NULL){
			string mode = subnode->Data();
			if(! convertToCaps(mode).compare("OFF") )
				reac.setDiffusion(reac.OFF);
			else
				reac.setDiffusion(reac.ON);
		}else{
			reac.setDiffusion(reac.OFF);
		}
					
			
	}else{
		throw ErrorHandler("Operating conditions are undefined \n",103);
	}

	
}
// read inlet conditions either fuel or oxidizer
void FlameLabIO::readInlet(FlameLab::Reactor &reac, const CamXML::Element &node){

	const CamXML::Element *subnode, *inletNode;
	std::string attrValue;

	inletNode = node.GetFirstChild("inlet");
	if(inletNode != NULL){
		subnode = inletNode->GetFirstChild("fuel");
		if(subnode != NULL){			
			readNozzleConditions(reac,reac.getFuelInletConditions(), *subnode);
		}
		subnode = inletNode->GetFirstChild("oxidizer");
		if(subnode != NULL){
			
			readNozzleConditions(reac,reac.getOxidizerInletConditions(), *subnode);
		}

	}else{
	
		throw ErrorHandler("Inlet conditions need to be defined \n",104);
	}

	
}
// read nozzle inlet conditions
void FlameLabIO::readNozzleConditions(FlameLab::Reactor &reac, 
									 FlameLab::InitialConditions &nozzle, 
									 const CamXML::Element &node)
{
	const CamXML::Attribute *attr;
	CamXML::Element *subnode;
	vector<CamXML::Element*> subnodes;
	vector<CamXML::Element*>::const_iterator p;
	std::string attrValue;

	// manage axial velocity settings
	subnode = node.GetFirstChild("velocity");
	if(subnode != NULL){
		attr = subnode->GetAttribute("unit");		
		attrValue = attr->GetValue();
		if(!attrValue.compare("m/s")){
			real vel = cdble(subnode->Data());
			nozzle.setVelocity(cdble(subnode->Data()));			
		}
		else if(!attrValue.compare("cm/s"))
			nozzle.setVelocity(cdble(subnode->Data())*0.01);
		else{
			throw ErrorHandler("supported units are m/s and cm/s\n",105);
		}
	}else{
		nozzle.setVelocity(0.0);
	}
	// manage flow rate settings
	subnode = node.GetFirstChild("flowrate");
	if(subnode != NULL){
		attr = subnode->GetAttribute("unit");
		attrValue = attr->GetValue();
		if(! convertToCaps(attrValue).compare("CGS")){
			real flow = cdble(subnode->Data())*10;
			nozzle.setFlowRate(flow);
		}else if(! convertToCaps(attrValue).compare("SI")){
			nozzle.setFlowRate(cdble(subnode->Data()));
		}else{
			throw ErrorHandler("supported units are CGS and SI\n",106);
		}

	}else{
		nozzle.setFlowRate(0.0);
	}
	//manege non-axial component of velocity setting
	subnode = node.GetFirstChild("velocity_gradient");
	if(subnode != NULL){
		attr = subnode->GetAttribute("unit");		
		attrValue = attr->GetValue();
		if(!attrValue.compare("1/s")){
			real vel = cdble(subnode->Data());
			nozzle.setRadialVelocityGrad(cdble(subnode->Data()));			
		}
		else{
			throw ErrorHandler("supported units is 1/s \n",105);
		}
	}else{
		nozzle.setRadialVelocityGrad(0.0);
	}

	// manage temperature settings
	subnode = node.GetFirstChild("temperature");
	if(subnode != NULL){
		attr = subnode->GetAttribute("unit");
		attrValue = attr->GetValue();
		if(!attrValue.compare("K"))
			nozzle.setTemperature(cdble(subnode->Data()));			
		else if(!attrValue.compare("C"))
			nozzle.setTemperature(cdble(subnode->Data())+273.15);
		else{
			throw ErrorHandler("Supported units for inlet temperatures \n are K and C \n", 106);
		}
	}
	subnode = node.GetFirstChild("massfrac");
	if(subnode != NULL){
		std::string nodeValue, finalSpecies;
		real sumFrac=0.0;
		nozzle.setMassOrMole(InitialConditions::Massfraction);
		subnode->GetChildren("species",subnodes);
		for(p=subnodes.begin(); p!= subnodes.end(); ++p){
			attr = (*p)->GetAttribute("name");
			if(attr != NULL){
				attrValue = attr->GetValue();
				nodeValue = (*p)->Data();
				if(!nodeValue.compare("*"))
					finalSpecies = attrValue;
				else{
					sumFrac += cdble(nodeValue);
					nozzle.setFraction(attrValue,cdble(nodeValue));
				}					
				
			}

		}

		nozzle.setFraction(finalSpecies,1-sumFrac);

	}else{
		subnode = node.GetFirstChild("molefrac");
		if(subnode != NULL){
			std::string nodeValue, finalSpecies;
			real sumFrac = 0.0;
			nozzle.setMassOrMole(InitialConditions::Molefraction);
			subnode->GetChildren("species",subnodes);
			for(p=subnodes.begin(); p!= subnodes.end(); ++p){
				attr = (*p)->GetAttribute("name");
				if(attr != NULL){
					attrValue = attr->GetValue();
					nodeValue = (*p)->Data();
					if(!nodeValue.compare("*"))
						finalSpecies = attrValue;
					else{
						sumFrac += cdble(nodeValue);
						nozzle.setFraction(attrValue,cdble(nodeValue));
					}					
				}
				
			}

			nozzle.setFraction(finalSpecies,1-sumFrac);
		}
	}
						
}

// read solver control variables
void FlameLabIO::readSolverControl(FlameLab::SolverControl &solver, const CamXML::Element &node){

	CamXML::Element *subnode, *solverNode;	
	vector<CamXML::Element*> subnodes;
	vector<CamXML::Element*>::const_iterator p;
	const CamXML::Attribute *attr, *attr2;
	string attrValue;
	solverNode = node.GetFirstChild("solver");
	if(solverNode != NULL){
		attr = solverNode->GetAttribute("mode");

		if(attr != NULL){
			attrValue = attr->GetValue();
			// get the reactor model and set the reactor model
			if(!attrValue.compare("steady")) solver.setSolMode(SolverControl::steadyState);
			if(!attrValue.compare("transient"))solver.setSolMode(SolverControl::transient);
		}


		subnode = solverNode->GetFirstChild("aTol");
		if(subnode != NULL) 
			solver.setATol(cdble(subnode->Data()));
		else
			solver.setATol(1.e-06);
					
		subnode = solverNode->GetFirstChild("rTol");
		if(subnode != NULL)
			solver.setRTol(cdble(subnode->Data()));
		else
			solver.setRTol(1.e-06);

		subnode = solverNode->GetFirstChild("iniStep");
		if(subnode != NULL)
			solver.setIniStep(cdble(subnode->Data()));
		else
			solver.setIniStep(1.e-04);		

		subnode = solverNode->GetFirstChild("maxStep");
		if(subnode != NULL)
			solver.setMaxStep(cdble(subnode->Data()));
		else
			solver.setMaxStep(0.0);
		subnode = solverNode->GetFirstChild("minStep");
		if(subnode != NULL)
			solver.setMinStep(cdble(subnode->Data()));
		else
			solver.setMinStep(0.0);

		subnode = solverNode->GetFirstChild("tMax");
		if(subnode != NULL)
			solver.setMaxTime(cdble(subnode->Data()));

		subnode = solverNode->GetFirstChild("output");
		if(subnode != NULL) subnode->GetChildren("interval",subnodes);


		string from, to;
		std::map<real,real> interval;
		for(p=subnodes.begin(); p < subnodes.end(); p++){

			attr = (*p)->GetAttribute("from");
			attr2 = (*p)->GetAttribute("to");

			if(attr != NULL){
				string nodeValue;
				from = attr->GetValue();
				to = attr2->GetValue();
				if(! to.compare("*")){
					nodeValue = (*p)->Data();
					interval.insert(pair<real,real>(cdble(from),cdble("0")));
					solver.setOutInterval(interval,cdble(nodeValue));
				}else{
					nodeValue = (*p)->Data();
					interval.insert(pair<real,real>(cdble(from),cdble(to)));
					solver.setOutInterval(interval,cdble(nodeValue));
				}
			}

		}

	}
	
}

void FlameLabIO::readInitialGuess(Reactor &reac, const CamXML::Element &node){
	CamXML::Element *subnode, *tempNode, *speciesNode;
	vector<CamXML::Element*> subnodes;
	vector<CamXML::Element*>::const_iterator p;
	const CamXML::Attribute *attr, *attr2;
	string attrValue, nodeValue, finalSpecies;
	
	subnode = node.GetFirstChild("initialize");
	
	if(subnode != NULL){
		tempNode = subnode->GetFirstChild("Tprofile");
		speciesNode = subnode->GetFirstChild("massfrac");

		if(speciesNode != NULL){
			reac.setInitialGuessCondition(reac.MASSFRACTION);
			speciesNode->GetChildren("species",subnodes);
			real sumFrac = 0.0;
			for(p=subnodes.begin(); p!= subnodes.end(); ++p){
				attr = (*p)->GetAttribute("name");
				if(attr != NULL){
					attrValue = attr->GetValue();
					nodeValue = (*p)->Data();
					if(!nodeValue.compare("*"))
						finalSpecies = attrValue;
					else{
						sumFrac += cdble(nodeValue);
						reac.setFraction(attrValue,cdble(nodeValue));
					}
				}
			}
			reac.setFraction(finalSpecies,1-sumFrac);
		}else{
			speciesNode = subnode->GetFirstChild("molefrac");
			if(speciesNode != NULL){
				reac.setInitialGuessCondition(reac.MOLEFRACTION);
				speciesNode->GetChildren("species",subnodes);
				real sumFrac = 0.0;
				for(p=subnodes.begin(); p!= subnodes.end(); ++p){
					attr = (*p)->GetAttribute("name");
					if(attr != NULL){
						attrValue = attr->GetValue();
						nodeValue = (*p)->Data();
						if(!nodeValue.compare("*"))
							finalSpecies = attrValue;
						else{
							sumFrac += cdble(nodeValue);
							reac.setFraction(attrValue,cdble(nodeValue));
						}
					}
				}
				reac.setFraction(finalSpecies,1-sumFrac);
			}
		}

	}
	real converterL, converterT;	
	if(tempNode != NULL){
		attr = tempNode->GetAttribute("unit_L");
		attr2 = tempNode->GetAttribute("unit_T");
		if(attr != NULL){
			attrValue = attr->GetValue();
			if(!convertToCaps(attrValue).compare("CM")) converterL = 0.01;
			if(!convertToCaps(attrValue).compare("M")) converterL = 1.0;
			if(!convertToCaps(attrValue).compare("IN")) converterL = 0.0254;
		}
		if(attr2 != NULL){
			attrValue = attr2->GetValue();
			if(!convertToCaps(attrValue).compare("C")) converterT=273.15;
			if(!convertToCaps(attrValue).compare("K")) converterT=0.0;
		}

		tempNode->GetChildren("position",subnodes);
		real position, temp;
		for(p=subnodes.begin(); p!= subnodes.end(); p++){
			position = cdble((*p)->GetAttributeValue("x"))*converterL;
			temp = cdble((*p)->Data())+converterT;
			reac.setUserTemperature(position,temp);
		}

		reac.naturalCubicSplineFit();
		
	}	
	
		
}

			

void FlameLabIO::readMonitor(const CamXML::Element &node){

	CamXML::Element *subnode;
	vector<CamXML::Element*> subnodes;
	vector<CamXML::Element*>::iterator p;
	const CamXML::Attribute *attr;
	string attrValue;

	subnode = node.GetFirstChild("monitor");
	attr = subnode->GetAttribute("mode");
	if( attr != NULL){
		attrValue = attr->GetValue();
		if( ! convertToCaps(attrValue).compare("ON"))
			setMonitorSwtich(ON);
		else if (! convertToCaps(attrValue).compare("OFF"))
			setMonitorSwtich(OFF);
		else
			throw ErrorHandler("Unknown mode for monitor optionn\nUse ON or OFF\n",301);
	}else{
		throw ErrorHandler("Monitor mode need to be specified\n",302);
	}
	
	attr = subnode->GetAttribute("species");
	if( attr != NULL){
		attrValue = attr->GetValue();
		if( ! convertToCaps(attrValue).compare("MOLE"))
			setSpeciesOut(MOLE);
		else
			setSpeciesOut(MASS);
	}else{
		setSpeciesOut(MOLE);
	}

	if(subnode != NULL){		
		subnode->GetChildren("species",subnodes);
		for(p=subnodes.begin(); p!= subnodes.end(); ++p){
			attr = (*p)->GetAttribute("name");
			if(attr != NULL){
				attrValue = attr->GetValue();
				monitor.push_back(convertToCaps(trim(attrValue)));
			}
		}
	}
}

void FlameLabIO::setMonitorSwtich(int n){
	monitorSwitch = n;
}

const int& FlameLabIO::getMonitorSwitch() const{
	return monitorSwitch;
}

void FlameLabIO::setSpeciesOut(int n){
	speciesOut = n;
}

int FlameLabIO::getSpeciesOut() const{
	return speciesOut;
}
//-------------------------------------- Output routines -----------------------//
//********************************************************************************//
void FlameLabIO::prepareConsole(Sprog::Mechanism &mech, FlameLab::Premix &flame){


	vector<string> header;
	consoleMask.clear();


	header.push_back("AxlPsn(m)");
	header.push_back("Temp (K)");
	header.push_back("rho(Kg/m3)");

	consoleMask.push_back(flame.getNeq());
	consoleMask.push_back(flame.TEMP-1);
	consoleMask.push_back(flame.DENS-1);
	for(unsigned int i=0; i!= monitor.size(); i++)	{
		int index = mech.FindSpecies(monitor[i]);
		if(index < 0){
			throw ErrorHandler("Monitor species "+monitor[i] +" not found in species list\n",200);
		}else{
			consoleMask.push_back(index);
			header.push_back(monitor[i]);
		}
	}
	//int nColumns = 6 > (monitor.size()+2) ? 6: (monitor.size()+2);
	flameLabConsole.PrintDivider();
	flameLabConsole.PrintRow(header);
	flameLabConsole.PrintDivider();	
			


}



void FlameLabIO::writeToConsole(Reactor &reac) const{
	static vector<real> outData;	
	reac.getMixture().GetMassFractions(outData);
	outData.push_back(reac.getMixture().Temperature());
	outData.push_back(reac.getMixture().MassDensity());

	vector<real> dz = reac.getGeometry();
	int cellId = reac.getAxialPosition();
	real sum = 0.0;
	if(cellId == 0){
		sum = 0.5*dz[cellId];
	}else{
		for(int i=0; i<cellId; ++i){
			sum += dz[i];
		}
		sum += 0.5*dz[cellId];
			
	}
	outData.push_back(sum);
	flameLabConsole.PrintRow(outData,consoleMask);
	

	
}
//prepare header for file output
void FlameLabIO::prepareFileOutput(FlameLab::Reactor &reac) {
	const Sprog::Mechanism *mech = reac.getMechanism();
	Sprog::SpeciesPtrVector spv = mech->Species();
	int count = mech->SpeciesCount();
	string name;
	string xy;
	fileHeader.clear();
	if(reac.getReactorRunModel() == reac.WDT || reac.getReactorRunModel() == reac.NDT)
		fileHeader.push_back("time(s)");

	fileHeader.push_back("x(m)"); //axial position
	fileHeader.push_back("rho(Kg/m3)");//density
	fileHeader.push_back("T(K)"); //temperature
	fileHeader.push_back("u(m/s)");//velocity

	if(getSpeciesOut() == MASS) 
		xy = "y";   //species names are appeneded with "Y"
	else
		xy = "x"; // species names are appended with "X"

	for(int l=0; l<count; l++){
		name = xy + spv[l]->Name();
		fileHeader.push_back(name);
	}

}


//prepare to write file
void FlameLabIO::writeToFile(const FlameLab::real &time, std::vector<SingleCell> &sc, FlameLab::Reactor &reac) {

	static int fileNr;
	fileNr += 1;
	string ext=".dat";
	string name = "profile_";
	std::stringstream intToString;
	intToString << fileNr;
	string stringFileNr = intToString.str();
	string fileName = name+stringFileNr+ext;

	flameReport.Open(fileName,true);
	prepareFileOutput(reac);
	flameReport.Write(fileHeader);
	
	vector<FlameLab::real> data,fraction;
	data.push_back(time);
	int nCell = reac.getnCells();
	InitialConditions ic = reac.getFuelInletConditions();

	if(getSpeciesOut() == MASS ) {
		ic.getFuelMixture().GetMassFractions(fraction); //get the mass fractions
	}else{
		fraction = ic.getFuelMixture().MoleFractions();
	}

	int nSpecies = reac.getMechanism()->SpeciesCount();
	//**** DO NOT CHANGE THIS ORDER ***************//
	data.clear();
	data.push_back(time); //time
	data.push_back(0.0); //position
	data.push_back(ic.getDensity()); //density
	data.push_back(ic.getTemperature()); //temperature
	data.push_back(ic.getVelocity()); //velocity
	for(int l=0; l<nSpecies; l++) 
		data.push_back(fraction[l]);

	//interior cells
	vector<real> dz= reac.getGeometry();
	real axPos = 0.5*dz[0];
	flameReport.Write(data);
	for(int i=0; i<nCell; i++){
		data.clear();		
		if(i>0) axPos+= dz[i];

		data.push_back(time);
		data.push_back(axPos);
		data.push_back(sc[i].getMixture().MassDensity());
		data.push_back(sc[i].getMixture().Temperature());
		data.push_back(sc[i].getVelocity());
		if(getSpeciesOut() == MASS)
			sc[i].getMixture().GetMassFractions(fraction);
		else
			fraction = sc[i].getMixture().MoleFractions();

		for(int l = 0; l<nSpecies; l++)
			data.push_back(fraction[l]);

		flameReport.Write(data);
	}
	//exit boundary
	data.clear();
	axPos += 0.5*dz[nCell-1];
	data.push_back(time);
	data.push_back(axPos);
	data.push_back(sc[nCell-1].getMixture().MassDensity());
	data.push_back(sc[nCell-1].getMixture().Temperature());
	data.push_back(sc[nCell-1].getVelocity());
	for(int l = 0;l<nSpecies; l++)
		data.push_back(fraction[l]);

	flameReport.Write(data);
	flameReport.Close();
}


