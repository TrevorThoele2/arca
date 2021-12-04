#pragma once

#include "Command.h"

#include "IsRelic.h"
#include "IsShard.h"
#include "RelicID.h"
#include "Index.h"
#include "Assigned.h"
#include "Assigning.h"

#include "Reliquary.h"

namespace Arca
{
    template<class T, class Enable = void>
    struct AssignCopy;

    template<class T>
    struct AssignCopy<T, std::enable_if_t<is_relic_v<T>>>
    {
        template<class... Args>
        explicit AssignCopy(RelicID id, Args ... args) :
            base(std::make_unique<Derived<Args...>>(
                id, std::forward<Args>(args)...))
        {}

        AssignCopy(const AssignCopy& arg) = default;
        AssignCopy(AssignCopy&& arg) noexcept = default;
        AssignCopy& operator=(const AssignCopy& arg) = default;
        AssignCopy& operator=(AssignCopy&& arg) noexcept = default;

        Index<T> Do(Reliquary& reliquary) const
        {
            return base->Do(reliquary.relics, reliquary.shards, reliquary.signals, reliquary);
        }
    private:
        class Base
        {
        public:
            virtual ~Base() = 0;

            virtual Index<T> Do(ReliquaryRelics& relics, ReliquaryShards& shards, ReliquarySignals& signals, Reliquary& reliquary) = 0;
        };

        std::shared_ptr<Base> base;

        template<class... Args>
        class Derived final : public Base
        {
        public:
            explicit Derived(RelicID id, Args&& ... args) :
                id(id),
                args(std::forward<Args>(args)...)
            {}

            Index<T> Do(ReliquaryRelics& relics, ReliquaryShards& shards, ReliquarySignals& signals, Reliquary& reliquary) override
            {
                return std::apply(
                    [this, &relics, &shards, &signals, &reliquary](auto&& ... args)
                    {
                        auto storage = relics.FindStorage<T>(id);
                        if (!storage)
                            throw CannotFind("relic", TypeFor<T>());

                        const auto index = relics.Find<T>(id);
                        signals.Raise(AssigningKnown<T>{ index });
                        shards.Clear(id);
                        Impl<T>(*storage, RelicInit{ id, reliquary, shards }, std::forward<Args>(args)...);
                        signals.Raise(AssignedKnown<T>{ index });

                        return index;
                    }, std::move(args));
            }
        private:
            RelicID id;
            std::tuple<Args...> args;
        private:
            template<
                class U,
                std::enable_if_t<
                    std::is_constructible_v<U, RelicInit, Args...> &&
                    !std::is_constructible_v<U, Args...>, int> = 0>
            static void Impl(T& storage, RelicInit init, Args&& ... args)
            {
                storage = std::move(T{ init, std::forward<Args>(args)... });
            }

            template<
                class U,
                std::enable_if_t<
                    !std::is_constructible_v<U, RelicInit, Args...> &&
                    std::is_constructible_v<U, Args...>, int> = 0>
            static void Impl(T& storage, RelicInit init, Args&& ... args)
            {
                storage = std::move(T{ std::forward<Args>(args)... });
            }
        };
    };

    template <class T>
    AssignCopy<T, typename std::enable_if<is_relic_v<T>>::type>::Base::~Base() = default;

    template<class T>
    struct Traits<AssignCopy<T, std::enable_if_t<is_relic_v<T>>>>
    {
        static constexpr ObjectType objectType = ObjectType::Command;
        static const inline TypeName typeName =
            "Arca::AssignCopy<" + Traits<std::remove_const_t<T>>::typeName + ">";
        using Result = Index<T>;
        static constexpr bool selfContained = true;
    };

    template<class T>
    struct AssignCopy<T, std::enable_if_t<is_shard_v<T>>>
    {
        template<class... Args>
        explicit AssignCopy(RelicID id, Args ... args) :
            base(std::make_unique<Derived<Args...>>(
                id, std::forward<Args>(args)...))
        {}

        AssignCopy(const AssignCopy& arg) = default;
        AssignCopy(AssignCopy&& arg) noexcept = default;
        AssignCopy& operator=(const AssignCopy& arg) = default;
        AssignCopy& operator=(AssignCopy&& arg) noexcept = default;

        Index<T> Do(Reliquary& reliquary) const
        {
            return base->Do(reliquary.shards, reliquary.signals, reliquary);
        }
    private:
        class Base
        {
        public:
            virtual ~Base() = 0;

            virtual Index<T> Do(ReliquaryShards& shards, ReliquarySignals& signals, Reliquary& reliquary) = 0;
        };

        std::shared_ptr<Base> base;

        template<class... Args>
        class Derived final : public Base
        {
        public:
            explicit Derived(RelicID id, Args ... args) :
                id(id),
                args(std::forward<Args>(args)...)
            {}

            Index<T> Do(ReliquaryShards& shards, ReliquarySignals& signals, Reliquary& reliquary) override
            {
                return std::apply(
                    [this, &shards, &signals, &reliquary](auto&& ... args)
                    {
                        auto storage = shards.FindStorage<T>(id);
                        if (!storage)
                            throw CannotFind("shard", TypeFor<T>());

                        const auto index = shards.Find<T>(id);
                        signals.Raise(AssigningKnown<T>{ index });
                        *storage = T{ std::forward<Args>(args)... };
                        signals.Raise(AssignedKnown<T>{ index });

                        return index;
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
        static constexpr ObjectType objectType = ObjectType::Command;
        static const inline TypeName typeName =
            "Arca::AssignCopy<" + Traits<std::remove_const_t<T>>::typeName + ">";
        using Result = Index<T>;
        static constexpr bool selfContained = true;
    };

    template<class T, class Enable = void>
    struct AssignMove;

    template<class T>
    struct AssignMove<T, std::enable_if_t<is_relic_v<T>>>
    {
        template<class... Args>
        explicit AssignMove(RelicID id, Args ... args) :
            base(std::make_unique<Derived<Args...>>(
                id, std::forward<Args>(args)...))
        {}

        AssignMove(const AssignMove& arg) = default;
        AssignMove(AssignMove&& arg) noexcept = default;
        AssignMove& operator=(const AssignMove& arg) = default;
        AssignMove& operator=(AssignMove&& arg) noexcept = default;

        Index<T> Do(Reliquary& reliquary) const
        {
            return base->Do(reliquary.relics, reliquary.shards, reliquary.signals, reliquary);
        }
    private:
        class Base
        {
        public:
            virtual ~Base() = 0;

            virtual Index<T> Do(ReliquaryRelics& relics, ReliquaryShards& shards, ReliquarySignals& signals, Reliquary& reliquary) = 0;
        };

        std::shared_ptr<Base> base;

        template<class... Args>
        class Derived final : public Base
        {
        public:
            explicit Derived(RelicID id, Args&& ... args) :
                id(id),
                args(std::forward<Args>(args)...)
            {}

            Index<T> Do(ReliquaryRelics& relics, ReliquaryShards& shards, ReliquarySignals& signals, Reliquary& reliquary) override
            {
                return std::apply(
                    [this, &relics, &shards, &signals, &reliquary](auto&& ... args)
                    {
                        auto storage = relics.FindStorage<T>(id);
                        if (!storage)
                            throw CannotFind("relic", TypeFor<T>());

                        const auto index = reliquary.Find<T>(id);
                        signals.Raise(AssigningKnown<T>{ index });
                        shards.Clear(id);
                        Impl<T>(*storage, RelicInit{ id, reliquary, shards }, std::forward<Args>(args)...);
                        signals.Raise(AssignedKnown<T>{ index });

                        return index;
                    }, std::move(args));
            }
        private:
            RelicID id;
            std::tuple<Args...> args;
        private:
            template<
                class U,
                std::enable_if_t<
                    std::is_constructible_v<U, RelicInit, Args...> &&
                    !std::is_constructible_v<U, Args...>, int> = 0>
            static void Impl(T& storage, RelicInit init, Args&& ... args)
            {
                storage = std::move(T{ init, std::forward<Args>(args)... });
            }

            template<
                class U,
                std::enable_if_t<
                    !std::is_constructible_v<U, RelicInit, Args...> &&
                    std::is_constructible_v<U, Args...>, int> = 0>
            static void Impl(T& storage, RelicInit init, Args&& ... args)
            {
                storage = std::move(T{ std::forward<Args>(args)... });
            }
        };
    };

    template <class T>
    AssignMove<T, typename std::enable_if<is_relic_v<T>>::type>::Base::~Base() = default;

    template<class T>
    struct Traits<AssignMove<T, std::enable_if_t<is_relic_v<T>>>>
    {
        static constexpr ObjectType objectType = ObjectType::Command;
        static const inline TypeName typeName =
            "Arca::AssignMove<" + Traits<std::remove_const_t<T>>::typeName + ">";
        using Result = Index<T>;
        static constexpr bool selfContained = true;
    };

    template<class T>
    struct AssignMove<T, std::enable_if_t<is_shard_v<T>>>
    {
        template<class... Args>
        explicit AssignMove(RelicID id, Args ... args) :
            base(std::make_unique<Derived<Args...>>(
                id, std::forward<Args>(args)...))
        {}

        AssignMove(const AssignMove& arg) = default;
        AssignMove(AssignMove&& arg) noexcept = default;
        AssignMove& operator=(const AssignMove& arg) = default;
        AssignMove& operator=(AssignMove&& arg) noexcept = default;

        Index<T> Do(Reliquary& reliquary) const
        {
            return base->Do(reliquary.shards, reliquary.signals, reliquary);
        }
    private:
        class Base
        {
        public:
            virtual ~Base() = 0;

            virtual Index<T> Do(ReliquaryShards& shards, ReliquarySignals& signals, Reliquary& reliquary) = 0;
        };

        std::shared_ptr<Base> base;

        template<class... Args>
        class Derived final : public Base
        {
        public:
            explicit Derived(RelicID id, Args&& ... args) :
                id(id),
                args(std::forward<Args>(args)...)
            {}

            Index<T> Do(ReliquaryShards& shards, ReliquarySignals& signals, Reliquary& reliquary) override
            {
                return std::apply(
                    [this, &shards, &signals, &reliquary](auto&& ... args)
                    {
                        auto storage = shards.FindStorage<T>(id);
                        if (!storage)
                            throw CannotFind("shard", TypeFor<T>());

                        const auto index = shards.Find<T>(id);
                        signals.Raise(AssigningKnown<T>{ index });
                        *storage = std::move(T{ std::forward<Args>(args)... });
                        signals.Raise(AssignedKnown<T>{ index });

                        return index;
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
        static constexpr ObjectType objectType = ObjectType::Command;
        static const inline TypeName typeName =
            "Arca::AssignMove<" + Traits<std::remove_const_t<T>>::typeName + ">";
        using Result = Index<T>;
        static constexpr bool selfContained = true;
    };
}