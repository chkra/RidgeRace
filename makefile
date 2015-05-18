CXX=g++
CPPFLAGS=-I include -I Emaeus/include
CXXFLAGS=-O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP

BUILDDIR=build/
PROGNAME=RidgeRace

OBJ_RR= \
	src/RR_Algorithms.o \
	src/RR_Node.o \
	src/RR_Tree.o \
	src/RR_TreeAnnotation.o \
	src/RR_TreeAnnotator.o \
	src/Result.o \
	src/RidgeRaceInput.o \
	src/RidgeRacer.o \
	src/RidgeSolver.o \
	src/Simulator.o \
	src/TreeEvaluator.o \
	src/TreeExtractor.o \
	src/main.o

OBJ_Emaeus= \
	Emaeus/src/Chameleon.o \
	Emaeus/src/ConfigFile.o \
	Emaeus/src/DataParser.o \
	Emaeus/src/Exception.o \
	Emaeus/src/Externals.o \
	Emaeus/src/Files.o \
	Emaeus/src/Math.o \
	Emaeus/src/NewickParser.o \
	Emaeus/src/Node.o \
	Emaeus/src/Output.o \
	Emaeus/src/SetsAndMaps.o \
	Emaeus/src/StringManipulation.o \
	Emaeus/src/System.o \

all: createBuildDir $(OBJ_RR) $(OBJ_Emaeus)
	$(CXX) -o "$(BUILDDIR)$(PROGNAME)" $(patsubst src/%, build/%, $(OBJ_RR)) $(patsubst Emaeus/src/%, build/%, $(OBJ_Emaeus)) 
	cp $(BUILDDIR)$(PROGNAME) .
	
createBuildDir: 
	if [ ! -d "$(BUILDDIR)" ]; then mkdir $(BUILDDIR); fi;

src/%.o: src/%.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -MF "$(BUILDDIR)$*.d" -MT"$(BUILDDIR)$*.d" -o "$(BUILDDIR)$*.o" "src/$*.cpp"

Emaeus/src/%.o: Emaeus/src/%.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -MF "$(BUILDDIR)$*.d" -MT"$(BUILDDIR)$*.d" -o "$(BUILDDIR)$*.o" "Emaeus/src/$*.cpp"

clean:
	rm -f $(BUILDDIR)*.o
	rm -f $(BUILDDIR)*.d

distclean: clean
	rm -f $(BUILDDIR)$(PROGNAME)
	rm -f $(PROGNAME)
	rm -rf $(BUILDDIR)