#!/bin/bash
if [ -z "$1" ] || [ -z "$2" ]
then
    echo "Parameters not set"
else 
    if [[ "$OSTYPE" == "linux-gnu" ]]; then
      ./../../project/AdaboostPredict/bin/recognition-linux $1 $2 $3 skipAdaboost
    elif [[ "$OSTYPE" == "darwin"* ]]; then
      ./../../project/AdaboostPredict/bin/recognition-mac $1 $2 $3 skipAdaboost
    elif [[ "$OSTYPE" == "cygwin" ]]; then
      ./../../project/AdaboostPredict/bin/recognition.exe $1 $2 $3 skipAdaboost
    elif [[ "$OSTYPE" == "msys" ]]; then
      ./../../project/AdaboostPredict/bin/recognition.exe $1 $2 $3 skipAdaboost
    fi

    rm -f CNNResult/*/*

    python3 ./../project/CNN/tf_cnn_mnist_run.py $1 $2

    for fi in CNNResult/xlsx/*.csv; do
        python3 ./../project/csv2xlsx.py $fi
        rm $fi
    done

    python3 ./../../project/csv2xlsx.py CNN-result.csv
    rm CNN-result.csv
fi
