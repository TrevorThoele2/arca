#pragma once

#include <vector>

#include "Type.h"
#include "TypeFor.h"

#include <Chroma/Iterate.h>

namespace Arca
{
    class Stage
    {
    private:
        template<Chroma::VariadicTemplateSize i>
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

            Chroma::IterateRange<
                Chroma::VariadicTemplateSize,
                AddIterator,
                sizeof...(Ts) - 1
            >(
                Chroma::VariadicTemplate<Ts...>{}, stage
            );

            return stage;
        }

        template<class T>
        void Add()
        {
            typeNames.push_back(TypeFor<T>().name);
        }

        [[nodiscard]] std::vector<TypeName> TypeNameList() const;

        [[nodiscard]] bool Empty() const;
    private:
        std::vector<TypeName> typeNames;
    };

    using Pipeline = std::vector<Stage>;
}