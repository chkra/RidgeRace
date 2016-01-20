# RidgeRace
ridge regression for continuous ancestral character estimation on phylogenetic trees

####DESCRIPTION:
To run a RidgeRace analysis on your own data, you need your tree in newick format together with a tab separated file encoding phenotypic values observed at the leaf nodes. See the [ExampleRuns/Configs/amoA.conf](ExampleRuns/Configs/amoA.conf) file for a simple example. Run RidgeRace by typing e.g. `./RidgeRace ExampleRuns/Configs/amoA.conf > yourOutput.tree`

The file yourOutput.tree will be written in Nexus format encoding the ancestral values in the annotation *ridge*, the phenotypic rates in the annotation *ridgeWeight* and the original leaf values in the annotation *simulation*. You can visualize your output tree e.g. with the help of the `figtree` tool of Andrew Rambaut, see [http://tree.bio.ed.ac.uk/software/figtree](http://tree.bio.ed.ac.uk/software/figtree)


####REQUIREMENTS:
- `ublas` of the boost library (http://www.boost.org/doc/libs/1_58_0/libs/numeric/ublas/doc/)
- `ape` R package (http://cran.r-project.org/web/packages/ape/index.html)
- `stringr` R package (http://cran.r-project.org/web/packages/stringr/index.html)
	

####INSTALL:
```
make PREFIX=/usr  
make install PREFIX=/usr
```

####REFERENCES:  
Christina Kratsch and Alice C. McHardy  
[RidgeRace: ridge regression for continuous ancestral character estimation on phylogenetic trees](http://dx.doi.org/10.1093/bioinformatics/btu477)  
Bioinformatics (2014) 30 (17): i527-i533.  

Christina Kratsch  
[Computational methods to study phenotype evolution and feature selection techniques for biological data under evolutionary constraints](excerptDissChristina.pdf)  
Dissertation (2014)
