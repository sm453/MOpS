#!/usr/bin/perl

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

use strict;
use warnings;

# Clean up any outputs from previous simulations
system("rm *stats.csv");

print "Test 1b: Nucleation and coagulation without transport";

# See if this is a windows system
my $windows = ($ENV{'OS'} =~ /windows.*/i);

# Choose the windows executable name if appropriate
my $program = "../bin/brush_d.x";
if($windows) {
    $program = "../bin/brush_d.exe";
}

# Arguments for simulation
my @simulationCommand = ($program,
                         "-g", "regress1/geom.xml",
                         "-c", "regress1/species.inp",
                         "-d", "regress1/profile.dat",
                         "-s", "regress1/sweep1b.xml",
                         "-t", "regress1/therm.dat",
                         "-a", "regress1/partsoln.xml",
                         "-b", "regress1/brush1b.xml");

# Run the simulation and wait for it to finish
system(@simulationCommand) == 0 or die "ERR: simulation failed: $!";

# Collect all the moment data together
system("../bin/merge-partstats.sh regress1b-NucCoag") == 0 or die "ERR: failed to merge moment files: $!";

# Parse the moments file
my $momentFile;
open($momentFile, "<regress1b-NucCoagMerged_partstats.csv") or die "ERR: failed to open merged moment file: $!";

my $m0 = 0;
my $m1 = 0;
my $count = 0;

while(<$momentFile>) {
  my @fields = split /,/;

  if(($fields[0] =~ /^\d+/) && (abs($fields[0] - 0.02) < 1e-4 )) {
      $m0 += $fields[3];
      #print "$fields[3], ";
      $m1 += $fields[11];
      #print "$fields[11] \n";
      ++$count;
  }
}

# Get mean values
$m0 /= $count;
$m1 /= $count;

print "$m0, $m1\n";

# Count number of failures
my $failures = 0;

# 20 simulations (3 separate cells in each simulation) giving a total
# of 60 samples give a sample mean and 99% confidence interval for the
# population mean of
#m0: (4.469 +- 0.132) e10 cm^-3
#m1: (3.818 +- 0.134) e-11 g cm^-3
# svn r821

if(abs($m0 - 4.465e10) > 8e8) {
  print "Simulated mean M0 was $m0, when 4.465e10 cm^-3 expected\n";
  print "**************************\n";
  print "****** TEST FAILURE ******\n";
  print "**************************\n";
  ++$failures;
}

if(abs($m1 - 3.823e-11) > 4e-13) {
  print "Simulated mean M1 was $m1, when 3.823e-11 g cm^-3 expected\n";
  print "**************************\n";
  print "****** TEST FAILURE ******\n";
  print "**************************\n";
  ++$failures;
}

#print "All tests passed\n";
exit $failures;
