#
# makefile prototype 
#
# Christophe Fiorio, LIRMM 1997
# fiorio@lirmm.fr
#
# Pour utiliser ce makefile il vous suffit d'adapter les variables
# concernant les répertoires (ci-dessous), ainsi que les variables de
# compilation CFLAGS, LDFLAGS, ...
# Ensuite il vous suffit d'indiquer pour chacun de vos programmes la liste
# des modules objets dont ils dependent. 
# Enfin il faut génerer la liste des dépendance grâce à la commande "make
# depend". Notez qúil faudra réexécuter cette commande à chaque ajout d'un
# fichier source ou header.
#
# Pour indiquer la liste des modules objet dont dépend un programme, il
# suffit de remplacer progc ou progc++ (selon si vous travaillez en C ou
# C++) et de donner la liste des .o dont il dépend à la place de progc.o
# modulec.o ou progc++.o modulec++.o.

#
# Repertoires divers
#

REP_OBJS         = ./Objs
REP_BIN          = .
REP_LIB          = .
REP_INC          = .

#
# Quelques definitions
#
DEFINES =


##### COMPILATEURS

CC=nice gcc
CXX=nice g++

#----- INCLUDES

LOCAL_INC = -I. -I$(REP_INC) -I/usr/local/include

#----- LIBRAIRIES

LOCAL_LIB = -L$(REP_LIB) -L/usr/local/lib -ltiff
LIBS	  = 

#----- OPTIONS DE COMPIL

WARNFLAGS = -Wbad-function-cast -Wcast-qual -Wwrite-strings -Wconversion\
            -Wstrict-prototypes -Wmissing-declarations -Wredundant-decls -Winline\
            -Woverloaded-virtual

GENFLAGS = -Wall $(DEFINES) 

CINCLUDES = $(LOCAL_INC)
#CFLAGS    = $(GENFLAGS) $(CINCLUDES) -g
CFLAGS    = $(GENFLAGS) $(CINCLUDES) -O3
 
CXXINLINE    = -O3 -fkeep-inline-functions -finline-functions
CXXTEMPLATES = -fno-implicit-templates 
CXXINCLUDE   = $(LOCAL_INC) 
CXXFLAGS     = $(GENFLAGS) $(WARNFLAGS) $(CXXINCLUDE) $(CXXINLINE)
#CXXFLAGS     = $(GENFLAGS) $(WARNFLAGS) $(CXXINCLUDE) -g

#----- OPTIONS DE L'EDITEUR DE LIENS

LDFLAGS   = $(LOCAL_LIB) $(LIBS)


# -------------------------------------------------------------------------
# variables
#
VPATH = $(REP_BIN) #chemin de recherche pour les dependances sur les binaires

CSRCS = $(wildcard *.c)
CXXSRCS = $(wildcard *.cc)


# -------------------------------------------------------------------------
# -------------------------------------------------------------------------
#
# CIBLES
#

# Il faut modifier les cibles des programmes pour adapter le makefile a
# vos besoins.
# Pour cela il suffit de changer le nom de la cible (progc ou progc++) et
# de donner la liste des .o (progc.o modulec.o ou progc++.o modulec++.o)
# dont le programme depend.

all: srmB

clean: 
	rm -f $(REP_OBJS)/*.o

#progc: $(addprefix $(REP_OBJS)/, progc.o modulec.o)
#	@echo ~~~~~~~~~~~~~~~~~~ $@
#	$(CC) $(CFLAGS)  $^ $(LDFLAGS) -o $@

srmB: $(addprefix $(REP_OBJS)/, noise.o saveimage.o tiff_greylevel.o options.o srmBproc.o srmB.o)
	@echo ~~~~~~~~~~~~~~~~~~ $@
	$(CXX) $(CXXFLAGS)  $^ $(LDFLAGS) -o $@


# -------------------------------------------------------------------------
# -------------------------------------------------------------------------
#
# CREATION DE LIBRAIRIES
#

##### programmes de création et de maintenance de librairies

AR = umask 002; /usr/bin/ar
ARFLAGS = rc
RANLIB = /usr/bin/ranlib


##### definition d'une librairie

LIBTATA_OBJ = $(REP_OBJS)/tata.o
LIBTATA     = $(REP_LIB)/libtata.a


libtata: $(LIBTATA_OBJ)
	$(AR) $(ARFLAGS) $(LIBTATA) $?
	$(RANLIB) $(LIBTATA)

clean-libtata:
	rm -f $(LIBTATA_OBJ)


# -------------------------------------------------------------------------
# -------------------------------------------------------------------------

# -------------------------------------------------------------------------
# dependences generiques
#
$(REP_OBJS)/%.o: %.cc 
	@echo ------------------ $@
	$(CXX) -c $(CXXFLAGS) $< -o $@

$(REP_OBJS)/%.o: %.c 
	@echo ------------------ $@
	$(CC) -c $(CFLAGS) $< -o $@


# -------------------------------------------------------------------------
# dependences automatiques
#
depend:
	csh -ec '$(CC) -MM $(CFLAGS) $(wildcard *.c) | \
		sed '\''s/\(^.*\.o\)/$$(REP_OBJS)\/\1/g'\'' >! makefile.d'
	csh -ec '$(CXX) -MM $(CXXFLAGS) $(wildcard *.cc) | \
		sed '\''s/\(^.*\.o\)/$$(REP_OBJS)\/\1/g'\'' >> makefile.d'

include makefile.d

