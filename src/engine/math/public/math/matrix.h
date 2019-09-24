/*#pragma once
#include <cinttypes>
#include "blaze/Math.h"

template <typename T>
class MatrixBase {
public:
    virtual T set(int32_t x, int32_t y, T value) = 0;
};

#define DEFINE_OPERATORS(ValueType, Type)                                   \
    friend Type operator+ (const Type& first, const Type& second) {         \
        return Type(first._matrix + second._matrix);                        \
    }                                                                       \
                                                                            \
    friend Type operator- (const Type& first, const Type& second) {         \
        return Type(first._matrix - second._matrix);                        \
    }                                                                       \
                                                                            \
    friend Type operator/ (const Type& first, const Type& second) {         \
        return Type(first._matrix / second._matrix);                        \
    }                                                                       \
                                                                            \
    friend Type operator* (const Type& first, const Type& second) {         \
        return Type(first._matrix * second._matrix);                        \
    }                                                                       \
                                                                            \
    friend Type operator* (ValueType first,                                 \
        const Type& second) {                                               \
            return Type(first * second._matrix);                            \
    }                                                                       \
                                                                            \
    friend Type operator* (const Type& first,                               \
        ValueType second) {                                                 \
            return Type(first._matrix * second);                            \
    }                                                                       \
                                                                            \
friend Type operator/ (ValueType first,                                     \
        const Type& second) {                                               \
            return Type(first / second._matrix);                            \
    }                                                                       \
                                                                            \
    friend Type operator/ (const Type& first,                               \
        ValueType second) {                                                 \
            return Type(first._matrix / second);                            \
    }                                                                       \
                                                                            \
    Type& operator*= (const Type& first) {                                  \
        _matrix *= first._matrix;                                           \
        return *this;                                                       \
    }                                                                       \
                                                                            \
    Type& operator/= (const Type& first) {                                  \
        _matrix /= first._matrix;                                           \
        return *this;                                                       \
    }                                                                       \
                                                                            \
    Type& operator+= (const Type& first) {                                  \
        _matrix += first._matrix;                                           \
        return *this;                                                       \
    }                                                                       \
                                                                            \
    Type& operator-= (const Type& first) {                                  \
        _matrix -= first._matrix;                                           \
        return *this;                                                       \
    }                                                                       \
                                                                            \


template <typename T>
class Matrix2x2 : public MatrixBase<T> {
public:
    Matrix2x2() { _matrix = blaze::Matrix<int>(3UL); }
    Matrix2x2(blaze::StaticMatrix<T, 2UL, 2UL> matrix) { _matrix = _matrix; }

    Matrix2x2(const Vector2Base<T>&) = default;
    Matrix2x2(Matrix2x2<T>&& other) { _matrix = std::move(other._matrix); }
    Matrix2x2<T>&& operator= (const Vector2Base<T>& other)
    {
        if (&other == this)
            return *this;

        _matrix = Matrix2x2<T>(other.);
    }

    DEFINE_OPERATORS(T, Matrix2x2<T>)

        virtual T magnitude() override { return blaze::length(_vector); }

    inline Vector2Base<T> normalized() const { return blaze::normalize(_vector); }
    inline Vector2Base<T> dot(const Vector2Base<T>& other) const { return blaze::dot(this, other); }

    // Simple move assignment operator
    Vector2Base& operator=(Vector2Base&& other) {
        member = std::move(other.member);
        return *this;
    }


protected:
    blaze::StaticMatrix<T, 2UL, 2UL> _matrix;
};

#undef DEFINE_OPERATORS */