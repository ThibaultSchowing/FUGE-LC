# FUGE-LC

This application implements the FuzzyCoCo algorithm. It constructs systems that predict the outcome of a human decision-making process while providing an understandable explanation of a possible reasoning leading to it. The constructed fuzzy systems are composed of rules and linguistic variables. For more information about the method, please refer to Prof. Carlos Andrés Peña Reyes' thesis entitled "Coevolutionary Fuzzy Modeling".

## Authors

- FuzzyCoCo algorithm based on *Fuzzy CoCo: a cooperative-coevolutionary approach to fuzzy modeling* from [Carlos Andrés Peña-Reyes](https://orcid.org/0000-0002-2113-6498)
- FUGE-LC by Jean-Philippe Meylan (2009-2010)
- Genetic algorithm update by Yvan Da Silva (2012)
- Code upgrade by [Rochus Keller](http://rochus-keller.ch/) <me@rochus.keller.ch> (2022)


## Releases

Check the new [v1.1.3 Release](https://github.com/ThibaultSchowing/FUGE-LC/releases)

## Notes

The source code was recently updated from an old deprecated version to a recent and functionnal one. It is intended to be modified and improved in the next years.

## Known bugs

- Random crash when running under Qt 5, which doesn't occur under Qt 4
- Experiment files not correctly saved under MacOS


## Last major updates

- Project restructuration
- QtScript module replaced by the Duktape JS engine
- Modification of integrated Qwt source code so that it also compiles with Qt 5 (known bugs)



# User Manual

# 1 INTRODUCTION

This application implements the FuzzyCoCo algorithm. It constructs systems that predict the outcome of a human decision-making process while providing an understandable explanation of a possible reasoning leading to it. The constructed fuzzy systems are composed of rules and linguistic variables. For more information about the method, please refer to Prof. Carlos Andrés Peña Reyes' thesis entitled "Coevolutionary Fuzzy Modeling".

# 2 INSTALLATION

## 2.1

Executables are available in the [v1.1.3 Release](https://github.com/ThibaultSchowing/FUGE-LC/releases).

## 2.2 SOURCE CODE COMPILATION
### 2.2.1 Requirements

- A C++98 compatible compiler
- Qt 4.8 (core, gui and xml modules)

### 2.2.2 FUGE-LC compilation

FUGE-LC is a Qt project so the commands are:

- \<path to Qt bin\>/qmake FUGE-LC.pro; make

# 3. FUNCTIONALITIES

The two main functionalities of FUGE-LC are the following:

- Modeling of a fuzzy system by coevolution
- Analysis and edition of a fuzzy system

The steps required to perform these two actions are described below.


# 4. FUZZY SYSTEM MODELING

In order to find a good system by coevolution, the following steps must be performed:

- 1. Choose a database
- 2. Edit the fuzzy, coevolution and fitness parameters
- 3. Run the evolution
- 4. Wait for the end of the evolution or manually stop it when a satisfying system has emerged

## 4.1 DATABASE CHOICE

The database can be selected via the Dataset menu or directly from the main window. The database file must meet the following requirements:

- CSV file type, separated by semicolons
- Each row (samples) and each column (variables) must start with a label
- The output variables must be placed in the last columns



## 4.2 PARAMETERS EDITION
The parameters can be divided in three distinct categories: fuzzy system, coevolution, fitness evaluation. These parameters can also be selected via a script file (see script section).

### 4.2.1 Fuzzy system parameters
These parameters determine the shape of the emerged fuzzy systems (number of rules, maximum number of variables per rule, number of input/output sets) and the number of bits used to code the different elements (variables, rules, membership functions) in the genome.

There is also parameter that indicates to the software how much output variables are present in the database. Without this indication, the application has no means to know the number output variables. 

Be very careful when editing these parameters. Incoherent values can lead to erroneous results and even to the crash of the application.

### 4.2.2 coevolution parameters

The coevolution algorithm can be configured via the following parameters:

- Number of generations to reach before stopping the evolution
- Maximum fitness threshold to reach before stopping the evolution
- Size of the populations (number of individuals)
- Size of the elite (best individuals kept unchanged between to generations)
- Crossover probability
- Probability that an individual is a target for a mutation
- Probability that a bit of an individual is mutated

#### 4.2.3 Fitness evaluation parameters

These parameters determine how the overall fitness of a system will be evaluated. A value is computed for all the following measurements and a weight for each one can be selected. The fitness value is the sum of these measurements multiplied by their weight. The measurements available are:

- Sensitivity : TruePos / (TruePos + FalseNeg)
- Specificity : TrueNeg / (TrueNeg + FalsePos)
- Accuracy : (TruePos +TrueNeg) / (TruePos + TrueNeg + FalsePos + FalseNeg)
- PPV : TruePos / (TruePos + FalsePos)
- RMSE : Root Mean Square Error
    
It is also possible to choose the threshold applied to the output values. Generally, it is selected in the middle of the minimum and maximum value.

## 4.3 COEVOLUTION RUN

Once the database is loaded and the parameters selected, the coevolution can be started with the Run command (or RunScript if a script is loaded). The run will continue until the termination conditions (max generations, max fitness) are reached, or until the Stop command is issued.

Note: It is always possible to save the current best system during the evolution.


## 4.4 END OF THE COEVOLUTION
When the evolution is finished, a popup window will appear asking if the user wants to save the best fuzzy system. At this point, the best system is also kept in memory for further analysis.


# 5. FUZZY SYSTEM ANALYSIS AND EDITION

In order to analyse it, a fuzzy system must be loaded in memory. The two ways to achieve this are the following:

- Run an evolution
- Load a previously saved fuzzy system
    
Once a fuzzy system is loaded, the following functionalities are available:

- Edition of the fuzzy system
- Validation of the fuzzy system
- Evaluation of the fuzzy system
-  Prediction with the fuzzy system
    
## 5.1 EDITION

The fuzzy system can be manually edited by selecting the Edit... command. A new window appears from which it is possible to make the following modifications:

- Edit the number and the names of the input/output sets
- Edit the input variables' membership functions
- Edit the rules of the system


## 5.2 VALIDATION

When this feature is selected, a new window appears showing how the system is able to perform on its database. If the database file cannot be found, a popup will ask the user to select one. The graph in the window shows the following curves:

- The expected output in blue (output of the database)
- The measured output in red (output of the system)
- The predicted output in green (output of the system with threshold applied)

## 5.3 EVALUATION

This feature is identical as the validation, except that the user is always asked to select the
database on which the fuzzy system must be applied.

## 5.4 PREDICTION

This feature allows predicting the results for a database without outputs. The database the
user specifies must have no output rows as the program will propose to save the
prediction. This will result in adding a row for the prediction of each output variable.

# 6. ADDITIONAL FEATURES

FUGE-LC provides also the following additional features:

- Script execution
- Command-line execution
- Log files

## 6.1 SCRIPT EXECUTION

It is possible to set the parameters and control the run the evolution with a script file. Once the dataset is loaded, instead of manually editing the parameter, a script file can be loaded. Then, the script file can be evaluated with the command ScriptRun. The script files have the .fs extension and are coded in the Javascript language. They are composed of the following elements:

- The definition of all the parameters
- The definition of the doSetParams() function
- The definition of the doRun() function which controls the execution of multiple (or single) runs of coevolution



All these elements must be present and complete in the script in order to make it valid. A reference valid script file called ref.fs is present in the bin/script/ folder. Its syntax is quite simple and should be easily understood. Basically, after having defined all the parameters and implemented the doSetParams() function, the doRun() function defines how much runs of evolution will be performed and which parameters will be modified between the runs.

## 6.2 COMMAND LINE EXECUTION

FUGE-LC can also be run directly from the command line, with or without GUI. However, this mode of execution is only available when using a script file. The command line execution syntax is the following:


    $ FUGE-LC -d datasetFile -s scriptFile -g useGUI

*datasetFile* must point to a valid dataset

*scriptFile* must point to a valid script file

*useGUI* can take the following values : yes or no


These options can also be consulted by running the following command:

    $ FUGE-LC –help

Note: a running X server is required event if the GUI is deactivated.


## 6.3 LOG FILES

Each coevolution run creates a log file located in bin/logs/. These files are named in a unique way which contains the date, some evolution parameters and the fitness of the best system found. These files are never deleted by the application. They contain the following information:

- A summary of the best system found, including its description (rules & membership functions)
- The parameters of the coevolution
- The statistics (fitness) of each generation














