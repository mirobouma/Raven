#pragma once
#include <glm/glm.hpp>


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

namespace Raven
{
    template <typename T>
    class VectorBase {
    public:
        virtual T magnitude() = 0;
        virtual T* data() = 0;
    };

    template <typename T>
    class Vector2Base : public VectorBase<T> {
    public:
        Vector2Base() : _vector() {}
        Vector2Base(T value) { _vector = { value, value }; }
        Vector2Base(T x, T y) { _vector = { x, y }; }

        Vector2Base(const Vector2Base<T>&) = default;
        Vector2Base(Vector2Base<T>&& other) { _vector = std::move(other._vector); }
        Vector2Base<T>&& operator= (const Vector2Base<T>& other)
        {
            if (&other == this)
                return *this;

            _vector = Vector2Base<T>(other._vector.x, other._vector.y);
        }

        DEFINE_OPERATORS(T, Vector2Base<T>)

            virtual T magnitude() override { return (T)_vector.length(); }
        virtual T* data() override { return (T*)&_vector; }

        inline Vector2Base<T> normalized() const { return glm::normalize(_vector); }
        inline Vector2Base<T> dot(const Vector2Base<T>& other) const { return blaze::dot(this, other); }

        inline T x() const { return _vector[0]; }
        inline T y() const { return _vector[1]; }

        inline void set_x(T value) { _vector[0] = value; }
        inline void set_y(T value) { _vector[1] = value; }

        // Simple move assignment operator
        Vector2Base& operator=(Vector2Base&& other) {
            _vector = std::move(other._vector);
            return *this;
        }
    protected:
        glm::vec<2, T, glm::packed_highp> _vector;
    };

    typedef Vector2Base<int> Vector2Int;
    typedef Vector2Base<double> Vector2Double;
    typedef Vector2Base<float> Vector2;

    template <typename T>
    class Vector3Base : public VectorBase<T> {
    public:
        Vector3Base() : _vector() {}
        Vector3Base(T value) { _vector = { value, value, value }; }
        Vector3Base(T x, T y, T z) { _vector = { x, y, z }; }

        Vector3Base(const Vector3Base<T>&) = default;
        Vector3Base(Vector3Base<T>&& other) { _vector = std::move(other._vector); }
        Vector3Base<T>&& operator= (const Vector3Base<T>& other)
        {
            if (&other == this)
                return *this;

            _vector = Vector3Base<T>(other._vector[0], other._vector[1], other._vector[2]);
        }

        DEFINE_OPERATORS(T, Vector3Base<T>)

            virtual T magnitude() override { return _vector.length(); }
        virtual T* data() override { return (T*)&_vector; }

        inline Vector3Base<T> normalized() const { return glm::normalize(_vector); }
        inline Vector3Base<T> dot(const Vector3Base<T>& other) const { return glm::dot(this, other); }
        inline Vector3Base<T> cross(const Vector3Base<T>& other) const { return glm::cross(this, other); }

        inline T x() const { return _vector[0]; }
        inline T y() const { return _vector[1]; }
        inline T z() const { return _vector[2]; }

        inline void set_x(T value) { _vector[0] = value; }
        inline void set_y(T value) { _vector[1] = value; }
        inline void set_z(T value) { _vector[2] = value; }

        // Simple move assignment operator
        Vector3Base& operator=(Vector3Base&& other)
        {
            _vector = std::move(other._vector);
            return *this;
        }

    protected:
        glm::vec<3, T, glm::packed_highp> _vector;
    };

    typedef Vector3Base<int> Vector3Int;
    typedef Vector3Base<double> Vector3Double;
    typedef Vector3Base<float> Vector3;

    template <typename T>
    class Vector4Base : public VectorBase<T> {
    public:
        Vector4Base() : _vector() {}
        Vector4Base(T value) { _vector = { value, value, value, value }; }
        Vector4Base(T x, T y, T z) { _vector = { x, y, z, w }; }

        Vector4Base(const Vector4Base<T>&) = default;
        Vector4Base(Vector4Base<T>&& other) { _vector = std::move(other._vector); }
        Vector4Base<T>&& operator= (const Vector4Base<T>& other)
        {
            if (&other == this)
                return *this;

            _vector = Vector4Base<T>(other._vector[0], other._vector[1], other._vector[2], other._vector[3]);
        }

        DEFINE_OPERATORS(T, Vector4Base<T>)

            virtual T magnitude() override { return _vector.length(); }
        virtual T* data() override { return (T*)&_vector; }

        inline Vector4Base<T> normalized() const { return glm::normalize(_vector); }
        inline Vector4Base<T> dot(const Vector4Base<T>& other) const { return glm::dot(this, other); }

        inline T x() const { return _vector[0]; }
        inline T y() const { return _vector[1]; }
        inline T z() const { return _vector[2]; }
        inline T w() const { return _vector[3]; }

        inline void set_x(T value) { _vector[0] = value; }
        inline void set_y(T value) { _vector[1] = value; }
        inline void set_z(T value) { _vector[2] = value; }
        inline void set_w(T value) { _vector[3] = value; }

        // Simple move assignment operator
        Vector4Base& operator=(Vector4Base&& other)
        {
            _vector = std::move(other._vector);
            return *this;
        }

    protected:
        glm::vec<4, T, glm::packed_highp> _vector;
    };

    typedef Vector4Base<int> Vector4Int;
    typedef Vector4Base<double> Vector4Double;
    typedef Vector4Base<float> Vector4;
}

#undef DEFINE_OPERATORS