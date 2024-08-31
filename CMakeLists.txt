cmake_minimum_required (VERSION 3.16)
project(onnx-benchmark)

set(CMAKE_CXX_STANDARD 17)

find_package(onnxruntime REQUIRED)

add_executable(onnx-cpp-benchmark onnx-cpp-benchmark.cpp)

target_link_libraries(onnx-cpp-benchmark onnxruntime::onnxruntime)
