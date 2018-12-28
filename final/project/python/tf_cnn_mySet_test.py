
from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import numpy as np
import tensorflow as tf
from mnist_data import mnist_data
from mySet_data import mySet_data, modelDir
from tf_cnn_model import cnn_model_fn


def main(unused_argv):
    testData = mySet_data()
    eval_data = np.asarray(testData.trainImg, dtype=np.float32) / 255
    eval_labels = np.asarray(testData.trainLabels, dtype=np.int32)

    mnist_classifier = tf.estimator.Estimator(
        model_fn=cnn_model_fn, model_dir=modelDir)

    eval_input_fn = tf.estimator.inputs.numpy_input_fn(
        x={"x": eval_data}, y=eval_labels, num_epochs=1, shuffle=False)
    eval_results = mnist_classifier.evaluate(input_fn=eval_input_fn)

    print(eval_results)


if __name__ == "__main__":
    tf.app.run()
