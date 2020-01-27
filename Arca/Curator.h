#pragma once

#include "HandledCommands.h"

#include "IsRelic.h"
#include "IsShard.h"
#include "RelicID.h"

#include "Serialization.h"

namespace Arca
{
    class Reliquary;

    class Curator
    {
    protected:
        class Stage
        {
        public:
            Stage() = default;
            void Abort();
            [[nodiscard]] bool IsAborted() const;
        private:
            bool isAborted = false;
        };
    public:
        Curator(const Curator& arg) = delete;
        Curator(Curator&& arg) = delete;
        
        virtual ~Curator() = 0;

        Curator& operator=(const Curator& arg) = delete;
        Curator& operator=(Curator&& arg) = delete;

        void PostConstruct(Reliquary& owner);
        void Initialize();

        void Work(Stage& stage);
    protected:
        Curator() = default;
    protected:
        template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
        RelicT* Data(RelicID id);
        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        ShardT* Data(RelicID id);
    protected:
        [[nodiscard]] Reliquary& Owner();
        [[nodiscard]] const Reliquary& Owner() const;
    protected:
        virtual void PostConstructImplementation();
        virtual void InitializeImplementation();

        virtual void WorkImplementation(Stage& stage);
    private:
        Reliquary* owner = nullptr;
    private:
        INSCRIPTION_ACCESS;
        friend Reliquary;
        friend class ReliquaryCurators;
    };

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int>>
    RelicT* Curator::Data(RelicID id)
    {
        return Owner().FindStorage<RelicT>(id);
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    ShardT* Curator::Data(RelicID id)
    {
        return Owner().FindStorage<ShardT>(id);
    }
}
