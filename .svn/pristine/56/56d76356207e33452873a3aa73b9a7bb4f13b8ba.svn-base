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
#   Author(s): Daniela Dorner  08/2009 <mailto:dorner@astro.uni-wuerzburg.de>
#
#   Copyright: MAGIC Software Development, 2000-2009
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

# queuing system
queuesys=condor # wuerzburg data center
condorpath=/usr/local/bin

# logging and setup
logpath=/magic/datacenter/autologs
lockpath=/magic/datacenter/locks
setuppath=/magic/datacenter/setup

## paths of data
#datapath=/magic/data
#subsystempath=/magic/subsystemdata
#sequpath=/magic/sequences
#datasetpath=/magic/datasets

# paths and setup for mc
mcpath=/magic/montecarlo
mcrawpath=$mcpath/rawfiles
mcsequpath=$mcpath/sequences
corsikapath=/home/operator/Corsika/Mmcs6500/
detectordir=/home/msmeyer/MC/MagicSoft/Simulation/Detector
reflectorversion="reflector 0.7"
cameraversion="camera 0.8"
export FLUPRO=$corsikapath'/fluka2008_3'


#webpath=/www/htdocs/datacenter


# get paths for mars, macros and scripts
mars=`dirname $0 | sed -e 's/\/datacenter\/scripts//'`
macrospath=$mars/datacenter/macros
scriptspath=$mars/datacenter/scripts
# rcfiles
# dependencies of steps
steps=$mars/resources/steps.rc
# file with db information
sqlrc=$mars/sql.rc


#addresses to which the errors are sent
erradrs="datacenter@astro.uni-wuerzburg.de" 
#addresses to which the changes are sent
adrs="datacenter@astro.uni-wuerzburg.de, tbretz@astro.uni-wuerzburg.de" 


#setup for jobmanager
sleeptime=30 #30
sleeptimelimit=360 #360
errorsleeptimedefault=60 #60
max=42 #maximum number of processes for one script in case there are more than one and the others do not have anything to do
totalmax=40 #maximum number of processes (total) overwrites pnototal(we) in case it is smaller

#
# FIXME: 
# max anz an prozessen setzen
# besser Verhaeltnis zwischen den Prozessen und das am besten noch 
# automatisch anpassen 
# evtl noch maximale anz an callistos setzen
# 

#hour:            0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23
pnototal=(       42 42 42 42 42 42 42 42 42 42 42 42 42 42 42 42 42 42 42 42 42 42 42 42 )
pnototalwe=(     42 42 42 42 42 42 42 42 42 42 42 42 42 42 42 42 42 42 42 42 42 42 42 42 )

pnocorsika=(     12 12 12 12 12 12 12 10  9  8  7  7  7  7  7  7  7  7  7  8  9 10 12 12 )
pnoreflector=(    8  8  8  8  8  8  8  8  8  8  8  8  8  8  8  8  8  8  8  8  8  8  8  8 )
pnocamera=(       4  4  4  4  4  4  4  4  4  4  4  4  4  4  4  4  4  4  4  4  4  4  4  4 )

pnocorsikawe=(   12 12 12 12 12 12 12 12 12 12 12 12 12 12 12 12 12 12 12 12 12 12 12 12 )
pnoreflectorwe=(  8  8  8  8  8  8  8  8  8  8  8  8  8  8  8  8  8  8  8  8  8  8  8  8 )
pnocamerawe=(     4  4  4  4  4  4  4  4  4  4  4  4  4  4  4  4  4  4  4  4  4  4  4  4 )

# set variables for jobmanager 
scripts=( "runcorsika" "runreflector" "runcamera" )
scriptscolname=( "fCorsikaFileAvail" "fReflectorFileAvail" "fCameraFileAvail" )
pnosweek=( ${pnocorsika[@]} ${pnoreflector[@]} ${pnocamera[@]} )
pnoswe=( ${pnocorsikawe[@]} ${pnoreflectorwe[@]} ${pnocamerawe[@]} )

