
from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import numpy as np
import tensorflow as tf
from mnist_data import mnist_data, modelDir
from tf_cnn_model import cnn_model_fn
from PIL import Image

def getResizedImage(imgPath):
    srcImg = Image.open(imgPath).resize((28, 28))
    return srcImg

def toImgData(testImg):
    mat = np.asarray(testImg, dtype=np.float32)
    return mat.reshape((1, 784)) / 255

def getImgData(imgPath):
    srcImg = Image.open(imgPath).resize((28, 28))
    mat = np.asarray(srcImg, dtype=np.float32)
    return mat.reshape((1, 784)) / 255

def getImgDataList(imgPathList):
    result = np.zeros((len(imgPathList), 784), dtype=np.float32)
    for i in range(len(imgPathList)):
        result[i] = getImgData(imgPathList[i])
    return result


def testImagesWithData(data):
    predict_input_fn = tf.estimator.inputs.numpy_input_fn(
        x={'x':data}, num_epochs=1, shuffle=False
    )
    mnist_classifier = tf.estimator.Estimator(
        model_fn=cnn_model_fn, model_dir=modelDir)
    predict_result = mnist_classifier.predict(input_fn=predict_input_fn)
    res = []
    for i, p in enumerate(predict_result):
        res.append(p['classes'])
    return res

#Images must all be size (28, 28)
def testImages(images):
    data = np.zeros((len(images), 784), dtype=np.float32)
    for i in range(len(images)):
        data[i] = toImgData(images[i])
    return testImagesWithData(data)

def testImagesWithNames(imgPathList):
    data = getImgDataList(imgPathList)
    return testImagesWithData(data)

def main(argv):
    if len(argv) == 1:
        print("Error: no file specified")
    else:
        res = testImagesWithNames(argv[1:])
        for i in range(len(res)):
            print('Predict %s: %d' % (argv[i+1], res[i]))


if __name__ == "__main__":
    tf.app.run()
