/*
 * Copyright (c) 2024 Maksim Masterov, SURF
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <string>
#include <metis.h>
#include <cmath>

#include "src/field.h"
#include "src/helpers.h"
#include "src/common.h"
#include "src/MPI/Decomposition/decomposition.h"
#include "src/MPI/Decomposition/decompositionMetis.h"
#include "src//MPI/topologies.h"
#include "src/graph.h"

void reportElapsedTime(double start, double end, const std::string &message) {
    findGlobalMin(start);
    findGlobalMax(end);
    printByRoot("Elapsed time (" + message + "): " + std::to_string(end - start) + "s.");
}

int main(int argc, char** argv) {

    Helpers helper;
    Field field;
    IndicesIJ elts_glob;        // Number of global cells in each direction
    IndicesIJ struct_part;      // Number of processes in each direction (for structured decomposition only)
    DecompositionStruct decomp_struct;
    DecompositionMetis decomp_metis;
    Graph graph(ADJ_LIST);
    int8_t type = STRUCTURED;
    int root_pid = 0;
    int32_t* partitioning;
    int32_t num_glob_elts;
    double elp_times[2];
    Topologies topology;
    double res = 0.;

    /* Initialize MPI region */
    initialize(argc, argv);

    /* Parse input from the CL */
    helper.parseInput(argc, argv, elts_glob, struct_part, type);
    num_glob_elts = elts_glob.i * elts_glob.j;

    /* Generate initial field and decompose the data by the root process */
    if (getMyRank() == root_pid) {
        field.initialize(elts_glob, elts_glob);
        field.generate();

        /* Print field to the file */
        field.print("original");

        if (type == STRUCTURED) {
            /* Call for structured decomposition */
            if (decomp_struct.decompose(struct_part, elts_glob) == EXIT_FAILURE) {
                terminateExecution();
            }

            /* Print structured decomposition to the file */
            decomp_struct.print("struct.dat", elts_glob);

            partitioning = decomp_struct.getPartitioning().data();
        } else if (type == METIS) {
            graph.generateStructured(elts_glob);

            /* Uncomment this line to print the graph to the terminal */
            // graph.print();

            std::vector<int32_t> weights;
            weights.resize(graph.getRows());
            for (int32_t n = 0; n < weights.size(); ++n) {
                /* Uncomment this line to change the weight distribution */
                // weights[n] = 100 * field(n);
                weights[n] = field.getLocalLoad(field(n));
            }

            /* Call for graph decomposition */
            if (decomp_metis.decompose(graph, weights.data()) == EXIT_FAILURE) {
                terminateExecution();
            }

            /* Print graph decomposition to the file */
            decomp_metis.print("graph.dat", elts_glob);

            partitioning = decomp_metis.getPartitioning().data();
        }
        else {
            printByRoot("Unknown decomposition type");
            terminateExecution();
        }
    }

    /* Distribute the field */
    field.distribute(partitioning, num_glob_elts, root_pid);

    /* Print local field for debugging */
    field.print("output");

    /* Perform some calculations and report the elapsed time */
    elp_times[0] = helper.tic();
    res = field.performDummyWork();
    elp_times[1] = helper.toc();

    /* Print result for the verification */
    findGlobalSum(res);
    printByRoot("result: " + std::to_string(res));
    reportElapsedTime(elp_times[0], elp_times[1], "Work");

    /* Finalize MPI region */
    finalize();

    return 0;
}
