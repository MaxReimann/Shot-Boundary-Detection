import os
import fnmatch
import shutil
import sys


def getImmediateSubdirectories(aDirectory):
  return [name for name in os.listdir(aDirectory)
    if os.path.isdir(os.path.join(aDirectory, name))]

def prepareData(pathPattern):
  frameFolderRoot = pathPattern.replace("[type]", "frames")
  truthFolderRoot = pathPattern.replace("[type]", "truth")

  frameSubfolders = getImmediateSubdirectories(frameFolderRoot)

  split(frameSubfolders)
  sets = [train, validation, test] = split(frameSubfolders)

  for setIndex, currentSet in enumerate(sets):
    for video in currentSet:
      # move truth
      truthFilePath = getCorrespondingTruthFile(truthFolderRoot, video)

      newTruthPath = truthFilePath.replace(truthFolderRoot, truthFolderRoot + "-" + str(setIndex))
      newTruthFolder = os.pathsep.join(newTruthPath.split(os.pathsep)[0:-1])

      if not os.path.exists(newTruthFolder):
        os.makedirs(newTruthFolder)


      print("truth", truthFilePath, newTruthPath)
      shutil.copy(truthFilePath, newTruthPath)


      # move frames
      frameFolderPath = os.path.join(frameFolderRoot, video)
      newFrameFolderPath = frameFolderPath.replace(frameFolderRoot, frameFolderRoot + "-" + str(setIndex))
      frameParentPath = os.pathsep.join(newFrameFolderPath.split(os.pathsep)[0:-1])

      if not os.path.exists(frameParentPath):
        os.makedirs(frameParentPath)

      print("frames", frameFolderPath, newFrameFolderPath)
      shutil.copy(frameFolderPath, newFrameFolderPath)


def getCorrespondingTruthFile(truthFolderRoot, frameFolderPath):
  folderName = frameFolderPath.split(os.pathsep)[-1]

  matches = []
  for root, dirnames, filenames in os.walk(truthFolderRoot):
    for filename in fnmatch.filter(filenames, '*' + folderName + '*'):
      matches.append(os.path.join(root, filename))

  assert(len(matches) == 1)
  return matches[0]


def split(frameSubfolders):
  def idxToClass(idx):
    percentage = 1.0 * idx / len(frameSubfolders)
    if percentage < 0.65:
      return 0 # train
    if percentage < 0.8:
      return 1 # validation
    return 2   # test

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