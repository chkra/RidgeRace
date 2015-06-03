library(ape)
require(stringr);


args <- commandArgs()
args <- args[4:length(args)]
print(args)

ml <- function(treePath, dataPath, id) {
	
	tree <- read.tree(treePath)
	tree$tip.label <- str_replace_all(tree$tip.label, "'", "")
	tree$node.label <- str_replace_all(tree$node.label, "'", "")

	
	plot(tree)

	tVals <- read.table(dataPath)
	names <- tVals[,1]
	rel <- grep("t", names)
	pheno <- tVals[rel,2]
	names(pheno) <- tVals[rel,1]
	
	acr <- ace(pheno, phy=tree,  method="REML")

	res <- cbind(tree$node.label, acr$ace)
	path <- paste(treePath, ".ml", sep="")

	write.table(res, path, col.names=FALSE, row.names=FALSE, sep="\t", quote=FALSE)
}


if (length(args) < 3) {
	cat("Usage: ml pathToTree\n")
	print(args)
	return -1;
}

ml(args[1], args[2], args[3])

#ml(args[1])
#ml("../trees/tree.newick.100")

