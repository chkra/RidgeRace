library(ape)

args <- commandArgs()
args <- args[4:length(args)]


createRandomTree <- function(number, outPath) {

	number <- as.numeric(number);
	tree <- rtree(number);
	write.tree(tree, file=outPath);

}




if (length(args) != 2) {
	cat("Usage: createRandomTree number path\n")
	print(args)
	return -1;
}

createRandomTree(args[1], args[2])
