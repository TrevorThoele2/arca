#pragma once

#include "Command.h"

#include "IsRelic.h"
#include "IsShard.h"
#include "RelicID.h"
#include "Index.h"

#include "ReliquaryRelics.h"
#include "ReliquaryShards.h"

namespace Arca
{
    class ReliquaryRelics;
    class ReliquaryShards;

    template<class T, class Enable = void>
    struct Create;

    template<class T>
    struct Create<T, std::enable_if_t<is_relic_v<T>>>
    {
        template<class... Args>
        explicit Create(Args&& ... args) :
            base(std::make_unique<Derived<Args...>>(
                std::forward<Args>(args)...))
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
            explicit Derived(Args&& ... args) :
                args(std::forward<Args>(args)...)
            {}

            Index<T> Do(ReliquaryRelics& relics) override
            {
                return std::apply(
                    [&relics](auto&& ... args)
                {
                    return relics.template Create<T>(std::forward<decltype(args)>(args)...);
                }, std::move(args));
            }
        private:
            std::tuple<Args...> args;
        };
    };

    template <class T>
    Create<T, typename std::enable_if<is_relic_v<T>>::type>::Base::~Base() = default;

    template<class T>
    struct Traits<Create<T, std::enable_if_t<is_relic_v<T>>>>
    {
        static const ObjectType objectType = ObjectType::Command;
        static inline const TypeName typeName = "Arca::Create<" + Traits<std::remove_const_t<T>>::typeName + ">";
        using Result = Index<T>;
        static const bool selfContained = true;
    };

    template<class T>
    struct Create<T, std::enable_if_t<is_shard_v<T>>>
    {
        template<class... Args>
        explicit Create(Args&& ... args) :
            base(std::make_unique<Derived<Args...>>(
                std::forward<Args>(args)...))
        {}

        Index<T> Do(Reliquary& reliquary) const
        {
            return base->Do(reliquary.shards);
        }
    private:
        class Base
        {
        public:
            virtual ~Base() = 0;

            virtual Index<T> Do(ReliquaryShards& shards) = 0;
        };

        std::unique_ptr<Base> base;

        template<class... Args>
        class Derived final : public Base
        {
        public:
            explicit Derived(Args&& ... args) :
                args(std::forward<Args>(args)...)
            {}

            Index<T> Do(ReliquaryShards& shards) override
            {
                return std::apply(
                    [&shards](auto&& ... args)
                    {
                        return shards.template Create<T>(std::forward<decltype(args)>(args)...);
                    }, std::move(args));
            }
        private:
            std::tuple<Args...> args;
        };
    };

    template <class T>
    Create<T, typename std::enable_if<is_shard_v<T>>::type>::Base::~Base() = default;

    template<class T>
    struct Traits<Create<T, std::enable_if_t<is_shard_v<T>>>>
    {
        static const ObjectType objectType = ObjectType::Command;
        static inline const TypeName typeName = "Arca::Create<" + Traits<std::remove_const_t<T>>::typeName + ">";
        using Result = Index<T>;
        static const bool selfContained = true;
    };
}