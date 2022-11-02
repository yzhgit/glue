//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include "glue/nn/types.h"

#include <cstdint>
#include <iostream>
#include <memory>
#include <vector>

namespace glue {
namespace ml {

using ShapeType = std::vector<uint32_t>;

class Quantization final {
  public:
    Quantization() : m_type(QuantType::NONE) {}
    Quantization(QuantType type, float scale, int32_t zero_point)
        : m_type(type), m_scales({scale}), m_zero_points({zero_point}) {}
    Quantization(QuantType type, int32_t channel_dim, std::vector<float> scales,
                 std::vector<int32_t> zero_points)
        : m_type(type), m_channel_dim(channel_dim), m_scales(std::move(scales)),
          m_zero_points(std::move(zero_points)) {}

    QuantType &Type() { return m_type; }
    const QuantType &Type() const { return m_type; }
    Quantization &SetType(QuantType type) {
        m_type = type;
        return *this;
    }

    int32_t &ChannelDim() { return m_channel_dim; }
    const int32_t &ChannelDim() const { return m_channel_dim; }
    Quantization &SetChannelDim(int32_t channel_dim) {
        m_channel_dim = channel_dim;
        return *this;
    }

    std::vector<float> &Scales() { return m_scales; }
    const std::vector<float> &Scales() const { return m_scales; }
    Quantization &SetScales(std::vector<float> scales) {
        m_scales = scales;
        return *this;
    }

    std::vector<int32_t> &ZeroPoints() { return m_zero_points; }
    const std::vector<int32_t> &ZeroPoints() const { return m_zero_points; }
    Quantization &SetZeroPoints(std::vector<int32_t> zero_points) {
        m_zero_points = zero_points;
        return *this;
    }

  protected:
    QuantType m_type{QuantType::NONE};
    int32_t m_channel_dim{-1};
    std::vector<float> m_scales;
    std::vector<int32_t> m_zero_points;
};

class TensorInfo final {
  public:
    TensorInfo() {}
    TensorInfo(DataType datatype, const ShapeType &shape)
        : m_DataType(datatype), m_Shape(shape) {}
    TensorInfo(DataType datatype, const ShapeType &shape,
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

    const DataType GetDataType() const { return m_DataType; }
    TensorInfo &SetDataType(DataType datatype) {
        m_DataType = datatype;
        return *this;
    }

    const ShapeType &GetShape() const { return m_Shape; }
    TensorInfo &SetShape(ShapeType &shape) {
        m_Shape = shape;
        return *this;
    }

    uint32_t GetNumDimensions() const;

    uint32_t GetNumElements() const;

    uint32_t GetNumBytes() const;

    const Quantization GetQuantization() const { return m_Quantization; }
    TensorInfo &SetQuantization(Quantization &quantization) {
        m_Quantization = quantization;
        return *this;
    }

  private:
    DataType m_DataType;
    ShapeType m_Shape;
    Quantization m_Quantization;
};

class Tensor final {
  public:
    Tensor();

    Tensor(const TensorInfo &info, void *data) : m_Info(info), m_Data(data) {}

    /// Tensors are copyable.
    Tensor(const Tensor &other);

    /// Tensors are copyable.
    Tensor &operator=(const Tensor &);

    const ShapeType &GetShape() const { return m_Info.GetShape(); }

    const DataType GetDataType() const { return m_Info.GetDataType(); }

    const Quantization GetQuantization() const {
        return m_Info.GetQuantization();
    }

    uint32_t GetNumDimensions() const { return m_Info.GetNumDimensions(); }

    uint32_t GetNumElements() const { return m_Info.GetNumElements(); }

    uint32_t GetNumBytes() const { return m_Info.GetNumBytes(); }

    void *GetData() const { return m_Data; }

  protected:
    /// Protected destructor to stop users from making these
    /// (could still new one on the heap and then leak it...)
    ~Tensor() {}

  private:
    TensorInfo m_Info;
    void *m_Data;
};

template <typename T>
inline std::vector<T> Quantize(const std::vector<float> &data, float scale,
                               int32_t zero_point) {
    std::vector<T> q;
    for (const auto &f : data) {
        q.push_back(static_cast<T>(std::max<float>(
            std::numeric_limits<T>::min(),
            std::min<float>(static_cast<float>(std::numeric_limits<T>::max()),
                            std::round(zero_point + (f / scale))))));
    }
    return q;
}

template <typename T>
inline std::vector<float> Dequantize(const std::vector<T> &data, float scale,
                                     int32_t zero_point) {
    std::vector<float> f;
    f.reserve(data.size());
    for (const T &q : data) {
        f.push_back(scale * (q - zero_point));
    }
    return f;
}

} // namespace ml
} // namespace glue
