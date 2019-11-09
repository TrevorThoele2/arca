#pragma once

#include <vector>
#include "CuratorTraits.h"
#include "TypeHandle.h"
#include "StaticAssert.h"
#include <Chroma/Iterate.h>

namespace Arca
{
    class CuratorStage
    {
    private:
        template<::Chroma::VariadicTemplateSize i>
        struct AddIterator
        {
            template<class Template>
            static void Do(Template, CuratorStage& stage)
            {
                using Piece = typename Template::template Parameter<i>::Type;

                stage.Add<Piece>();
            }
        };
    public:
        CuratorStage() = default;

        template<class... Curator>
        static CuratorStage All()
        {
            CuratorStage stage;

            ::Chroma::IterateRange<
                ::Chroma::VariadicTemplateSize,
                AddIterator,
                sizeof...(Curator) - 1
            >(
                ::Chroma::VariadicTemplate<Curator...>{}, stage
            );

            return stage;
        }

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