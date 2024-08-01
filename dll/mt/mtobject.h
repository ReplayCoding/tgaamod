#pragma once
#include <memory>

class MtDTI;
class MtPropertyList;

class MtObject
{
public:
    virtual ~MtObject();

    virtual void unk_1();
    virtual void unk_2();
    virtual void create_property(MtPropertyList *);
    virtual MtDTI *get_dti();

    std::unique_ptr<MtPropertyList> create_property();
};