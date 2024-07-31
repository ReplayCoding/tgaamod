#include <string>
#include <fmt/format.h>

#include "mt/mtproperty.h"

const char *PROP_TYPE_NAMES[] = {
    "undefined",
    "class",
    "classref",
    "bool",
    "u8",
    "u16",
    "u32",
    "u64",
    "s8",
    "s16",
    "s32",
    "s64",
    "f32",
    "f64",
    "string",
    "color",
    "point",
    "size",
    "rect",
    "matrix44",
    "vector3",
    "vector4",
    "quaternion",
    "property",
    "event",
    "group",
    "pagebegin",
    "pageend",
    "event32",
    "array",
    "propertylist",
    "groupend",
    "cstring",
    "time",
    "float2",
    "float3",
    "float4",
    "float3x3",
    "float4x3",
    "float4x4",
    "easecurve",
    "line",
    "linesegment",
    "ray",
    "Plane",
    "sphere",
    "capsule",
    "aabb",
    "obb",
    "cylinder",
    "triangle",
    "cone",
    "torus",
    "ellpsoid",
    "range",
    "rangef",
    "rangeu16",
    "hermitecurve",
    "enumlist",
    "float3x4",
    "linesegment4",
    "aabb4",
    "oscillator",
    "variable",
    "vector2",
    "matrix33",
    "rect3d_xz",
    "rect3d",
    "rect3d_collision",
    "plane_xz",
    "ray_y",
    "pointf",
    "sizef",
    "rectf",
    "event64"};

static_assert(
    (sizeof(PROP_TYPE_NAMES) / sizeof(const char *)) == static_cast<uint32_t>(MtProperty::PropType::END),
    "Length of PROP_TYPE_NAMES must be equal to the number of enum items");

std::string MtProperty::type_name()
{
    if (type() < static_cast<uint32_t>(MtProperty::PropType::END))
    {
        return PROP_TYPE_NAMES[type()];
    }

    return fmt::format("custom ({})", type());
};

MtProperty *MtPropertyList::find_property(const char *name)
{
    MtProperty *prop = elements();
    do
    {
        std::string_view prop_name = std::string_view(prop->name());
        if (prop_name == name)
        {
            return prop;
        }
    } while ((prop = prop->prev()) != nullptr);

    return nullptr;
}