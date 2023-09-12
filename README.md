# Expressions : Visitor Pattern Implementation

## Steps to Compile and Run

MacOS :

1. Deactivate any Anaconda environment if running
```
conda deactivate <env-name>
```

2. Install cmake
```
brew install cmake
```

3. Install GoogleTest : Required to write, execute tests for the codebase

```
brew install googletest
```

4. Make build directory
```
mkdir build
```

4. Build cmake project using following commands
```
cmake -S src -B build
cmake --build build
```

4. Test the project with
```
cd build/
./test0_dd
```