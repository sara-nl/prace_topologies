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

#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>

#include "field.h"
#include "common.h"

Field::Field() { }

Field::~Field() {

    data.clear();
    _elts_loc.i = _elts_loc.j = 0;
    _elts_glob.i = _elts_glob.j = 0;
}

void Field::initialize(IndicesIJ elts_loc, IndicesIJ elts_glob) {

    _elts_loc = elts_loc;
    _elts_glob = elts_glob;

    data.clear();
    data.resize(_elts_loc.i * _elts_loc.j);
}

void Field::generate() {

    if (data.empty()) {
        std::cerr << "Error! Memory was not allocated..." << std::endl;
        return;
    }

    for (int i = 0; i < _elts_loc.i; ++i) {
        for (int j = 0; j < _elts_loc.j; ++j) {
            int id_glob = i * j; //getLocalID(i, j);
            int max_elts = _elts_glob.i * _elts_glob.j;
            this->operator()(i, j) = log(id_glob + 1.) / log (max_elts + 1.);// * pow(id_glob, 2.);
        }
    }
}

void Field::print(std::string base_name) {

    std::ofstream out_str;

    out_str.open(base_name + "_" + std::to_string(getMyRank()) + ".dat", std::ios::out);

    if (out_str.is_open()) {
        for (int32_t i = 0; i < _elts_loc.i; ++i) {
            for (int32_t j = 0; j < _elts_loc.j; ++j) {
                out_str << this->operator()(i, j) << " ";
            }
            out_str << "\n";
        }

        out_str.close();
    }
    else {
        std::cerr << "Error! Can't open the output file..." << std::endl;
    }
}

double Field::performDummyWork() {

    double result = 0.;
    for(int n = 0; n < data.size(); ++n) {
        double value = getLocalLoad(data[n]);
        for (int m = 0; m < (int) value; ++m) {
            result += (std::log(value) + std::cos(value)) / std::exp(value);
        }
    }
    return result;
}

void Field::distribute(int32_t* partitioning, int num_glob_elts, int root_pid) {

    int num_procs = getNumProcs();
    int tag_field = 9999;
    MPI_Request request_arr[num_procs];
    std::vector<Field> snd_field(num_procs);
    std::vector<int> num_elts(num_procs);

    // Sync all processes first
    MPI_Barrier(MPI_COMM_WORLD);

    if (getMyRank() == root_pid) {

        // Count number of elements in each partition
        for (int n = 0; n < num_procs; ++n) {
            num_elts[n] = std::count(partitioning,partitioning + num_glob_elts, n);
        }

        // Assemble the buffers
        for (int n = 0; n < num_procs; ++n) {
            IndicesIJ num_elts_loc = {num_elts[n], 1};
            int counter = 0;

            snd_field[n].initialize(num_elts_loc, num_elts_loc);

            for (int m = 0; m < num_glob_elts; ++m) {
                if (partitioning[m] == n) {
                    snd_field[n](counter) = data[m];
                    ++counter;
                    if (counter == num_elts[n])
                        break;
                }
            }

            // Send the field
            MPI_Isend(snd_field[n].getData().data(), num_elts[n],
                      MPI_DOUBLE, n, tag_field, MPI_COMM_WORLD, &request_arr[n]);
        }
    }

    // Receive data by all processes (including the root process)
    // Count the message size first
    MPI_Status status;
    MPI_Status status_arr[num_procs];
    int msg_size = 0;
    MPI_Probe(root_pid, tag_field, MPI_COMM_WORLD, &status);
    MPI_Get_count(&status, MPI_DOUBLE, &msg_size);

    // Resize the receiving buffer
    IndicesIJ num_elts_loc = {msg_size, 1};
    initialize(num_elts_loc, num_elts_loc);

    // Receive the message
    MPI_Recv(data.data(), msg_size, MPI_DOUBLE, root_pid,
             tag_field, MPI_COMM_WORLD, &status);

    // Wait for finalized delivery
    if (getMyRank() == root_pid) {
        MPI_Waitall(num_procs, request_arr, status_arr);
    }

    MPI_Barrier(MPI_COMM_WORLD);
}