#pragma once

#include "RelicID.h"
#include "RelicDynamism.h"

#include "RelicScribe.h"
#include "Serialization.h"

namespace Arca
{
    class Reliquary;

    class Relic
    {
    public:
        template<class ShardT>
        ShardT* CreateShard();
        template<class ShardT>
        void DestroyShard();
        template<class ShardT>
        [[nodiscard]] ShardT* FindShard();
        template<class ShardT>
        [[nodiscard]] const ShardT* FindShard() const;
        template<class ShardT>
        [[nodiscard]] bool HasShard() const;

        [[nodiscard]] Reliquary& Owner();
        [[nodiscard]] const Reliquary& Owner() const;
        [[nodiscard]] RelicID ID() const;

        [[nodiscard]] RelicDynamism Dynamism() const;
    private:
        Reliquary* owner;
        RelicID id;
        RelicDynamism dynamism;
    private:
        Relic(RelicID id, RelicDynamism dynamism, Reliquary& owner);
    private:
        friend Reliquary;
    private:
        INSCRIPTION_ACCESS;
    };
}

namespace Inscription
{
    template<>
    class Scribe<::Arca::Relic, BinaryArchive> final :
        public CompositeScribe<::Arca::Relic, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override;
    };
}