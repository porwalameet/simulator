#
# Makefile for mont program
#

# Parameters
INSTDIR = bin
MONT = sim
CFLAGS = -g
LEX = lex

csrc = $(wildcard common/*.c) \
	$(wildcard bgp/*.c)

obj = $(csrc:.c=.o)

LDFLAGS = -Lbin -ljsmn

# Targets 
all : OPENSRC $(MONT)
	mkdir -p bin
	mv $(MONT) ${INSTDIR}

OPENSRC: 
	(cd jsmn; make all)

$(MONT): $(obj)
	$(CXX) -g -o $@ $^ $(LDFLAGS)

clean:
	rm -rf common/*.o bin/*
	(cd jsmn; make clean)

