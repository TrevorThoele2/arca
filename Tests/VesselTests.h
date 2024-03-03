#pragma once

#include "GeneralFixture.h"

#include <Arca/TypedVessel.h>
#include <Arca/Relic.h>
#include "Inscription/BaseScriven.h"

using namespace Arca;

class VesselTestsFixture : public GeneralFixture
{
public:
    class BasicRelic
    {
    public:
        std::string myValue;
    public:
        BasicRelic() = default;
        explicit BasicRelic(std::string myValue);
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
};

namespace Arca
{
    template<>
    struct RelicTraits<::VesselTestsFixture::BasicRelic>
    {
        static const TypeHandle typeHandle;
    };

    template<>
    struct RelicTraits<::VesselTestsFixture::OtherRelic>
    {
        static const TypeHandle typeHandle;
    };

    template<>
    struct VesselTraits<::VesselTestsFixture::BasicTypedVessel>
    {
        static const TypeHandle typeHandle;
    };

    template<>
    struct VesselTraits<::VesselTestsFixture::StaticVessel>
    {
        static const TypeHandle typeHandle;
    };
}

namespace Inscription
{
    template<>
    class Scribe<::VesselTestsFixture::BasicRelic, BinaryArchive> final
        : public RelicScribe<::VesselTestsFixture::BasicRelic, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {
            archive(object.myValue);
        }
    };

    template<>
    class Scribe<::VesselTestsFixture::OtherRelic, BinaryArchive> final
        : public RelicScribe<::VesselTestsFixture::OtherRelic, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {
            archive(object.myValue);
        }
    };

    template<>
    class Scribe<::VesselTestsFixture::BasicTypedVessel, BinaryArchive> final
        : CompositeScribe<::VesselTestsFixture::BasicTypedVessel, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {
            BaseScriven<TypedVessel<::VesselTestsFixture::BasicRelic>>(object, archive);
        }
    };

    template<>
    class Scribe<::VesselTestsFixture::StaticVessel, BinaryArchive> final
        : CompositeScribe<::VesselTestsFixture::StaticVessel, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {
            BaseScriven<TypedVessel<::VesselTestsFixture::BasicRelic>>(object, archive);
        }
    };
}