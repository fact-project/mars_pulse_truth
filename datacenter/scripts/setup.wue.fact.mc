#!/bin/sh
#
# ========================================================================
#
# *
# * This file is part of MARS, the MAGIC Analysis and Reconstruction
# * Software. It is distributed to you in the hope that it can be a useful
# * and timesaving tool in analysing Data of imaging Cerenkov telescopes.
# * It is distributed WITHOUT ANY WARRANTY.
# *
# * Permission to use, copy, modify and distribute this software and its
# * documentation for any purpose is hereby granted without fee,
# * provided that the above copyright notice appear in all copies and
# * that both that copyright notice and this permission notice appear
# * in supporting documentation. It is provided "as is" without express
# * or implied warranty.
# *
#
#
#   Author(s): Daniela Dorner  06/2010 <mailto:daniela.dorner@unige.ch>
#
#   Copyright: MAGIC Software Development, 2000-2010
#
#
# ========================================================================
#
# This a resource file for the scripts, in which the standard paths and 
# functions, which are needed more often are stored. 
#

rootsys=/opt/root_v5.12.00f
if ! export | grep $rootsys  >|/dev/null
then
   export ROOTSYS=$rootsys
   export PATH=$PATH:$ROOTSYS/bin
   export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$ROOTSYS/lib
fi

# site
processingsite=wue
sitekey=1
storagesite=wue

# for db backup
dbnames=( "TestFACTSetup" )

# queuing system
queuesys=condor # wuerzburg data center
condorpath=/usr/local/bin

# logging and setup
logpath=/magic/datacenter/fact/autologs
lockpath=/magic/datacenter/fact/locks
setuppath=/magic/simulated/setup

## paths of data
#datapath=/magic/data
#subsystempath=/magic/subsystemdata
#sequpath=/magic/sequences
#datasetpath=/magic/datasets

## paths and setup for mc
mcpath=/magic/simulated
#mcrawpath=$mcpath/rawfiles
mcsequpath=$mcpath/sequences
corsikapath=/home/montecarlo/MagicSoft/Simulation/Corsika/Mmcs6500
export FLUPRO=$corsikapath'/fluka2008_3'


#webpath=/www/htdocs/datacenter

# get paths for mars, macros and scripts
mars=`dirname $0 | sed -e 's/\/datacenter\/scripts//'`
macrospath=$mars/datacenter/macros
scriptspath=$mars/datacenter/scripts
# rcfiles
# dependencies of steps
steps=$mars/resources/steps_fact.rc
# file with db information
sqlrc=$mars/sql.rc


#addresses to which the errors are sent
erradrs="dorner@astro.uni-wuerzburg.de" 
#addresses to which the changes are sent
adrs="dorner@astro.uni-wuerzburg.de" 


#
#setup for jobmanager
#
sleeptime=600 #30
sleeptimelimit=360 #360
errorsleeptimedefault=60 #60

algorithm=2
#hour:            0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23
pnototal=(       32 32 32 32 32 32 32 32 32 32 32 32 32 32 32 32 32 32 32 32 32 32 32 32 )
pnototalwe=(     32 32 32 32 32 32 32 32 32 32 32 32 32 32 32 32 32 32 32 32 32 32 32 32 )


# set variables for jobmanager 
#scripts=( "runcorsika" "runceres" "runcallisto_mc" "runstar_mc" ) 
#scriptscolname=( "Corsika" "Ceres" "Callisto" "Star" ) 
#ratio=( 0.2 0.6 0.1 0.1 )
#maxjobs=( 20 20 25 25 )

scripts=( "runceres" "runcallisto_mc" "runstar_mc" ) 
scriptscolname=( "Ceres" "Callisto" "Star" ) 
ratio=( 0.6 0.2 0.2 )
maxjobs=( 30 25 25 )

# further wishlist: adapt ratio automatically

