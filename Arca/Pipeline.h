#pragma once

#include <vector>
#include "CuratorTraits.h"
#include "TypeHandle.h"
#include <Chroma/Iterate.h>

namespace Arca
{
    class Stage
    {
    private:
        template<::Chroma::VariadicTemplateSize i>
        struct AddIterator
        {
            template<class Template>
            static void Do(Template, Stage& stage)
            {
                using Piece = typename Template::template Parameter<i>::Type;

                stage.Add<Piece>();
            }
        };
    public:
        Stage() = default;

        template<class... Ts>
        static Stage All()
        {
            Stage stage;

            ::Chroma::IterateRange<
                ::Chroma::VariadicTemplateSize,
                AddIterator,
                sizeof...(Ts) - 1
            >(
                ::Chroma::VariadicTemplate<Ts...>{}, stage
            );

            return stage;
        }

        template<class T>
        void Add()
        {
            typeHandleNames.push_back(TypeHandleFor<T>().name);
        }

        [[nodiscard]] std::vector<TypeHandleName> TypeHandleNameList() const;

        [[nodiscard]] bool Empty() const;
    private:
        std::vector<TypeHandleName> typeHandleNames;
    };

    using Pipeline = std::vector<Stage>;
}