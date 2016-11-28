#!/bin/bash

inpath=$PWD/$1
outpath=${inpath}/fit
macro=${PWD}/fit_spectra.C

mkdir -p $outpath
cp ${macro} ${outpath}

files_out=`ls -1r ${inpath}  | grep \.root`
files_fit=`ls -1r ${outpath} | grep \.root`

for file in $files_out
do
    infile=$inpath"/"${file}
    outfile=${outpath}"/"${file}

    if [ -e $outfile ]
    then 
       continue
    fi

    cmd="ROOT_HIST=0 ${ROOTSYS}/bin/root -b -q -l ${macro}+\(\\\"${infile}\\\",\\\"${outfile}\\\"\)"

    cd ~/Mars

    echo "echo '${cmd}'; date; ${cmd}" | qsub -N f${file} -q test -cwd -e ${outfile}".err" -o ${outfile}".log"

    cd - > /dev/null
done

