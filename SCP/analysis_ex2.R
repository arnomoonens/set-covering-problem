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

plot(solq.ils, solq.aco, main="Correlation plot for ils and aco", xlab="Solution quality ILS", ylab="Solution quality ACO")
model <- lm(solq.aco ~ solq.ils)
abline(model, col = "red")

cat("P value when using Wilcoxon test: ", wilcox.test(solq.ils, solq.aco, paired=TRUE)$p.value, "\n", sep="")


cat("Part 2: qualified RTDs\n")
algos <- c("ils", "aco")
runs <- 25
instances <- c("A.1", "B.1", "C.1", "D.1")
part2.results <- list()
trace.config <- read.csv(paste0(results.folder, "trace_config.csv"), row.names=1)
for(algo in algos) {
    part2.results[[algo]] <- list()
    for(instance in instances) {
        part2.results[[algo]][[instance]] <- list()
        for(i in 1:runs) {
            part2.results[[algo]][[instance]][[i]] <- read.csv(paste0(results.folder, algo, "/", instance, "/", i, ".csv"), header = FALSE)
            colnames(part2.results[[algo]][[instance]][[i]]) <- c("time", "cost")
        }
    }
}

rtd <- function(results, instance, t, q) {
    total <- 0
    for(run in 1:runs) {
        df <- results[[instance]][[run]]
        before.time <- df[df$time <= t,]
        total <- total + ifelse(nrow(before.time) == 0,0.0, (tail(before.time,n=1)$cost <= q))
    }
    total/runs
}

plot_qrtd <- function(algo, instance) {
    min.time <- Inf
    max.time <- 0
    for(run in runs) {
        df <- part2.results[[algo]][[instance]][[run]]
        min.time <- min(min.time, head(df, n=1)$time)
        max.time <- max(max.time, tail(df, n=1)$time)
    }
    time <- min.time * 0.5
    stop.time <- max.time * 1.5
    x <- list()
    y <- list()
    while(time <= stop.time) {
        x <- c(x, time)
        res <- rtd(part2.results[[algo]], instance, time, trace.config[instance,]$best*1.02)
        y <- c(y, res)
        time <- time * 1.05
    }
    plot(x,y, xlab="Run-time", ylab="P(solve)", main=paste0("Qualified run distribution for ", algo, " on instance ", instance))
    lines(x,y)
}

for (algo in algos) {
    for (instance in instances) {
        plot_qrtd(algo, instance)
    }
}
