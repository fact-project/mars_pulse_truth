# DO NOT DELETE

FileIODep.d MRead.o: MRead.h ../mbase/MTask.h ../mbase/MInputStreamID.h ../mbase/MParContainer.h ../mbase/MAGIC.h ../mbase/MLog.h ../mbase/MLogManip.h ../mbase/MDirIter.h
FileIODep.d MChain.o: MChain.h
FileIODep.d MReadTree.o: MReadTree.h MRead.h ../mbase/MTask.h ../mbase/MInputStreamID.h ../mbase/MParContainer.h ../mbase/MAGIC.h MChain.h ../mbase/MFilter.h ../mbase/MParList.h ../mbase/MTaskList.h ../mbase/MParameters.h ../mbase/MParEmulated.h ../mbase/MStatusDisplay.h ../mbase/MLog.h ../mbase/MLogManip.h
FileIODep.d MReadFiles.o: MReadFiles.h MRead.h ../mbase/MTask.h ../mbase/MInputStreamID.h ../mbase/MParContainer.h ../mbase/MAGIC.h ../mbase/MLog.h ../mbase/MLogManip.h ../mcore/izstream.h ../mbase/MDirIter.h
FileIODep.d MReadReports.o: MReadReports.h MRead.h ../mbase/MTask.h ../mbase/MInputStreamID.h ../mbase/MParContainer.h ../mbase/MAGIC.h ../mbase/MLog.h ../mbase/MLogManip.h ../mbase/MTime.h ../mbase/MParList.h ../mbase/MTaskList.h MReadMarsFile.h MReadTree.h
FileIODep.d MReadMarsFile.o: MReadMarsFile.h MReadTree.h MRead.h ../mbase/MTask.h ../mbase/MInputStreamID.h ../mbase/MParContainer.h ../mbase/MAGIC.h ../mbase/MLog.h ../mbase/MLogManip.h ../mbase/MParList.h ../mbase/MTaskList.h ../mraw/MRawRunHeader.h ../mbase/MTime.h ../mmc/MMcRunHeader.hxx
FileIODep.d MReadScanFile.o: MReadScanFile.h MReadFiles.h MRead.h ../mbase/MTask.h ../mbase/MInputStreamID.h ../mbase/MParContainer.h ../mbase/MAGIC.h ../mbase/MLog.h ../mbase/MLogManip.h ../mbase/MParameters.h ../mbase/MParList.h
FileIODep.d MWriteFile.o: MWriteFile.h ../mbase/MTask.h ../mbase/MInputStreamID.h ../mbase/MParContainer.h ../mbase/MAGIC.h ../mbase/MLog.h ../mbase/MLogManip.h ../mbase/MParList.h
FileIODep.d MWriteAsciiFile.o: MWriteAsciiFile.h MWriteFile.h ../mbase/MTask.h ../mbase/MInputStreamID.h ../mbase/MParContainer.h ../mbase/MAGIC.h ../mbase/MIter.h ../mdata/MDataList.h ../mdata/MData.h ../mdata/MDataPhrase.h ../mbase/MArrayD.h ../mbase/MArray.h ../mdata/MDataValue.h ../mdata/MDataMember.h ../mbase/MLog.h ../mbase/MLogManip.h ../mbase/MParList.h
FileIODep.d MWriteRootFile.o: MWriteRootFile.h MWriteFile.h ../mbase/MTask.h ../mbase/MInputStreamID.h ../mbase/MParContainer.h ../mbase/MAGIC.h ../mbase/MLog.h ../mbase/MLogManip.h MRead.h ../mbase/MParList.h ../mbase/MStatusDisplay.h
FileIODep.d MWriteFitsFile.o: ../mbase/MLogManip.h ../mbase/MLog.h ../mbase/MArrayS.h ../mbase/MArray.h ../mbase/MAGIC.h ../mbase/MArrayB.h ../mbase/MArrayF.h MRead.h ../mbase/MTask.h ../mbase/MInputStreamID.h ../mbase/MParContainer.h ../mbase/MParList.h ../mbase/MStatusDisplay.h MWriteRootFile.h MWriteFile.h MWriteFitsFile.h MTopFitsGroup.h ../mcore/fits.h ../mcore/FITS.h ../mcore/checksum.h ../mcore/ofits.h MFitsArray.h ../mraw/MRawRunHeader.h ../mbase/MTime.h
FileIODep.d MTopFitsGroup.o: MTopFitsGroup.h ../mcore/fits.h ../mcore/FITS.h ../mcore/checksum.h ../mcore/ofits.h
FileIODep.d MFitsArray.o: ../mbase/MAGIC.h ../mbase/MLogManip.h ../mbase/MLog.h MFitsArray.h ../mcore/fits.h ../mcore/FITS.h ../mcore/checksum.h ../mcore/ofits.h
FileIODep.d MMatrix.o: MMatrix.h ../mbase/MParContainer.h ../mbase/MAGIC.h ../mbase/MLog.h ../mbase/MLogManip.h
FileIODep.d: Makefile
