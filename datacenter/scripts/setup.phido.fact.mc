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
#   Author(s): Daniela Dorner  09/2010 <mailto:daniela.dorner@unige.ch>
#
#   Copyright: MAGIC Software Development, 2000-2010
#
#
# ========================================================================
#
# This a resource file for the scripts, in which the standard paths and 
# functions, which are needed more often are stored. 
#

rootsys=/fhgfs/groups/app/magic/software/root_v5.12.00g
if ! export | grep $rootsys  >|/dev/null
then
   export ROOTSYS=$rootsys
   export PATH=$PATH:$ROOTSYS/bin
   export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$ROOTSYS/lib
fi

# site
processingsite=phido
sitekey=2
storagesite=wue

# queuing system
queuesys=pbs # wuerzburg data center
pbspath=/cm/shared/apps/torque/2.3.7/bin
setwalltime="yes"
setpmem="yes"


mainpath=/fhgfs/groups/app

# logging and setup
logpath=$mainpath/fact/datacenter/autologs
lockpath=$mainpath/fact/datacenter/locks
setuppath=$mainpath/fact/simulated/setup

## paths of data
#datapath=/magic/data
#subsystempath=/magic/subsystemdata
#sequpath=/magic/sequences
#datasetpath=/magic/datasets

## paths and setup for mc
mcpath=$mainpath/fact/simulated
mcsequpath=$mcpath/sequences
corsikapath=$mainpath/magic/software/MagicSoft/Simulation/Corsika/Mmcs6500
corsikapath=$mainpath/fact/Mmcs6500
export FLUPRO=$corsikapath'/fluka2008_3c'


#webpath=/www/htdocs/datacenter

# get paths for mars, macros and scripts
if [ "$SOURCEFILEPATH" = "" ]
then 
   mars=`dirname $0 | sed -e 's/\/datacenter\/scripts//'`
else
   mars=`echo $SOURCEFILEPATH | sed -e 's/\/datacenter\/scripts//'`
fi
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
sleeptime=900 #900

algorithm=1
tosubmittotal=150 #150
limitidle=10

# set variables for jobmanager 
scripts=( "runcorsika"  ) 
scriptscolname=( "Corsika" ) 
ratio=( 1 )
maxjobs=( 800 )

# settings for pbs
walltimes=( "72:00:00" )
pmems=( "1gb" ) 


#
#information for copying: 
#

# steps to be copied
tocopy=( "Corsika" )

# machine
remotemachine="dc10"
remotedir="/magic"

