#!/usr/bin/env Rscript

args = commandArgs(trailingOnly=TRUE)
if (length(args)<3) {
  stop("3 arguments needed. Usage: Rscript analysis.R PATH/TO/best-known.txt PATH/TO/maxtimes.txt PATH/TO/RESULTS/DIRECTORY", call.=FALSE)
}

results.folder <- args[3]

solutionquality <- function(df, best) 100 * (df - best) / best


cat("Part 1: Solution qualities\n")
filename.ils <- "ils.txt"
filename.aco <- "aco.txt"
costs <- read.table(paste0(results.folder, filename.ils), header = FALSE, sep = " ", row.names = 1)
costs.aco <- read.table(paste0(results.folder, filename.aco), header = FALSE, sep = " ", row.names = 1)
bestknown <- read.csv(args[1], header = FALSE, sep=" ", row.names = 1)
maxtimes <- read.csv(args[2], header = FALSE, sep = " ", row.names = 1)
colnames(maxtimes) <- c("maxtime")

colnames(costs) <- c("ils")
filtered <- subset(bestknown, row.names(bestknown) %in% row.names(costs))
costs$best <- filtered$V2
costs$aco <- costs.aco$V2

solution.qualities <- data.frame(row.names = row.names(costs))
solution.qualities$ils <- solutionquality(costs$ils, costs$best)
solution.qualities$aco <- solutionquality(costs$aco, costs$best)
cat("Config: ils; Average deviation: ", mean(solution.qualities$ils), "\n", sep="")
cat("Config: aco; Average deviation: ", mean(solution.qualities$aco), "\n", sep="")

plot(solution.qualities$ils, solution.qualities$aco, main="Correlation plot for ils and aco", xlab="Solution quality ILS", ylab="Solution quality ACO", sub=paste("Correlation coefficient: r=", cor(solution.qualities$ils, solution.qualities$aco)))
model <- lm(solution.qualities$aco ~ solution.qualities$ils)
abline(model, col = "red")

cat("P value when using Wilcoxon test: ", wilcox.test(solution.qualities$ils, solution.qualities$aco, paired=TRUE)$p.value, "\n", sep="")

# Code to make the csv file for the appendix table
tosave <- costs
tosave$solq.ils <- solution.qualities$ils
tosave$solq.aco <- solution.qualities$aco
tosave$maxtime <- maxtimes$maxtime
tosave <- tosave[c("best", "maxtime", "ils", "solq.ils", "aco", "solq.aco")]
colnames(tosave) <- c("best cost", "maximum runtime", "ils cost", "ils solution quality", "aco cost", "aco solution quality")
write.csv(format(tosave, digits=3), "results.csv", quote=FALSE)


cat("Part 2: qualified RTDs\n")
algos <- c("ils", "aco")
runs <- 25
instances <- c("A.1", "B.1", "C.1", "D.1")
part2.results <- list()
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
        total <- total + ifelse(nrow(before.time) == 0, 0.0, (tail(before.time,n=1)$cost <= q))
    }
    total/runs
}

plot_qrtd <- function(algo, instance) {
    all.times <- c()
    for(run in 1:runs) {
        df <- part2.results[[algo]][[instance]][[run]]
        all.times <- c(all.times, df$time)
    }
    all.times <- sort(unique(all.times))
    x <- list()
    y <- list()
    for(time in all.times) {
        x <- c(x, time)
        res <- rtd(part2.results[[algo]], instance, time, costs[instance,]$best*1.02)
        y <- c(y, res)
    }
    plot(x,y, xlab="Run-time (seconds)", ylab="P(solve)", main=paste0("Qualified run distribution for ", algo, " on instance ", instance), pch=NA, ylim=c(0,1))
    lines(x,y)
}

for (algo in algos) {
    for (instance in instances) {
        plot_qrtd(algo, instance)
    }
}
