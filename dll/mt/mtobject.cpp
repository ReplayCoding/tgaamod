#include <memory>
#include "mt/mtdti.h"
#include "mt/mtobject.h"
#include "mt/mtproperty.h"

std::unique_ptr<MtPropertyList> MtObject::create_property()
{
    std::unique_ptr<MtPropertyList> prop_list = MtDTI::from<MtPropertyList>()->new_instance_owned<MtPropertyList>();
    this->create_property(prop_list.get());

    return prop_list;
};
