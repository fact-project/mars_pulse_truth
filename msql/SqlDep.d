# DO NOT DELETE

SqlDep.d MSQLServer.o: MSQLServer.h MTreeSQL.h
SqlDep.d MSQLMagic.o: MSQLMagic.h MSQLServer.h
SqlDep.d MTreeSQL.o: MSQLServer.h MTreeSQL.h MBasketSQL.h
SqlDep.d MBasketSQL.o: MBasketSQL.h MBufferSQL.h
SqlDep.d MBufferSQL.o: MBufferSQL.h ../mbase/MTime.h ../mbase/MParContainer.h ../mbase/MAGIC.h
SqlDep.d: Makefile
