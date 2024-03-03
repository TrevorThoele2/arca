#pragma once

#include "Command.h"

#include "IsRelic.h"
#include "Handle.h"
#include "RelicStructure.h"
#include "Index.h"

#include "ReliquaryRelics.h"

namespace Arca
{
    class ReliquaryRelics;

    template<class T>
    struct IdentifiedCreateChildWith
    {
        template<class... Args>
        explicit IdentifiedCreateChildWith(RelicID id, const Handle& parent, const RelicStructure& structure, Args&& ... args) :
            base(std::make_unique<Derived<RelicID, Handle, RelicStructure, Args...>>(
                id, parent, structure, std::forward<Args>(args)...))
        {}

        template<class... Args>
        explicit IdentifiedCreateChildWith(RelicID id, const Handle& parent, const std::string& structureName, Args&& ... args) :
            base(std::make_unique<Derived<RelicID, Handle, std::string, Args...>>(
                id, parent, structureName, std::forward<Args>(args)...))
        {}

        Index<T> Do(Reliquary& reliquary) const
        {
            return base->Do(reliquary.relics);
        }
    private:
        class Base
        {
        public:
            virtual ~Base() = 0;

            virtual Index<T> Do(ReliquaryRelics& relics) = 0;
        };

        std::unique_ptr<Base> base;

        template<class... Args>
        class Derived final : public Base
        {
        public:
            explicit Derived(Args ... args) :
                args(std::forward<Args>(args)...)
            {}

            Index<T> Do(ReliquaryRelics& relics) override
            {
                return std::apply(
                    [&relics](auto&& ... args)
                    {
                        return relics.template IdentifiedCreateChildWith<T>(std::forward<decltype(args)>(args)...);
                    }, std::move(args));
            }
        private:
            std::tuple<Args...> args;
        };

        static_assert(is_relic_v<T>, "IdentifiedCreateChildWith can only be used with relics.");
    };

    template <class T>
    IdentifiedCreateChildWith<T>::Base::~Base() = default;

    template<class T>
    struct Traits<IdentifiedCreateChildWith<T>>
    {
        static const ObjectType objectType = ObjectType::Command;
        static inline const TypeName typeName = "Arca::IdentifiedCreateChildWith<" + Traits<std::remove_const_t<T>>::typeName + ">";
        using Result = Index<T>;
        static const bool selfContained = true;
    };
}