#pragma once

#include <vector>

#include "RelicTypeDescription.h"

namespace Arca
{
    class RelicTypeDescriptionGroup
    {
    public:
        using List = std::vector<RelicTypeDescription>;

        using iterator = List::iterator;
        using const_iterator = List::const_iterator;
    public:
        RelicTypeDescriptionGroup();
        explicit RelicTypeDescriptionGroup(List list);

        [[nodiscard]] bool IsIn(const TypeHandle& typeHandle) const;

        [[nodiscard]] iterator begin();
        [[nodiscard]] const_iterator begin() const;
        [[nodiscard]] iterator end();
        [[nodiscard]] const_iterator end() const;
    private:
        List list;
    };
}