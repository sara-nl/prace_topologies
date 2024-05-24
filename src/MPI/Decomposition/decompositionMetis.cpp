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

#include <algorithm>
#include <iostream>
#include <fstream>

#include "decompositionMetis.h"
#include "../../common.h"

int DecompositionMetis::decompose(Graph &graph, int32_t* weights) {

    /* idx_t is a typedef of int32_t used by METIS */
    idx_t ncon;
    idx_t nparts;
    idx_t edgecut;
    idx_t nvtxs;
    idx_t *xadj;
    idx_t *adjncy;
    int error = METIS_ERROR;

    /*
     * Hints
     * Step1: 
     *  Get data:
     *   - get the number of nodes in the graph:        graph.getRows()
     *   - get the raw pointer to the array of offsets: graph.getOffsets().data()
     *   - get the raw pointer to the adjacency list:   graph.getNodes().data()
     *   - get the raw pointer to the partitioning:     part.data()
     *   - get the number of processes:                 getNumprocs()
     *
     * Step 2:
     *   Execute partitioning:
     *    - keep the number of balancing constraints equal to 1
     *    - note that `nvtxs` has to be defined
     */
    NOT_IMPLEMENTED

    part.resize(nvtxs);

    if (getNumProcs() == 1) {
        std::cout << "Warning! Serial execution, nothing will be done...\n";

        for (int n = 0; n < nvtxs; ++n) {
            part[n] = 0;
        }
        return EXIT_SUCCESS;
    }

    // error = METIS_PartGraphKway(...);
    // Keep the following arguments NULL: 
    // - the number of balancing constraints is 1
    // - the size of the vertices for computing the total communication volume
    // - the weights of the edges
    // - desired weight for each partition and constraint
    // - the allowed load imbalance tolerance for each constraint
    // - an array of options
    NOT_IMPLEMENTED

    if (error == METIS_OK) {
        assembleMapOfProcesses(graph);
        std::cout << "The graph has been successfully partitioned...\n";
        return EXIT_SUCCESS;
    }
    else {
        std::cout << "Warning! An error code (" << error << ") has been returned "
                     "by the partitioning algorithm...\n";
        return EXIT_FAILURE;
    }

}

void  DecompositionMetis::assembleMapOfProcesses(Graph &graph) {

    // Traverse through the partitioned domain and check for neighbors with a different PIDs
    for (int32_t row = 0; row < graph.getRows(); ++row) {
        int current_rank = part[row];
        for (int32_t ckey = graph.getOffsets()[row]; ckey < graph.getOffsets()[row + 1]; ++ckey) {
            int node = graph.getNodes()[ckey];
            int ngb_rank = part[node];
            if (ngb_rank != current_rank) {
                map_of_procs[current_rank].push_back(ngb_rank);
            }
        }
    }

    // Remove all duplicates
    for (auto it = map_of_procs.begin(); it != map_of_procs.end(); ++it) {
        int32_t id = it->first;
        std::sort(map_of_procs[id].begin(), map_of_procs[id].end());
        map_of_procs[id].erase(std::unique(map_of_procs[id].begin(),
                                                map_of_procs[id].end()),
                               map_of_procs[id].end());
    }
}

void DecompositionMetis::print(const std::string file_name, const IndicesIJ elts_glob) {

    if (!part.empty()) {
        std::ofstream out_str;
        int32_t id = 0;

        out_str.open(file_name, std::ios::out);

        if (out_str.is_open()) {
            out_str << "\n";
            for (int32_t i = 0; i < elts_glob.i; ++i) {
                for (int32_t j = 0; j < elts_glob.j; ++j) {
                    id = j + elts_glob.j * i;
                    out_str << part[id] << " ";
                }
                out_str << "\n";
            }
            out_str << "\n";
        }

        out_str.close();
    }
}
