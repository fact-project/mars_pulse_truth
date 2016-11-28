# DO NOT DELETE

DataDep.d MData.o: MData.h ../mbase/MParContainer.h ../mbase/MAGIC.h ../mbase/MLog.h
DataDep.d MDataPhrase.o: MDataPhrase.h MData.h ../mbase/MParContainer.h ../mbase/MAGIC.h ../mbase/MArrayD.h ../mbase/MArray.h ../mbase/MLog.h ../mbase/MLogManip.h MDataValue.h MDataMember.h MDataElement.h
DataDep.d MDataArray.o: MDataArray.h ../mbase/MParContainer.h ../mbase/MAGIC.h ../mbase/MLog.h ../mbase/MLogManip.h MDataPhrase.h MData.h ../mbase/MArrayD.h ../mbase/MArray.h
DataDep.d MDataElement.o: MDataElement.h MData.h ../mbase/MParContainer.h ../mbase/MAGIC.h ../mhbase/MHMatrix.h ../mhbase/MH.h ../mbase/MLog.h ../mbase/MLogManip.h ../mbase/MParList.h
DataDep.d MDataMember.o: MDataMember.h MData.h ../mbase/MParContainer.h ../mbase/MAGIC.h ../mbase/MLog.h ../mbase/MLogManip.h ../mbase/MParList.h
DataDep.d MDataValue.o: MDataValue.h MData.h ../mbase/MParContainer.h ../mbase/MAGIC.h ../mbase/MLog.h ../mbase/MLogManip.h
DataDep.d MDataList.o: MDataList.h MData.h ../mbase/MParContainer.h ../mbase/MAGIC.h ../mbase/MLog.h ../mbase/MLogManip.h
DataDep.d MDataChain.o: MDataChain.h MData.h ../mbase/MParContainer.h ../mbase/MAGIC.h ../mbase/MLog.h ../mbase/MLogManip.h MDataList.h MDataValue.h MDataMember.h MDataFormula.h MDataElement.h
DataDep.d MDataFormula.o: MDataFormula.h MData.h ../mbase/MParContainer.h ../mbase/MAGIC.h ../mbase/MLog.h ../mbase/MLogManip.h MDataChain.h
DataDep.d: Makefile
