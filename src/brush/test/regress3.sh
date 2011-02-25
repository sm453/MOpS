#!/bin/bash

#  Copyright (C) 2009 Robert I A Patterson.
#
#
# Licence:
#    This file is part of "brush".
#
#    brush is free software; you can redistribute it and/or
#    modify it under the terms of the GNU General Public License
#    as published by the Free Software Foundation; either version 2
#    of the License, or (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#
#  Contact:
#    Prof Markus Kraft
#    Dept of Chemical Engineering
#    University of Cambridge
#    New Museums Site
#    Pembroke Street
#    Cambridge
#    CB2 3RA
#    UK
#
#    Email:       mk306@cam.ac.uk
#    Website:     http://como.cheng.cam.ac.uk

# Run the test
./regress3/regress3a.pl

if(($?!=0))
  then
    echo "Simulation script 3a failed"
    echo "**************************"
    echo "****** TEST FAILURE ******"
    echo "**************************"
    exit $?
fi

# Run the test
./regress3/regress3b.pl

if(($?!=0))
  then
    echo "Simulation 3b script failed"
    echo "**************************"
    echo "****** TEST FAILURE ******"
    echo "**************************"
    exit $?
fi

# All tests passed if we get to here
echo "All tests passed"
exit 0
