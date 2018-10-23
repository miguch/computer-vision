# 计算机视觉-Ex3

## Ex 3-1

###要求

输入：普通 A4 打印纸，上面可能有手写笔记或者打印内容，但是拍照时可能角度不正。 

输出: 

1. 图像的边缘点(输出边缘图像𝐈𝑒𝑑𝑔𝑒 )
2. 计算 A4 纸边缘的各直线方程, 输出如下结果: (a)各个直线的参数方程; (b) 在上面的边缘图𝐈𝑒𝑑𝑔𝑒绘制直线, 用蓝色显示, 得到图像𝐈2; (c)在𝐈2图上显示 A4 纸的相关边缘点,用红色点显示, 得到图像𝐈3

3. 输出 A4 纸的四个角点(在𝐈3上用半径为 5 的圆绘制角点,得到图像𝐈4 ) 

### 程序简述

​	代码实现中使用到了上一次作业实现的 Canny 进行图像边缘的输出，并添加了一个 A4Detect 类，类的构造需传入原图文件名、霍夫变换后筛选线时的阈值比例、Canny 边缘检测的阈值（为提升边缘检测的速度，双阈值处理时使用两个相同的阈值，即仅执行一遍阈值处理），以及一个代表是否跳过 NMS 的 bool 型变量。类中使用霍夫变换进行 A4 纸边缘的直线检测。

​	构建一个`A4Detect` 对象后调用其 `runHough` 方法进行 A4纸边缘检测，该方法将调用 Canny 获取边缘图并保存至`cannyEdge.bmp `中, 然后调用`houghTransform` 方法获取以极坐标形式表示的所有检测到的直线，然后转为笛卡尔坐标系形式的直线，将直线方程输出到程序的标准输出中，最后将与 A4纸相关的直线段及其交点绘制在原图上并返回绘制后的一个 `CImg` 对象。

​	在`houghTransform` 方法中，将首先获取 `houghSpace`，原理是使用一个列为θ角，行为 r 的二维矩阵，全部元素初始化为0。对于每一个有效的边缘点，将过该点的从0˚到360˚的所有直线在矩阵中加一，直线极坐标方程中 r 的公式为$r =\sqrt{(x^2+y^2)} \times cos(\theta_0 -\theta)$, 其中$\theta_0$ 为边缘点的角度，$\theta$ 为直线的角度。

​	`houghSpace` 中存储的即为每一条直线的"票数"，接下来首先找出票数最多的一条线，然后乘以输入的阈值比例，使用该结果对所有线进行筛选，再排除较为接近的直线获得最后的 A4纸边缘的直线。

​	在原图上标注角点和A4纸相关边缘的做法是在找出的直线间找交点作为角点，然后再对每两个交点进行计算，若存在一条在找到的直线集合中的直线，则代表这两交点间的线段是 A4纸边缘，将其用红色标注出来。

​	用于测试的主程序的用法如下：

`./a4detect [-t thresh(default 0.5) cannyThresh(default 50)][filename]` 

​	-t 选项后接霍夫变换和 canny 边缘检测的阈值，最后一个参数指定读入的文件名。程序完成检测后会将 Canny 输出的边缘图片存入`cannyEdge.bmp `文件，并将输出的标注好角点与边缘的图像进行显示并存入coins.bmp 文件。

### 程序编译

​	程序编译使用 cmake 进行，在 Mac, Linux 以及 Windows 环境下分别在代码目录下使用如下对应的命令进行：

```shell
#Linux/Mac
cmake .
make
#Windows (需使用 Native Tools Command Line Prompt for Visual Studio)
cmake -G "NMake Makefiles" .
nmake
```

	### 程序测试

使用给定的测试集进行测试，测试的结果图片与终端输出及所使用的程序参数如下：

1. **IMG_20150320_143133.bmp** （霍夫变换阈值0.5， Canny 阈值80）

   `./a4detect -t 0.5 80 IMG_20150320_143133.bmp`

   ![a4-1](/Users/mig/Desktop/大三上/computer-vision/assignment/screenshoot/a4-1.jpg)

   ![a4-cli-1](/Users/mig/Desktop/大三上/computer-vision/assignment/screenshoot/a4-cli-1.jpg)

2. IMG_20150320_143145.bmp（霍夫变换阈值0.6， Canny 阈值60）

   `./a4detect -t 0.6 60 IMG_20150320_143145.bmp`

   ![a4-2](/Users/mig/Desktop/大三上/computer-vision/assignment/screenshoot/a4-2.jpg)

   ![a4-cli-2](/Users/mig/Desktop/大三上/computer-vision/assignment/screenshoot/a4-cli-2.jpg)

3. IMG_20150320_143211.bmp(霍夫变换阈值0.6， Canny 阈值80)

   `./a4detect -t 0.6 80 IMG_20150320_143211.bmp`

   ![a4-3](/Users/mig/Desktop/大三上/computer-vision/assignment/screenshoot/a4-3.jpg)

   ![a4-cli-3](/Users/mig/Desktop/大三上/computer-vision/assignment/screenshoot/a4-cli-3.jpg)

4. IMG_20150320_143220.bmp(霍夫变换阈值0.5， Canny 阈值60)

   `./a4detect -t 0.5 60 IMG_20150320_143220.bmp`

   ![a4-4](/Users/mig/Desktop/大三上/computer-vision/assignment/screenshoot/a4-4.jpg)

   ![a4-cli-4](/Users/mig/Desktop/大三上/computer-vision/assignment/screenshoot/a4-cli-4.jpg)

5. IMG_20150320_143334.bmp(霍夫变换阈值0.5， Canny 阈值60)

   `./a4detect -t 0.5 50 IMG_20150320_143334.bmp`

   ![a4-5](/Users/mig/Desktop/大三上/computer-vision/assignment/screenshoot/a4-5.jpg)

   ![a4-cli-5](/Users/mig/Desktop/大三上/computer-vision/assignment/screenshoot/a4-cli-5.jpg)

   6. IMG_20150320_143345.bmp(霍夫变换阈值0.5， Canny 阈值50)

      `./a4detect -t 0.5 50 IMG_20150320_143345.bmp`

      ![a4-6](/Users/mig/Desktop/大三上/computer-vision/assignment/screenshoot/a4-6.jpg)

      ![a4-cli-6](/Users/mig/Desktop/大三上/computer-vision/assignment/screenshoot/a4-cli-6.jpg)

   ### 测试中的参数分析

   ​	A4纸边缘检测程序接收的参数为霍夫变换后筛选直线的阈值和 Canny 检测的阈值，霍夫变换的阈值若太小则可能会存在某一条边未检测出来，太小则可能会出现检测出的边缘过多的情况。

   ​	Canny 的阈值则影响了 Canny 边缘检测时检测出的边缘点数目，若设得太小则结果会受到字或其他边缘的干扰，如第三个测试样例，纸张上有一条较长的线，为避免该线被识别为纸张边缘该图的 Canny 阈值需要设得较高。



   ## Ex 3-2

   	### 要求

   输入：图像上面有若干硬币。

   输出：

   1. 图像的边缘(输出边缘图像𝐈𝑒𝑑𝑔𝑒 )
   2. 把图像中边缘拟合成圆(在图像𝐈𝑒𝑑𝑔𝑒 绘制出对应圆形,用蓝色显示, 得到图像 𝐈2),圆周相关的像素(在𝐈2上显示与圆形相关的像素, 用红色显示, 得到图像𝐈3) 
   3. 输出图像中硬币的数量 


### 程序简述

​	硬币识别的程序的主要过程与 A4纸边缘识别基本相似，包含了一个类`CoinDetect` ，构造时接收的参数为 Canny 的阈值，霍夫变换后筛选圆的阈值比例，输入文件名以及需要查找的圆的最小和最大半径。

​	构造`CoinDetect` 对象后调用其`runHough` 方法进行硬币检测，该方法将先使用 Canny 进行边缘检测，然后调用`getHoughCircles` 方法进行霍夫变换检测圆。

​	因为圆可以由圆心和半径进行定义，程序使用了一个三个元素的数组来表示圆。

​	霍夫变换检测圆的过程是对于输入参数的最小最大半径区间按5为步长进行圆的扫描，扫描过程中对于每一个有效边缘点作半径为当前扫描半径的圆，在票数表中将所有圆心为刚作的圆上的点，半径为扫描半径的圆票数加一。

​	扫描后找出最高的票数与阈值比例相乘获得筛选的阈值，然后在高于筛选阈值的圆中排除相似的圆，剩下的圆即为求得的结果，圆的数量就是图中硬币的数量，程序将会在标准输出中输出检测出的硬币数目。

​	对于找到的所有圆，将在原图上画出，为了使画出的圆在图上更为明显，会画出圆半径±5范围内的所有圆，增加画出的圆的线条厚度。

​	用于测试的主程序的用法如下：

`./coin-detect  [-t thresh(default 0.5) cannyThresh(default 50)] filename low_r high_r`

​	参数中文件名和 r 的大小值是必须指定的，程序完成检测后将会把 Canny 检测出的边缘存入`cannyEdges.bmp`， 标记完圆后的图片存入`coins.bmp`。

​	程序编译方式与 A4纸检测的程序编译方式相同。

### 程序测试

​	使用给出的测试集进行测试，使用的参数和测试结果图片与终端输出如下：

1. 2.bmp(Canny 阈值 70， 大小半径为150，160)

   `./coin-detect -t 0.7 60 2.bmp 150 160`

   ![coins-1](/Users/mig/Desktop/大三上/computer-vision/assignment/screenshoot/coins-1.png)

   ![coin-cli-1](/Users/mig/Desktop/大三上/computer-vision/assignment/screenshoot/coin-cli-1.png)

2. 5.bmp(Canny 阈值80，大小半径200， 225)

   `./coin-detect -t 0.7 80 5.bmp 200 225`

   ![coins-2](/Users/mig/Desktop/大三上/computer-vision/assignment/screenshoot/coins-2.png)

   ![coin-cli-2](/Users/mig/Desktop/大三上/computer-vision/assignment/screenshoot/coin-cli-2.png)

3. 6.bmp(Canny 阈值50，大小半径150， 180)

   `./coin-detect -t 0.7 50 6.bmp 150 180`

   ![coins-3](/Users/mig/Desktop/大三上/computer-vision/assignment/screenshoot/coins-3.png)

   ![coin-cli-3](/Users/mig/Desktop/大三上/computer-vision/assignment/screenshoot/coin-cli-3.png)

4. 11.bmp(Canny 阈值40，大小半径175， 200)

   `./coin-detect -t 0.7 40 11.bmp 175 200`

   ![coins-4](/Users/mig/Desktop/大三上/computer-vision/assignment/screenshoot/coins-4.png)

   ![coin-cli-4](/Users/mig/Desktop/大三上/computer-vision/assignment/screenshoot/coin-cli-4.png)

5. 12.bmp(Canny 阈值50，大小半径470, 520)

   `./coin-detect -t 0.7 50 12.bmp 470 520`

   ![coins-5](/Users/mig/Desktop/大三上/computer-vision/assignment/screenshoot/coins-5.png)

   ![coin-cli-5](/Users/mig/Desktop/大三上/computer-vision/assignment/screenshoot/coin-cli-5.png)

6. f55bcad35f5d993c80f928f3dc940aa8.bmp(Canny 阈值50，大小半径40, 70)

   `./coin-detect -t 0.7 50 f55bcad35f5d993c80f928f3dc940aa8.bmp 40 70`

   ![coins-6](/Users/mig/Desktop/大三上/computer-vision/assignment/screenshoot/coins-6.png)

   ![coin-cli-6](/Users/mig/Desktop/大三上/computer-vision/assignment/screenshoot/coin-cli-6.png)

### 测试输入参数分析

​	测试程序接收的参数为霍夫变换后筛选圆形的阈值比例，Canny 的阈值以及搜索圆形时半径的范围。其作用分别如下：

1. 圆形的阈值比例作用是在投票后根据最高的票数乘以该比例作为阈值排除票数较低的圆，在本次的测试样例中该参数均被设为0.7 ，原因是本次的测试样例的圆大小差异较小，所获得票数的差异也因此不大。当出现图中圆大小差异较大的情况时，阈值比例需要对应降低以避免较小的圆被排除。
2. Canny 的阈值用于排除梯度值较小的边缘，过低时检测的结果可能会受到其他边缘点的干扰，太高时部分的圆的边缘可能会有不完整的情况，导致检测结果中可能会漏掉应被检测出的圆。
3. 检测时圆的半径范围并不是检测圆形的程序正确运行所必须的参数，但若不指定范围程序将需要检测非常大的一个半径范围，对于程序运行时间影响较大，因此在此次程序中选择了对于输入的图片需要指定半径范围的做法。

