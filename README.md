# Minimum Degree Spanning Tree

### DESCRIPTION
Iterative algorithm that finds MDST within one of the optimal degree.

### PREREQUISITES
- Python 3.7+
- g++ 9+
- cmake 3.17+
- make
- gcov
- lcov
```
pip install -r ./requirements.txt
```

### BUILDING PROJECT
Enter project directory and execute this list of commands:
```
mkdir build
cd build
cmake .. [-DDEBUG=ON]
make
```
Output file can be found in ```bin``` directory. By default program is compiled in release mode but it can be toggled to debug via ```DEBUG``` option.

```main``` binary executes selected algorithm of given data.

### RUNNING TESTS AND GETTING COVERAGE
Enter project directory and execute this list of commands:
```
mkdir build
cd build
cmake .. -DTESTING=ON
make coverage
```
Output files can be found in ```build``` directory in folders which names end with ```_coverage```. Results for each of test groups are stored in separate folders. Binaries for testing can be found in ```bin``` directory. 

### RUNNING STATISTICS
Enter project directory and execute this command:
```
python3 stats/validator.py
```
Plot file will be named as ```accuracy_plot.png``` in ```stats``` directory. 
