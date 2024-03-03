#include "RelicTypeDescriptionGroup.h"

namespace Arca
{
    RelicTypeDescriptionGroup::RelicTypeDescriptionGroup() = default;

    RelicTypeDescriptionGroup::RelicTypeDescriptionGroup(List list) : list(std::move(list))
    {}

    bool RelicTypeDescriptionGroup::IsIn(const TypeHandle& typeHandle) const
    {
        for (auto& loop : list)
            if (loop.typeHandle == typeHandle)
                return true;

        return false;
    }

    RelicTypeDescriptionGroup::iterator RelicTypeDescriptionGroup::begin()
    {
        return list.begin();
    }

    RelicTypeDescriptionGroup::const_iterator RelicTypeDescriptionGroup::begin() const
    {
        return list.begin();
    }

    RelicTypeDescriptionGroup::iterator RelicTypeDescriptionGroup::end()
    {
        return list.end();
    }

    RelicTypeDescriptionGroup::const_iterator RelicTypeDescriptionGroup::end() const
    {
        return list.end();
    }
}