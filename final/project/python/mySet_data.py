import numpy as np
from PIL import Image
import os, sys

scriptPath = os.path.dirname(os.path.abspath(sys.argv[0]))
modelDir = os.path.join(scriptPath, "myTrainSet", "mySet_convnet_model")

class mySet_data:
    def __init__(self):
        self.trainImg, self.trainLabels = self.readImgSet('./myTrainSet')

    def readImgSet(self, path):
        fileCounts = []
        with open(path, 'r') as digNums:
            fileCounts = [int(c) for c in next(digNums).split()]
        imagesCount = 0
        for c in fileCounts:
            imagesCount += c
        imagesData = np.zeros((imagesCount, 784), dtype=np.uint8)
        labelsData = np.zeros((imagesCount, ), dtype=np.uint8)
        index = 0
        for i in range(10):
            for k in range(fileCounts[i]):
                filename = os.path.join(int(i), int(k) + '.jpg')
                img = Image.open(filename)
                imgData = np.asarray(img, dtype=np.uint8)
                imagesData[index] = imgData.reshape((1, 784))
                labelsData[index] = i
                index += 1
        return imagesData, labelsData
