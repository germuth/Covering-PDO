#
# Makefile for program "cover" by Kari J. Nurmela.
#

CC = gcc
LIBS= -lm

#FLAGS= -ggdb -W # unoptimized, debuggable
FLAGS= -W -O2 # optimized, not debuggable

HEADERS= cover.h bincoef.h tables.h setoper.h anneal.h solcheck.h exp.h arg.h
OBJECTS= cover.o bincoef.o tables.o setoper.o anneal.o solcheck.o exp.o arg.o
CS=      cover.c bincoef.c tables.c setoper.c anneal.c solcheck.c exp.c arg.c
AUTOBACKUPS= *~

.c.o :
	$(CC) $(FLAGS) -c $<

cover:	$(OBJECTS)
	rm -f cover
	$(CC) -o cover $(OBJECTS) \
	$(LIBS)

distrib:
	tar cvf cover.tar anneal.c arg.c bincoef.c cover.c exp.c setoper.c solcheck.c tables.c anneal.h arg.h bincoef.h cover.h exp.h setoper.h solcheck.h tables.h Makefile README
	compress cover.tar
	zip cover.zip anneal.c arg.c bincoef.c cover.c exp.c setoper.c solcheck.c tables.c anneal.h arg.h bincoef.h cover.h exp.h setoper.h solcheck.h tables.h Makefile README
	echo "NOTE: distribution contains files cover.tar.Z, cover.zip, and README."

clean:
	rm -f $(OBJECTS)
	rm -f $(AUTOBACKUPS)
	rm -f core
	rm -f cover.tar
	rm -f cover.tar.Z

cover.o: \
	cover.h \
	bincoef.h \
	tables.h \
	anneal.h \
	exp.h \
	solcheck.h

bincoef.o: \
	cover.h \
	bincoef.h

tables.o: \
	cover.h \
	tables.h \
	bincoef.h \
	setoper.h

setoper.o: \
	cover.h \
	setoper.h \
	bincoef.h

anneal.o: \
	cover.h \
	anneal.h \
	bincoef.h \
	exp.h \
	tables.h

solcheck.o: \
	cover.h \
	setoper.h \
	bincoef.h \
	tables.h \
	solcheck.h

arg.o: \
	cover.h \
	arg.h \
	tables.h
