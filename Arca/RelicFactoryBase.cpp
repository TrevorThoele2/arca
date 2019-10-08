#include "RelicFactoryBase.h"

namespace Arca
{
    RelicFactoryBase::RelicFactoryBase(Reliquary& owner) : owner(&owner)
    {}

    RelicFactoryBase::~RelicFactoryBase() = default;

    Reliquary& RelicFactoryBase::Owner()
    {
        return *owner;
    }

    const Reliquary& RelicFactoryBase::Owner() const
    {
        return *owner;
    }
}