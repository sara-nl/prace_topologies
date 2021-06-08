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

#include <iostream>
#include <iomanip>

#include "graph.h"

void Graph::generateStructured(IndicesIJ size) {

    int estimated_nnz;
    int counter;
    int tmp;

    num_rows = size.i * size.j;
    num_cols = num_rows;

    estimated_nnz = num_rows                        // diagonal
                    + 2 * (num_rows - size.j)       // off-diagonal
                    + 2 * size.i * (size.j - 1);    // "far" off-diagonal

    if (g_type == ADJ_MATRIX) {
        nodes.resize(estimated_nnz);
        columns.resize(estimated_nnz);
    }
    else {
        // we do not need diagonal elements in the adjacency list
        estimated_nnz = estimated_nnz - num_rows;
        nodes.resize(estimated_nnz);
    }
    offsets.resize(num_rows + 1);

    counter = 0;
    for(int32_t row = 0; row < num_rows; ++row) {

        offsets[row] = counter;

        if (row >= size.j && row < num_rows) {
            tmp = row - size.j;
            if (g_type == ADJ_MATRIX) {
                nodes[counter] = 1;
                columns[counter] = tmp;
            }
            else {
                nodes[counter] = tmp;
            }
            ++counter;
        }

        if ( row % size.j ) {
            tmp = row - 1;
            if (g_type == ADJ_MATRIX) {
                nodes[counter] = 1;
                columns[counter] = tmp;
            }
            else {
                nodes[counter] = tmp;
            }
            ++counter;
        }

        if (g_type == ADJ_MATRIX) {
            nodes[counter] = 1;
            columns[counter] = row;
            ++counter;
        }

        if ( (row+1) % size.j ) {
            tmp = row + 1;
            if (g_type == ADJ_MATRIX) {
                nodes[counter] = 1;
                columns[counter] = tmp;
            }
            else {
                nodes[counter] = tmp;
            }
            ++counter;
        }

        if (row >= 0 && row < (num_rows - size.j)) {
            tmp = row + size.j;
            if (g_type == ADJ_MATRIX) {
                nodes[counter] = 1;
                columns[counter] = row + size.j;
            }
            else {
                nodes[counter] = row + size.j;
            }
            ++counter;
        }
    }

    offsets[num_rows] = estimated_nnz;
}

void Graph::print() {

    if (nodes.empty()) {
        std::cout << "Warning! The graph is empty...";
    }
    else {
        std::cout << "\n";
        std::cout << "offsets:\n";
        for (int32_t row = 0; row < offsets.size(); ++row) {
            std::cout << offsets[row] << " ";
        }
        std::cout << "\n\n";

        std::cout << "data:\n";
        // Print columns numbers
        if (g_type == ADJ_MATRIX) {
            std::cout << "     ";
            for (int32_t col = 0; col < getCols(); ++col) {
                std::cout << std::setw(3) << col;
            }
            std::cout << "\n";
            std::cout << "     ";
            for (int32_t col = 0; col < getCols(); ++col) {
                std::cout << std::setw(3) << "-";
            }
            std::cout << "\n";
        }

        for (int32_t row = 0; row < getRows(); ++row) {
            std::cout << std::setw(3) << row << ": ";
            if (g_type == ADJ_MATRIX) {
                for (int32_t col = 0; col < getCols(); ++col) {
                    bool match = false;
                    for (int32_t ckey = offsets[row]; ckey < offsets[row + 1]; ++ckey) {
                        if (col == columns[ckey]) {
                            std::cout << std::setw(3) << nodes[ckey];
                            match = true;
                            break;
                        }
                    }
                    if (!match)
                        std::cout << std::setw(3) << 0;
                }
            }
            else {
                for (int32_t ckey = offsets[row]; ckey < offsets[row + 1]; ++ckey) {
                    std::cout << nodes[ckey] << " ";
                }
            }
            std::cout << "\n";
        }
        std::cout << "\n";
    }
}