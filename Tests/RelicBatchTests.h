#pragma once

#include "ReliquaryFixture.h"

#include <Arca/Relic.h>
#include <Arca/TypedVessel.h>

using namespace Arca;

class RelicBatchFixture : public ReliquaryFixture
{
public:
    RelicBatchFixture();

    class Relic;
    class UnregisteredRelic;
    class StaticVessel;
};

class RelicBatchFixture::Relic
{
public:
    int value = 0;
public:
    Relic() = default;
    explicit Relic(int value);
};

class RelicBatchFixture::UnregisteredRelic
{};

class RelicBatchFixture::StaticVessel : public TypedVessel<Relic>
{
public:
    Relic* relic = nullptr;
public:
    StaticVessel(VesselID id, Reliquary& owner);
private:
    void Setup();
};

namespace Arca
{
    template<>
    struct RelicTraits<::RelicBatchFixture::Relic>
    {
        static const TypeHandle typeHandle;
    };

    template<>
    struct RelicTraits<::RelicBatchFixture::UnregisteredRelic>
    {
        static const TypeHandle typeHandle;
    };

    template<>
    struct VesselTraits<::RelicBatchFixture::StaticVessel>
    {
        static const TypeHandle typeHandle;
    };
}

namespace Inscription
{
    template<>
    class Scribe<::RelicBatchFixture::Relic, BinaryArchive> final
        : public RelicScribe<::RelicBatchFixture::Relic, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {
            archive(object.value);
        }
    };

    template<>
    class Scribe<::RelicBatchFixture::StaticVessel, BinaryArchive> final
        : public RelicScribe<::RelicBatchFixture::StaticVessel, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {
            archive(object.relic);
        }
    };
}