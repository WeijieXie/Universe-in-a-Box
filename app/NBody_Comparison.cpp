#include <mpi.h>
#include <fstream>
#include "Utils.hpp"
#include "Simulation.hpp"

int main(int argc, char **argv)
{
    MPI_Init(NULL, NULL);
    int process_id;
    MPI_Comm_rank(MPI_COMM_WORLD, &process_id);
    int num_proc;
    MPI_Comm_size(MPI_COMM_WORLD, &num_proc);

    std::string outputFolder;
    double minExpansionFactor = 0.0;
    double maxExpansionFactor = 0.0;

    // in order to reduce sending messages, all the processes read from the command line
    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];
        if (arg == "-h")
        {
            std::cout << "Usage: NBody_Comparison [options]\n"
                      << "Options:\n"
                      << "  -h                              Show this help message\n"
                      << "  -o                              Specify the output folder path\n"
                      << "  -min                            Specify the minimum expansion factor\n"
                      << "  -max                            Specify the maximum expansion factor\n";
            return 0;
        }
        else if (arg == "-o" && i + 1 < argc)
        {
            outputFolder = argv[++i];
        }
        else if (arg == "-min" && i + 1 < argc)
        {
            minExpansionFactor = std::stod(argv[++i]);
        }
        else if (arg == "-max" && i + 1 < argc)
        {
            maxExpansionFactor = std::stod(argv[++i]);
        }
        else
        {
            std::cerr << "Unknown option: " << arg << std::endl;
            return 1;
        }
    }

    double ExpansionFactor = minExpansionFactor + process_id * (maxExpansionFactor - minExpansionFactor) / (num_proc - 1); // set the expanFac depending on the process_id
    int numParticles = 1326510;                                                                                            // 51*51*51*10
    particle::massSetter(100000.0 / numParticles);
    Simulation simulation(1.50, 0.01, particles(numParticles, 23093556), 100.0, 51, ExpansionFactor);

    simulation.run();
    vector<double> corrInfo = correlationFunction(simulation.particlesSimu.particleInfo, 100); // calculate the correlation
    int corrInfoSize = corrInfo.size();                                                        // in this case, the size is 100

    // printf("Message from Process %d, with ExpansionFactor %f, corrInfo[0] %f\n", process_id, ExpansionFactor, corrInfo.at(0));

    if (process_id == 0)
    {
        double masterArr[num_proc * corrInfoSize];              // create the master array to store the corrInfo form different processes
        std::copy(corrInfo.begin(), corrInfo.end(), masterArr); // store corrInfo of main process itself first
        int seq[num_proc];                                      // record the arrival sequence of the corrInfo from different processes
        seq[0] = 0;                                             // the first one is itself(process_id is 0)

        // recieve all the corrInfo from other processes
        for (int i = 1; i < num_proc; i++)
        {
            double *masterArrPtr = masterArr + i * corrInfoSize; // get the pointer for corrInfo
            MPI_Status status;                                   // get the process_id of the sender
            MPI_Recv(masterArrPtr,
                     corrInfoSize,
                     MPI_DOUBLE,
                     MPI_ANY_SOURCE,
                     MPI_ANY_TAG,
                     MPI_COMM_WORLD,
                     &status);
            seq[i] = status.MPI_SOURCE; // store process_id of the sender in seq
            // printf("No. %d data is from process %d\n", i, seq[i]);
        }
        // printf("%f,%f,%f,%f\n", masterArr[0], masterArr[1 * corrInfoSize], masterArr[2 * corrInfoSize], masterArr[3 * corrInfoSize]);

        std::filesystem::path path("Correlations");

        // create "Correlations" folder if not exists
        if (!std::filesystem::exists(path))
        {
            std::filesystem::create_directories("Correlations");
        }

        path /= outputFolder + ".csv";
        // std::cout << path << std::endl;

        std::ofstream outFile(path);

        // write the expanFac to the first line
        for (int i = 0; i < num_proc; ++i)
        {
            double firstLineValue = minExpansionFactor + seq[i] * (maxExpansionFactor - minExpansionFactor) / (num_proc - 1); // use the arrive sequence of process_id to get the expanFac
            outFile << firstLineValue;
            if (i < num_proc - 1)
                outFile << ",";
        }
        outFile << "\n";

        // wrire data into the csv file
        for (int row = 0; row < corrInfoSize; ++row)
        {
            for (int col = 0; col < num_proc; ++col)
            {
                int index = col * corrInfoSize + row; // calculate the index of the element in masterArr
                outFile << masterArr[index];
                if (col < num_proc - 1)
                    outFile << ",";
            }
            outFile << "\n";
        }
        outFile.close();
    }
    else
    {
        double *buffer = corrInfo.data();

        // send the corrInfo to the main process(process_id=0)
        MPI_Send(buffer,
                 corrInfoSize,
                 MPI_DOUBLE,
                 0,
                 0,
                 MPI_COMM_WORLD);
        // printf("process %d has sent out the data...\n", process_id);
    }

    MPI_Finalize();
    return 0;
}
