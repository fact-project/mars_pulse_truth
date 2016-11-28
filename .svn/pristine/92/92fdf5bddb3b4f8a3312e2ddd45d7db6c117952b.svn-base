#!/bin/bash

threshold=5
maxdist=14
macro=$PWD/extract_singles.C
outpath=$PWD/files

mkdir -p $outpath
cp ${macro} ${outpath}

where="(((fNumPedestalTrigger=3000 OR fNumPedestalTrigger=5000) AND fRunTypeKey=2) OR (fNumPedestalTrigger=10000 AND fRunTypeKey=3) OR fRunTypeKey=17)"
wheredrs="(drs.fROI=single.fROI AND fDrsStep=2)"

query="SELECT Concat(fNight, '_', fRunID, '_', (SELECT fRunID FROM RunInfo \`drs\` WHERE drs.fNight=single.fNight AND "$wheredrs" AND single.fRunID<30+drs.fRunID ORDER BY ABS(CAST(single.fRunID AS SIGNED) - CAST(drs.fRunID AS SIGNED)) ASC LIMIT 0,1)) AS num "
query=$query" FROM RunInfo \`single\` WHERE "$where

runpairs=( `mysql -u factread --password=r3adfac! --host=lp-fact factdata -s -e "$query"` )

for runpair in ${runpairs[@]}
do
   if [ $runpair = 'NULL' ]
   then
      continue
   fi

   year=`echo $runpair | cut -c1-4`
   month=`echo $runpair | cut -c5-6`
   day=`echo $runpair | cut -c7-8`
   night=`echo $runpair | cut -d_ -f1`
   run=`echo $runpair | cut -d_ -f2`
   drs=`echo $runpair | cut -d_ -f3`
   name=`echo $night | cut -c3-`

   runnum=`printf %03d $run`
   drsnum=`printf %03d $drs`

   #runpath="/archive/fact/fails/raw/"$year"/"$month"/"$day
   #drspath="/fact/raw/"$year"/"$month"/"$day

   runpath="/fact/raw/"$year"/"$month"/"$day
   drspath="/fact/raw/"$year"/"$month"/"$day

   runfile=$runpath"/"$night"_"
   drsfile=$drspath"/"$night"_"$drsnum.drs.fits.gz
   log=$outpath"/"$night"_"$runnum

   outfile=$outpath/`basename $runfile`${runnum}_${runnum}.root

   if [ -e $outfile ]
   then
      continue
   fi

#  echo "Submitting "${night}"_"${runnum}"["${drsnum}"]"

   cd ~/Mars

   cmd="ROOT_HIST=0 ${ROOTSYS}/bin/root -q -b -l ${macro}+\($maxdist,$threshold,\\\"$runfile\\\",$run,$run,\\\"$drsfile\\\",\\\"${outpath}\\\"\)"

   echo "echo '${cmd}'; date; ${cmd}" | qsub -N e${name}${runnum} -q test -cwd -e ${log}".err" -o ${log}".log"

   cd - > /dev/null
done


