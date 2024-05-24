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

#include <sys/time.h>

#include "helpers.h"
#include "common.h"

double Helpers::tic() {

    double current_time = 0.0;
#ifdef USE_MPI
    /* Always synchronize the processes before starting the timer. */
    MPI_Barrier(MPI_COMM_WORLD);
    current_time = MPI_Wtime();
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    current_time=tv.tv_sec;
#endif

    return current_time;
}

double Helpers::toc() {

    double current_time = 0.0;
#ifdef USE_MPI
    /* We do not need to sync the processes when the timer is stopped because we are
     * interested in which process reached this point last. */
    current_time = MPI_Wtime();
    MPI_Barrier(MPI_COMM_WORLD);
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    current_time=tv.tv_sec;
#endif

    return current_time;
}

void Helpers::terminateDueToParserFailure() {
    printByRoot("\nError! Incorrect arguments were passed to the command line.\n"
                "Use the following keys:\n"
                "  -s - set number of the grid cells in each direction (i j)\n"
                "  -d - set decomposition for each direction (i j)\n"
                "       (doesn’t affect the METIS decomposition, but should be\n"
                "        set anyway!)\n"
                "  -t - set decomposition type ('m' for METIS, 's' for STRUCTURED)\n"
                "Example:\n"
                "  ./a.out -s 10 10 -d 1 1 -t m");
    terminateExecution();
}

void Helpers::parseInput(int argc, char** argv, IndicesIJ &elts_glob, IndicesIJ &num_procs, int8_t &type) {

    /* Assign the default values first. */
    elts_glob.i = elts_glob.j = 10;
    num_procs.i = num_procs.j = 1;
    type = STRUCTURED;

    elts_glob.i = 3;
    elts_glob.j = 5;

    if (argc > 1 && argc != 9) {
        terminateDueToParserFailure();
    }
    else if (argc == 9) {
        int found_keys = 0;

        for (int pos = 1; pos < argc; ++pos) {
            if (std::string(argv[pos]) == "-s") {
                elts_glob.i = atoi(argv[pos + 1]);
                elts_glob.j = atoi(argv[pos + 2]);
                ++found_keys;
                pos += 2;
            }
            else if (std::string(argv[pos]) == "-d") {
                num_procs.i = atoi(argv[pos + 1]);
                num_procs.j = atoi(argv[pos + 2]);
                ++found_keys;
                pos += 2;
            }
            else if (std::string(argv[pos]) == "-t") {
                if (std::string(argv[pos + 1]) == "m")
                    type = METIS;
                else if (std::string(argv[pos + 1]) == "s")
                    type = STRUCTURED;
                ++found_keys;
                ++pos;
            }
        }

        if (found_keys != 3)
            terminateDueToParserFailure();
    }
}
