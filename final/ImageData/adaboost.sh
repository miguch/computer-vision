#!/bin/sh
if [ -z "$1" ] || [ -z "$2" ]
then
    echo "Parameters not set"
else 
    if [[ "$OSTYPE" == "linux-gnu" ]]; then
      ./../project/bin/recognition-linux $1 $2
    elif [[ "$OSTYPE" == "darwin"* ]]; then
      ./../project/bin/recognition-mac $1 $2
    elif [[ "$OSTYPE" == "cygwin" ]]; then
      ./../project/bin/recognition.exe $1 $2
    elif [[ "$OSTYPE" == "msys" ]]; then
      ./../project/bin/recognition.exe $1 $2
    fi

    for fi in AdaboostResult/xlsx/*.csv; do
        python3 ./../project/python/csv2xlsx.py $fi
        rm $fi
    done
fi
