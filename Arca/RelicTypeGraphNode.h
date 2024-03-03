#pragma once

#include <memory>
#include <vector>

#include "TypeHandle.h"

namespace Arca
{
    class RelicTypeGraphNode
    {
    public:
        const TypeHandle typeHandle;
    public:
        explicit RelicTypeGraphNode(TypeHandle typeHandle);

        void CreateChild(const TypeHandle& typeHandle);
    private:
        RelicTypeGraphNode* parent;

        using NodePtr = std::unique_ptr<RelicTypeGraphNode>;
        using Children = std::vector<NodePtr>;
        Children children;
    private:
        friend class RelicTypeGraph;
        template<class T>
        friend class RelicTypeGraphIteratorBase;
    };
}