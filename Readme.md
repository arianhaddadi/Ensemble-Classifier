# Ensemble Classifier
A Multiprocess Classifier Written Using Linux Pipes and FIFOs.

## How to Build
This project is a cmake project. There is also a `Makefile` file
that can be used on supported systems to run the cmake. The result
will be in `build/bin/` folder. 

## How to Run
The project can be run by this command:

` 
 ./ensemble <validation_directory> <weights_directory>
`

Where `<validation_directory>` is the directory that holds 
the `dataset.csv` and `labels.csv` file and `<weights_directory>` 
is the directory that has the weights of each linear classifier.

## Execution steps
1. The ensemble classifier with fork-exec linear classifier as 
many as there are classifier files in the `<weights_directory>`. 
It will pass the information that linear classifiers need 
through linux pipes.
2. Each linear classifier will do a linear classification on each
of the 1000 data points inside `dataset.csv` file in the 
`<validation_directory>`. Each of them will write the result of 
their classifications inside a named pipe 
(created by using `mkfifo(3)` system call).
3. The ensemble classifier will also fork-exec a voter process 
which will read the classification results of each linear
classifier after they are done and for each data point,
choose the class that has the most votes 
(more linear classifiers have classified that data point with 
that class).
4. The voter process will write the results for each data point in
another named pipe between the voter and ensemble classifier 
processes and the ensemble will calculate the accuracy of 
the classification by comparing the results to the actual 
classes found in `labels.csv` file inside `<validation_directory>`
and output the results.
