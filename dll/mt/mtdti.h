#pragma once
#include <memory>
#include <cstdint>

class MtObject;

class MtDTI
{
public:
    virtual ~MtDTI();
    virtual MtObject *new_instance();

    template <typename T>
    inline std::unique_ptr<T> new_instance_owned()
    {
        static_assert(std::is_base_of_v<MtObject, T> == true, "Cannot create instance of non-MtObject type from DTI");

        MtObject *instance = new_instance();
        return std::unique_ptr<T>(static_cast<T *>(instance));
    }
    inline const char *name() const { return m_name; };
    static MtDTI *from_hash(uint32_t hash);

private:
    char *m_name;

    MtDTI *m_sibling;
    MtDTI *m_child;
    MtDTI *m_parent;
    MtDTI *m_hash_sibling;

    uint32_t m_size_allocindex_attr;
    uint32_t m_hash;
};