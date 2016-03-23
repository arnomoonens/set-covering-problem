# Set Covering problem
Solving the set covering problem in _C_ using heuristic optimization.

# Usage
First the _C_ code needs to be compiled with a _C_ compiler. On _Mac OS X_ (with [_Apple Developer Tools_](http://developer.apple.com/technologies/tools/) installed) and _Ubuntu Linux_ (with _gcc_ installed), this can be done using the following command (when in the directory with the _C_ source code): `gcc -o lsscp main.c utils.h utils.c`. This should result in a binary file called `lsscp`.
 The resulting binary can then be called by some parameters. These are the following:

- `--seed SEED`: The `SEED` to use for the random number generator. If no value is provided, `1234567` is used as a seed.
- `--instance path/to/instance`: The path to the instance file that needs to be used. This value is required.
- `--ch1`, `--ch2`, `--ch3` or `--ch4`: Constructive method to be used to build the initial solution. Exactly one of these has to be provided.
- `--re`: This optional parameter signals that redundancy elimination needs to be applied after the initial solution is constructed.
- `--bi` or `--fi`: Whether to use respectively best improvement or first improvement. Maximally one of these parameters may be provided.

It is also possible to execute all the possible configurations (which constructive method, redundancy elimination or not and possibly an improvement method) from exercise _1.1_ and _1.2_ on all the instances using a shell file called `run.sh`. These file should be called like this:
`run.sh PATH/TO/BINARY PATH/TO/INSTANCES/DIRECTORY PATH/TO/RESULTS/DIRECTORY`. These are all relative paths. This file works as follows: for each configuration, the binary file is run with that configuration on each instance. Each execution gives us a cost of the resulting solution. For each instance, this cost is written to a file that describes the configuration, e.g. `ch1+re+bi.txt`. After a configuration is run on all the instances, the time that it took to be executed on all instances is written to a file with the time of each configuration in an exercise. These files are for exercise _1.1_ and _1.2_ respectively `ex11_durations.txt` and `ex12_durations.txt`.

When [software to execute _R_ code](https://www.r-project.org/) is installed, the file `analysis.R` can be executed to analyze the results obtained by running the `run.sh` file. In the command line, this can be called like this: `Rscript analysis.R PATH/TO/best-known.txt PATH/TO/RESULTS/DIRECTORY`. The results of this analysis are then printed to the command line.
