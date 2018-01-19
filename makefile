# Makefile for EXGW013
# 2011/9/8 [SZJ] Created

include ./make.defines

BIN = demo

CXXFLAGS = ${DEFINES}
LDFLAGS = ${LIBS}
OBJECT =  testTraderApi.o TraderSpi.o

all: veryclean $(BIN)
	 

$(BIN):	$(OBJECT)
	${CXX} $(CXXFLAGS) -o $@ $(OBJECT) $(LDFLAGS)

clean:
	rm -f ${CLEANFILES}

veryclean: clean
	rm -f $(BIN)
