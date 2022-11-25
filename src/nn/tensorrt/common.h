/*
 * Copyright (c) 2021, NVIDIA CORPORATION. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef TENSORRT_COMMON_H
#define TENSORRT_COMMON_H

#include <cuda_runtime_api.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "NvInfer.h"
#include "NvInferPlugin.h"
#include "logger.h"

using namespace nvinfer1;
using namespace plugin;

#define CHECK(status)                                                  \
    do {                                                               \
        auto ret = (status);                                           \
        if (ret != 0) {                                                \
            sample::gLogError << "Cuda failure: " << ret << std::endl; \
            abort();                                                   \
        }                                                              \
    } while (0)

namespace samplesCommon {

struct InferDeleter {
    template <typename T>
    void operator()(T* obj) const {
        delete obj;
    }
};

template <typename T>
using SampleUniquePtr = std::unique_ptr<T, InferDeleter>;

// Ensures that every tensor used by a network has a scale.
//
// All tensors in a network must have a range specified if a calibrator is not used.
// This function is just a utility to globally fill in missing scales for the entire network.
//
// If a tensor does not have a scale, it is assigned inScales or outScales as follows:
//
// * If the tensor is the input to a layer or output of a pooling node, its scale is assigned
// inScales.
// * Otherwise its scale is assigned outScales.
//
// The default parameter values are intended to demonstrate, for final layers in the network,
// cases where scaling factors are asymmetric.
inline void setAllTensorScales(INetworkDefinition* network,
                               float inScales = 2.0f,
                               float outScales = 4.0f) {
    // Ensure that all layer inputs have a scale.
    for (int i = 0; i < network->getNbLayers(); i++) {
        auto layer = network->getLayer(i);
        for (int j = 0; j < layer->getNbInputs(); j++) {
            ITensor* input{layer->getInput(j)};
            // Optional inputs are nullptr here and are from RNN layers.
            if (input != nullptr && !input->dynamicRangeIsSet()) {
                input->setDynamicRange(-inScales, inScales);
            }
        }
    }

    // Ensure that all layer outputs have a scale.
    // Tensors that are also inputs to layers are ingored here
    // since the previous loop nest assigned scales to them.
    for (int i = 0; i < network->getNbLayers(); i++) {
        auto layer = network->getLayer(i);
        for (int j = 0; j < layer->getNbOutputs(); j++) {
            ITensor* output{layer->getOutput(j)};
            // Optional outputs are nullptr here and are from RNN layers.
            if (output != nullptr && !output->dynamicRangeIsSet()) {
                // Pooling must have the same input and output scales.
                if (layer->getType() == LayerType::kPOOLING) {
                    output->setDynamicRange(-inScales, inScales);
                } else {
                    output->setDynamicRange(-outScales, outScales);
                }
            }
        }
    }
}

inline void setAllDynamicRanges(INetworkDefinition* network,
                                float inRange = 2.0f,
                                float outRange = 4.0f) {
    return setAllTensorScales(network, inRange, outRange);
}

inline void enableDLA(IBuilder* builder,
                      IBuilderConfig* config,
                      int useDLACore,
                      bool allowGPUFallback = true) {
    if (useDLACore >= 0) {
        if (builder->getNbDLACores() == 0) {
            std::cerr << "Trying to use DLA core " << useDLACore
                      << " on a platform that doesn't have any DLA cores" << std::endl;
            assert("Error: use DLA core on a platfrom that doesn't have any DLA cores" && false);
        }
        if (allowGPUFallback) {
            config->setFlag(BuilderFlag::kGPU_FALLBACK);
        }
        if (!config->getFlag(BuilderFlag::kINT8)) {
            // User has not requested INT8 Mode.
            // By default run in FP16 mode. FP32 mode is not permitted.
            config->setFlag(BuilderFlag::kFP16);
        }
        config->setDefaultDeviceType(DeviceType::kDLA);
        config->setDLACore(useDLACore);
        config->setFlag(BuilderFlag::kSTRICT_TYPES);
    }
}

}  // namespace samplesCommon

inline std::ostream& operator<<(std::ostream& os, const nvinfer1::Dims& dims) {
    os << "(";
    for (int i = 0; i < dims.nbDims; ++i) {
        os << (i ? ", " : "") << dims.d[i];
    }
    return os << ")";
}

#endif  // TENSORRT_COMMON_H
