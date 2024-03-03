#include "RelicTypeGraphNode.h"

namespace Arca
{
    RelicTypeGraphNode::RelicTypeGraphNode(TypeHandle typeHandle) : typeHandle(std::move(typeHandle)), parent(nullptr)
    {}

    void RelicTypeGraphNode::CreateChild(const TypeHandle& typeHandle)
    {
        auto created = std::make_unique<RelicTypeGraphNode>(typeHandle);

        created->parent = this;

        children.push_back(std::move(created));
    }
}