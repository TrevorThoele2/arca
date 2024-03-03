#include "ChildRelicBatchSource.h"

namespace Arca
{
    ChildRelicBatchSourceBase::~ChildRelicBatchSourceBase() = default;


    [[nodiscard]] RelicID ChildRelicBatchSourceBase::Parent() const
    {
        return parent;
    }

    ChildRelicBatchSourceBase::ChildRelicBatchSourceBase(RelicID parent) : parent(parent)
    {}
}