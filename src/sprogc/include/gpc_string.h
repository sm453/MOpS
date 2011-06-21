/*
  Author(s):      Weerapong Phadungsukanan (wp214)
  Project:        sprog (gas-phase chemical kinetics).
  Sourceforge:    http://sourceforge.net/projects/mopssuite

  Copyright (C) 2008 Weerapong Phadungsukanan.

  File purpose:
    This is the main include file for other projects which use
    mops.  All components of sprog are included here.

  Licence:
    This file is part of "sprog".

    sprog is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
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

#ifndef GPC_STRING_FUNC
#define GPC_STRING_FUNC

#include "comostrings.h"
#include <iostream>
#include <stdlib.h>
#include <string>
#include <stdexcept>

namespace Sprog {
    namespace IO {
        namespace StringFunc {
            // KEYWORDs must be provided in capital letters and longer than 4 letters
            // because a sub string of size 4 can also be used as keyword
            const std::string EL_KEYWORD = "ELEMENTS";
            const std::string RT_KEYWORD = "REACTIONS";
            const std::string TM_KEYWORD = "THERMO";
            const std::string SP_KEYWORD = "SPECIES";
            const std::string END_KEYWORD = "END";
            typedef struct {
                size_t begin;
                size_t end;
            } ck_pos;
            // get positions of keywords
            ck_pos getKeyPos(const std::string &key, std::string &ckstr);
            // extract only element string from chemkin string
            std::string extract_CK_elements_str(std::string &ckstr);
            // extract only species string from chemkin string
            std::string extract_CK_species_str(std::string &ckstr);
            // extract only reactions string from chemkin string
            std::string extract_CK_reactions_str(std::string &ckstr);
            // extract only thermo string from chemkin string
            std::string extract_CK_thermo_str(std::string &ckstr);
            // convert a CHEMKIN file istream into a string with all comments removed.
            // Tab and return character are replaced by space and new line character
            std::string CK_is2str(std::istream &isin);
            // remove specific keyword from part of chemkin definition
            void remove_CK_keyword(std::string &ckstr, const std::string &key);
        }
    }
}

#endif

