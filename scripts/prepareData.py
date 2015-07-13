import os
import fnmatch
import shutil
import sys
import logging

def _logpath(path, names):
    logging.info('Working in %s' % path)
    return []   # nothing will be ignored

def getImmediateSubdirectories(aDirectory):
  return [name for name in os.listdir(aDirectory)
    if os.path.isdir(os.path.join(aDirectory, name))]

def prepareData(pathPattern):
  frameFolderRoot = pathPattern.replace("[type]", "frames")
  truthFolderRoot = pathPattern.replace("[type]", "truth")
  newTruthFolderRoot = pathPattern.replace("[type]", "generated_soft_cuts" + os.path.sep + "truth")
  newFrameFolderRoot = pathPattern.replace("[type]", "generated_soft_cuts" + os.path.sep + "frame")
  frameSubfolders = getImmediateSubdirectories(frameFolderRoot)

  split(frameSubfolders)
  sets = [train, validation, test] = split(frameSubfolders)

  print("sets", sets)

  for setIndex, currentSet in enumerate(sets):
    for video in currentSet:
      # move truth
      truthFilePath = getCorrespondingTruthFile(truthFolderRoot, video)

      newTruthPath = truthFilePath.replace(truthFolderRoot, newTruthFolderRoot + "-" + str(setIndex))
      newTruthFolder = os.path.sep.join(newTruthPath.split(os.path.sep)[0:-1])

      if not os.path.exists(newTruthFolder):
        os.makedirs(newTruthFolder)

      print("truth")
      print("  " + truthFilePath)
      print(" >" + newTruthPath)
      shutil.copy(truthFilePath, newTruthPath)

      # move frames
      frameFolderPath = os.path.join(frameFolderRoot, video)
      newFrameFolderPath = frameFolderPath.replace(frameFolderRoot, newFrameFolderRoot + "-" + str(setIndex))
      frameParentPath = os.path.sep.join(newFrameFolderPath.split(os.path.sep)[0:-1])

      if not os.path.exists(frameParentPath):
        os.makedirs(frameParentPath)

      # if os.path.exists(newFrameFolderPath):
      #   print(newFrameFolderPath, "already exists. skipping this dataset")
      #   continue

      print("frames")
      print("  " + frameFolderPath)
      print(" >" + newFrameFolderPath)
      # shutil.copytree(frameFolderPath, newFrameFolderPath, ignore=_logpath)
      for root, dirnames, filenames in os.walk(frameFolderPath):
        for filename in filenames:
          oldFilePath = os.path.join(frameFolderPath, filename)
          newFilePath = os.path.join(newFrameFolderPath, filename)
          if not os.path.exists(newFilePath):
            print("copy", oldFilePath, "to", newFilePath)
            shutil.copy(oldFilePath, newFilePath)
          else:
            print("skip", oldFilePath)


def getCorrespondingTruthFile(truthFolderRoot, frameFolderPath):
  folderName = frameFolderPath.split(os.path.sep)[-1].lower()

  matches = []
  for root, dirnames, filenames in os.walk(truthFolderRoot):
    for filename in filenames:
        if folderName.lower() in filename.lower():
            matches.append(os.path.join(root, filename))

  if len(matches) != 1:
    print("too many matches?", matches)
    print("truthFolderRoot", truthFolderRoot)
    print("frameFolderPath", frameFolderPath)
    print("folderName", folderName)
    print("")
  assert(len(matches) == 1)
  return matches[0]


def split(frameSubfolders):
  def idxToClass(idx):
    percentage = 1.0 * idx / len(frameSubfolders)
    if percentage < 0.65:
      return 0 # train 0.65
    if percentage < 0.8:
      return 1 # validation 0.15
    return 2   # test 0.20

  values = set(map(lambda x : idxToClass(x[0]), enumerate(frameSubfolders)))
  partitions = [[x[1] for x in enumerate(frameSubfolders) if idxToClass(x[0]) == v] for v in values]

  while len(partitions) < 3:
    partitions.append([])

  return partitions

if len(sys.argv) == 2:
  prepareData(sys.argv[1])
else:
  path = os.path.join("..", "resources", "[type]", "2007")
  print("Provide the path as an argument. E.g.:", path)
