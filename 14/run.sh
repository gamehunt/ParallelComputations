#!/bin/bash

./build.sh
mpirun -n 4 build/program
