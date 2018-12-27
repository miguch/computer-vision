import numpy as np

trainImagePath = "./mnist/train-images-idx3-ubyte"
testImagePath = "./mnist/t10k-images-idx3-ubyte"
trainLabelPath = "./mnist/train-labels-idx1-ubyte"
testLabelPath = "./mnist/t10k-labels-idx1-ubyte"

class mnist_data:
    def __init__(self):
        self.testImg = self.readImgFile(testImagePath)
        self.trainImg = self.readImgFile(trainImagePath)
        self.testLabels = self.readLabelFile(testLabelPath)
        self.trainLabels = self.readLabelFile(trainLabelPath)

    def readImgFile(self, path):
        with open(path, 'rb') as imgFile:
            magic = imgFile.read(4)
            items = int.from_bytes(imgFile.read(4), byteorder='big')
            rows = int.from_bytes(imgFile.read(4), byteorder='big')
            cols = int.from_bytes(imgFile.read(4), byteorder='big')
            imgSize = cols * rows
            data = np.zeros((items, imgSize), dtype=np.uint8)
            for i in range(items):
                data[i] = np.frombuffer(imgFile.read(imgSize), dtype=np.uint8)
            return data

    def readLabelFile(self, path):
        with open(path, 'rb') as labelFile:
            magic = labelFile.read(4)
            items = int.from_bytes(labelFile.read(4), byteorder='big')
            data = np.frombuffer(labelFile.read(items), dtype=np.uint8)
            return data

