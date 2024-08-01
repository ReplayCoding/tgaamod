#pragma once
#include <cstdint>
#include <cstddef>
#include <string>

#include "mt/mtobject.h"

class MtProperty
{
public:
    enum class PropType : uint32_t
    {
        UNDEFINED = 0,
        CLASS,
        CLASSREF,
        BOOL,
        U8,
        U16,
        U32,
        U64,
        S8,
        S16,
        S32,
        S64,
        F32,
        F64,
        STRING,
        COLOR,
        POINT,
        SIZE,
        RECT,
        MATRIX44,
        VECTOR3,
        VECTOR4,
        QUATERNION,
        PROPERTY,
        EVENT,
        GROUP,
        PAGEBEGIN,
        PAGEEND,
        EVENT32,
        ARRAY,
        PROPERTYLIST,
        GROUPEND,
        CSTRING,
        TIME,
        FLOAT2,
        FLOAT3,
        FLOAT4,
        FLOAT3X3,
        FLOAT4X3,
        FLOAT4X4,
        EASECURVE,
        LINE,
        LINESEGMENT,
        RAY,
        PLANE,
        SPHERE,
        CAPSULE,
        AABB,
        OBB,
        CYLINDER,
        TRIANGLE,
        CONE,
        TORUS,
        ELLPSOID,
        RANGE,
        RANGEF,
        RANGEU16,
        HERMITECURVE,
        ENUMLIST,
        FLOAT3X4,
        LINESEGMENT4,
        AABB4,
        OSCILLATOR,
        VARIABLE,
        VECTOR2,
        MATRIX33,
        RECT3D_XZ,
        RECT3D,
        RECT3D_COLLISION,
        PLANE_XZ,
        RAY_Y,
        POINTF,
        SIZEF,
        RECTF,
        EVENT64,

        END,
    };

    static const uint32_t ATTR_DYNAMIC = 128;

    inline char *name() { return m_name; };

    inline uint32_t type() { return m_bitfield_0x8 & 0xFFFF; };
    std::string type_name();

    inline uint32_t attr() { return m_bitfield_0x8 >> 16; };
    inline MtProperty *prev()
    {
        static_assert(offsetof(MtProperty, m_prev) == 0x48);

        return m_prev;
    };

    char *get_cstring()
    {
        return get_value<char *, PropType::CSTRING>();
    };

    MtObject *get_classref()
    {
        return get_value<MtObject *, PropType::CLASSREF>();
    };

    float get_f32()
    {
        return get_value<float, PropType::F32>();
    };

private:
    template <typename T, PropType expected_type>
    T get_value()
    {

        // TODO: handle customs
        if (static_cast<uint32_t>(expected_type) != type())
        {
            throw new std::runtime_error("prop type doesn't match expected type!");
        }

        using GetterFuncPtrType = T (*)(MtObject *);
        if ((attr() & ATTR_DYNAMIC) != 0)
        {
            // TODO: how does this handle non-trivial types?
            if ((reinterpret_cast<uintptr_t>(m_getter_or_address) & 1) != 0)
            {
                throw new std::runtime_error("prop getter has low bit set, what is this?");
            }

            GetterFuncPtrType getter = reinterpret_cast<GetterFuncPtrType>(m_getter_or_address);
            return getter(m_owner);
        }
        else
        {
            if (m_getter_or_address == nullptr)
            {
                throw new std::runtime_error("prop value address is null!");
            }

            return *reinterpret_cast<T *>(m_getter_or_address);
        }
    }

    // NOTE: This is actually SHIFT-JIS encoded. This WILL cause problems later
    char *m_name;
    uint32_t m_bitfield_0x8;
    MtObject *m_owner;

    void *m_getter_or_address;
    uint8_t m_pad_20[8];
    void *m_setter;
    uint8_t m_pad_30[16];

    MtProperty *m_next;
    MtProperty *m_prev;
};

// Provides a formatter for fmt
std::string format_as(MtProperty::PropType type);

class MtPropertyList : public MtObject
{
public:
    static const uint32_t DTI_HASH = 0x2a031018;

    inline MtProperty *elements() { return m_element; };
    MtProperty *find_property(const char *name);

private:
    MtProperty *m_element;
};