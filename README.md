# Shot-Boundary-Detection

## Hard Cut Detection

To do hard cut detection, use the following command (change executable file ending accorging to your OS):

`sbd.exe --hard_cut ../resources/[type]/ --classify_folder ../resources/classify/[type]`

The correspnding folder structure would be:
    
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

It will also cache the calculated data in two files and save those files in `resources/`. `differenceHistograms.yaml` is the training data cache, `differenceHistogramsEvaluation.yaml` is the cache of the classification data. If you change any of the data folders, you have to manually delete the corresponding cache. 
If you have just a cache file as training data, you can place it in `resources/` and leave `resources/frames/` empty. `resources/truth` still has to contain the truth data for the classify videos in order to calculate performance measures. The command to start hard cut detection stays the same.

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

## Data Generation

To generate new soft cut transitions, use the following command:

`sbd --generate ../resources/[type]`

The provided path should point to the data folder for the images and the truth data.
Thus, it must contain the placeholder [type], which will be replaced by 'frames' or 'truth'".
