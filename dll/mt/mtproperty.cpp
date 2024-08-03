#include <string>
#include <fmt/format.h>

#include "mt/mtdti.h"
#include "mt/mtcolor.h"
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

std::string MtProperty::formatted_value()
{
    switch (type())
    {
    case static_cast<uint32_t>(PropType::CSTRING):
    {
        char* string = get_cstring();
        if (string == nullptr) {
            return "null [cstring]";
        };

        return string;
    }
    case static_cast<uint32_t>(PropType::F32):
    {
        return std::to_string(get_f32());
    }
    case static_cast<uint32_t>(PropType::S32):
    {
        return std::to_string(get_s32());
    }
    case static_cast<uint32_t>(PropType::U32):
    {
        return std::to_string(get_u32());
    }
    case static_cast<uint32_t>(PropType::BOOL):
    {
        return std::to_string(get_bool());
    }
    case static_cast<uint32_t>(PropType::COLOR):
    {
        MtColor color = get_color();
        return fmt::format("#{:02x}{:02x}{:02x}{:02x}", color.m_r, color.m_g, color.m_b, color.m_a);
    }
    case static_cast<uint32_t>(PropType::CLASSREF):
    {
        MtObject *classref = get_classref();

        if (classref == nullptr)
        {
            return "null [classref]";
        }
        else
        {
            return classref->get_dti()->name();
        }
    }
    case static_cast<uint32_t>(PropType::CLASS):
    {
        MtObject *class_ = get_class();

        if (class_ == nullptr)
        {
            return "null [class]";
        }
        else
        {
            return class_->get_dti()->name();
        }
    }
    default:
    {
        return fmt::format("<UNHANDLED VALUE TYPE {}>", type_name());
    }
    };
}

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