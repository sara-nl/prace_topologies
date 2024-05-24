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

#ifndef UNBALANCED_WORKLOAD_TOPOLOGIES_H
#define UNBALANCED_WORKLOAD_TOPOLOGIES_H

#include "../common.h"
#include "../General/structs.h"
#include "../graph.h"
#include "Decomposition/decompositionMetis.h"

class Topologies {
public:
    Topologies() : comm(MPI_COMM_WORLD) { }
    ~Topologies() { comm = MPI_COMM_WORLD; }

    /*!
     * @brief Create Cartesian topology.
     * @param struct_part Number of sub-domains in each direction
     */
    void createCartTopology(IndicesIJ struct_part);

    /*!
     * @brief Create distributed graph topology.
     */
    void createGraphTopology(DecompositionMetis& decomp_metis, int root_pid);

    /*!
     * @brief A simple test for the Cartesian topology.
     */
    void testCartTopology();

    /*!
     * @brief A simple test for the distributed graph topology.
     */
    void testGraphTopology();

private:
    /*!
     * @brief Distribute the graph of processes interconnections.
     * @param decomp_metis Object of decomposition performed with METIS
     * @param root_pid ID of the root process that stores \e decomp_metis object
     * @return A vector of neighboring processes
     */
    std::vector<int> distributeGraph(DecompositionMetis& decomp_metis, int root_pid);

private:
    MPI_Comm comm;
};


#endif //UNBALANCED_WORKLOAD_TOPOLOGIES_H
