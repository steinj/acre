Automated Code Review and Evaluation (ACRE)
====

Automated Code Review and Evaluation of the LLVM family of coding standards


Description
-------------------------------

Code quality has been an important cornerstone in the software development process 
in order to ensure the software has been developed according to some set of defined standards.
Measuring code quality is a challenging process since not all metrics of quality can be quantitatively represented.
Code reviews have been developed as a means for developers to weigh in on the quality of a piece of software
in relation to a set of coding standards.
We will develop a tool that analyzes source code according to some specific quality standards,
in our case we will use the [coding standards](https://ix.cs.uoregon.edu/~norris/pat/refs/CodingStandards/CodingStandards.html) of the physicists at Fermilab.
Our tool will attempt to address coding standards that a tool such as *grep* are unable to handle cleanly.
The output of our program will be the location of the code that violates the standards as well as a description of the specific standard that has been violated. 


Coding Standards Checked
-------------------------------

- Usage of "#include <iostream>"
- Usage of static constructors
- Usage of end() in the condition of loops
- If statements that could possibly be used for early exits
    - Only considers size of the if statement body
- structs that have protected/private members
- Usage of return/break/continue/etc. in the if body of an if-else statement


Build/Cleanup Instructions
-------------------------------

Build: run "make"
  It will produce the following:
    acre   (the code quality tool)
    build/ (directory of object files)

Clean: run "make clean"
  It will remove the following:
    acre
    build/


Usage
-------------------------------

ACRE is to be used in place of the compiler used in the build system for C/C++ projects (ACRE uses gcc/g++ as its backend).

  Command Line Option                 | Description
  ------------------------------------|-------------------------------------------
  -acre:config_file <filename>        | File to use as the config file (Default: ${HOME}/.acre/config)
  -acre:output_file <filename>        | File to output coding standard files (Default: cout)
  -acre:early_exit_threshold <value>  | Threshold for number of statements that must be reached for an if-stmt to be a candidate for early exit (Default: 20)

Configuration File:
  Notes
    Format: libconfig (www.hyperrealm.com/libconfig/)

  Settings (i.e. what can be defined in the file)
    early_exit_threshold = <value>;    // same as early_exit_threshold defined above
    output_file = "<path-to-file>";  // string, must include quotes
    ignore = [ "<path-to-file>", "<directory>", ... ];  // list of files and/or directories to be ingored (defined as strings)
                                                        // directories must end with a "/"

Interactions between CLI and Configuration File:
  Order of application
    1. Configuration File
    2. CLI Options
         * Any option provided as a CLI option overrides the value defined in the configuration file


Output
-------------------------------

ACRE outputs the code standard violation in addition to the file and line number of the violation.
Additional information is included within square brackets, i.e. comments within [].
The violations are output in the following format:

<violation description>: <filename> @ <line number> [additional info]


Pragmas (used within source code)
-------------------------------

```
#pragma acre ignore [recursive]
```

  Ignores all coding standards that violate the the following statement.
  If "recursive" is also defined, then all nested statements are also ignored.


Example Usage
-------------------------------

Compile and analyze the globals.cpp source code
```
> acre -c tests/tests/staticConstructorTraversalTests/globals.cpp
```

Use of ACRE within a CMake build system
```
> export CC=/path/to/acre
> export CXX=/path/to/acre
> cmake CMakeLists.txt
```

Notes
-------------------------------

struct-class keyword violations
  - Does NOT check for conflicts between forward declaration and definiting declaration due to a limitation in ROSE where defining declaration = forward declaration.
  - Extracts the class type (e.g. class/struct/etc.) from the unparsed string since template structs are internally represented as template classes in ROSE.
