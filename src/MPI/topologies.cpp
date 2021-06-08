/*
 * Copyright (c) 2021 Maksim Masterov, SURF
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

#include <algorithm>
#include <map>

#include "topologies.h"

void Topologies::createCartTopology(IndicesIJ struct_part) {

    int ndims;
    int dims[2];
    int periods[2];
    int reorder;

    /*
     * Define the above variables and create a Cartesian topology
     * Hints:
     *  - use already declared private variable `comm` to store the communicator
     */
    NOT_IMPLEMENTED
}

void Topologies::createGraphTopology(DecompositionMetis& decomp_metis, int root_pid) {

    std::vector<int> loc_map_of_ngb;
    int reorder = 0;

    /* Distribute the graph across all processes (at this point,
     * it is only stored by the root process) */
    loc_map_of_ngb = distributeGraph(decomp_metis, root_pid);

    /*
     * Create a distributed adjacent graph topology
     * Hints:
     *  - create bidirected graph, i.e. sources == destinations
     *  - use already declared private variable `comm` to store the communicator
     *  - get number of neighbors: loc_map_of_ngb.size()
     *  - get a list of neighbors: loc_map_of_ngb.data()
     */
    NOT_IMPLEMENTED
}

void Topologies::testCartTopology() {

    /*
     * Get coordinates of the process by rank
     * Get rank of the process by the coordinates
     * Perform shift and test what processes are visible to the current one
     */
    NOT_IMPLEMENTED
}

void Topologies::testGraphTopology() {

    /*
     * Find the number of neighboring processes
     * Find the list of neighboring processes
     */
    NOT_IMPLEMENTED
}

std::vector<int> Topologies::distributeGraph(DecompositionMetis& decomp_metis, int root_pid) {

    /* Note that at this point the map of all processes is stored by the root process only */
    std::map<int32_t, std::vector<int32_t> > *map_of_procs = &decomp_metis.getMapOfProcs();
    std::vector<int> loc_neighb;
    int my_rank = getMyRank();
    int num_procs = getNumProcs();
    MPI_Status status;

    /* Iterate over the map and distribute information about the neighboring PIDs across all processes */
    if (num_procs > 1) {
        /* Broadcast the map from the root process */
        int tag = 159;
        if (my_rank == root_pid) {
            loc_neighb.resize(map_of_procs->at(my_rank).size());
            loc_neighb = map_of_procs->at(my_rank);

            for (int pid = 0; pid < num_procs; ++pid) {
                if (pid == root_pid)
                    continue;
                MPI_Send(map_of_procs->at(pid).data(), map_of_procs->at(pid).size(), MPI_INT, pid, tag, MPI_COMM_WORLD);
            }
        } else {
            int recv_size;
            /* Check the message */
            MPI_Probe(root_pid,
                      tag,
                      MPI_COMM_WORLD,
                      &status);

            /* Get the message size */
            MPI_Get_count(&status,
                          MPI_INT,
                          &recv_size);

            /* Allocate memory */
            loc_neighb.resize(recv_size);

            MPI_Recv(loc_neighb.data(), loc_neighb.size(), MPI_INT, root_pid, tag, MPI_COMM_WORLD, &status);
        }
    }

    /* Uncomment to print the received info process-by-process */
//    for (int n = 0; n < num_procs; ++n) {
//        if (n == my_rank) {
//            std::cout << n << " : ";
//            for (int m = 0; m < loc_neighb.size(); ++m) {
//                std::cout << loc_neighb[m] << " ";
//            }
//            std::cout << "\n";
//        }
//        MPI_Barrier(MPI_COMM_WORLD);
//    }

    return loc_neighb;
}