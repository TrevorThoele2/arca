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
            found.first->second.children.push_back(&created.first->second);
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

    RelicTypeDescriptionGroup RelicTypeGraph::GroupFor(const TypeHandle& typeHandle) const
    {
        using DescriptionList = std::vector<RelicTypeDescription>;

        DescriptionList descriptions;

        const auto extractGroup = [&](const Node* from) -> void
        {
            auto extractGroupImpl = [&](const Node* from, auto& extractGroupRef) mutable -> void
            {
                descriptions.push_back(from->description);
                for (auto& loop : from->children)
                    extractGroupRef(loop, extractGroupRef);
            };

            return extractGroupImpl(from, extractGroupImpl);
        };

        for (auto& loop : nodes)
        {
            if (loop.first != typeHandle)
                continue;

            extractGroup(&loop.second);
            break;
        }

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
}