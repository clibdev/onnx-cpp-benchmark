#!/bin/bash

rm -rf deps

VERSION=$(curl -s "https://api.github.com/repos/microsoft/onnxruntime/releases/latest" | grep -Po '"tag_name": "v\K[0-9.]+')
curl -sSLo onnxruntime.tgz https://github.com/microsoft/onnxruntime/releases/latest/download/onnxruntime-linux-x64-gpu-$VERSION.tgz

mkdir -p deps/include/onnxruntime temp
tar xf onnxruntime.tgz --strip-components=1 -C temp
mv temp/lib deps
mv temp/include/* deps/include/onnxruntime
rm -rf onnxruntime.tgz temp

sed -i 's/lib64/lib/' deps/lib/cmake/onnxruntime/onnxruntimeTargets-release.cmake
sed -i 's/lib64/lib/' deps/lib/pkgconfig/libonnxruntime.pc
