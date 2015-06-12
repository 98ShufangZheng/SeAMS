$(REP_OBJS)/tiff_color.o: tiff_color.c
$(REP_OBJS)/options.o: options.cc getopt.h var_commune.hh types_communs.hh
$(REP_OBJS)/srmproc.o: srmproc.cc var_commune.hh types_communs.hh \
 srmproc.hh tiff_color.h
$(REP_OBJS)/srm.o: srm.cc var_commune.hh types_communs.hh \
 srmproc.hh tiff_color.h options.hh
