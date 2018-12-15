import struct
import numpy as np

trainingFilePath = './../mnist/train-images-idx3-ubyte'
trainingLabelPath = './../mnist/train-labels-idx1-ubyte'
testFilePath = './../mnist/t10k-images-idx3-ubyte'
testLabelPath = './../mnist/t10k-labels-idx1-ubyte'

class Data:
    def __init__(self):
        with open(trainingFilePath, 'rb') as trainingFile:
            self.trainingBuffer = trainingFile.read()
        with open(trainingLabelPath, 'rb') as trainingLableFile:
            self.trainingLabels = trainingLableFile.read()
        with open(testFilePath, 'rb') as trainingFile:
            self.testBuffer = trainingFile.read()
        with open(trainingLabelPath, 'rb') as trainingLableFile:
            self.testLabels = trainingLableFile.read()

    def getImage(self, buffer, index):
        index = index * struct.calcsize('>784B')
        index += struct.calcsize('>IIII')
        data = struct.unpack_from('>784B', buffer, index)
        img = np.reshape(data, (28, 28))
        return img

    def getLabel(self, labelBuffer, index):
        index += struct.calcsize('>II')
        return struct.unpack_from('>9B', labelBuffer, index)[0]

    def getTestImage(self, index):
        return self.getImage(self.testBuffer, index)

    def getTrainImage(self, index):
        return self.getImage(self.trainingBuffer, index)

    def getTestLabel(self, index):
        return self.getLabel(self.testLabels, index)

    def getTrainLabel(self, index):
        return self.getLabel(self.trainingLabels, index)

mnist = Data()
