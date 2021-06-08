#!/bin/bash

compiler='mpicxx'
extra_flags=(-lmpi -lmetis)
exe_name=topologies

$compiler \
    -g3 -O3 --std=c++11 \
    "${extra_flags[@]}" \
    -o $exe_name \
    main.cpp \
    src/field.cpp \
    src/helpers.cpp \
    src/MPI/Decomposition/decomposition.cpp \
    src/graph.cpp \
    src/MPI/Decomposition/decompositionMetis.cpp \
    src/MPI/topologies.cpp
