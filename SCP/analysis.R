bestknown <- read.csv('~/MA1-AI/Heuristic Optimization/exercises/impl_ex1/best-known.txt', header = FALSE, sep=" ")
colnames(bestknown) <- c("Instance", "Cost")


results.folder <- '/Users/arnomoonens/MA1-AI/Heuristic Optimization/exercises/impl_ex1/code_scp/SCP/SCP/results/'

solutionquality <- function(df, best) 100 * (df$Cost - best$Cost) / best$Cost

cat("Exercise 1.1\n")
for(algo in c("ch1", "ch2", "ch3", "ch4")) {
        filename.re <- paste0(algo, "+re.txt")
        filename.nore <- paste0(algo, ".txt")
        result.re <- read.table(paste0(results.folder, filename.re), header = FALSE, sep = " ")
        result.nore <- read.table(paste0(results.folder, filename.nore), header = FALSE, sep = " ")
        colnames(result.re) <- c("Instance", "Cost")
        colnames(result.nore) <- c("Instance", "Cost")
        filtered <- bestknown[bestknown$Instance %in% result.re$Instance,] #Remove results from best known that don't appear in experiment results
        cat("File: ", filename.re, "; Average deviation: ", mean(solutionquality(filtered, result.re)), "\n", sep="")
        cat("File: ", filename.nore, "; Average deviation: ", mean(solutionquality(filtered, result.nore)), "\n", sep="")
        cat("Percentages of instances with better results using reduncancy elimination: ", mean(result.re$Cost < result.nore$Cost), "\n", sep="")
        improvements <- result.nore$Cost - result.re$Cost # differences are always >= 0
        cat("Average improvement: ", mean(improvements), "; minimum: ", min(improvements), "; maximum: ", max(improvements), "\n", sep="")
        cat("Tests for difference in solution quality between ", algo, " and ", algo, "+RE:\n", sep="")
        cat("P value when using t test: ", t.test(solutionquality(result.re, filtered), solutionquality(result.nore, filtered), paired=TRUE)$p.value, "\n", sep="")
        cat("P value when using Wilcoxon test: ", wilcox.test(solutionquality(result.re, filtered), solutionquality(result.nore, filtered), paired=TRUE)$p.value, "\n", sep="")
}
computation.times <- read.table(paste0(results.folder, "ex11_durations.txt"), header = FALSE, sep = " ")
colnames(computation.times) = c("Experiment", "Seconds")
cat("Computation times:")
print(computation.times, row.names = FALSE)
cat("\n", sep="")

cat("Exercise 1.2\n")
for(algo in c("ch1", "ch4")) {
    for(re in c(TRUE, FALSE)) {
        for(imp.algo in c("fi", "bi")) {
            if(re) {
                filename <- paste0(algo, "+re+", imp.algo, ".txt")
                filename.noimp <- paste0(algo, "+re.txt")
            } else {
                filename <- paste0(algo, "+", imp.algo, ".txt")
                filename.noimp <- paste0(algo, '.txt')
            }
            result <- read.table(paste0(results.folder, filename), header = FALSE, sep = " ")
            colnames(result) <- c("Instance", "Cost")
            filtered <- bestknown[bestknown$Instance %in% result$Instance,] #Remove results from best known that don't appear in experiment results
            cat("File: ", filename, "; Average deviation: ", mean(solutionquality(filtered, result)), "\n", sep="")
            result.noimp <- read.table(paste0(results.folder, filename.noimp), header = FALSE, sep = " ")
            colnames(result.noimp) <- c("Instance", "Cost")
            cat("Percentages of instances with better results using improvement algorithm ", imp.algo, ": ", mean(result$Cost < result.noimp$Cost), "\n", sep="")
        }
    }
}

computation.times <- read.table(paste0(results.folder, "ex12_durations.txt"), header = FALSE, sep = " ")
colnames(computation.times) = c("Experiment", "Seconds")
cat("Computation times:")
print(computation.times, row.names = FALSE)