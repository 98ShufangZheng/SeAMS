$(REP_OBJS)/tiff_greylevel.o: tiff_greylevel.c
$(REP_OBJS)/noise.o: noise.cc 
$(REP_OBJS)/saveimage.o: saveimage.cc 
$(REP_OBJS)/options.o: options.cc getopt.h var_commune.hh types_communs.hh
$(REP_OBJS)/srmBproc.o: srmBproc.cc var_commune.hh types_communs.hh \
 srmBproc.hh tiff_greylevel.h
$(REP_OBJS)/srmB.o: srmB.cc var_commune.hh types_communs.hh noise.hh saveimage.hh \
 srmBproc.hh tiff_greylevel.h options.hh
