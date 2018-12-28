#!/bin/bash
if [ -z "$1" ] || [ -z "$2" ]
then
    echo "Parameters not set"
else 
    rm -f AdaboostResult/*/*
    if [[ "$OSTYPE" == "linux-gnu" ]]; then
      ./../project/AdaboostPredict/bin/recognition-linux $1 $2 $3
    elif [[ "$OSTYPE" == "darwin"* ]]; then
      ./../project/AdaboostPredict/bin/recognition-mac $1 $2 $3
    elif [[ "$OSTYPE" == "cygwin" ]]; then
      ./../project/AdaboostPredict/bin/recognition.exe $1 $2 $3
    elif [[ "$OSTYPE" == "msys" ]]; then
      ./../project/AdaboostPredict/bin/recognition.exe $1 $2 $3
    fi


    for fi in AdaboostResult/xlsx/*.csv; do
        python3 ./../project/csv2xlsx.py $fi
        rm $fi
    done
    python3 ./../project/csv2xlsx.py adaboost-result.csv
    rm adaboost-result.csv
fi
