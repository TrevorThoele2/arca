#pragma once

#include "Command.h"

#include "IsRelic.h"
#include "RelicStructure.h"
#include "Index.h"

#include "ReliquaryRelics.h"

namespace Arca
{
    class ReliquaryRelics;

    template<class T>
    struct CreateWith
    {
        template<class... Args>
        explicit CreateWith(const RelicStructure& structure, Args&& ... args) :
            base(std::make_unique<Derived<RelicStructure, Args...>>(
                structure, std::forward<Args>(args)...))
        {}

        template<class... Args>
        explicit CreateWith(const std::string& structureName, Args&& ... args) :
            base(std::make_unique<Derived<std::string, Args...>>(
                structureName, std::forward<Args>(args)...))
        {}

        Index<T> Do(ReliquaryRelics& relics) const
        {
            return base->Do(relics);
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
                        return relics.template CreateWith<T>(std::forward<decltype(args)>(args)...);
                    }, std::move(args));
            }
        private:
            std::tuple<Args...> args;
        };

        static_assert(is_relic_v<T>, "CreateWith can only be used with relics.");
    };

    template <class T>
    CreateWith<T>::Base::~Base() = default;

    template<class T>
    struct Traits<CreateWith<T>>
    {
        static const ObjectType objectType = ObjectType::Command;
        static inline const TypeName typeName = "CreateWith<" + Traits<std::remove_const_t<T>>::typeName + ">";
        using Result = Index<T>;
    };
}