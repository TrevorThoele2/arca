#include "RelicTypeGraph.h"

namespace Arca
{
    RelicTypeGraph::RelicTypeGraph() = default;

    void RelicTypeGraph::AddDescription(const RelicTypeDescription& add)
    {
        auto created = nodes.emplace(add.typeHandle, Node(add));

        for (auto& loop : add.baseTypes)
        {
            auto found = nodes.emplace(loop.typeHandle, Node(loop));
            created.first->second.bases.push_back(&found.first->second);
        }
    }

    RelicTypeDescriptionGroup RelicTypeGraph::AllDescriptions() const
    {
        using DescriptionList = std::vector<RelicTypeDescription>;

        DescriptionList descriptions;

        for (auto& loop : nodes)
            descriptions.push_back(loop.second.description);

        return RelicTypeDescriptionGroup(std::move(descriptions));
    }

    RelicTypeDescriptionGroup RelicTypeGraph::AllBasesFor(const TypeHandle& typeHandle) const
    {
        using DescriptionList = std::vector<RelicTypeDescription>;
        
        const auto found = std::find_if(nodes.begin(), nodes.end(),
            [typeHandle](const std::pair<TypeHandle, Node>& entry)
            {
                return entry.first == typeHandle;
            });

        if (found == nodes.end())
            return RelicTypeDescriptionGroup(DescriptionList());

        DescriptionList descriptions;
        PushBasesTo(descriptions, found->second);
        return RelicTypeDescriptionGroup(std::move(descriptions));
    }

    bool RelicTypeGraph::HasTypeHandle(const TypeHandle& typeHandle) const
    {
        for (auto& loop : nodes)
            if (loop.second.description.typeHandle == typeHandle)
                return true;

        return false;
    }

    RelicTypeGraph::Node::Node(const RelicTypeDescription& description) : description(description)
    {}

    void RelicTypeGraph::PushBasesTo(std::vector<RelicTypeDescription>& descriptions, const Node& node) const
    {
        for (auto& loop : node.bases)
        {
            PushBasesTo(descriptions, *loop);
            descriptions.push_back(loop->description);
        }
    }
}