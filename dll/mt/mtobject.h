#pragma once

class MtPropertyList;
class MtDTI;

class MtObject
{
public:
    virtual ~MtObject();

    virtual void unk_1();
    virtual void unk_2();
    virtual void create_property(MtPropertyList *);
    virtual MtDTI *get_dti();
};