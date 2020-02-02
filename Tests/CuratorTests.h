#pragma once

#include "GeneralFixture.h"

#include <Arca/Curator.h>

#include <Inscription/BinaryArchive.h>
#include "Chroma/StringUtility.h"

using namespace Arca;

class CuratorTestsFixture : public GeneralFixture
{
public:
    class BasicCurator final : public Curator
    {
    public:
        bool shouldAbort = false;
        std::function<void()> onWork = [](){};

        using Curator::Curator;

        void Work();

        [[nodiscard]] Reliquary& OwnerFromOutside();
        [[nodiscard]] const Reliquary& OwnerFromOutside() const;
    public:
        int value = 0;
    };

    class OtherBasicCurator final : public Curator
    {
    public:
        int value = 0;

        using Curator::Curator;
    };

    class CuratorWithNonDefaultConstructor final : public Curator
    {
    public:
        int myValue = 0;

        explicit CuratorWithNonDefaultConstructor(Reliquary& owner, int myValue);
    };
};

namespace Inscription
{
    template<>
    class Scribe<CuratorTestsFixture::BasicCurator, BinaryArchive> final :
        public ArcaCompositeScribe<CuratorTestsFixture::BasicCurator, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override;
    };

    template<>
    class Scribe<CuratorTestsFixture::OtherBasicCurator, BinaryArchive> final :
        public ArcaCompositeScribe<CuratorTestsFixture::OtherBasicCurator, BinaryArchive>
    {
    public:
        static std::vector<Type> InputTypes(const ArchiveT& archive);
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override;
    };

    template<>
    class Scribe<CuratorTestsFixture::CuratorWithNonDefaultConstructor, BinaryArchive> final :
        public ArcaNullScribe<CuratorTestsFixture::CuratorWithNonDefaultConstructor, BinaryArchive>
    {};
}