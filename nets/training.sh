#!/usr/bin/env bash

# Check if called with name
if [ $# -ne 2 ]; then
    echo "Usage: $0 [experiment_name] [architecture]"
	echo "       experiment_name: Name of the subfolder in ./experiments/ for the current experiment."
    echo "       architecture:    Subfolder in . that contains the network definition."
	echo "Exiting."
	exit 1
fi

# Set Vars
DATE=`date +%Y%m%d-%H%M%S`
FOLDER_NAME="${DATE}_$1"
ARCH="$2"
TRAINING_LOG_NAME="uc101.tlog"

echo "Saving experiment in experiments/$FOLDER_NAME"
mkdir experiments/$FOLDER_NAME

# Function for saving results and making plots
function cleanup() {
    echo $1
    
    echo "Copying snapshots"
    ls -v -1 snapshots/ | tail -n 2 | xargs -i mv snapshots/{} experiments/$FOLDER_NAME
    
    echo "Parsing logs"
    $CAFFE_ROOT/tools/extra/parse_log.sh $TRAINING_LOG_NAME
    
    echo "Copying logs"
    cp $TRAINING_LOG_NAME $TRAINING_LOG_NAME.train $TRAINING_LOG_NAME.test experiments/$FOLDER_NAME
    
    echo "Building plots"
    gnuplot -e "filename='$TRAINING_LOG_NAME'" -p plot_log.gnuplot
    mv *.png experiments/$FOLDER_NAME
    
    rm ${TRAINING_LOG_NAME}.test ${TRAINING_LOG_NAME}.train
    echo "Clean up finished"
}

# Clean snapshots
rm snapshots/* 2> /dev/null

# Saving setup
cp $ARCH/net.prototxt $ARCH/solver.prototxt training.sh experiments/$FOLDER_NAME

# Setting interrupt trap
trap 'cleanup "Training interrupted"; exit 1' INT

# Calling caffe
# export CAFFE_ROOT="$HOME/caffe-tmbo"

WEIGHTS=$CAFFE_ROOT/models/bvlc_reference_caffenet/bvlc_reference_caffenet.caffemodel                                                     
$CAFFE_ROOT/build/tools/caffe train \
    -solver $MP_HOME/nets/flow/experiments/$FOLDER_NAME/solver.prototxt
    -weights $WEIGHTS 2> $TRAINING_LOG_NAME

# Resetting interrupt handling
trap - INT

# Check if Training successful
if [ $? -ne 0 ]; then
    echo "Training not successful. Exiting."
    exit 2
fi

cleanup "Training finished"

