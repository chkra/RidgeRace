CXX=g++
CPPFLAGS=-I include -I Emaeus/include
CXXFLAGS=-O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP
PREFIX=$(HOME)
INSTALL=install

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
	src/prefix.o \
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

all: checkDependencies createBuildDir writeSettings $(OBJ_RR) $(OBJ_Emaeus)
	$(CXX) -o "$(BUILDDIR)$(PROGNAME)" $(patsubst src/%, build/%, $(OBJ_RR)) $(patsubst Emaeus/src/%, build/%, $(OBJ_Emaeus)) 
	
createBuildDir: 
	if [ ! -d "$(BUILDDIR)" ]; then mkdir $(BUILDDIR); fi;

src/%.o: src/%.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -MF "$(BUILDDIR)$*.d" -MT"$(BUILDDIR)$*.d" -o "$(BUILDDIR)$*.o" "src/$*.cpp"

Emaeus/src/%.o: Emaeus/src/%.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -MF "$(BUILDDIR)$*.d" -MT"$(BUILDDIR)$*.d" -o "$(BUILDDIR)$*.o" "Emaeus/src/$*.cpp"

checkDependencies:
	@R --version | grep "The R Foundation" > /dev/null || (echo "The software R seems to be missing. Please install R first (http://www.r-project.org/)." && exit 1)
	@echo 'library("stringr")' | R --vanilla > /dev/null 2> /dev/null || (echo "The R package 'stringr' is missing (http://cran.r-project.org/web/packages/stringr/index.html). Please install it first." && exit 1)
	@echo 'library("ape")' | R --vanilla > /dev/null 2> /dev/null || (echo "The R package 'ape' is missing (http://cran.r-project.org/web/packages/ape/index.html). Please install it first." && exit 1)
	@echo "All dependencies are met."

writeSettings:
	printf "#include \"prefix.h\"\n\nnamespace RidgeRace {\n\tconst char prefix[] = \"$(PREFIX)\";\n\tconst char progname[] = \"$(PROGNAME)\";\n}\n" > src/prefix.cpp

install:
	@if [ `grep "const char prefix" src/prefix.cpp | cut -d '"' -f 2` != $(PREFIX) ]; then \
		echo "The value of PREFIX is now different from what was used when compiling the program, i.e. running the 'make all' target. Please re-run make with the same PREFIX!"; \
		exit 1; \
	fi;
	@if [ `grep "const char progname" src/prefix.cpp | cut -d '"' -f 2` != $(PROGNAME) ]; then \
		echo "The value of PROGNAME is now different from what was used when compiling the program, i.e. running the 'make all' target. Please re-run make with the same PROGNAME!"; \
		exit 1; \
	fi;
	if [ ! -d "$(PREFIX)" ]; then $(INSTALL) -d $(PREFIX); fi;
	if [ ! -d "$(PREFIX)/bin" ]; then $(INSTALL) -d $(PREFIX)/bin; fi;
	if [ ! -d "$(PREFIX)/share" ]; then $(INSTALL) -d $(PREFIX)/share; fi;
	if [ ! -d "$(PREFIX)/share/$(PROGNAME)" ]; then $(INSTALL) -d $(PREFIX)/share/$(PROGNAME); fi;
	if [ ! -d "$(PREFIX)/share/$(PROGNAME)/scripts" ]; then $(INSTALL) -d $(PREFIX)/share/$(PROGNAME)/scripts; fi;
	$(INSTALL) -m 755 build/$(PROGNAME) $(PREFIX)/bin;
	$(INSTALL) -m 644 scripts/*.r $(PREFIX)/share/$(PROGNAME)/scripts/;


clean:
	rm -f $(BUILDDIR)*.o
	rm -f $(BUILDDIR)*.d

distclean: clean
	rm -f $(BUILDDIR)$(PROGNAME)
	rm -f $(PROGNAME)
	rm -rf $(BUILDDIR)