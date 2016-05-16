# Set Covering problem
Solving the set covering problem in _C_ using heuristic optimization.

# Usage
First the _C_ code needs to be compiled with a _C_ compiler.
On _Mac OS X_ (with _Apple Developer Tools_ installed) and _Ubuntu Linux_ (with _gcc_ installed), this can be done using the following command (when in the directory with the _C_ source code):
```
gcc *.c -o SCP
```
This should result in a binary file called `SCP`.
 The resulting binary can then be called by some parameters. For this implementation exercise, the following are relevant:
- `--seed SEED`: The `SEED` to use for the random number generator. If no value is provided, `1234567` is used as a seed.
- `--instance path/to/instance`: The path to the instance file that needs to be used. This value is required.
- `--ils` or `--aco`: The implemented algorithm to use. On of these is needed to execute an algorithm implemented for this exercise.
- `--mt`: Maximum time to run `--ils` or `--aco`.
- `--mc`: Cost at which to stop iterating.
- `--co`: Cut-off time when using `--mc`.
- `--trace`: File to which better costs with their time of achieving needs to be written.

To execute the experiments, one first needs to calculate the maximum time that an algorithm may run. To calculate this and write the results to a file, the following command is needed:
```
  ./calculate_maxtimes.sh
    PATH/TO/SCP
    PATH/TO/INSTANCES/DIRECTORY
    /PATH/TO/maxtimes.txt
```

Once this is done, the bash file to execute the actual experiments can be run using the following command:
```
  ./run_ex2.sh
    PATH/TO/SCP
    /PATH/TO/maxtimes.txt
    PATH/TO/INSTANCES/DIRECTORY
    /PATH/TO/best-known.txt
    PATH/TO/RESULTS/DIRECTORY
```
These are all relative paths. For the first experiment, each algorithm is run on each instance for a certain time, which is the one specified in `maxtimes.txt`. The resulting costs are then written to a text file specifically for that algorithm in the results directory.

For the second experiment, both algorithms were run 25 times on instances _A.1_, _B.1_, _C.1_ and _D.1_. Using the necessary parameters, trace files were written to the results directory.

When software to execute _R_ code is installed, the file `analysis\_ex2.R` can be executed to analyze the results obtained by running the `run\_ex2.sh` file. In the command line, this can be called like this:
```
Rscript analysis_ex2.R
    PATH/TO/best-known.txt
    /PATH/TO/maxtimes.txt
    PATH/TO/RESULTS/DIRECTORY
```
The results of this analysis are then printed to the command line.