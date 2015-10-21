# Shot-Boundary-Detection
An experimental tool for hard- and softcut detection using machine learning.
Hardcuts are detected using a SVM, softcuts with neural networks. In-depth information on the approach can be found in the technical paper in the paper/ directory. 

### Building
Build dependencies are OpenCV, Boost and Caffe. 

## Hard Cut Detection

To do hard cut detection, use the following command (change executable file ending according to your OS):

`sbd.exe --hard_cut ../resources/[type]/ --classify_folder ../resources/classify/[type]`

The `[type]` placeholder will be substituted by the program, so leave it as it is.

The corresponding folder structure would be:
    
    |-Shot-Boundary-Detection/
        |-build/
            |-sbd.exe
        |-resources/
            |-frames/
                |-trainig-video-1/
                    |-00001.jpg
                    |-...
                |-trainig-video-2/
                    |-00001.jpg
                    |-...
            |-truth/
                |-ref_training-video-1.xml
                |-ref_training-video-2.xml
                |-ref_test-video-1.xml
            |-classify/
                |-frames/
                    |-test-video-1/
                        |-00001.jpg
                        |-...
            |-(differenceHistograms.yaml)
            |-(differenceHistogramsEvaluation.yaml)
            
This tells the program to do hard cut detection, take the training data from `../resources/frames/`, take the gold standard from `../resources/truth/` and run hard cut detection on all videos in `../resources/classify/frames`.
Due to the prototypical state of this project, it is crucial that the resources folder is in the parent directory of the execution root.

It will also cache the calculated training data in `resources/differenceHistograms.yaml`. If you change any of the data folders, you have to manually delete the corresponding cache or use the `--no_cache` flag. 
If you have just a cache file as training data (there is one in `resources/` that can be used), you can place it in `resources/` and leave `resources/frames/` empty. `resources/truth` still has to contain the truth data for the classify videos in order to calculate performance measures. The command to start hard cut detection stays the same.

If everything works well, after reading in all the data, you will see an output like this:
    
    Training SVM.
    Training on 225/2589 positive instances.
    Evaluating on test set ...
    Evaluating on 98/36995 positive instances.
    TP: 96 FP: 30 TN: 36867 FN: 2
    Precision: 0.76
       Recall: 0.98
           F1: 0.86
     Accuracy: 1
    Press any key to continue . . .
    
## Visualization
The results of the hard cut detection can be visualized in an interactive bar chart, but this only works, if the flag `--classify_folder` is used, e.g.:

`sbd.exe --hard_cut ../resources/[type]/ --classify_folder ../resources/classify/[type]`

During hard cut detection, a file `resources/d3/data/visData.tsv` will be created. Start an http-server in the `resources/` folder, e.g., using

    python -m SimpleHTTPServer
    
and open in your web browser: `localhost:8000/d3/index.html`
It will look for the video folders in `resources/classify/`.

![Hard Cut Visualization](paper/images/hard_cut_visualization.png?raw=true "Hard Cut Visualization")

## Soft Cut Detection

### Data Generation

To generate new soft cut transitions, use the following command:

`sbd --generate ../resources/[type]`

The provided path should point to the data folder for the images and the truth data.
Thus, it must contain the placeholder [type], which will be replaced by 'frames' or 'truth'".

Our generated data can be found on the server fb10dl01 in `/opt/data_sets/video_sbd_dataset/generated_soft_cuts`.
In `gen-2007-0` and `gen-2007-1` are the generated data of a transition length of 11.
The folders `gen-21-2007-0` and `gen-21-2007-1` contain the generated data of a transistion length of 21.

### Soft Cut Detection Evaluation

The different net architectures can be found on the server fb10dl01 in `/home/pva_t1/Shot-Boundary-Detection/nets/experiments/`:
- CNN + one LSTM: `one-lstm`
- CNN + two LSTMs: `20150710-090510_two-lstm` (transition length 11), `20150718-095654_two_lstm_21` (transition length 21)
- one convolutional layer + two LSTMs: `20150719-140428_only_lstm_168`
Each of those folders contains the final model, the net.prototxt, the solver.prototxt and the log file.

The LMDB we used to train our models can be found in `/mnt/naruto/data_sets/video_sbd_dataset/lmdb`.
Unfortunately, we had not enough space to keep both LMDBs, one with a transition length of 11 and another with a transition length of 21, at the same time.
The current LMDB in that folder has an transition length of 21.

To start the evaluation of the soft cut detection, use the following command:

`sbd --soft_cut`

The parameter, e.g., the model to evaluate, the test data to use, are located in the file: `src/soft_cut/softcut_detection.hpp`
- preModel: the model to use for the evaluation
- protoFile: the deploy.prototxt file of the model
- useCPU: true, if the CPU should be used, false, if the GPU should be used
- channels: the number of channels in the frames
- isDepug: true, if debug prints should be displayed, false, otherwise
- resultLayer: the name of result layer in the prototxt file
- dataLayer: the name of the data layer in the prototxt file
- batchSize: the batchSize used in the net
- nrClasses: the number of classes
- sequenceSize: the number of frames per sequence (21 or 11)
- txtFile: the txt file containing the frames of the video(s), which should be evaluated

The results are printed to the console.
The output will look like this:

	Predicting video BG_35050
	Predicting 83112 frames of video.
	0% 10% 20% 30% 40% 50% 60% 70% 80% 90% 100%
	Majority-Voting-Diagonally: 76705/83112 = 0.922911
	Confusion Matrix (rows are actual):
	  75574    6183
	    224    1131
	0: Precision = 0.997045, Recall = 0.924373
	1: Precision = 0.154635, Recall = 0.834686
	Majority-Voting-Diagonally with gap filler of size 1: 76466/83112 = 0.920036
	Confusion Matrix (rows are actual):
	  75326    6431
	    215    1140
	0: Precision = 0.997154, Recall = 0.92134
	1: Precision = 0.150575, Recall = 0.841328
	...


