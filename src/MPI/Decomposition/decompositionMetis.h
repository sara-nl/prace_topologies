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

#ifndef UNBALANCED_WORKLOAD_DECOMPOSITIONMETIS_H
#define UNBALANCED_WORKLOAD_DECOMPOSITIONMETIS_H

#include <metis.h>
#include <map>
#include <vector>

#include "../../graph.h"
#include "../../field.h"

class DecompositionMetis {
public:
    DecompositionMetis() { }

    ~DecompositionMetis() { part.clear(); }

    int decompose(Graph& graph, int32_t* weights);

    void print(const std::string file_name, const IndicesIJ elts_glob);

    inline std::vector<int32_t>& getPartitioning() {
        return part;
    }

    inline std::map<int32_t, std::vector<int32_t> >& getMapOfProcs() {
        return map_of_procs;
    }

private:
    void assembleMapOfProcesses(Graph &graph);

private:
    std::map<int32_t, std::vector<int32_t> > map_of_procs;
    std::vector<int32_t> part;    // partitions
};


#endif //UNBALANCED_WORKLOAD_DECOMPOSITIONMETIS_H
