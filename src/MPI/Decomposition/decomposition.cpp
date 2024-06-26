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

/*!
 * \file decomposition.h
 * @brief Contains definition of methods from the class \e DecompositionStruct.
 */
#include <iostream>
#include <fstream>
#include <cmath>

#include "decomposition.h"

void DecompositionStruct::getProcCoord(int rank, int &proc_ind_i, int &proc_ind_j) {

    /*
     * Note, according to the standard, C and C++ always round down results of
     * the integer division!
     */
    proc_ind_i = rank / num_subdomains.j;               // index of the process
                                                        // in i-th direction
    proc_ind_j = rank - proc_ind_i * num_subdomains.j;  // index of the process
                                                        // in j-th direction
}

//int DecompositionStruct::getProcInd(int proc_ind_i, int proc_ind_j) {
//
//    return proc_ind_j + proc_ind_i * num_subdomains.j;
//}

int DecompositionStruct::decompose(const IndicesIJ num_procs, const IndicesIJ elts_glob) {

    int num_procs_avail = getNumProcs();

    part.resize(elts_glob.i * elts_glob.j);

    /* Check if number of processes correspond to the decomposition size. */
    if (num_procs_avail != num_procs.i * num_procs.j) {
        printByRoot("The specified number of processes doesn't "
                    "match the available number of processes: "
                    + std::to_string(num_procs.i * num_procs.j)
                    + " vs. "
                    + std::to_string(num_procs_avail));
        /* Note, all processes should exit the function! */
        return EXIT_FAILURE;
    }

    /* Assign number of subdomains to local variables. */
    num_subdomains.i = num_procs.i;
    num_subdomains.j = num_procs.j;

    /*
     * Assume that all processes are enumerated in the "natural" order. For a 2d
     * decomposition among 9 processes the enumeration will look like:
     *   2 5 8
     *   1 4 7
     *   0 3 6
     * We need to ensure that processes at the most right columns and the most top
     * row have correct number of elements. This is important in situations, when
     * the total number of elements in i-th or j-th directions are not divisible
     * by the number of specified processes in the same direction. For instance,
     * we have 3x3 decomposition (see above). Let's apply it to the domain of
     * 10x10 elements. Thus, processes 0,1,3,4 should have 3x3 elements each,
     * while processes 2 and 5 should have 3x4 elements, processes 6 and 7 should
     * have 4x3 elements, and process 8 should have 4x4 elements. Summing up, all
     * this processes will result in total number of 100 elements.
     */

    for (int pid = 0; pid < getNumProcs(); ++pid) {
        int proc_ind_i = 0;     // index of the process in i-th direction.
        int proc_ind_j = 0;     // index of the process in j-th direction.
        IndicesIJ beg_ind_glob;
        IndicesIJ end_ind_glob;
        IndicesIJ elts_loc;

        /* Get process "coordinates". Note: my_rank = proc_ind_j + proc_ind_i * nj. */
        getProcCoord(pid, proc_ind_i, proc_ind_j);

        /* Assign local dimensions */
        elts_loc.i = floor(elts_glob.i / num_subdomains.i);
        elts_loc.j = floor(elts_glob.j / num_subdomains.j);

        /*
         * Get the global indices that correspond to the very first (bottom/left) cells
         * of the current sub-domain.
         */
        beg_ind_glob.i = proc_ind_i * elts_loc.i;
        beg_ind_glob.j = proc_ind_j * elts_loc.j;

        /* Correct the number of cells for the most top/right sub-domains */
        if ((elts_glob.i % num_subdomains.i) && (proc_ind_i == num_subdomains.i - 1)) {
            int remainder = elts_glob.i - elts_loc.i * (proc_ind_i + 1);
            elts_loc.i += remainder;
        }

        if ((elts_glob.j % num_subdomains.j) && (proc_ind_j == num_subdomains.j - 1)) {
            int remainder = elts_glob.j - elts_loc.j * (proc_ind_j + 1);
            elts_loc.j += remainder;
        }

        /*
         * Get the global indices that correspond to the very last (top/right) cells
         * of the current sub-domain.
         */
        if (proc_ind_i + 1 < num_subdomains.i) {
            end_ind_glob.i = (proc_ind_i + 1) * elts_loc.i;
        }
        else {
            end_ind_glob.i = elts_glob.i;
        }
        if (proc_ind_j + 1 < num_subdomains.i) {
            end_ind_glob.j = (proc_ind_j + 1) * elts_loc.j;
        }
        else {
            end_ind_glob.j = elts_glob.j;
        }

        /* Fill in the partition array */
        for (int i = beg_ind_glob.i; i < end_ind_glob.i; ++i) {
            for (int j = beg_ind_glob.j; j < end_ind_glob.j; ++j) {
                part[j + i * elts_glob.j] = pid;
            }
        }


    }

    return EXIT_SUCCESS;
}

void DecompositionStruct::print(const std::string file_name, const IndicesIJ elts_glob) {

    if (!part.empty()) {
        std::ofstream out_str;
        int32_t id = 0;

        out_str.open(file_name, std::ios::out);

        if (out_str.is_open()) {
            out_str << "\n";
            for (int32_t j = 0; j < elts_glob.j; ++j) {
                for (int32_t i = 0; i < elts_glob.i; ++i) {
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
