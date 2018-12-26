mkdir a4 AdaboostResult CNNResult segmentations thresh
mkdir AdaboostResult/xlsx
mkdir CNNResult/xlsx

for i in $(seq -1 9); do
  mkdir AdaboostResult/${i}
  mkdir CNNResult/${i}
done


