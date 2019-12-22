#pragma once

#include "GeneralFixture.h"

#include <Arca/ClosedTypedRelicAutomation.h>
#include <Arca/OpenTypedRelicAutomation.h>
#include <Arca/ShardTraits.h>

#include <Inscription/BinaryArchive.h>

using namespace Arca;

class ReliquarySerializationTestsFixture : public GeneralFixture
{
public:
    class BasicShard
    {
    public:
        std::string myValue;
    public:
        BasicShard() = default;
        explicit BasicShard(std::string myValue);
    };

    class BasicShardWithDifferentInputHandle
    {
    public:
        std::string myValue;
    public:
        BasicShardWithDifferentInputHandle() = default;
        explicit BasicShardWithDifferentInputHandle(int myValue);
    };

    class OtherShard
    {
    public:
        int myValue;
    public:
        OtherShard() = default;
        explicit OtherShard(int myValue);
    };

    class TypedClosedRelic final : public ClosedTypedRelicAutomation<TypedClosedRelic, BasicShard>
    {
    public:
        int myInt = 0;
        Ptr<BasicShard> basicShard;
    public:
        TypedClosedRelic() = default;
    protected:
        void InitializeImplementation() override;
    };

    class TypedOpenRelic final : public OpenTypedRelicAutomation<TypedOpenRelic, BasicShard>
    {
    public:
        int myInt = 0;
        Ptr<BasicShard> basicShard;
    public:
        TypedOpenRelic() = default;
    protected:
        void InitializeImplementation() override;
    };

    class GlobalRelic final : public ClosedTypedRelicAutomation<GlobalRelic, BasicShard>
    {
    public:
        int myInt = 0;
        Ptr<BasicShard> basicShard;
    public:
        GlobalRelic() = default;
    protected:
        void InitializeImplementation() override;
    };

    class BasicCurator final : public Curator
    {
    public:
        int myInt = 0;
    };

    struct BasicSignal
    {};

    class BasicShardNullInscription
    {
    public:
        std::string myValue;
    public:
        BasicShardNullInscription() = default;
        explicit BasicShardNullInscription(std::string myValue);
    };

    class OtherShardNullInscription
    {
    public:
        int myValue;
    public:
        OtherShardNullInscription() = default;
        explicit OtherShardNullInscription(int myValue);
    };

    class TypedClosedRelicNullInscription final :
        public ClosedTypedRelicAutomation<TypedClosedRelicNullInscription, BasicShardNullInscription>
    {
    public:
        int myInt = 0;
        Ptr<BasicShardNullInscription> basicShard;
    public:
        TypedClosedRelicNullInscription() = default;
    protected:
        void InitializeImplementation() override;
    };

    class TypedOpenRelicNullInscription final :
        public OpenTypedRelicAutomation<TypedOpenRelicNullInscription, BasicShardNullInscription>
    {
    public:
        int myInt = 0;
        Ptr<BasicShardNullInscription> basicShard;
    public:
        TypedOpenRelicNullInscription() = default;
    protected:
        void InitializeImplementation() override;
    };

    class GlobalRelicNullInscription final :
        public ClosedTypedRelicAutomation<GlobalRelicNullInscription, BasicShardNullInscription>
    {
    public:
        int myInt = 0;
        Ptr<BasicShardNullInscription> basicShard;
    public:
        GlobalRelicNullInscription() = default;
    protected:
        void InitializeImplementation() override;
    };

    class BasicCuratorNullInscription final : public Curator
    {
    public:
        int myInt = 0;
    };

    struct BasicSignalNullInscription
    {};
};

namespace Arca
{
    template<>
    struct Traits<::ReliquarySerializationTestsFixture::BasicShard>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static const TypeName typeName;
    };

    template<>
    struct Traits<::ReliquarySerializationTestsFixture::BasicShardWithDifferentInputHandle>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static const TypeName typeName;
    };

    template<>
    struct Traits<::ReliquarySerializationTestsFixture::OtherShard>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static const TypeName typeName;
    };

    template<>
    struct Traits<::ReliquarySerializationTestsFixture::TypedClosedRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static const TypeName typeName;
    };

    template<>
    struct Traits<::ReliquarySerializationTestsFixture::TypedOpenRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static const TypeName typeName;
    };

    template<>
    struct Traits<::ReliquarySerializationTestsFixture::GlobalRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static const TypeName typeName;
        static const Locality locality = Locality::Global;
    };

    template<>
    struct Traits<::ReliquarySerializationTestsFixture::BasicCurator>
    {
        static const ObjectType objectType = ObjectType::Curator;
        static const TypeName typeName;
    };

    template<>
    struct Traits<::ReliquarySerializationTestsFixture::BasicSignal>
    {
        static const ObjectType objectType = ObjectType::Signal;
        static const TypeName typeName;
    };

    template<>
    struct Traits<::ReliquarySerializationTestsFixture::BasicShardNullInscription>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static const TypeName typeName;
    };

    template<>
    struct Traits<::ReliquarySerializationTestsFixture::OtherShardNullInscription>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static const TypeName typeName;
    };

    template<>
    struct Traits<::ReliquarySerializationTestsFixture::TypedClosedRelicNullInscription>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static const TypeName typeName;
    };

    template<>
    struct Traits<::ReliquarySerializationTestsFixture::TypedOpenRelicNullInscription>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static const TypeName typeName;
    };

    template<>
    struct Traits<::ReliquarySerializationTestsFixture::GlobalRelicNullInscription>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static const TypeName typeName;
        static const Locality locality = Locality::Global;
    };

    template<>
    struct Traits<::ReliquarySerializationTestsFixture::BasicCuratorNullInscription>
    {
        static const ObjectType objectType = ObjectType::Curator;
        static const TypeName typeName;
    };

    template<>
    struct Traits<::ReliquarySerializationTestsFixture::BasicSignalNullInscription>
    {
        static const ObjectType objectType = ObjectType::Signal;
        static const TypeName typeName;
    };
}

namespace Inscription
{
    template<>
    class Scribe<::ReliquarySerializationTestsFixture::BasicShard, BinaryArchive> final
        : public ArcaCompositeScribe<::ReliquarySerializationTestsFixture::BasicShard, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {
            archive(object.myValue);
        }
    };

    template<>
    class Scribe<::ReliquarySerializationTestsFixture::BasicShardWithDifferentInputHandle, BinaryArchive> final
        : public ArcaCompositeScribe<::ReliquarySerializationTestsFixture::BasicShardWithDifferentInputHandle, BinaryArchive>
    {
    public:
        static std::vector<Type> InputTypes(const ArchiveT& archive)
        {
            return { "ReliquarySerializationTestsBasicShard" };
        }
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {
            archive(object.myValue);
        }
    };

    template<>
    class Scribe<::ReliquarySerializationTestsFixture::OtherShard, BinaryArchive> final
        : public ArcaCompositeScribe<::ReliquarySerializationTestsFixture::OtherShard, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {
            archive(object.myValue);
        }
    };

    template<>
    class Scribe<::ReliquarySerializationTestsFixture::TypedClosedRelic, BinaryArchive> final
        : public ArcaCompositeScribe<::ReliquarySerializationTestsFixture::TypedClosedRelic, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {
            archive(object.myInt);
        }
    };

    template<>
    class Scribe<::ReliquarySerializationTestsFixture::TypedOpenRelic, BinaryArchive> final
        : public ArcaCompositeScribe<::ReliquarySerializationTestsFixture::TypedOpenRelic, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {
            archive(object.myInt);
        }
    };

    template<>
    class Scribe<::ReliquarySerializationTestsFixture::GlobalRelic, BinaryArchive> final
        : public ArcaCompositeScribe<::ReliquarySerializationTestsFixture::GlobalRelic, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {
            archive(object.myInt);
        }
    };

    template<>
    class Scribe<::ReliquarySerializationTestsFixture::BasicCurator, BinaryArchive> final :
        public ArcaCompositeScribe<::ReliquarySerializationTestsFixture::BasicCurator, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {
            archive(object.myInt);
        }
    };

    template<>
    class Scribe<::ReliquarySerializationTestsFixture::BasicShardNullInscription, BinaryArchive> final
        : public ArcaNullScribe<::ReliquarySerializationTestsFixture::BasicShardNullInscription, BinaryArchive>
    {};

    template<>
    class Scribe<::ReliquarySerializationTestsFixture::OtherShardNullInscription, BinaryArchive> final
        : public ArcaNullScribe<::ReliquarySerializationTestsFixture::OtherShardNullInscription, BinaryArchive>
    {};

    template<>
    class Scribe<::ReliquarySerializationTestsFixture::TypedClosedRelicNullInscription, BinaryArchive> final
        : public ArcaNullScribe<::ReliquarySerializationTestsFixture::TypedClosedRelicNullInscription, BinaryArchive>
    {};

    template<>
    class Scribe<::ReliquarySerializationTestsFixture::TypedOpenRelicNullInscription, BinaryArchive> final
        : public ArcaNullScribe<::ReliquarySerializationTestsFixture::TypedOpenRelicNullInscription, BinaryArchive>
    {};

    template<>
    class Scribe<::ReliquarySerializationTestsFixture::GlobalRelicNullInscription, BinaryArchive> final
        : public ArcaNullScribe<::ReliquarySerializationTestsFixture::GlobalRelicNullInscription, BinaryArchive>
    {};

    template<>
    class Scribe<::ReliquarySerializationTestsFixture::BasicCuratorNullInscription, BinaryArchive> final :
        public ArcaNullScribe<::ReliquarySerializationTestsFixture::BasicCuratorNullInscription, BinaryArchive>
    {};
}