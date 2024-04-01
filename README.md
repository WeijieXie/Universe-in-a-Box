# UniverseInABox

This project simulates the movement of particles in a space affected by an expansion factor. It includes tools for testing the simulation's correctness, benchmarking performance, visualizing particle movement, and comparing simulation outcomes under different conditions.

## Getting Started

### Compilation

To compile the project, open terminal and Run CMake to build the project:  

```bash  
cmake -B build  
cmake --build build  
```

Upon successful compilation, this will generate four executable files in the `build/bin/` directory:  

- `TestSimulation`  
- `NBody_Visualiser`
- `BenchmarkSimulation`
- `NBody_Comparison`  

## Testing  

It is recommended to run the tests before using the sampling programs. Execute the following command to perform unit tests:  

```bash  
./build/bin/TestSimulation
```

In normal circumstances, all tests should pass. These tests are designed to verify the integrity of the sampling algorithms.  

## Running the Programs  

### NBody_Visualiser

Visualize the movement of particles with a variety of configuration options. Here is how to use it, along with an example:

```bash  
./build/bin/NBody_Visualiser -nc <number_of_cells> -np <number_of_particles_per_cell> -t <total_time> -dt <time_step> -F <expansion_factor> -o <output_folder> -s <random_seed>
```

Example usage:  

```bash  
./build/bin/NBody_Visualiser -nc 51 -np 10 -t 1.5 -dt 0.01 -F 1.02 -o nc51-np10-t1_5-dt0_01-F1_02 -s 23093556
```

After running this line in the terminal, it will produce a series of diagrams(.pbm) in the folder `Images/nc51-np10-t1_5-dt0_01-F1_02` which illustrate the density of particles during different iterations.

### BenchmarkSimulation

This program benchmarks the performance of the simulation under different thread counts (1, 2, 4, 8) and prints detailed performance information, such as execution time.

Run the benchmark with:  

```bash  
./build/bin/BenchmarkSimulation
```

After execution, you will see output similar to this:  

```bash  
Actual number of threads have been set as: 2
Benchmarking Simulation::densityCalculator with 2 threads.
Time = 0.896979
Info: parallelization A, 10000000 numOfParticles with seed 23093556, 101 cells wide the box
```

### NBody_Comparison

This program compares simulation outcomes under different expansion factors and processes, outputting a CSV file with correlation information between particles. Use MPI for running multiple processes.

Example command:  

```bash  
mpirun -np 4 ./build/bin/NBody_Comparison -o <output_path> -min <min_expansion_factor> -max <max_expansion_factor>
```

Example usage:

```bash  
mpirun -np 4 ./build/bin/NBody_Comparison -o nc51-np10-t1_5-dt0_01-process4 -min 1.00 -max 1.06
```

After execution, a CSV file with the simulation's correlation information under different expansion factors will be generated in the specified output path.  

## Conclusion

The `UniverseInABox` project provides a suite of tools for simulating, testing, and analyzing the movement of particles in an expanding space. Through its various components, users can visualize the dynamics of particle systems, benchmark the performance of the simulation functions, and conduct comparative analyses with different expansion factors. This project is an interesting practice in the study of particle dynamics and parallelism techniques.  
