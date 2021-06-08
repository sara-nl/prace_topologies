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

#ifndef UNBALANCED_WORKLOAD_COMMON_H
#define UNBALANCED_WORKLOAD_COMMON_H

#include <iostream>

#define USE_MPI

#ifdef USE_MPI
#include <mpi.h>
#endif

inline void findGlobalMin(double &value) {

#ifdef USE_MPI
    MPI_Allreduce(MPI_IN_PLACE, &value, 1, MPI_DOUBLE, MPI_MIN, MPI_COMM_WORLD);
#endif
}

inline void findGlobalMax(double &value) {

#ifdef USE_MPI
    MPI_Allreduce(MPI_IN_PLACE, &value, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
#endif
}

inline int getMyRank() {

    int my_rank = 0;
#ifdef USE_MPI
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
#endif
    return my_rank;
}

inline int getNumProcs() {

    int num_procs = 1;
#ifdef USE_MPI
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
#endif
    return num_procs;
}

inline void findGlobalSum(double &value) {
#ifdef USE_MPI
    MPI_Allreduce(MPI_IN_PLACE, &value, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
#endif
}

inline void findGlobalSum(int &value) {
#ifdef USE_MPI
    MPI_Allreduce(MPI_IN_PLACE, &value, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
#endif
}

inline void initialize(int argc, char** argv) {
#ifdef USE_MPI
    MPI_Init(&argc, &argv);
#endif
}

inline void finalize() {
#ifdef USE_MPI
    MPI_Finalize();
#endif
}

inline void terminateExecution() {
#ifdef USE_MPI
    MPI_Abort(MPI_COMM_WORLD, 1);
#else
    exit(1);
#endif
}

inline void printByRoot(const std::string& str) {

    if (getMyRank() == 0) {
        std::cout << str << "\n";
    }
}


#endif //UNBALANCED_WORKLOAD_COMMON_H
