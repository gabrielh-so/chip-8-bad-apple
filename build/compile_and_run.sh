#!/bin/bash

echo "regenerating make files"
cmake ..
echo "using make files"
make
echo "running program"
echo ""
echo "~~~~~~~~~~~~~~~"
echo ""
./chip-8
