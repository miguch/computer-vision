
from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import numpy as np
import tensorflow as tf
from mnist_data import mnist_data
from tf_cnn_model import cnn_model_fn


def testImg(imgPath):
    tf.image.decode_jpeg(imgPath, channels=1)
    
    mnist_classifier = tf.estimator.Estimator(
        model_fn=cnn_model_fn, model_dir="mnist/mnist_convnet_model")




def main(argv):
    if len(argv) == 1:
        print("Error: no file specified")
    else:
        testImg(argv[1])


if __name__ == "__main__":
    tf.app.run()
