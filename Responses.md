# Responses(23093556)

## Representing Particles (1.2)  

To represent the particles, I wrote 2 classes: class `particle` and class `particles`.  

- class `particle` (related file: ["particle.hpp"](include/particle.hpp), ["particle.cpp"](lib/particle.cpp)): it is naturally to create a class to store the properties of a single particle. It has `position` and `velocity` which are `std::vector<double>`. For that `mass` and `dt` share across all instances, I made them static. `dt` may seem strange here, but it is useful for updating the particle's state over time, so I make it a member variable of the class. Operations such as updating positions and velocities are inherently actions performed on individual particles. Therefore, I have designed `velocityRescaler` and `updater` as member methods, adhering to the principles of object-oriented programming. Among these, `positionSetter` serves as a helper method, acting as a wrapper that encapsulates the functionality.  
- class `particles` (related file: ["particles.hpp"](include/particles.hpp), ["particles.cpp"](lib/particles.cpp)): this class is a container which is used for initializing and storing multiple instances of `particle`. `particleInfo` is `std::vector<particle>`. In terms of efficiency, such a structure greatly facilitates traversal operations.  (in commit 5cc1389, refactor the initializtion method to improve efficiency).  

## Complexity (1.10)  

- **the density calculation**: For that `densityCalculator()` goes through every particles to find which cell it belongs to, and it does not care what **N<sub>c</sub>** is.Thus, it is **O(n)** for **n<sub>p</sub>**, **O(1)** for **N<sub>c</sub>**.  
- **the potential calculation**: the size of input/output buffer to FFTW is **N<sub>c</sub>** which results in **O(nlog(n))** for **N<sub>c</sub>**, and it need to go through every entry of the buffer to do scaling which is **O(n)** for **N<sub>c</sub>**. For that **O(nlog(n))** > **O(n)**, the overall complicity is **O(nlog(n))** for **N<sub>c</sub>**. It does not care about **n<sub>p</sub>**, so it is **O(1)** for **n<sub>p</sub>**.
- **the gravitational field calculation** : `accelerationCalculator()` needs to go through each cell to calculate their acceleration effect on particles, and the calculation in a single cell has nothing to do with **N<sub>c</sub>** or **n<sub>p</sub>**. Thus, it is **O(n)** for **N<sub>c</sub>**, **O(1)** for **n<sub>p</sub>**.
- **the the particle update function**: `particlesUpdater()` go through every particles to update their positions and velocities according to which cell they located in. However, the cellIdentifier for a single particle is just a plain calculation independent of both **N<sub>c</sub>** and **n<sub>p</sub>**. Thus, it is **O(n)** for **n<sub>p</sub>**, **O(1)** for **N<sub>c</sub>**.  
- In **PM method**, the highest complexity comes from **O(nlog(n))** for **N<sub>c</sub>** or **O(n)** for **n<sub>p</sub>**. Given that the number of particles is always larger than the total number of cells, then **O(nlog(n))** for **N<sub>c</sub>** is always smaller than **O(nlog(n))** for **n<sub>p</sub>**, and **O(nlog(n))** for **n<sub>p</sub>** is smaller than **O(n<sup>2</sup>)** for **n<sub>p</sub>** in **PP method**. Thus the complexity of **O(nlog(n))** for **N<sub>c</sub>** in **PM method** will never exceed **O(n<sup>2</sup>)** for **n<sub>p</sub>** in **PP method**. To sum up, Particle-Mesh method is better than the Particle-Particle method for large simulations.  

## Visualising A Developing Universe (1.11)  

- I ran the simulation with this 2 set of parameters:  
  - `./build/bin/NBody_Visualiser -nc 51 -np 10 -t 1.5 -dt 0.01 -F 1.02 -o nc51-np10-t1_5-dt0_01-F1_02  -s 23093556` (images stores here: [Images/nc51-np10-t1_5-dt0_01-F1_02](Images/nc51-np10-t1_5-dt0_01-F1_02))  
  - `./build/bin/NBody_Visualiser -nc 51 -np 10 -t 1.5 -dt 0.01 -F 1.00 -o nc51-np10-t1_5-dt0_01-F1_00  -s 23093556` (images stores here: [Images/nc51-np10-t1_5-dt0_01-F1_00](Images/nc51-np10-t1_5-dt0_01-F1_00))  

## Shared Memory Parallelism (2.1)  

### Class `Particle`

no OpenMP is used here, for that the loop in this class is really small with only 3 iterations, the overhead of using OpenMP may overwhelm the benefit of optimization.  

### Class `Particles`

this class is mainly used for initializing and storing particles, which should not be paralleised in order to get the same initialized particles.  

### Class `Simulation`

- `Simulation::kSquareUpdater`: I tried `collapse` and `schedule` here, for that the loop iteration is completely independent and the loop is quite balanced. In each following cases, 50 loops will run to estimate the excuting time.  
  - **original**: `BenchmarkSimulation` displays:  
  *Actual number of threads have been setted as: 8  
  Benchmarking Simulation::kSquareUpdater with 8 threads.  
  Time = 1.73178  
  Info: original, 300 cells wide the box*  
  - **#pragma omp parallel for**: after running `BenchmarkSimulation` with plain `#pragma omp parallel for`, it displays:  
  *Actual number of threads have been setted as: 8  
  Benchmarking Simulation::kSquareUpdater with 8 threads.  
  Time = 0.921958  
  Info: #pragma omp parallel for, 300 cells wide the box*  
  - **#pragma omp parallel for collapse(3)**: I assumed that `collapse(3)` would have a better performance for that the loop iteration here is completely independent. However, `BenchmarkSimulation` displays:  
  *Actual number of threads have been setted as: 8  
  Benchmarking Simulation::kSquareUpdater with 8 threads.  
  Time = 1.24242  
  Info: #pragma omp parallel for collapse(3), 300 cells wide the box*  
  It may because the inside for loop is not complex enough. So the benefits from `collapse(3)` is less than its overhead. Thus the executing time increases.
  - **#pragma omp parallel for collapse(2)**: now the `BenchmarkSimulation` displays:  
  *Actual number of threads have been setted as: 8  
  Benchmarking Simulation::kSquareUpdater with 8 threads.  
  Time = 0.842812  
  Info: #pragma omp parallel for collapse(2), 300 cells wide the box*  
  Better than `collapse(3)` and plain `#pragma omp parallel for`, I will try different schedules on basis of this.  
  - **collapse(2) schedule(static)**: because  the loop is quite balanced, I assumed a static schedule will be good. With `schedule(static)`, it displays:  
  *Actual number of threads have been setted as: 8  
  Benchmarking Simulation::kSquareUpdater with 8 threads.  
  Time = 0.881825  
  Info: collapse(2) schedule(static), 300 cells wide the box*  
  No improvement at all?!  
  - **collapse(2) schedule(guided)**: `BenchmarkSimulation` displays:  
  *Actual number of threads have been setted as: 8  
  Benchmarking Simulation::kSquareUpdater with 8 threads.  
  Time = 0.889854  
  Info: collapse(2) schedule(guided), 300 cells wide the box*  
  Not good.
  - **collapse(2) schedule(dynamic)**: `BenchmarkSimulation` displays:  
  *Actual number of threads have been setted as: 8  
  Benchmarking Simulation::kSquareUpdater with 8 threads.  
  Time = 0.768747  
  Info: collapse(2) schedule(dynamic), 300 cells wide the box*  
  Surprisingly `schedule(dynamic)` is better.  
  - thus the final directives for `Simulation::kSquareUpdater` will be `#pragma omp parallel for collapse(2) schedule(dynamic)`, which is weird.
- `Simulation::densityCalculator`: I tried several different approaches to accelerate this method and hope to get some experience to reduce repetitive trials when dealing with the following methods.  
  - Without parallelization: I run the `BenchmarkSimulation` and get such result:  
  *Actual number of threads have been setted as: 8  
  Benchmarking Simulation::densityCalculator with 8 threads.  
  Time = 1.4174  
  Info: without parallelization, 10000000 numOfParticles with seed 23093556, 101 cells wide the box*
  - **parallelization A**: I tried `#pragma omp parallel for` and to avoid racing when the threads tring to visit the same entry in the buffer, I added the preprocessor directive `#pragma omp critical`. `BenchmarkSimulation` displays:  
  *Actual number of threads have been setted as: 8  
  Benchmarking Simulation::densityCalculator with 8 threads.  
  Time = 1.86652  
  Info: parallelization A, 10000000 numOfParticles with seed 23093556, 101 cells wide the box*  
  It is weird that the excuting time increases. I think it is because of the overhead of 8 threads in `critical` preprocessor directive, then I decreased the number of threads to 2 and rerun `BenchmarkSimulation`. It displays:  
  *Actual number of threads have been setted as: 2  
  Benchmarking Simulation::densityCalculator with 2 threads.  
  Time = 0.896979  
  Info: parallelization A, 10000000 numOfParticles with seed 23093556, 101 cells wide the box*  
  Now the result seems to be good, which proves my guess. However, I am not quite happy with the performance.  
  - **parallelization B**: I then tried using `#pragma omp reduction`. However, reduction does not support such directives as `reduction(+ : densityBuffer[index][0])`, for that it requires a *compiled-time-known* virable instead of a *run-time-known* virable. Thus, `#pragma omp reduction` is not suitable here.  
  - **parallelization C**: I found an alternative directive `#pragma omp atomic`, which has the same effect avoiding racing as `#pragma omp critical` in **parallelization A** but more efficient. I ran `BenchmarkSimulation` with 8 threads. It displays:  
  *Actual number of threads have been setted as: 8  
  Benchmarking Simulation::densityCalculator with 8 threads.  
  Time = 0.252781  
  Info: parallelization C, 10000000 numOfParticles with seed 23093556, 101 cells wide the box*  
  It looks much better!  
  - **parallelization D** and **parallelization E**: D and E are almost the same, the only difference is D uses `#pragma omp critical` and E uses `#pragma omp atomic`. And deducing by the results of A and C, **parallelization E** may be better. I was hoping to decrease the frequency of multiple threads trying to access the same entry, thus they don't have to wait each other. I create each thread a private buffer called `localDensity`, thus when dealing with the particles they are assigned, they will not race. Racing only happens when the threads trying to summarize their `localDensity` into `this->densityBuffer`. Also, I added `#pragma omp for nowait` to neglect the implicit barrier of for loop. I ran `BenchmarkSimulation` for both of them with 8 threads.  
  for **parallelization D**:  
  *Actual number of threads have been setted as: 8  
  Benchmarking Simulation::densityCalculator with 8 threads.  
  Time = 0.405116  
  Info: parallelization D, 10000000 numOfParticles with seed 23093556, 101 cells wide the box*  
  for **parallelization E**:  
  *Actual number of threads have been setted as: 8  
  Benchmarking Simulation::densityCalculator with 8 threads.  
  Time = 0.403171  
  Info: parallelization E, 10000000 numOfParticles with seed 23093556, 101 cells wide the box*  
  Compare **parallelization D** with **parallelization E**, it again shows the superiority of `#pragma omp atomic` over `#pragma omp critical`. However, **parallelization C** is still better. In addition, it seems not memory-friendly to create a `localDensity` for each thread. Thus, next I will do further acceleration on the basis of **parallelization C**.  
  - **parallelization C(static)**: For the workload in the loop is quite balanced here, using `schedule(static)` is reasonable. I guess it's the best. `BenchmarkSimulation` displays:  
  *Actual number of threads have been setted as: 8  
  Benchmarking Simulation::densityCalculator with 8 threads.  
  Time = 0.230743  
  Info: parallelization C(static), 10000000 numOfParticles with seed 23093556, 101 cells wide the box*  
  - **parallelization C(guided)**: for `schedule(guided)`, `BenchmarkSimulation` displays:  
  *Actual number of threads have been setted as: 8  
  Benchmarking Simulation::densityCalculator with 8 threads.  
  Time = 0.262915  
  Info: parallelization C(guided), 10000000 numOfParticles with seed 23093556, 101 cells wide the box*
  - **parallelization C(dynamic)**: for `schedule(dynamic)`, `BenchmarkSimulation` displays:  
  *Actual number of threads have been setted as: 8  
  Benchmarking Simulation::densityCalculator with 8 threads.  
  Time = 0.441495  
  Info: parallelization C(dynamic), 10000000 numOfParticles with seed 23093556, 101 cells wide the box*  
  - Thus, **parallelization C(static)** is the final choice!  
- `Simulation::potentialCalculator`: I will try the 3 diferent schedules in this method. Because the workload in each itration is really balanced, I guess either `schedule(static)` or `schedule(guided)` will be the best choice. In each following cases, 10 loops will run to estimate the excuting time.  
  - **#pragma omp parallel for**: it displays:  
  *Actual number of threads have been setted as: 8  
  Benchmarking Simulation::potentialCalculator with 8 threads.  
  Time = 2.06726  
  Info: #pragma omp parallel for, 10000000 numOfParticles with seed 23093556, 101 cells wide the box*  
  - **schedule(static)**: it displays:  
*Actual number of threads have been setted as: 8  
Benchmarking Simulation::potentialCalculator with 8 threads.  
Time = 1.91923  
Info: schedule(static), 10000000 numOfParticles with seed 23093556, 101 cells wide the box*  
which is slightly better than plain `#pragma omp parallel for`.  
  - **schedule(guided)**: it displays:  
*Built target TestSimulation  
Actual number of threads have been setted as: 8  
Benchmarking Simulation::potentialCalculator with 8 threads.  
Time = 1.80623  
Info: schedule(guided), 10000000 numOfParticles with seed 23093556, 101 cells wide the box*  
which is better than plain `schedule(static)`.  
  - **schedule(dynamic)**: it displays:  
*Actual number of threads have been setted as: 8  
Benchmarking Simulation::potentialCalculator with 8 threads.  
Time = 2.15968  
Info: schedule(dynamic), 10000000 numOfParticles with seed 23093556, 101 cells wide the box*  
Not good.  
  - thus the best directives will be `#pragma omp parallel for schedule(guided)`  
- `Simulation::accelerationCalculator`: In each following cases, 100 loops will run to estimate the excuting time.  
  - **original**:  
  *Actual number of threads have been setted as: 8  
  Benchmarking Simulation::accelerationCalculator with 8 threads.  
  Time = 0.788532  
  Info: original, 10000000 numOfParticles with seed 23093556, 101 cells wide the box*  
  - **pragma omp parallel for collapse(2)**:  
  *Actual number of threads have been setted as: 8  
  Benchmarking Simulation::accelerationCalculator with 8 threads.  
  Time = 0.429736  
  Info: #pragma omp parallel for collapse(2), 10000000 numOfParticles with seed 23093556, 101 cells wide the box*  
  - **#pragma omp parallel for collapse(3)**:  
  *Actual number of threads have been setted as: 8  
  Benchmarking Simulation::accelerationCalculator with 8 threads.  
  Time = 0.471839  
  Info: #pragma omp parallel for collapse(3), 10000000 numOfParticles with seed 23093556, 101 cells wide the box*  
  - schedule will be adjusted on the basis of the `collapse(2)`
  - **collapse(2) schedule(static)**:  
  *Actual number of threads have been setted as: 8  
  Benchmarking Simulation::accelerationCalculator with 8 threads.  
  Time = 0.503655  
  Info: collapse(2) schedule(static), 10000000 numOfParticles with seed 23093556, 101 cells wide the box*  
  - **collapse(2) schedule(guided)**:  
  *Actual number of threads have been setted as: 8  
  Benchmarking Simulation::accelerationCalculator with 8 threads.  
  Time = 0.442359  
  Info: collapse(2) schedule(guided), 10000000 numOfParticles with seed 23093556, 101 cells wide the box*  
  - **collapse(2) schedule(dynamic)**:  
  *Actual number of threads have been setted as: 8  
  Benchmarking Simulation::accelerationCalculator with 8 threads.  
  Time = 0.390548  
  Info: collapse(2) schedule(dynamic), 10000000 numOfParticles with seed 23093556, 101 cells wide the box*  
  - the best choice is `#pragma omp parallel for collapse(2) schedule(dynamic)  
- `Simulation::particlesUpdater`:  
  - **original**  
  *Actual number of threads have been setted as: 8  
  Benchmarking Simulation::particlesUpdater with 8 threads.  
  Time = 5.04805  
  Info: original, 10000000 numOfParticles with seed 23093556, 101 cells wide the box*
  - **#pragma omp parallel for**  
  *Actual number of threads have been setted as: 8  
  Benchmarking Simulation::particlesUpdater with 8 threads.  
  Time = 0.898555  
  Info: #pragma omp parallel for, 10000000 numOfParticles with seed 23093556, 101 cells wide the box*
  - **schedule(static)**  
  *Actual number of threads have been setted as: 8  
  Benchmarking Simulation::particlesUpdater with 8 threads.  
  Time = 0.965709  
  Info: schedule(static), 10000000 numOfParticles with seed 23093556, 101 cells wide the box*
  - **schedule(guided)**  
  *Actual number of threads have been setted as: 8  
  Benchmarking Simulation::particlesUpdater with 8 threads.  
  Time = 1.10204  
  Info: schedule(guided), 10000000 numOfParticles with seed 23093556, 101 cells wide the box*
  - **schedule(dynamic)**  
  *Actual number of threads have been setted as: 8  
  Benchmarking Simulation::particlesUpdater with 8 threads.  
  Time = 1.46029  
  Info: schedule(dynamic), 10000000 numOfParticles with seed 23093556, 101 cells wide the box*  
  - the best choice would be `#pragma omp parallel for schedule(static)`.
- `Simulation::boxExpander`: also test different schedule here. Because the workload in each itration is really balanced, I guess either `schedule(static)` or `schedule(guided)` will be the best choice. In each following cases, 10 loops will run to estimate the excuting time.  
  - **original**  
  *Actual number of threads have been setted as: 8  
  Benchmarking Simulation::boxExpander with 8 threads.  
  Time = 0.789137  
  Info: original, 10000000 numOfParticles with seed 23093556, 101 cells wide the box*
  - **#pragma omp parallel for**  
  *Actual number of threads have been setted as: 8  
  Benchmarking Simulation::boxExpander with 8 threads.  
  Time = 0.670401  
  Info: #pragma omp parallel for, 10000000 numOfParticles with seed 23093556, 101 cells wide the box*
  - **schedule(static)**  
  *Actual number of threads have been setted as: 8  
  Benchmarking Simulation::boxExpander with 8 threads.  
  Time = 0.619324  
  Info: schedule(static), 10000000 numOfParticles with seed 23093556, 101 cells wide the box*
  - **schedule(guided)**  
  *Actual number of threads have been setted as: 8  
  Benchmarking Simulation::boxExpander with 8 threads.  
  Time = 0.617823  
  Info: schedule(guided), 10000000 numOfParticles with seed 23093556, 101 cells wide the box*
  - **schedule(dynamic)**  
  *Actual number of threads have been setted as: 8  
  Benchmarking Simulation::boxExpander with 8 threads.  
  Time = 2.51554  
  Info: schedule(dynamic), 10000000 numOfParticles with seed 23093556, 101 cells wide the box*  
  - the best choice would be `#pragma omp parallel for schedule(guided)`.  
- below is the the timing of each of the functions for different numbers of threads:  

  Actual number of threads have been set as: 1  
  Benchmarking `Simulation::kSquareUpdater` with 1 threads.  
  Time = **2.18324**  
  Info: **#pragma omp parallel for collapse(2) schedule(dynamic)**, 300 cells wide the box  

  Actual number of threads have been set as: 2  
  Benchmarking `Simulation::kSquareUpdater` with 2 threads.  
  Time = **1.35312**  
  Info: **#pragma omp parallel for collapse(2) schedule(dynamic)**, 300 cells wide the box  

  Actual number of threads have been set as: 4  
  Benchmarking `Simulation::kSquareUpdater` with 4 threads.  
  Time = **0.836639**  
  Info: **#pragma omp parallel for collapse(2) schedule(dynamic)**, 300 cells wide the box  

  Actual number of threads have been set as: 8  
  Benchmarking `Simulation::kSquareUpdater` with 8 threads.  
  Time = **0.755969**  
  Info: **#pragma omp parallel for collapse(2) schedule(dynamic)**, 300 cells wide the box  

  Actual number of threads have been set as: 1  
  Benchmarking `Simulation::densityCalculator` with 1 threads.  
  Time = **1.48534**  
  Info: **parallelization C(static)**, 10000000 numOfParticles with seed 23093556, 101 cells wide the box  

  Actual number of threads have been set as: 2  
  Benchmarking `Simulation::densityCalculator` with 2 threads.  
  Time = **0.746189**  
  Info: **parallelization C(static)**, 10000000 numOfParticles with seed 23093556, 101 cells wide the box  

  Actual number of threads have been set as: 4  
  Benchmarking `Simulation::densityCalculator` with 4 threads.  
  Time = **0.403126**  
  Info: **parallelization C(static)**, 10000000 numOfParticles with seed 23093556, 101 cells wide the box  

  Actual number of threads have been set as: 8  
  Benchmarking `Simulation::densityCalculator` with 8 threads.  
  Time = **0.253533**  
  Info: **parallelization C(static)**, 10000000 numOfParticles with seed 23093556, 101 cells wide the box  

  Actual number of threads have been set as: 1  
  Benchmarking `Simulation::potentialCalculator` with 1 threads.  
  Time = **1.77982**  
  Info: **#pragma omp parallel for schedule(guided)**, 10000000 numOfParticles with seed 23093556, 101 cells wide the box  

  Actual number of threads have been set as: 2  
  Benchmarking `Simulation::potentialCalculator` with 2 threads.  
  Time = **1.81438**  
  Info: **#pragma omp parallel for schedule(guided)**, 10000000 numOfParticles with seed 23093556, 101 cells wide the box  

  Actual number of threads have been set as: 4  
  Benchmarking `Simulation::potentialCalculator` with 4 threads.  
  Time = **1.76488**  
  Info: **#pragma omp parallel for schedule(guided)**, 10000000 numOfParticles with seed 23093556, 101 cells wide the box  

  Actual number of threads have been set as: 8  
  Benchmarking `Simulation::potentialCalculator` with 8 threads.  
  Time = **1.74391**  
  Info: **#pragma omp parallel for schedule(guided)**, 10000000 numOfParticles with seed 23093556, 101 cells wide the box  

  Actual number of threads have been set as: 1  
  Benchmarking `Simulation::accelerationCalculator` with 1 threads.  
  Time = **1.1202**  
  Info: **#pragma omp parallel for collapse(2) schedule(dynamic)**, 10000000 numOfParticles with seed 23093556, 101 cells wide the box  

  Actual number of threads have been set as: 2  
  Benchmarking `Simulation::accelerationCalculator` with 2 threads.  
  Time = **0.695672**  
  Info: **#pragma omp parallel for collapse(2) schedule(dynamic)**, 10000000 numOfParticles with seed 23093556, 101 cells wide the box  

  Actual number of threads have been set as: 4  
  Benchmarking `Simulation::accelerationCalculator` with 4 threads.  
  Time = **0.42809**  
  Info: **#pragma omp parallel for collapse(2) schedule(dynamic)**, 10000000 numOfParticles with seed 23093556, 101 cells wide the box  

  Actual number of threads have been set as: 8  
  Benchmarking `Simulation::accelerationCalculator` with 8 threads.  
  Time = **0.417898**  
  Info: **#pragma omp parallel for collapse(2) schedule(dynamic)**, 10000000 numOfParticles with seed 23093556, 101 cells wide the box  

  Actual number of threads have been set as: 1  
  Benchmarking `Simulation::particlesUpdater` with 1 threads.  
  Time = **4.8406**  
  Info: **#pragma omp parallel for schedule(static)**, 10000000 numOfParticles with seed 23093556, 101 cells wide the box  

  Actual number of threads have been set as: 2  
  Benchmarking `Simulation::particlesUpdater` with 2 threads.  
  Time = **2.56569**  
  Info: **#pragma omp parallel for schedule(static)**, 10000000 numOfParticles with seed 23093556, 101 cells wide the box  

  Actual number of threads have been set as: 4  
  Benchmarking `Simulation::particlesUpdater` with 4 threads.  
  Time = **1.42031**  
  Info: **#pragma omp parallel for schedule(static)**, 10000000 numOfParticles with seed 23093556, 101 cells wide the box  

  Actual number of threads have been set as: 8  
  Benchmarking `Simulation::particlesUpdater` with 8 threads.  
  Time = **0.835074**  
  Info: **#pragma omp parallel for schedule(static)**, 10000000 numOfParticles with seed 23093556, 101 cells wide the box  

  Actual number of threads have been set as: 1  
  Benchmarking `Simulation::boxExpander` with 1 threads.  
  Time = **0.817516**  
  Info: **schedule(guided)**, 10000000 numOfParticles with seed 23093556, 101 cells wide the box  

  Actual number of threads have been set as: 2  
  Benchmarking `Simulation::boxExpander` with 2 threads.  
  Time = **0.707282**  
  Info: **schedule(guided)**, 10000000 numOfParticles with seed 23093556, 101 cells wide the box  

  Actual number of threads have been set as: 4  
  Benchmarking `Simulation::boxExpander` with 4 threads.  
  Time = **0.647681**  
  Info: **schedule(guided)**, 10000000 numOfParticles with seed 23093556, 101 cells wide the box  

  Actual number of threads have been set as: 8  
  Benchmarking `Simulation::boxExpander` with 8 threads.  
  Time = **0.66219**  
  Info: **schedule(guided)**, 10000000 numOfParticles with seed 23093556, 101 cells wide the box  

  - `Simulation::potentialCalculator` looks weird. The executing time is **1.77982**, **1.81438**, **1.76488**, **1.74391**. It may cause by the `FFTW` related issue.  

## Distributed Memory （2.2）

- about `correlationFunction` function:  
  - to make it suit my data sructure, I modified it to let it take a vector of particle `vector<particle> particleInfo`. In which each particle contains a position information. For example, now `position[i][0]` is replaced by `particleInfo[i].position[0]`.  
  - there seems to be an error in the original function, where `i` in outer loop may equal `j` in inner loop, which means it measures the distance between a particle and itself, and it will later cause `inf` in `CR`. To solve it, I modify `for (int j = i; j < N; j += 1)` to `for (int j = i + 1; j < N; j += 1)`.  
- the command line arguments is: `mpirun -np 4 ./build/bin/NBody_Comparison -o nc51-np10-t1_5-dt0_01-process4 -min 1.00 -max 1.06`  
