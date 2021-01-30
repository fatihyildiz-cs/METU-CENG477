#!/bin/bash
echo "make command:"
make
echo "black screen blocker:"
export MESA_GL_VERSION_OVERRIDE=3.3
echo "running:"
./hw4 height_map.jpg texture_map.jpg
