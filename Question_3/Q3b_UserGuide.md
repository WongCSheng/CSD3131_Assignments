# Instructions of how to run the program

## Step 1:
A makefile has been provided in the folder.
Enter the command "make run". You should obtain an executable "q3b.exe" in the folder and
be able to run it automatically.

## Step 2:
Upon running the application, you will see the following options:
 1 - Heap vs Radix Benchmark (6-digit Integers), n = 100 to 10000000
 2 - Heap vs Radix Benchmark (6-letter Strings), n = 100 to 10000
 3 - Correctness Tests (empty, single, sorted, reversed, duplicates, negatives)
 4 - Radix Degradation Demo (key width scales with n)
 S - Heap Sort special case (equal keys)
 Q - Quit Application

You can enter numbers 1-5 to take a look at the test cases, or Q to quit the application.
Note that Test Cases 1 amd 2 will take a longer time to run, as they are running
benchmarking on both Heap and Radix Sort with increasingly larger data sets.