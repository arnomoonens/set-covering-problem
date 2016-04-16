#!/usr/bin/env Rscript

args = commandArgs(trailingOnly=TRUE)
if (length(args)<2) {
  stop("2 arguments needed. Usage: Rscript analysis.R PATH/TO/best-known.txt PATH/TO/RESULTS/DIRECTORY", call.=FALSE)
}
bestknown <- read.csv(args[1], header = FALSE, sep=" ")
colnames(bestknown) <- c("Instance", "Cost")

results.folder <- args[2]

solutionquality <- function(df, best) 100 * (df$Cost - best$Cost) / best$Cost


cat("Part 1: Solution qualities\n")
filename.ils <- "ils.txt"
filename.aco <- "aco.txt"
result.ils <- read.table(paste0(results.folder, filename.ils), header = FALSE, sep = " ")
result.aco <- read.table(paste0(results.folder, filename.aco), header = FALSE, sep = " ")
colnames(result.ils) <- c("Instance", "Cost")
colnames(result.aco) <- c("Instance", "Cost")
filtered <- bestknown[bestknown$Instance %in% result.ils$Instance,]
solq.ils <- solutionquality(result.ils, filtered)
solq.aco <- solutionquality(result.aco, filtered)
cat("Config: ils; Average deviation: ", mean(solq.ils), "\n", sep="")
cat("Config: aco; Average deviation: ", mean(solq.aco), "\n", sep="")

plot(solq.ils, solq.aco, main="Scatterplot Example", xlab="Solution quality ILS", ylab="Solution quality ACO")
model <- lm(solq.aco ~ solq.ils)
abline(model, col = "red")

cat("P value when using Wilcoxon test: ", wilcox.test(solq.ils, solq.aco, paired=TRUE)$p.value, "\n", sep="")