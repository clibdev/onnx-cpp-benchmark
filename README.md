# Fork of [ami-iit/onnx-cpp-benchmark](https://github.com/ami-iit/onnx-cpp-benchmark)

Differences between original repository and fork:

* ONNX Runtime installation with [setup.sh](setup.sh) file.
* CLI11 dependency removed.

# Installation

```shell
./setup.sh
```

# Usage

Download a simple `.onnx` file:

```shell
curl -sSLo yolov5n-face.onnx https://github.com/clibdev/yolov5-face/releases/latest/download/yolov5n-face.onnx
```

Build project:

```shell
rm -rf build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=deps
cmake --build build -j$(nproc)
```

Run the benchmark on CPU:

```shell
./build/onnx-cpp-benchmark yolov5n-face.onnx cpu
```

Run the benchmark on CUDA:

```shell
./build/onnx-cpp-benchmark yolov5n-face.onnx cuda
```
