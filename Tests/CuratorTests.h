#pragma once

#include "GeneralFixture.h"

#include <Arca/Curator.h>

#include <Inscription/BinaryArchive.h>

using namespace Arca;

class CuratorTestsFixture : public GeneralFixture
{
public:
    CuratorTestsFixture();

    class BasicCurator : public Curator
    {
    public:
        int value = 0;

        explicit BasicCurator(Reliquary& owner);
    };
};

namespace Inscription
{
    template<>
    class Scribe<CuratorTestsFixture::BasicCurator, BinaryArchive> final :
        public CuratorScribe<CuratorTestsFixture::BasicCurator, BinaryArchive>
    {
    public:
        static TypeHandle PrincipleTypeHandle(const ArchiveT& archive);
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override;
    };
}