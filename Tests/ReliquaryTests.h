#pragma once

#include "GeneralFixture.h"

#include <Arca/TypedVessel.h>
#include <Arca/Relic.h>

#include <Inscription/BinaryArchive.h>
#include "Inscription/BaseScriven.h"

using namespace Arca;

class ReliquaryTestsFixture : public GeneralFixture
{
public:
    ReliquaryTestsFixture();

    class BasicRelic
    {
    public:
        std::string myValue;
    public:
        BasicRelic() = default;
        explicit BasicRelic(std::string myValue);
    };

    class OtherBasicRelic
    {
    public:
        std::string myValue;
    public:
        OtherBasicRelic() = default;
        explicit OtherBasicRelic(int myValue);
    };

    class OtherRelic
    {
    public:
        int myValue;
    public:
        OtherRelic() = default;
        explicit OtherRelic(int myValue);
    };

    class BasicTypedVessel : public TypedVessel<BasicRelic>
    {
    public:
        BasicRelic* basicRelic = nullptr;
    public:
        BasicTypedVessel(VesselID id, Reliquary& owner);
    private:
        void Setup();
    };

    class StaticVessel : public TypedVessel<BasicRelic>
    {
    public:
        BasicRelic* basicRelic = nullptr;
    public:
        StaticVessel(VesselID id, Reliquary& owner);
    private:
        void Setup();
    };

    class BasicCurator final : public Curator
    {
    public:
        explicit BasicCurator(Reliquary& owner);
    };

    struct BasicSignal
    {};
};

namespace Arca
{
    template<>
    struct RelicTraits<::ReliquaryTestsFixture::BasicRelic>
    {
        static const TypeHandle typeHandle;
    };

    template<>
    struct RelicTraits<::ReliquaryTestsFixture::OtherBasicRelic>
    {
        static const TypeHandle typeHandle;
    };

    template<>
    struct RelicTraits<::ReliquaryTestsFixture::OtherRelic>
    {
        static const TypeHandle typeHandle;
    };

    template<>
    struct VesselTraits<::ReliquaryTestsFixture::BasicTypedVessel>
    {
        static const TypeHandle typeHandle;
    };

    template<>
    struct VesselTraits<::ReliquaryTestsFixture::StaticVessel>
    {
        static const TypeHandle typeHandle;
    };

    template<>
    struct CuratorTraits<::ReliquaryTestsFixture::BasicCurator>
    {
        static const TypeHandle typeHandle;
    };
}

namespace Inscription
{
    template<>
    class Scribe<::ReliquaryTestsFixture::BasicRelic, BinaryArchive> final
        : public RelicScribe<::ReliquaryTestsFixture::BasicRelic, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {
            archive(object.myValue);
        }
    };

    template<>
    class Scribe<::ReliquaryTestsFixture::OtherBasicRelic, BinaryArchive> final
        : public RelicScribe<::ReliquaryTestsFixture::OtherBasicRelic, BinaryArchive>
    {
    public:
        static std::vector<TypeHandle> InputTypeHandles(const ArchiveT& archive)
        {
            return { "ReliquaryTestsBasicRelic" };
        }
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {
            archive(object.myValue);
        }
    };

    template<>
    class Scribe<::ReliquaryTestsFixture::OtherRelic, BinaryArchive> final
        : public RelicScribe<::ReliquaryTestsFixture::OtherRelic, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {
            archive(object.myValue);
        }
    };

    template<>
    class Scribe<::ReliquaryTestsFixture::BasicTypedVessel, BinaryArchive> final
        : CompositeScribe<::ReliquaryTestsFixture::BasicTypedVessel, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {
            BaseScriven<TypedVessel<::ReliquaryTestsFixture::BasicRelic>>(object, archive);
        }
    };

    template<>
    class Scribe<::ReliquaryTestsFixture::StaticVessel, BinaryArchive> final
        : CompositeScribe<::ReliquaryTestsFixture::StaticVessel, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {
            BaseScriven<TypedVessel<::ReliquaryTestsFixture::BasicRelic>>(object, archive);
        }
    };

    template<>
    class Scribe<::ReliquaryTestsFixture::BasicCurator, BinaryArchive> final :
        public CuratorScribe<::ReliquaryTestsFixture::BasicCurator, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {}
    };
}