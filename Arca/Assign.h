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
    struct AssignCopy;

    template<class T>
    struct AssignCopy<T, std::enable_if_t<is_relic_v<T>>>
    {
        template<class... Args>
        explicit AssignCopy(RelicID id, Args&& ... args) :
            base(std::make_unique<Derived<Args...>>(
                id, std::forward<Args>(args)...))
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
            explicit Derived(RelicID id, Args&& ... args) :
                id(id),
                args(std::forward<Args>(args)...)
            {}

            Index<T> Do(ReliquaryRelics& relics) override
            {
                return std::apply(
                    [this, &relics](auto&& ... args)
                    {
                        auto storage = relics.FindStorage<T>(id);
                        if (!storage)
                            throw relics.CannotFind(TypeFor<T>());

                        *storage = T{ RelicInit{id, relics.Owner()}, std::forward<Args>(args)... };
                        return Index<T>(id, relics.Owner());
                    }, std::move(args));
            }
        private:
            RelicID id;
            std::tuple<Args...> args;
        };
    };

    template <class T>
    AssignCopy<T, typename std::enable_if<is_relic_v<T>>::type>::Base::~Base() = default;

    template<class T>
    struct Traits<AssignCopy<T, std::enable_if_t<is_relic_v<T>>>>
    {
        static const ObjectType objectType = ObjectType::Command;
        static inline const TypeName typeName = "AssignCopy<" + Traits<std::remove_const_t<T>>::typeName + ">";
        using Result = Index<T>;
    };

    template<class T>
    struct AssignCopy<T, std::enable_if_t<is_shard_v<T>>>
    {
        template<class... Args>
        explicit AssignCopy(RelicID id, Args&& ... args) :
            base(std::make_unique<Derived<Args...>>(
                id, std::forward<Args>(args)...))
        {}

        Index<T> Do(ReliquaryShards& shards) const
        {
            return base->Do(shards);
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
            explicit Derived(RelicID id, Args&& ... args) :
                id(id),
                args(std::forward<Args>(args)...)
            {}

            Index<T> Do(ReliquaryShards& shards) override
            {
                return std::apply(
                    [this, &shards](auto&& ... args)
                    {
                        auto storage = shards.FindStorage<T>(id);
                        if (!storage)
                            throw shards.CannotFind(TypeFor<T>());

                        *storage = T{ std::forward<Args>(args)... };
                        return Index<T>(id, shards.Owner());
                    }, std::move(args));
            }
        private:
            RelicID id;
            std::tuple<Args...> args;
        };
    };

    template <class T>
    AssignCopy<T, typename std::enable_if<is_shard_v<T>>::type>::Base::~Base() = default;

    template<class T>
    struct Traits<AssignCopy<T, std::enable_if_t<is_shard_v<T>>>>
    {
        static const ObjectType objectType = ObjectType::Command;
        static inline const TypeName typeName = "AssignCopy<" + Traits<std::remove_const_t<T>>::typeName + ">";
        using Result = Index<T>;
    };

    template<class T, class Enable = void>
    struct AssignMove;

    template<class T>
    struct AssignMove<T, std::enable_if_t<is_relic_v<T>>>
    {
        template<class... Args>
        explicit AssignMove(RelicID id, Args&& ... args) :
            base(std::make_unique<Derived<Args...>>(
                id, std::forward<Args>(args)...))
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
            explicit Derived(RelicID id, Args&& ... args) :
                id(id),
                args(std::forward<Args>(args)...)
            {}

            Index<T> Do(ReliquaryRelics& relics) override
            {
                return std::apply(
                    [this, &relics](auto&& ... args)
                    {
                        auto storage = relics.FindStorage<T>(id);
                        if (!storage)
                            throw relics.CannotFind(TypeFor<T>());

                        *storage = std::move(T{ RelicInit{id, relics.Owner()}, std::forward<Args>(args)... });
                        return Index<T>(id, relics.Owner());
                    }, std::move(args));
            }
        private:
            RelicID id;
            std::tuple<Args...> args;
        };
    };

    template <class T>
    AssignMove<T, typename std::enable_if<is_relic_v<T>>::type>::Base::~Base() = default;

    template<class T>
    struct Traits<AssignMove<T, std::enable_if_t<is_relic_v<T>>>>
    {
        static const ObjectType objectType = ObjectType::Command;
        static inline const TypeName typeName = "AssignMove<" + Traits<std::remove_const_t<T>>::typeName + ">";
        using Result = Index<T>;
    };

    template<class T>
    struct AssignMove<T, std::enable_if_t<is_shard_v<T>>>
    {
        template<class... Args>
        explicit AssignMove(RelicID id, Args&& ... args) :
            base(std::make_unique<Derived<Args...>>(
                id, std::forward<Args>(args)...))
        {}

        Index<T> Do(ReliquaryShards& shards) const
        {
            return base->Do(shards);
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
            explicit Derived(RelicID id, Args&& ... args) :
                id(id),
                args(std::forward<Args>(args)...)
            {}

            Index<T> Do(ReliquaryShards& shards) override
            {
                return std::apply(
                    [this, &shards](auto&& ... args)
                    {
                        auto storage = shards.FindStorage<T>(id);
                        if (!storage)
                            throw shards.CannotFind(TypeFor<T>());

                        *storage = std::move(T{ std::forward<Args>(args)... });
                        return Index<T>(id, shards.Owner());
                    }, std::move(args));
            }
        private:
            RelicID id;
            std::tuple<Args...> args;
        };
    };

    template <class T>
    AssignMove<T, typename std::enable_if<is_shard_v<T>>::type>::Base::~Base() = default;

    template<class T>
    struct Traits<AssignMove<T, std::enable_if_t<is_shard_v<T>>>>
    {
        static const ObjectType objectType = ObjectType::Command;
        static inline const TypeName typeName = "AssignMove<" + Traits<std::remove_const_t<T>>::typeName + ">";
        using Result = Index<T>;
    };
}