import matplotlib.pyplot as plt
from data import mnist

def main():
    img = mnist.getTrainImage(3)
    print(mnist.getTrainLabel(3))
    plt.imshow(img, cmap='gray')
    plt.show()

if __name__ == '__main__':
    main()
