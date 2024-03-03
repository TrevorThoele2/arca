#pragma once

#include "Command.h"

#include "IsRelic.h"
#include "Handle.h"
#include "Index.h"

#include "ReliquaryRelics.h"

namespace Arca
{
    class ReliquaryRelics;

    template<class T>
    struct IdentifiedCreateChild
    {
        template<class... Args>
        explicit IdentifiedCreateChild(RelicID id, const Handle& parent, Args ... args) :
            base(std::make_unique<Derived<RelicID, Handle, Args...>>(
                id, parent, std::forward<Args>(args)...))
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

        std::shared_ptr<Base> base;

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
                    [&relics](auto ... args)
                    {
                        return relics.template IdentifiedCreateChild<T>(std::forward<decltype(args)>(args)...);
                    }, std::move(args));
            }
        private:
            std::tuple<Args...> args;
        };

        static_assert(is_relic_v<T>, "IdentifiedCreateChild can only be used with relics.");
    };

    template <class T>
    IdentifiedCreateChild<T>::Base::~Base() = default;

    template<class T>
    struct Traits<IdentifiedCreateChild<T>>
    {
        static const ObjectType objectType = ObjectType::Command;
        static TypeName TypeName() { return "Arca::IdentifiedCreateChild<" + Traits<std::remove_const_t<T>>::TypeName() + ">"; }
        using Result = Index<T>;
        static const bool selfContained = true;
    };
}