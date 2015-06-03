#!/usr/bin/env bash

WEIGHTS=$CAFFE_ROOT/models/bvlc_reference_caffenet/bvlc_reference_caffenet.caffemodel

$CAFFE_ROOT/build/tools/caffe train \
    -solver $SBD_HOME/nets/solver.prototxt \
    -weights $WEIGHTS 2>&1 | tee soft_cuts.tlog | less
#    -snapshot snapshots/_iter_50000.solverstate

