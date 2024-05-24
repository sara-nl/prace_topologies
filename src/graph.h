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

#ifndef UNBALANCED_WORKLOAD_GRAPH_H
#define UNBALANCED_WORKLOAD_GRAPH_H

#include <vector>

#include "General/structs.h"

/*!
 * @brief Represents graph in a form of adjacency matrix.
 * This class uses CSR format to store the graph.
 */
class Graph {
public:

    Graph(int8_t type) {

        g_type = type;
    }

    ~Graph() {

        nodes.clear();
        columns.clear();
        offsets.clear();
    }

    void generateStructured(IndicesIJ size);

    void print();

    inline int32_t getRows() {
        return num_rows;
    }

    inline int32_t getCols() {
        return num_cols;
    }

    inline std::vector<int32_t>& getNodes() {
        return nodes;
    }

    inline std::vector<int32_t>& getColumns() {
        return columns;
    }

    inline std::vector<int32_t>& getOffsets() {
        return offsets;
    }

private:
    int8_t g_type;                  // Storage type: adjacency matrix or list
    int32_t num_rows;               // Number of rows in the graph
    int32_t num_cols;               // Number of columns in the graph
    std::vector<int32_t> nodes;     // Nodes value
    std::vector<int32_t> columns;   // Column indices
    std::vector<int32_t> offsets;   // Index offsets for rows
};

#endif //UNBALANCED_WORKLOAD_GRAPH_H
