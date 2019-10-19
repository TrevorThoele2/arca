#pragma once

#include <unordered_map>

#include "RelicTypeGraphNode.h"
#include "RelicTypeGraphIterator.h"
#include "RelicTypeDescription.h"
#include "RelicTypeDescriptionGroup.h"

namespace Arca
{
    class RelicTypeGraph
    {
    public:
        using iterator = RelicTypeGraphIteratorBase<RelicTypeGraphNode>;
        using const_iterator = RelicTypeGraphIteratorBase<const RelicTypeGraphNode>;
    public:
        RelicTypeGraph();

        void AddDescription(const RelicTypeDescription& add);

        [[nodiscard]] RelicTypeDescriptionGroup AllDescriptions() const;
        [[nodiscard]] RelicTypeDescriptionGroup AllBasesFor(const TypeHandle& typeHandle) const;
        [[nodiscard]] bool HasTypeHandle(const TypeHandle& typeHandle) const;
    private:
        struct Node
        {
            RelicTypeDescription description;
            std::vector<Node*> bases;

            explicit Node(const RelicTypeDescription& description);
        };

        std::unordered_map<TypeHandle, Node> nodes;

        void PushBasesTo(std::vector<RelicTypeDescription>& descriptions, const Node& node) const;
    };
}