#pragma once

#include "GeneralFixture.h"

#include <Arca/Shard.h>
#include <Arca/ClosedTypedRelicAutomation.h>
#include <Arca/OpenTypedRelicAutomation.h>

using namespace Arca;

class PtrTestsFixture : public GeneralFixture
{
public:
    struct Shard
    {
        int myInt;
    };

    template<size_t i>
    struct DifferentiableShard
    {
        int myInt;
    };

    class TypedClosedRelic final : public ClosedTypedRelicAutomation<TypedClosedRelic, Shard>
    {
    public:
        LocalPtr<Shard> shard;
    public:
        void PostConstruct(ShardTuple shards);
    };

    class TypedOpenRelic final : public ClosedTypedRelicAutomation<TypedOpenRelic, Shard>
    {
    public:
        LocalPtr<Shard> shard;
    public:
        void PostConstruct(ShardTuple shards);
    };

    class GlobalRelic final : public ClosedTypedRelicAutomation<GlobalRelic, Shard>
    {
    public:
        LocalPtr<Shard> shard;
    public:
        void PostConstruct(ShardTuple shards);
    };
};

namespace Arca
{
    template<>
    struct Traits<PtrTestsFixture::Shard>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static inline const TypeName typeName = "PtrTestsShard";
    };

    template<size_t i>
    struct Traits<PtrTestsFixture::DifferentiableShard<i>>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static inline const TypeName typeName = "PtrTestsDifferentiableShard" + Chroma::ToString(i);
    };

    template<>
    struct Traits<PtrTestsFixture::TypedClosedRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "PtrTestsTypedClosedRelic";
    };

    template<>
    struct Traits<PtrTestsFixture::TypedOpenRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "PtrTestsTypedOpenRelic";
    };

    template<>
    struct Traits<PtrTestsFixture::GlobalRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "PtrTestsGlobalRelic";
        static const Locality locality = Locality::Global;
    };
}

namespace Inscription
{
    template<>
    class Scribe<PtrTestsFixture::Shard, BinaryArchive> final :
        public ArcaNullScribe<PtrTestsFixture::Shard, BinaryArchive>
    {};

    template<size_t i>
    class Scribe<PtrTestsFixture::DifferentiableShard<i>, BinaryArchive> final :
        public ArcaNullScribe<PtrTestsFixture::DifferentiableShard<i>, BinaryArchive>
    {};

    template<>
    class Scribe<PtrTestsFixture::TypedClosedRelic, BinaryArchive> final :
        public ArcaNullScribe<PtrTestsFixture::TypedClosedRelic, BinaryArchive>
    {};

    template<>
    class Scribe<PtrTestsFixture::TypedOpenRelic, BinaryArchive> final :
        public ArcaNullScribe<PtrTestsFixture::TypedOpenRelic, BinaryArchive>
    {};

    template<>
    class Scribe<PtrTestsFixture::GlobalRelic, BinaryArchive> final :
        public ArcaNullScribe<PtrTestsFixture::GlobalRelic, BinaryArchive>
    {};
}