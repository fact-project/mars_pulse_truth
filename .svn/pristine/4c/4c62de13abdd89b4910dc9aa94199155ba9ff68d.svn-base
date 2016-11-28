#!/bin/bash

macro=$PWD/extract_temp.C
outpath=$PWD/temp

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

   night=`echo $runpair | cut -d_ -f1`
   runnum=`printf %03d $run`

   log=$outpath"/"$night"_"$runnum

   outfile=$outpath/${runnum}_${runnum}.root

   if [ -e $outfile ]
   then
      continue
   fi

   cd ~/Mars

   cmd="ROOT_HIST=0 ${ROOTSYS}/bin/root -q -b -l ${macro}+\($night,$runnum,\\\"${outfile}\\\"\)"

   echo "date; echo '${cmd}'; ${cmd}" | qsub -N t${night}${runnum} -q test -cwd -e ${log}".err" -o ${log}".log"

   cd - > /dev/null
done


