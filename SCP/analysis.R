#!/usr/bin/env Rscript
args = commandArgs(trailingOnly=TRUE)
if (length(args)<2) {
  stop("2 arguments needed. Usage: Rscript analysis.R PATH/TO/best-known.txt PATH/TO/RESULTS/DIRECTORY", call.=FALSE)
}
bestknown <- read.csv(args[1], header = FALSE, sep=" ")
colnames(bestknown) <- c("Instance", "Cost")

results.folder <- args[2]

solutionquality <- function(df, best) 100 * (df$Cost - best$Cost) / best$Cost

cat("Exercise 1.1\n")
for(algo in c("ch1", "ch2", "ch3", "ch4")) {
    config.re <- paste0(algo, "+re")
    config.nore <- algo
    cat(config.nore, " and ", config.re, ":\n")
    filename.re <- paste0(config.re, ".txt")
    filename.nore <- paste0(config.nore, ".txt")
    result.re <- read.table(paste0(results.folder, filename.re), header = FALSE, sep = " ")
    result.nore <- read.table(paste0(results.folder, filename.nore), header = FALSE, sep = " ")
    colnames(result.re) <- c("Instance", "Cost")
    colnames(result.nore) <- c("Instance", "Cost")
    filtered <- bestknown[bestknown$Instance %in% result.re$Instance,] #Remove results from best known that don't appear in experiment results
    solutionquality.re <- solutionquality(result.re, filtered)
    solutionquality.nore <- solutionquality(result.nore, filtered)
    cat("Config: ", config.nore, "; Average deviation: ", mean(solutionquality.nore), "\n", sep="")
    cat("Config: ", config.re, "; Average deviation: ", mean(solutionquality.re), "\n", sep="")
    cat("Percentages of instances with better results using reduncancy elimination: ", mean(result.re$Cost < result.nore$Cost)*100, "%\n", sep="")
    improvements <- result.nore$Cost - result.re$Cost # differences are always >= 0
    cat("Average improvement: ", mean(improvements), "; minimum: ", min(improvements), "; maximum: ", max(improvements), "\n", sep="")
    cat("Tests for difference in solution quality between ", config.nore, " and ", config.re, ":\n", sep="")
    # cat("P value when using t test: ", t.test(solutionquality(result.re, filtered), solutionquality(result.nore, filtered), paired=TRUE)$p.value, "\n", sep="")
    shapiro.re <- shapiro.test(solutionquality.re)$p.value
    shapiro.nore <- shapiro.test(solutionquality.nore)$p.value
    cat("Shapiro test for ", config.nore, ": p-value: ", shapiro.nore, "<", 0.05, shapiro.nore<0.05, "\n")
    cat("Shapiro test for ", config.re, ": p-value: ", shapiro.re, "<", 0.05, shapiro.re<0.05, "\n")
    cat("P value when using Wilcoxon test: ", wilcox.test(solutionquality.re, solutionquality.nore, paired=TRUE)$p.value, "\n\n\n", sep="")
}
computation.times <- read.table(paste0(results.folder, "ex11_durations.txt"), header = FALSE, sep = " ")
colnames(computation.times) = c("Experiment", "Seconds")
cat("Computation times:\n")
print(computation.times, row.names = FALSE)
cat("\n", sep="")

cat("Exercise 1.2\n")
ex12.results <- list()
for(algo in c("ch1", "ch4")) {
    for(re in c(TRUE, FALSE)) {
        for(imp.algo in c("fi", "bi")) {
            if(re) {
                config <- paste0(algo, "+re+", imp.algo)
                filename <- paste0(config, ".txt")
                config.noimp <- paste0(algo, "+re")
                filename.noimp <- paste0(config.noimp, ".txt")
            } else {
                config <- paste0(algo, "+", imp.algo)
                filename <- paste0(config, ".txt")
                config.noimp <- algo
                filename.noimp <- paste0(config.noimp, '.txt')
            }
            result <- read.table(paste0(results.folder, filename), header = FALSE, sep = " ")
            colnames(result) <- c("Instance", "Cost")
            ex12.results[[config]] <- result
            filtered <- bestknown[bestknown$Instance %in% result$Instance,] #Remove results from best known that don't appear in experiment results
            cat("File: ", filename, "; Average deviation: ", mean(solutionquality(filtered, result)), "\n", sep="")
            result.noimp <- read.table(paste0(results.folder, filename.noimp), header = FALSE, sep = " ")
            colnames(result.noimp) <- c("Instance", "Cost")
            ex12.results[[config.noimp]] <- result.noimp
            cat("Percentages of instances with better results using improvement algorithm ", imp.algo, ": ", mean(result$Cost < result.noimp$Cost)*100, "%\n", sep="")
        }
    }
}

wilcox.quality.test <- function(x, y) {
    quality.x <- solutionquality(x, filtered)
    quality.y <- solutionquality(y, filtered)
    test.result <- wilcox.test(quality.x, quality.y, paired=TRUE)
    return(test.result$p.value)
}

d <- data.frame(matrix(NA,nrow = length(ex12.results),ncol = length(ex12.results)))
colnames(d) <- rownames(d) <- names(ex12.results)
for(i in row.names(d) ){
    for(j in colnames(d)){
        d[i,j] <- format(wilcox.quality.test(ex12.results[[i]],ex12.results[[j]]), digits=3)
    }}
    d[lower.tri(d,diag = T)] <- NA #Values under the diagonal should be the same as above: ignore them

    write.table(d, file = "p-matrix.txt", sep = " | ")
    print(d)

    computation.times <- read.table(paste0(results.folder, "ex12_durations.txt"), header = FALSE, sep = " ")
    colnames(computation.times) = c("Experiment", "Seconds")
    cat("Computation times:\n")
    print(computation.times, row.names = FALSE)