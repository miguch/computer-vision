
from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import numpy as np
import tensorflow as tf
from mnist_data import mnist_data, modelDir
from tf_cnn_model import cnn_model_fn


tf.logging.set_verbosity(tf.logging.INFO)


def main(unused_argv):
    print('Trainning with MNIST dataset...')
    mnist = mnist_data()
    train_data = np.asarray(mnist.trainImg, dtype=np.float32) / 255
    train_labels = np.asarray(mnist.trainLabels, dtype=np.int32)
    eval_data = np.asarray(mnist.testImg, dtype=np.float32) / 255
    eval_labels = np.asarray(mnist.testLabels, dtype=np.int32)

    mnist_classifier = tf.estimator.Estimator(
        model_fn=cnn_model_fn, model_dir=modelDir)

    tensors_to_log = {"probabilities": "softmax_tensor"}
    logging_hook = tf.train.LoggingTensorHook(
        tensors=tensors_to_log, every_n_iter=50)

    train_input_fn = tf.estimator.inputs.numpy_input_fn(
        x={"x": train_data},
        y=train_labels,
        batch_size=200,
        num_epochs=None,
        shuffle=True)

    mnist_classifier.train(
        input_fn=train_input_fn,
        steps=30000,
        hooks=[logging_hook])

    # Evaluate the model and print results
    eval_input_fn = tf.estimator.inputs.numpy_input_fn(
        x={"x": eval_data}, y=eval_labels, num_epochs=1, shuffle=False)
    eval_results = mnist_classifier.evaluate(input_fn=eval_input_fn)

    print(eval_results)


if __name__ == "__main__":
    tf.app.run()
