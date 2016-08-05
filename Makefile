# Build targets
SimRank_O= core/SRBenchmark.cpp \
           core/srgs.cpp \
		   core/accuratesimrank.cpp \
		   core/FingerPrint.cpp \
		   core/SimRankMOD14.cpp \
		   core/SimRankMOD14Index.cpp \
		   core/SimRankWWW05.cpp \
		   core/topsim.cpp \
		   core/logmanager.cpp \
		   core/onewaygraph.cpp \
		   core/dynamicowgmanager.cpp \
		   core/mytime.cpp \
		   core/KronSimRank.cpp\
		   core/origKronSimRank.cpp\
		   core/EffiEVD.cpp\
		   core/EffiSVD.cpp\
		   core/OptEffiSVD.cpp\
		   core/OIPSimRank.cpp\
		   core/PSRSimRank.cpp\
           core/naiveSR.cpp\
	
USER_FLAGS=-Wno-unused-result -Wno-unused-but-set-variable -Wno-sign-compare
USER_LIBS=

# Compiler flags
CC  = gcc
CXX = g++
CFLAGS=$(USER_FLAGS) -O3 -fPIC -Wall -g  -I./include -I ./armadillo/include -llapack -lblas 
CXXFLAGS=$(CFLAGS)
LDFLAGS=-lpthread

# The name of the excution that will be built
SimRank=SRbenchmark

all:
	$(CXX) -w  -pthread $(LDFLAGS) -o $(SimRank) $(SimRank_O) $(CXXFLAGS)
	
clean:
	rm -f $(SimRank)
	find . -name '*.o' -print | xargs rm -f
