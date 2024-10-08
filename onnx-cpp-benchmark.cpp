#include <cstdint>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <vector>
#include <random>
#include <chrono>
#include <onnxruntime_cxx_api.h>

std::unique_ptr<Ort::Session> g_ortSession;

std::string convertEnumToString(const ONNXTensorElementDataType value) {
    switch (value) {
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_UNDEFINED:
            return "UNDEFINED";
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT:
            return "FLOAT";
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_UINT8:
            return "UINT8";
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_INT8:
            return "INT8";
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_UINT16:
            return "UINT16";
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_INT16:
            return "INT16";
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_INT32:
            return "INT32";
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_INT64:
            return "INT64";
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_STRING:
            return "STRING";
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_BOOL:
            return "BOOL";
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT16:
            return "FLOAT16";
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_DOUBLE:
            return "DOUBLE";
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_UINT32:
            return "UINT32";
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_UINT64:
            return "UINT64";
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_COMPLEX64:
            return "COMPLEX64";
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_COMPLEX128:
            return "COMPLEX128";
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_BFLOAT16:
            return "BFLOAT16";
        default:
            return "UNKNOWN";
    }
}

Ort::Value CreateTensor(const ONNXTensorElementDataType orttype, const std::vector<int64_t> &realShape) {
    const Ort::AllocatorWithDefaultOptions allocator;
    switch (orttype) {
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT:
            return Ort::Value::CreateTensor<float>(allocator, realShape.data(), realShape.size());
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_UINT8:
            return Ort::Value::CreateTensor<uint8_t>(allocator, realShape.data(), realShape.size());
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_INT8:
            return Ort::Value::CreateTensor<int8_t>(allocator, realShape.data(), realShape.size());
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_UINT16:
            return Ort::Value::CreateTensor<uint16_t>(allocator, realShape.data(), realShape.size());
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_INT16:
            return Ort::Value::CreateTensor<int16_t>(allocator, realShape.data(), realShape.size());
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_INT32:
            return Ort::Value::CreateTensor<int32_t>(allocator, realShape.data(), realShape.size());
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_INT64:
            return Ort::Value::CreateTensor<int64_t>(allocator, realShape.data(), realShape.size());
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_BOOL:
            return Ort::Value::CreateTensor<bool>(allocator, realShape.data(), realShape.size());
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_DOUBLE:
            return Ort::Value::CreateTensor<double>(allocator, realShape.data(), realShape.size());
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_UINT32:
            return Ort::Value::CreateTensor<uint32_t>(allocator, realShape.data(), realShape.size());
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_UINT64:
            return Ort::Value::CreateTensor<uint64_t>(allocator, realShape.data(), realShape.size());
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_STRING:
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_COMPLEX64:
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_COMPLEX128:
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_BFLOAT16:
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_UNDEFINED:
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT16:
        default:
            std::cerr << "onnx-cpp-benchmark: Error in CreateTensor, unable to create tensor of type "
                    << convertEnumToString(orttype) << std::endl;
            std::exit(1);
    }
}

template<typename T>
void FillTensorWithRandomDataHelper(Ort::Value &tensor, std::vector<int64_t> &realShape) {
    std::default_random_engine generator;
    generator.seed(0);

    std::uniform_real_distribution<float> distribution(-100.0, 100.0);

    int64_t tensor_size = 1;
    for (const auto i: realShape) {
        tensor_size = tensor_size * i;
    }

    T *tensor_data = tensor.GetTensorMutableData<T>();

    for (int i = 0; i < tensor_size; ++i) {
        float random_value = distribution(generator);
        tensor_data[i] = random_value;
    }
}

void FillTensorWithRandomData(Ort::Value &tensor) {
    const ONNXTensorElementDataType orttype = tensor.GetTypeInfo().GetTensorTypeAndShapeInfo().GetElementType();
    std::vector<int64_t> realShape = tensor.GetTypeInfo().GetTensorTypeAndShapeInfo().GetShape();
    switch (orttype) {
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT:
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_UINT8:
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_INT8:
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_UINT16:
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_INT16:
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_INT32:
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_INT64:
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_BOOL:
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_DOUBLE:
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_UINT32:
            return FillTensorWithRandomDataHelper<float>(tensor, realShape);
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_UINT64:
            return FillTensorWithRandomDataHelper<uint64_t>(tensor, realShape);
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_STRING:
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_COMPLEX64:
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_COMPLEX128:
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_BFLOAT16:
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_UNDEFINED:
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT16:
        default:
            std::cerr << "onnx-cpp-benchmark: Error in FillTensorWithRandomData, unable to create tensor of type "
                    << convertEnumToString(orttype) << std::endl;
            std::exit(1);
    }
}

int main(int argc, char *argv[]) {
    auto onnxfilepath = std::string(argv[1]);
    auto backend = std::string(argv[2]);

    if (backend != "cpu" && backend != "cuda") {
        std::cerr << "ERROR: Unsupported backend " << backend << std::endl;
        return 1;
    }

    // Number of inputs fed to the network fed to the network in one inference run
    int64_t nrOfParallelInputsInOneIteration = 10;

    // Number of iterations
    int64_t numberOfIterations = 1000;

    OrtSessionOptions *sessionOptionsCPtr;

    // Enable CUDA if requested
    OrtCUDAProviderOptionsV2 *cuda_options = nullptr;
    if (backend == "cuda") {
        OrtStatus *status = Ort::GetApi().CreateCUDAProviderOptions(&cuda_options);
        if (Ort::Status statusCxx(status); !statusCxx.IsOK()) {
            std::cerr << "CreateCUDAProviderOptions error " << statusCxx.GetErrorMessage() << std::endl;
            return 1;
        }

        status = Ort::GetApi().SessionOptionsAppendExecutionProvider_CUDA_V2(sessionOptionsCPtr, cuda_options);
        if (Ort::Status statusCxx(status); !statusCxx.IsOK()) {
            std::cerr << "SessionOptionsAppendExecutionProvider_CUDA_V2 error " << statusCxx.GetErrorMessage() <<
                    std::endl;
            return 1;
        }
    }

    Ort::SessionOptions sessionOptions{sessionOptionsCPtr};
    // Ort::Session's constructor is OS-dependent, wants wchar_t* on Windows and char* on other OSs
    // Note: this only works with single-byte characters, such as ASCII or ISO-8859-1,
    // see https://stackoverflow.com/questions/2573834/c-convert-string-or-char-to-wstring-or-wchar-t
    std::basic_string<ORTCHAR_T> networkModelPathAsOrtString(onnxfilepath.begin(), onnxfilepath.end());

    Ort::Env env(OrtLoggingLevel::ORT_LOGGING_LEVEL_WARNING);
    Ort::AllocatorWithDefaultOptions allocator;
    g_ortSession = std::make_unique<Ort::Session>(env, networkModelPathAsOrtString.c_str(), sessionOptions);

    if (g_ortSession == nullptr) {
        std::cerr << "Unable to instantiate the model in: " << onnxfilepath << std::endl;
        return 1;
    }

    // Get size of inputs
    std::vector<Ort::TypeInfo> inputsInfo;
    std::vector<std::string> inputNames;
    for (size_t inputIdx = 0; inputIdx < g_ortSession->GetInputCount(); inputIdx++) {
        inputNames.emplace_back(g_ortSession->GetInputNameAllocated(inputIdx, allocator).get());
        inputsInfo.push_back(g_ortSession->GetInputTypeInfo(inputIdx));
    }

    // Get size of outputs
    std::vector<Ort::TypeInfo> outputsInfo;
    std::vector<std::string> outputNames;
    for (size_t outputIdx = 0; outputIdx < g_ortSession->GetOutputCount(); outputIdx++) {
        outputNames.emplace_back(g_ortSession->GetOutputNameAllocated(outputIdx, allocator).get());
        outputsInfo.push_back(g_ortSession->GetOutputTypeInfo(outputIdx));
    }

    std::cout << "Model " << onnxfilepath << " has " << inputsInfo.size() << " inputs and " << outputsInfo.size()
            << " outputs." << std::endl;

    for (size_t inputIdx = 0; inputIdx < g_ortSession->GetInputCount(); inputIdx++) {
        std::cout << "  Input " << inputIdx << " name: " << inputNames[inputIdx]
                << " type: " << convertEnumToString(inputsInfo[inputIdx].GetTensorTypeAndShapeInfo().GetElementType())
                << " shape: ";
        auto shape = inputsInfo[inputIdx].GetTensorTypeAndShapeInfo().GetShape();
        for (auto i: shape) {
            std::cout << i << ",";
        }
        std::cout << std::endl;
    }

    for (size_t outputIdx = 0; outputIdx < g_ortSession->GetOutputCount(); outputIdx++) {
        std::cout << "  Output " << outputIdx << " name: " << outputNames[outputIdx]
                << " type: " << convertEnumToString(outputsInfo[outputIdx].GetTensorTypeAndShapeInfo().GetElementType())
                << " shape: ";
        auto shape = outputsInfo[outputIdx].GetTensorTypeAndShapeInfo().GetShape();
        for (auto i: shape) {
            std::cout << i << ",";
        }
        std::cout << std::endl;
    }

    std::vector<Ort::Value> inputTensors;
    for (size_t inputIdx = 0; inputIdx < g_ortSession->GetInputCount(); inputIdx++) {
        ONNXTensorElementDataType orttype = inputsInfo[inputIdx].GetTensorTypeAndShapeInfo().GetElementType();
        std::vector<int64_t> shape = inputsInfo[inputIdx].GetTensorTypeAndShapeInfo().GetShape();
        std::vector<int64_t> realShape = shape;
        std::replace(realShape.begin(), realShape.end(), static_cast<int64_t>(-1), nrOfParallelInputsInOneIteration);
        inputTensors.emplace_back(CreateTensor(orttype, realShape));
    }

    std::vector<Ort::Value> outputTensors;
    for (size_t outputIdx = 0; outputIdx < g_ortSession->GetOutputCount(); outputIdx++) {
        ONNXTensorElementDataType orttype = outputsInfo[outputIdx].GetTensorTypeAndShapeInfo().GetElementType();
        auto shape = outputsInfo[outputIdx].GetTensorTypeAndShapeInfo().GetShape();
        std::vector<int64_t> realShape = shape;
        std::replace(realShape.begin(), realShape.end(), static_cast<int64_t>(-1), nrOfParallelInputsInOneIteration);
        outputTensors.emplace_back(CreateTensor(orttype, realShape));
    }

    std::vector<const char *> inputNames_cstr;
    for (const std::string &name: inputNames) {
        inputNames_cstr.push_back(name.c_str());
    }

    std::vector<const char *> outputNames_cstr;
    for (const std::string &name: outputNames) {
        outputNames_cstr.push_back(name.c_str());
    }

    std::vector<Ort::Value *> inputTensor_ptrs;
    for (size_t inputIdx = 0; inputIdx < g_ortSession->GetInputCount(); inputIdx++) {
        inputTensor_ptrs.push_back(&(inputTensors[inputIdx]));
    }

    std::vector<Ort::Value *> outputTensor_ptrs;
    for (size_t outputIdx = 0; outputIdx < g_ortSession->GetOutputCount(); outputIdx++) {
        outputTensor_ptrs.push_back(&(outputTensors[outputIdx]));
    }

    std::cout << "iterations (i.e. number of inference run used for benchmark): " << numberOfIterations << std::endl;
    std::cout << "batch_size (i.e. number of parallel inputs fed to network in one run/iteration): "
            << nrOfParallelInputsInOneIteration << std::endl;

    std::vector<double> durations;
    for (size_t iter = 0; iter < numberOfIterations; iter++) {
        for (size_t inputIdx = 0; inputIdx < g_ortSession->GetInputCount(); inputIdx++) {
            FillTensorWithRandomData(inputTensors[inputIdx]);
        }

        auto start_time = std::chrono::high_resolution_clock::now();
        g_ortSession->Run(Ort::RunOptions(),
                          inputNames_cstr.data(),
                          inputTensors.data(),
                          inputNames_cstr.size(),
                          outputNames_cstr.data(),
                          outputTensors.data(),
                          outputNames_cstr.size());
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = end_time - start_time;
        double duration_s = static_cast<double>(duration.count()) / 1000000.0;
        durations.push_back(duration_s);
    }

    double sum = std::accumulate(std::begin(durations), std::end(durations), 0.0);
    double m = sum / static_cast<double>(durations.size());
    double accum = 0.0;
    std::for_each(std::begin(durations), std::end(durations), [&](const double d) {
        accum += (d - m) * (d - m);
    });

    double stdev = sqrt(accum / (static_cast<double>(durations.size()) - 1));

    std::cerr << "=======================" << std::endl;
    std::cerr << "======= Results =======" << std::endl;
    std::cerr << "=======================" << std::endl;

    std::cerr << "Time (in seconds) for inference: mean:  " << m << " stddev:" << stdev << std::endl;
    std::cerr << "Time (in seconds) for input: mean:  " << m / static_cast<double>(nrOfParallelInputsInOneIteration)
            << std::endl;

    if (backend == "cuda") {
        Ort::GetApi().ReleaseCUDAProviderOptions(cuda_options);
    }

    return 0;
}
