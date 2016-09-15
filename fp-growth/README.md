# ABS


Mining maximal frequent itemset with ABS algorithm ([FIMI workshop](http://fimi.ua.ac.be) implementation)


## FIMI rules

All submissions should contain a single Makefile which, after issuing the command 'make' (GNU Make version 3.79.1), creates an executable 'fim_all' (for all frequent itemsets), 'fim_closed' (for closed frequent itemsets) and 'fim_maximal' (for maximal frequent itemsets).

The program should be executable with 3 parameters:
* The path of the dataset-file
* The minimal support threshold, which should be a non-negative integer. An itemset is frequent if its absolute support is larger or equal to this threshold.
* The output file. (should be able to be /dev/null!)

### Input data  

The dataset input must use the following ascii format only!
Each transaction is stored on a separate line as a list of items separated by white space and ending with a newline. Each item is a non-negative integer.
The items in the test datasets will be consecutively numbered starting from 0, and each transaction will be sorted ascending. (Note that this is not yet the case for the provided test datasets.)
For fairness of comparison, the use of multi-threading, advanced pipelining, low level memory optimizations, or direct expolitation of the hardware, etc. are disallowed! 

### Output data

After execution of the program, the output file should contain all frequent itemsets together with their absolute support.

Each line contains a single frequent itemset as a list of items separated by whitespace. At the end of the line, its absolute support is printed between parantheses.
For example:

1 2 3 (100)

represents the itemset containing items 1,2 and 3 and it has an absolute support of 100.
Note that also the empty set is considered as an itemset and its support should also be printed on a separate line in the output file. 

During or after execution, the program must print the total number of frequent itemsets and this number for each length between 0 and k separately to the standard output stream, with k the size of the largest (closed/maximal) frequent set.
If there is no maximal/closed frequent itemset for a given length, print 0!
For example:

32
1
5
10
10
5
1

means that there are 32 frequent itemsets, of which 1 frequent itemset is of length 0, 5 of length 1, 10 of length 2, 10 of length 3, 5 of length 4 and 1 of length 5. 

No other output is printed to the standard output stream. 

The exit status of the algorithm is 0 unless incorrect termination of the algorithm. 

No output is sent to the error output stream unless incorrect termination of the algorithm.
