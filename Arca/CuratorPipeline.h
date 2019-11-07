#pragma once

#include <vector>
#include "CuratorTraits.h"
#include "TypeHandle.h"
#include "StaticAssert.h"

namespace Arca
{
    class CuratorStage
    {
    public:
        template<class CuratorT>
        void Add()
        {
            STATIC_ASSERT_TYPE_DERIVED_FROM_CURATOR(CuratorT);

            typeHandles.push_back(CuratorTraits<CuratorT>::typeHandle);
        }

        [[nodiscard]] std::vector<TypeHandle> TypeHandleList() const;
    private:
        std::vector<TypeHandle> typeHandles;
    };

    using CuratorPipeline = std::vector<CuratorStage>;
}