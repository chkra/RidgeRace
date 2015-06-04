library(RColorBrewer)


# returns for every std the correlation averaged over all regimes
averageOverRegimes <- function(X) {

	allStds <- unique( X$sigmaBase )
	corr <- rep(0, length(allStds))
	for (i in 1:length(allStds)) {
	
		rel <- X$sigmaBase == allStds[i]
		corr[i] <- mean(X$corr[rel])
	}
	return(cbind(allStds,corr))
}

# returns for every regime the correlation averaged over all stds
averageOverStds <- function(X) {

	allRegimes <- unique( X$regimes )
	corr <- rep(0, length(allRegimes))
	for (i in 1:length(allRegimes)) {
	
		rel <- X$regimes == allRegimes[i]
		corr[i] <- mean(X$corr[rel])
	}
	return(cbind(allRegimes, corr))
}


makeRegimePlots <- function(X) {

	X <- X[X$nrNodes>30,]
	X <- X[X$regimes>=2,]
	X <- X[X$sigmaBase>=1.5,]

	allLambs <- unique( X[X$method=="Ridge",]$lam)
	pLength <- length(allLambs)
	#colors <- colorRampPalette(brewer.pal(9,"Spectral"))(pLength)[pLength:1]
	colors <- colorRampPalette(brewer.pal(9,"Blues"))(pLength)[1:pLength]
	#leg <- list()

	#plot(X$regimes, X$corr, type="n", bty="n", xlim=c(1,8), ylim=c(0,1), xlab="number of regimes", ylab="correlation")
	#for (i in pLength:1) {
	i <- pLength
		lam <- allLambs[i]
		#curRdg <- X[(X$method=="Ridge" ),]
		#curRdg <- curRdg[curRdg$lam==lam,]
		#res <- averageOverStds(curRdg)
		#lines(res[,1], res[,2], col=colors[i], type="b", pch=22, bg=colors[i])
	#}

	D <- as.data.frame(X)
	
	# a very complicated way to say: take the last lambda
	s <- X$method=="Ridge" & ( (X$nrNodes >= 300 & X$lam == 0.1) | (X$nrNodes < 300)  )

	places <- 1:length(unique(D$regimes))
	boxplot(formula=corr~regimes, xlim=c(0.5,8.5), outline=FALSE, data=D, xlab="number of regimes", ylab="correlation",  bty="n", subset = s, at=places-0.25, boxwex=0.2, col=colors[pLength/2+1], xaxt="n", type="n", cex.lab=1.6)

	if (length(unique(X$regimes))>1) {
		m <- lm(formula=corr~regimes, data=D, subset = method=="Ridge")
		abline(m, col=colors[pLength/2+1]); print(m)
		m <- lm(formula=corr~regimes, data=D, subset = method=="GLS")
		abline(m, col="coral2"); print(m)
		m <- lm(formula=corr~regimes, data=D, subset = method=="ML")
		abline(m, col="gold"); print(m)
	}

	rrB <- boxplot(formula=corr~regimes, xlim=c(0.5,8.5), outline=FALSE, data=D, xlab="number of regimes", ylab="correlation",  bty="n", subset = s, at=places-0.25, boxwex=0.2, col=colors[pLength/2+1], cex.axis=1.6, add=TRUE, cex.lab=1.6)
	glsB <- boxplot(formula=corr~regimes, outline=FALSE, data=D, subset = method=="GLS", bty="n", at=places, boxwex=0.2, add=TRUE, col="coral2", axes=FALSE)
	mlB <- boxplot(formula=corr~regimes, outline=FALSE, data=D, subset = method=="ML", bty="n", at=places+0.25, boxwex=0.2, add=TRUE, col="gold", axes=FALSE)

	colnames(rrB$stats) <- rrB$names
	colnames(glsB$stats) <- glsB$names
	colnames(mlB$stats) <- mlB$names

	print(rrB$stats[3,]-glsB$stats[3,])
	print(max(rrB$stats[3,]-glsB$stats[3,]))

	print(rrB$stats[3,]-mlB$stats[3,])
	print(max(rrB$stats[3,]-mlB$stats[3,]))


	legend("topright", legend=c("Ridge", "GLS", "ML"), fill = c(colors[pLength/2], "coral2", "gold"))


}



makeStdPlots <- function(X) {

	
	X <- X[X$nrNodes>30,]

	allLambs <- unique( X[X$method=="Ridge",]$lam)
	pLength <- length(allLambs)
	colors <- colorRampPalette(brewer.pal(9,"Blues"))(pLength)[1:pLength]

	i <- pLength
	lam <- allLambs[i]
	curRdg <- X[(X$method=="Ridge" ),]
	curRdg <- curRdg[curRdg$lam==lam,]
	res <- averageOverStds(curRdg)

	D <- as.data.frame(X)
	s <- X$method=="Ridge" & ( (X$nrNodes >= 300 & X$lam == 0.1) | (X$nrNodes < 300)  )

	places <- 1:length(unique(D$sigmaBase))
	rrB <- boxplot(formula=corr~sigmaBase, outline=FALSE, data=D, xlab="sigma", ylab="correlation", bty="n", subset = s, at=places-0.3, boxwex=0.20, col=colors[pLength/2], cex.axis=1.6, cex.lab=1.6)
	glsB <- boxplot(formula=corr~sigmaBase, outline=FALSE, data=D, subset = method=="GLS", bty="n", at=places, boxwex=0.20, add=TRUE, col="coral2", axes=FALSE)
	mlB <- boxplot(formula=corr~sigmaBase, outline=FALSE, data=D, subset = method=="ML", bty="n", at=places+0.3, boxwex=0.20, add=TRUE, col="gold", axes=FALSE)
	legend("bottomright", legend=c("Ridge", "GLS", "ML"), fill = c(colors[pLength/2], "coral2", "gold"), bg="white", cex=1.6)

	colnames(rrB$stats) <- rrB$names
	colnames(glsB$stats) <- glsB$names
	colnames(mlB$stats) <- mlB$names

	print(rrB$stats[3,]-glsB$stats[3,])
	print(max(rrB$stats[3,]-glsB$stats[3,]))

	print(rrB$stats[3,]-mlB$stats[3,])
	print(max(rrB$stats[3,]-mlB$stats[3,]))

}


makeNodePlots <- function(X) {

	X <- X[X$nrNodes>30,]
	allLambs <- unique( X[X$method=="Ridge",]$lam)
	pLength <- length(allLambs)
	#colors <- colorRampPalette(brewer.pal(9,"Spectral"))(pLength)[pLength:1]
	colors <- colorRampPalette(brewer.pal(9,"Blues"))(pLength)[1:pLength]
	#leg <- list()

	#plot(X$regimes, X$corr, type="n", bty="n", xlim=c(1,8), ylim=c(0,1), xlab="number of regimes", ylab="correlation")
	#for (i in pLength:1) {
	i <- pLength
		lam <- allLambs[i]
		curRdg <- X[(X$method=="Ridge" ),]
		curRdg <- curRdg[curRdg$lam==lam,]
		res <- averageOverStds(curRdg)
		#lines(res[,1], res[,2], col=colors[i], type="b", pch=22, bg=colors[i])
	#}

	D <- as.data.frame(X)
	s <- X$method=="Ridge" & ( (X$nrNodes >= 300 & X$lam == 0.1) | (X$nrNodes < 300)  )

	places <- 1:length(unique(D$nrNodes))
	rrB <- boxplot(formula=corr~nrNodes, outline=FALSE, data=D, xlab="number of nodes in tree", ylab="correlation", bty="n", subset = s, at=places-0.25, boxwex=0.15, col=colors[pLength/2], cex.axis=1.6, cex.lab=1.6)
	glsB <-boxplot(formula=corr~nrNodes, outline=FALSE, data=D, subset = method=="GLS", bty="n", at=places, boxwex=0.2, add=TRUE, axes=FALSE, col="coral2")
	mlB  <-boxplot(formula=corr~nrNodes, outline=FALSE, data=D, subset = method=="ML", bty="n", at=places+0.25, boxwex=0.2, add=TRUE, col="gold", axes=FALSE, xaxt="n")
	legend("bottomright", legend=c("Ridge", "GLS", "ML"), fill = c(colors[pLength/2], "coral2", "gold"), bg="white", cex=1.6)

	colnames(rrB$stats) <- rrB$names
	colnames(glsB$stats) <- glsB$names
	colnames(mlB$stats) <- mlB$names

	print(rrB$stats[3,]-glsB$stats[3,])
	print(max(rrB$stats[3,]-glsB$stats[3,]))

	print(rrB$stats[3,]-mlB$stats[3,])
	print(max(rrB$stats[3,]-mlB$stats[3,]))

	#axis(1, cex.axis=1.6)
	#axis(2, cex.axis=1.6)


}


args <- commandArgs()
args <- args[4:length(args)]

X <- read.table(args[1], fill=T)
#X <- read.table("../log/summary.log", fill=T)
#p <- args[1]
#rp <- substr(basename(p), 1, nchar(basename(p)) - 4) 
#out <- paste("plot/", rp, ".pdf", sep="")
out <- args[2] 
#out <- "performance_pearsCorrelation.pdf"

#X <- read.table("../log/testRateCorrelation.dat_2013-4-8-16:35.log")


colnames(X) <- c("method", "nrNodes", "regimes", "mu", "sigmaBase", "lam", "corr", "err", "meanErr")

#inval <- X$nrNodes*0.2 < X$regimes
#X <- X[!inval,] 


pdf(out, width=10)
#makeRegimePlots(X)
makeStdPlots(X)
makeNodePlots(X)
dev.off()
