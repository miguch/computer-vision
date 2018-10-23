为减小压缩包体积，此处图片使用 jpg 格式，若需要使用 bmp 格式的图片，可在命令行下执行下面指令获得 bmp 格式的文件（需安装 ffmpeg）：

```shell
for fi in *.jpg; do 
	ffmpeg -i $fi ${fi%%.*}.bmp
done
```

