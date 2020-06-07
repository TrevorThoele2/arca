#include "ReliquaryDependencies.h"

#include "ReliquaryDependencyAlreadyAdded.h"

namespace Arca
{
    ReliquaryDependencies::Entry::Entry(int id, std::string name, Reliquary& reliquary) :
        id(id), name(name), reliquary(&reliquary)
    {}

    ReliquaryDependencies::ReliquaryDependencies(Init init) : Curator(init)
    {}

    void ReliquaryDependencies::Handle(const AddReliquaryDependency& command)
    {
        {
            for(auto& entry : entries)
            {
                if (entry.name == command.name)
                    throw ReliquaryDependencyAlreadyAdded(command.name);
                else if (entry.reliquary == command.reliquary)
                    throw ReliquaryDependencyAlreadyAdded(*command.reliquary);
            }
        }

        entries.emplace_back(entries.size() + 2, command.name, *command.reliquary);
    }

    auto ReliquaryDependencies::Handle(const FindReliquaryDependency& command) -> Entry*
    {
        const auto found = std::find_if(
            entries.begin(),
            entries.end(),
            [command](const Entry& entry)
            {
                return entry.name == command.name;
            });
        if (found != entries.end())
            return &*found;
        else
            return nullptr;
    }

    auto ReliquaryDependencies::Handle(const AllReliquaryDependencies&) -> std::vector<Entry>
    {
        return entries;
    }
}