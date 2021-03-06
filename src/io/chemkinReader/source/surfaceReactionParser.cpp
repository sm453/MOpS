/*
 * reactionParser.cpp
 *
 *  Created on: Jun 08, 2012
 *      Author: M. Martin 
 *     License: Apache 2.0
 */

#include <boost/algorithm/string.hpp>
#include "boost/algorithm/string/trim.hpp"

#include "surfaceReactionParser.h"
#include "stringFunctions.h"
#include "reaction.h"

using namespace std;
using namespace boost;

const regex IO::SurfaceReactionParser::reactionSingleRegex // This is just for the reaction line (first line)
(
    "(.*?)\\s*"
    "(<=>|=>|=)\\s*"
    "(.*?)"
    "\\s+((?:[0-9]+|\\.)\\.*[0-9]*(?:[eEgG][-+]?[0-9]*)*)"
    "\\s+(.*?)"
    "\\s+(.*?)$|\\n"
);

const regex IO::SurfaceReactionParser::blankLine
(
    "\\s*\\n*$" 
);

const regex IO::SurfaceReactionParser::DUPLICATE // Checked by mm864!
(
    "DUPLICATE|DUP" 
);

const regex IO::SurfaceReactionParser::STICK // Checked by mm864!
(
    "(STICK)\\s*\\"
);

const regex IO::SurfaceReactionParser::COV // Checked by mm864!
(
    "(COV)\\s*\\/\\s*(.*?)\\s+(.*?)\\s+(.*?)\\s+(.*?)\\s*\\/"
);

const regex IO::SurfaceReactionParser::FORD // Checked by mm864!
(
    "(FORD)\\s*\\/\\s*(.*?)\\s+(.*?)\\s*\\/"
);

const regex IO::SurfaceReactionParser::REV // Checked by mm864!
(
    "(REV)\\s*\\/\\s*(.*?)\\s+(.*?)\\s+(.*?)\\s*\\/"
);

const regex IO::SurfaceReactionParser::DCOL // Checked by mm864 
(

    "(DCOL)\\s*\\/\\s*(.*?)\\s*\\/"
 );


const regex IO::SurfaceReactionParser::LANG  // Checked by mm864!
(
    "(LANG)\\s*\\/\\s*(.*?)\\s+(.*?)\\s+(.*?)\\s+(.*?)\\s+(.*?)\\s*\\/"
);

const regex IO::SurfaceReactionParser::LHDE  // Checked by mm864!
(
    "(LHDE)\\s*\\/\\s*(.*?)\\s*\\/"
);

// Empty default constructor, can be removed but leave it there just in case.
IO::SurfaceReactionParser::SurfaceReactionParser
(
    const string reactionString
)
:
    reactionString_(reactionString)
{
    split(reactionStringLines_, reactionString, boost::is_any_of("\n|\r\n"));

    // Check for a blank line and erase.
    for (size_t i=0; i<reactionStringLines_.size(); ++i)
    {
        if(isBlankLine(reactionStringLines_[i]))
        {
            reactionStringLines_.erase(reactionStringLines_.begin()+i);
            --i;
        }
    }
}

void IO::SurfaceReactionParser::parse(vector<IO::Reaction>& reactions)
{

    for (size_t i=0; i<reactionStringLines_.size(); ++i)
    {

        cout << reactionStringLines_[i] << endl;

        Reaction surfReaction;

        // Check for pressure dependency now as it screws up reactionSingleRegex.
        if (checkForPressureDependentReaction(reactionStringLines_[i]))
        {
            reactionStringLines_[i] = regex_replace(reactionStringLines_[i], pressureDependent, "");
            surfReaction.setPressureDependent();
        }

        smatch what;
        string::const_iterator start = reactionStringLines_[i].begin();
        string::const_iterator end = reactionStringLines_[i].end();

        if (regex_search(start, end, what, reactionSingleRegex))
        {
            surfReaction.setReactants(parseReactionSpecies(what[1]));

            if (what[2] == "=>") reaction.setReversible(false);

            surfReaction.setProducts(parseReactionSpecies(what[3]));

            surfReaction.setArrhenius
            (
                from_string<double>(what[4]),
                from_string<double>(what[5]),
                from_string<double>(what[6])
            );

            while (i < reactionStringLines_.size()-1)
            {

                cout << "Next = " << reactionStringLines_[i+1] << endl;

                start = reactionStringLines_[i+1].begin();
                end = reactionStringLines_[i+1].end();

                if (regex_search(start, end, reactionSingleRegex))
                {
                    break;
                }
                else if (regex_search(start, end, DUPLICATE))
                {
                    surfReaction.setDuplicate();
                    // Skip one line when looking for the next reaction.
                    ++i;
                    //break;
                }
                else if (regex_search(start, end, REV))
                {
                    vector<double> reverseArrhenius = parseLOWTROEREV(reactionStringLines_[i+1], REV);
                    surfReaction.setArrhenius(reverseArrhenius[0],reverseArrhenius[1],reverseArrhenius[2],true);
                    // Skip one line when looking for the next reaction.
                    ++i;
                    //break;
                }

		else if (regex_search(start, end, COV))
		  {

		  }

		else if (regex_search(start, end, LANG))
		  {

		  }

		else if (regex_search(start, end, STICK))
		  {

		  }
                else if (reaction.hasThirdBody() || reaction.isPressureDependent())
                {
                    string lineType = findLineType(reactionStringLines_[i+1]);
                    if (lineType == "THIRDBODY")
                    {
                        reaction.setThirdBodies(parseThirdBodySpecies(reactionStringLines_[i+1]));
                        ++i;
                    }
                    if (lineType == "LOW")
                    {
                        reaction.setLOW(parseLOWTROEREV(reactionStringLines_[i+1], LOW));
                        ++i;
                    }
                    if (lineType == "TROE")
                    {
                        reaction.setTROE(parseLOWTROEREV(reactionStringLines_[i+1], TROE));
                        ++i;
                    }
                    if (lineType == "SRI")
                    {
                        reaction.setSRI(parseLOWTROEREV(reactionStringLines_[i+1], SRI));
                        ++i;
                    }
                }
                else
                {
                    throw std::logic_error("Reaction "+reactionStringLines_[i+1]+" is not supported.");
                }

            }

            reactions.push_back(reaction);

        }

    }

}

multimap<string, double>
IO::SurfaceReactionParser::parseReactionSpecies(string reactionSpecies)
{

    std::multimap<std::string, double> reactionSpeciesMap;

    regex splitSpecies("\\+");
    regex splitStoichiometry("([0-9]*)([A-Z].*)");

    sregex_token_iterator i(reactionSpecies.begin(), reactionSpecies.end(), splitSpecies,-1);
    sregex_token_iterator j;

    while (i != j)
    {
        // *i Gives a reactant species. Now get its stoichiometry.
        smatch splitStoic;
        // ++ iterates to the next reactant!
        string reactionSpecie = *i++;

        string::const_iterator start = reactionSpecie.begin();
        string::const_iterator end = reactionSpecie.end();

        regex_search(start, end, splitStoic, splitStoichiometry);

        string speciesName = splitStoic[2];

        if (splitStoic[1] == "")
        {
            reactionSpeciesMap.insert
            (
                pair<string,double>
                (
                    trim_copy(speciesName),
                    1.0
                )
            );
        }
        else
        {
            reactionSpeciesMap.insert
            (
                pair<string,double>
                (
                    trim_copy(speciesName),
                    from_string<double>(splitStoic[1])
                )
            );
        }
    }

    return reactionSpeciesMap;

}

multimap<string, double>
IO::SurfaceReactionParser::parseThirdBodySpecies(const string& thirdBodies)
{

    string trim_copymed = trim_copy(thirdBodies);
    multimap<string, double> thirdBodyMap;
    // Split the next line using / as delimiter.
    regex splitThirdBodies("\\/");

    sregex_token_iterator j
    (
        trim_copymed.begin(),
        trim_copymed.end(),
        splitThirdBodies,
        -1
    );
    sregex_token_iterator k;

    while (j != k)
    {
        string name = *j++;
        string trim_copyName = trim_copy(name);
        double efficiencyFactor = from_string<double>(*j++);
        thirdBodyMap.insert(pair<string,double>(trim_copyName,efficiencyFactor));
    }

    return thirdBodyMap;

}

bool
IO::ReactionParser::isBlankLine(const string& line)
{

    string::const_iterator start = line.begin();
    string::const_iterator end = line.end();

    return regex_match(start, end, blankLine);

}

string
IO::SurfaceReactionParser::findLineType(const string& line)
{

    string::const_iterator start = line.begin();
    string::const_iterator end = line.end();

    if (regex_search(start, end, LOW))
        return "LOW";
    if (regex_search(start, end, TROE))
        return "TROE";
    if (regex_search(start, end, SRI))
        return "SRI";

    return "THIRDBODY";

}


