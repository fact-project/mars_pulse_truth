# DO NOT DELETE

RawDep.d MRawRunHeader.o: MRawRunHeader.h ../mbase/MTime.h ../mbase/MParContainer.h ../mbase/MAGIC.h ../mbase/MLog.h ../mbase/MLogManip.h ../mbase/MArrayS.h ../mbase/MArray.h ../mbase/MString.h
RawDep.d MRawEvtHeader.o: MRawEvtHeader.h ../mbase/MParContainer.h ../mbase/MAGIC.h ../mcalib/MCalibrationCam.h ../mgui/MCamEvent.h ../mbase/MLog.h ../mbase/MLogManip.h ../mbase/MTime.h ../mbase/MArrayB.h ../mbase/MArray.h MRawRunHeader.h
RawDep.d MRawEvtData.o: MRawEvtData.h ../mbase/MParContainer.h ../mbase/MAGIC.h ../mgui/MCamEvent.h ../mbase/MLog.h ../mbase/MLogManip.h ../mbase/MArrayS.h ../mbase/MArray.h ../mbase/MArrayB.h ../mbase/MArrayI.h ../mgeom/MGeomCam.h ../mbase/MQuaternion.h MRawCrateArray.h MRawCrateData.h MRawRunHeader.h ../mbase/MTime.h MRawEvtPixelIter.h
RawDep.d MRawEvtPixelIter.o: MRawEvtPixelIter.h ../mbase/MAGIC.h MRawEvtData.h ../mbase/MParContainer.h ../mgui/MCamEvent.h ../mbase/MArrayS.h ../mbase/MArray.h ../mbase/MArrayB.h
RawDep.d MRawCrateArray.o: MRawCrateArray.h ../mbase/MParContainer.h ../mbase/MAGIC.h ../mbase/MLog.h MRawCrateData.h
RawDep.d MRawCrateData.o: MRawCrateData.h ../mbase/MParContainer.h ../mbase/MAGIC.h ../mbase/MLog.h ../mbase/MLogManip.h
RawDep.d MRawBoardsFACT.o: MRawBoardsFACT.h ../mbase/MParContainer.h ../mbase/MAGIC.h
RawDep.d MRawRead.o: MRawRead.h ../mfileio/MRead.h ../mbase/MTask.h ../mbase/MInputStreamID.h ../mbase/MParContainer.h ../mbase/MAGIC.h ../mbase/MLog.h ../mbase/MLogManip.h ../mbase/MTime.h ../mbase/MParList.h MRawRunHeader.h MRawEvtHeader.h ../mcalib/MCalibrationCam.h ../mgui/MCamEvent.h MRawEvtData.h MRawCrateData.h MRawCrateArray.h
RawDep.d MRawFileRead.o: MRawFileRead.h MRawRead.h ../mfileio/MRead.h ../mbase/MTask.h ../mbase/MInputStreamID.h ../mbase/MParContainer.h ../mbase/MAGIC.h ../mbase/MLog.h ../mbase/MLogManip.h ../mcore/izstream.h ../mbase/MTime.h ../mbase/MParList.h ../mbase/MStatusDisplay.h MRawRunHeader.h MRawEvtHeader.h ../mcalib/MCalibrationCam.h ../mgui/MCamEvent.h MRawEvtData.h MRawCrateData.h MRawCrateArray.h
RawDep.d MRawFitsRead.o: MRawFitsRead.h MRawFileRead.h MRawRead.h ../mfileio/MRead.h ../mbase/MTask.h ../mbase/MInputStreamID.h ../mbase/MParContainer.h ../mbase/MAGIC.h ../mbase/MLog.h ../mbase/MLogManip.h ../mcore/factfits.h ../mcore/zfits.h ../mcore/fits.h ../mcore/FITS.h ../mcore/checksum.h ../mcore/huffman.h ../mbase/MTime.h ../mbase/MArrayB.h ../mbase/MArray.h ../mbase/MArrayS.h ../mbase/MParList.h MRawRunHeader.h MRawEvtHeader.h ../mcalib/MCalibrationCam.h ../mgui/MCamEvent.h MRawEvtData.h MRawBoardsFACT.h
RawDep.d: Makefile
