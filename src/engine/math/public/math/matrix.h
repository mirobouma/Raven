#pragma once
#include <cinttypes>

template <typename T>
class MatrixBase {
public:
    virtual T set(int32_t x, int32_t y, T value) = 0;
};

#define DEFINE_OPERATORS(ValueType, Type)                                   \
    friend Type operator+ (const Type& first, const Type& second) {         \
        return Type(first._vector + second._vector);                        \
    }                                                                       \
                                                                            \
    friend Type operator- (const Type& first, const Type& second) {         \
        return Type(first._vector - second._vector);                        \
    }                                                                       \
                                                                            \
    friend Type operator/ (const Type& first, const Type& second) {         \
        return Type(first._vector / second._vector);                        \
    }                                                                       \
                                                                            \
    friend Type operator* (const Type& first, const Type& second) {         \
        return Type(first._vector * second._vector);                        \
    }                                                                       \
                                                                            \
    friend Type operator* (ValueType first,                                 \
        const Type& second) {                                               \
            return Type(first * second._vector);                            \
    }                                                                       \
                                                                            \
    friend Type operator* (const Type& first,                               \
        ValueType second) {                                                 \
            return Type(first._vector * second);                            \
    }                                                                       \
                                                                            \
friend Type operator/ (ValueType first,                                     \
        const Type& second) {                                               \
            return Type(first / second._vector);                            \
    }                                                                       \
                                                                            \
    friend Type operator/ (const Type& first,                               \
        ValueType second) {                                                 \
            return Type(first._vector / second);                            \
    }                                                                       \
                                                                            \
    Type& operator*= (const Type& first) {                                  \
        _vector *= first._vector;                                           \
        return *this;                                                       \
    }                                                                       \
                                                                            \
    Type& operator/= (const Type& first) {                                  \
        _vector /= first._vector;                                           \
        return *this;                                                       \
    }                                                                       \
                                                                            \
    Type& operator+= (const Type& first) {                                  \
        _vector += first._vector;                                           \
        return *this;                                                       \
    }                                                                       \
                                                                            \
    Type& operator-= (const Type& first) {                                  \
        _vector -= first._vector;                                           \
        return *this;                                                       \
    }                                                                       \
                                                                            \


template <typename T>
class Vector2Base : public VectorBase<T> {
public:
    Vector2Base() : _vector() {}
    Vector2Base(T value) { _vector = { value, value }; }
    Vector2Base(T x, T y) { _vector = { x, y }; }
    Vector2Base(blaze::StaticVector<T, 2UL> vector) { _vector = vector; }

    Vector2Base(const Vector2Base<T>&) = default;
    Vector2Base(Vector2Base<T>&& other) { _vector = std::move(other._vector); }
    Vector2Base<T>&& operator= (const Vector2Base<T>& other)
    {
        if (&other == this)
            return *this;

        _vector = Vector2Base<T>(other._vector[0], other._vector[1]);
    }

    DEFINE_OPERATORS(T, Vector2Base<T>)

        virtual T magnitude() override { return blaze::length(_vector); }

    inline Vector2Base<T> normalized() const { return blaze::normalize(_vector); }
    inline Vector2Base<T> dot(const Vector2Base<T>& other) const { return blaze::dot(this, other); }

    inline T x() { return _vector[0]; }
    inline T y() { return _vector[1]; }

    inline void set_x(T value) { _vector[0] = value; }
    inline void set_y(T value) { _vector[1] = value; }

    // Simple move assignment operator
    Vector2Base& operator=(Vector2Base&& other) {
        member = std::move(other.member);
        return *this;
    }


protected:
    blaze::StaticVector<T, 2UL> _vector;
};
