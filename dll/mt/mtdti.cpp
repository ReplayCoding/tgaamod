#include <cstdint>

#include "mt/mtdti.h"

const uintptr_t MTDTI_FROM_ADDR = 0x1402e5700;
MtDTI *MtDTI::from_hash(uint32_t hash)
{
    auto mtdti_from = reinterpret_cast<MtDTI *(*)(uint32_t)>(MTDTI_FROM_ADDR);
    return mtdti_from(hash);
};