#!/usr/bin/env bash

TRAINING_LOG="uc101.tlog"

if [ `whoami` == "mpss2015" ]; then
	echo "Please run from a local user."
	exit 1
else
#	scp mpss2015@172.16.23.193:~/video-classification/nets/activity_recognition/$TRAINING_LOG . &> /dev/null
	scp mpss2015@172.16.23.193:~/caffe-tmbo/examples/activity_recognition/$TRAINING_LOG . &> /dev/null
	if [ $? -ne 0 ]; then
		echo "Could not download training log file '$TRAINING_LOG'"
		exit 2
	fi
	$CAFFE_ROOT/tools/extra/parse_log.sh $TRAINING_LOG
	gnuplot -p plot_log.gnuplot
fi
