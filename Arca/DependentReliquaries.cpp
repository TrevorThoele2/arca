#include "DependentReliquaries.h"

namespace Arca
{
    DependentReliquaries::Entry::Entry(int id, Reliquary& reliquary) : id(id), reliquary(&reliquary)
    {}

    DependentReliquaries::DependentReliquaries(Init init) : Curator(init)
    {}

    void DependentReliquaries::Handle(const AddDependentReliquary& command)
    {
        entries.emplace_back(entries.size() + 2, *command.reliquary);
    }

    auto DependentReliquaries::Handle(const AllDependentReliquaries&) -> std::vector<Entry>
    {
        return entries;
    }
}