/*
 * species.cpp
 *
 *  Created on: Jun 22, 2011
 *      Author: lrm29
 */

#include "species.h"
#include "stringFunctions.h"

using namespace std;
using namespace boost;

IO::Species::Species
(
    const string name
)
:
    name_(name),
    siteOccupancy_(0), // Added by mm864  
    molecularWeight_(-1),
    phaseName_(), 
    transport_(),
    thermo_(name),
    speciesComposition_()
{}

IO::Species::~Species()
{
    //if (transport_ != NULL) delete transport_;
}

void IO::Species::setSiteOccupancy(const int OccupancyNo) 
{
siteOccupancy_ = OccupancyNo;
}

void IO::Species::setPhaseName(const std::string &phName) 
{
phaseName_ = phName;
}

const int& IO::Species::getSiteOccupancy() const
{
  return this-> siteOccupancy_;
}


/* NO NEED 
std::string IO::Species::phase()
{
  return this-> phaseName_;
}
const std::string& IO::Species::phase() const
{
  return this-> phaseName_;
}
*/

IO::Transport& IO::Species::transport()
{
    return transport_;
}

const IO::Transport& IO::Species::transport() const
{
    return transport_;
}

IO::Thermo& IO::Species::thermo()
{
    return thermo_;
}

const IO::Thermo& IO::Species::thermo() const
{
    return thermo_;
}



/*void IO::Species::checkElementsInSpecies(const vector<Element>& elements)
{

    for (size_t elementIndex=0; elementIndex<elements.size(); ++elementIndex)
    {

        string elementName = elements[elementIndex].getName();

        regex elementRegex
        (
            "("
            +elementName
            +")([0-9]+|)"
        );
        smatch what;
        std::string::const_iterator start, end;
        start = name_.begin();
        end = name_.end();
        sregex_iterator i(start, end, elementRegex);
        sregex_iterator j;
        while (i != j)
        {
            string element = (*i)[0].first;
            int sub = 1;
            if (what[2] != "") sub=(*i)[0].second;
            cout << element << " " << sub << " ";
            if (speciesComposition_.count(elementName))
            {
                speciesComposition_.find(elementName)->second += sub;
            }
            else
            {
                speciesComposition_.insert
                (
                    pair<string,double>
                    (
                        element,
                        sub
                    )
                );
            }
        }
        std::cout << std::endl;
    }

}*/

namespace IO
{
	ostream& operator<<(std::ostream& output, const Species& species) 
	
    {
        map<string,double>::const_iterator iter;

        output << "(\n"
               << "    Species: \n"
               << "    (\n"
               << "        Name      : \"" << species.name_ << "\"\n";
        for (iter = species.speciesComposition_.begin(); iter != species.speciesComposition_.end(); ++iter)
                {
        output << "        Element : " << iter->first << " | Number : " << iter->second << "\n";
        }
        output << "        Mol. Mass : " << species.molecularWeight_ << "\n"
			   << "    )\n"
			   << "        Site. Occp. : " << species.siteOccupancy_ << "\n"
			   << "    )\n"
			   << "		   Phase: " << species.phaseName_ << "\n"
               << species.transport_ << "\n"
               << species.thermo_ << "\n"
               << ")";
        return output;
    }

}
