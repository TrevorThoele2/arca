#pragma once

#include "Command.h"

#include "CreateData.h"
#include "IsRelic.h"
#include "IsShard.h"
#include "RelicID.h"
#include "Index.h"

#include "Reliquary.h"

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
        explicit Create(Args ... args) :
            base(std::make_unique<Derived<Args...>>(
                CreateData{},
                std::forward<Args>(args)...))
        {}

        template<class... Args>
        explicit Create(CreateData createData, Args ... args) :
            base(std::make_unique<Derived<Args...>>(
                createData,
                std::forward<Args>(args)...))
        {}

        Create(const Create& arg) = default;
        Create(Create&& arg) noexcept = default;
        Create& operator=(const Create& arg) = default;
        Create& operator=(Create&& arg) noexcept = default;

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
            explicit Derived(CreateData createData, Args ... args) :
                createData(createData),
                args(std::forward<Args>(args)...)
            {}

            Index<T> Do(ReliquaryRelics& relics) override
            {
                return std::apply(
                    [this, &relics](auto ... args)
                {
                    return relics.Create<T>(createData, std::forward<decltype(args)>(args)...);
                }, std::move(args));
            }
        private:
            CreateData createData;
            std::tuple<Args...> args;
        };
    };

    template <class T>
    Create<T, typename std::enable_if<is_relic_v<T>>::type>::Base::~Base() = default;

    template<class T>
    struct Traits<Create<T, std::enable_if_t<is_relic_v<T>>>>
    {
        static constexpr ObjectType objectType = ObjectType::Command;
        static constexpr TypeName TypeName() { return "Arca::Create<" + Traits<std::remove_const_t<T>>::TypeName() + ">"; }
        using Result = Index<T>;
        static constexpr bool selfContained = true;
    };

    template<class T>
    struct Create<T, std::enable_if_t<is_shard_v<T>>>
    {
        template<class... Args>
        explicit Create(Args ... args) :
            base(std::make_unique<Derived<Args...>>(
                std::forward<Args>(args)...))
        {}

        template<class U, class... Args, std::enable_if_t<is_relic_v<U>, int> = 0>
        explicit Create(Index<U> relic, Args ... args) :
            base(std::make_unique<Derived<RelicID, Args...>>(
                relic.ID(), std::forward<Args>(args)...))
        {}

        Create(const Create& arg) = default;
        Create(Create && arg) noexcept = default;
        Create& operator=(const Create & arg) = default;
        Create& operator=(Create && arg) noexcept = default;

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

        std::shared_ptr<Base> base;

        template<class... Args>
        class Derived final : public Base
        {
        public:
            explicit Derived(Args ... args) :
                args(std::forward<Args>(args)...)
            {}

            Index<T> Do(ReliquaryShards& shards) override
            {
                return std::apply(
                    [&shards](auto ... args)
                    {
                        return shards.Create<T>(std::forward<decltype(args)>(args)...);
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
        static constexpr ObjectType objectType = ObjectType::Command;
        static constexpr TypeName TypeName() { return "Arca::Create<" + Traits<std::remove_const_t<T>>::TypeName() + ">"; }
        using Result = Index<T>;
        static constexpr bool selfContained = true;
    };
}