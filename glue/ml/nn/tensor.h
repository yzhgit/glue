//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include "ml/nn/types.h"

#include <cstdint>
#include <iostream>
#include <memory>
#include <vector>

namespace glue {
namespace ml {

using TensorShape = std::vector<uint32_t>;

class Quantization {
  public:
    Quantization() : type_(QuantType::NONE) {}
    Quantization(QuantType type, float scale, int32_t zero_point)
        : type_(type), scales_({scale}), zero_points_({zero_point}) {}
    Quantization(QuantType type, int32_t channel_dim, std::vector<float> scales,
                 std::vector<int32_t> zero_points)
        : type_(type), channel_dim_(channel_dim), scales_(std::move(scales)),
          zero_points_(std::move(zero_points)) {}

    QuantType &Type() { return type_; }
    const QuantType &Type() const { return type_; }
    Quantization &SetType(QuantType type) {
        this->type_ = type;
        return *this;
    }

    int32_t &ChannelDim() { return this->channel_dim_; }
    const int32_t &ChannelDim() const { return this->channel_dim_; }
    Quantization &SetChannelDim(int32_t channel_dim) {
        this->channel_dim_ = channel_dim;
        return *this;
    }

    std::vector<float> &Scales() { return this->scales_; }
    const std::vector<float> &Scales() const { return this->scales_; }
    Quantization &SetScales(std::vector<float> scales) {
        this->scales_ = scales;
        return *this;
    }

    std::vector<int32_t> &ZeroPoints() { return this->zero_points_; }
    const std::vector<int32_t> &ZeroPoints() const {
        return this->zero_points_;
    }
    Quantization &SetZeroPoints(std::vector<int32_t> zero_points) {
        this->zero_points_ = zero_points;
        return *this;
    }

  protected:
    QuantType type_{QuantType::NONE};
    int32_t channel_dim_{-1};
    std::vector<float> scales_;
    std::vector<int32_t> zero_points_;
};

class TensorInfo {
  public:
    TensorInfo() {}

    TensorInfo(DataType datatype, const TensorShape &shape)
        : m_DataType(datatype), m_Shape(shape) {}

    TensorInfo(DataType datatype, const TensorShape &shape,
               const Quantization &quantization)
        : TensorInfo(datatype, shape) {
        m_Quantization = quantization;
    }

    TensorInfo(const TensorInfo &other) {
        m_DataType = other.m_DataType;
        m_Shape = other.m_Shape;
        m_Quantization = other.m_Quantization;
    }

    TensorInfo &operator=(const TensorInfo &other) {
        m_DataType = other.m_DataType;
        m_Shape = other.m_Shape;
        m_Quantization = other.m_Quantization;
        return *this;
    }

    bool operator==(const TensorInfo &other) const;
    bool operator!=(const TensorInfo &other) const;

    DataType GetDataType() const { return m_DataType; }

    TensorInfo &SetDataType(DataType datatype) {
        m_DataType = datatype;
        return *this;
    }

    const TensorShape &GetShape() const { return m_Shape; }

    TensorShape &GetShape() { return m_Shape; }

    TensorInfo &SetShape(TensorShape &shape) {
        m_Shape = shape;
        return *this;
    }

    Quantization GetQuantization() const { return m_Quantization; }

    TensorInfo &SetQuantization(Quantization &quantization) {
        m_Quantization = quantization;
        return *this;
    }

    unsigned int GetNumBytes() const;

  private:
    DataType m_DataType;
    TensorShape m_Shape;
    Quantization m_Quantization;
};

class Tensor {
  public:
    Tensor();

    Tensor(const TensorInfo &info, void *data);

    /// Tensors are copyable.
    Tensor(const Tensor &other);

    /// Tensors are copyable.
    Tensor &operator=(const Tensor &);

    const TensorInfo &GetInfo() const { return m_Info; }
    TensorInfo &GetInfo() { return m_Info; }
    const TensorShape &GetShape() const { return m_Info.GetShape(); }
    TensorShape &GetShape() { return m_Info.GetShape(); }

    DataType GetDataType() const { return m_Info.GetDataType(); }
    unsigned int GetNumDimensions() const { return m_Info.GetNumDimensions(); }
    unsigned int GetNumBytes() const { return m_Info.GetNumBytes(); }
    unsigned int GetNumElements() const { return m_Info.GetNumElements(); }

    void *GetData() const { return m_Data; }

  protected:
    /// Protected destructor to stop users from making these
    /// (could still new one on the heap and then leak it...)
    ~Tensor() {}

    void *m_Data;

  private:
    TensorInfo m_Info;
};

} // namespace ml
} // namespace glue
