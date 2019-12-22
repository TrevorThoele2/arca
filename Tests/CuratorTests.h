#pragma once

#include "GeneralFixture.h"

#include <Arca/Curator.h>

#include <Inscription/BinaryArchive.h>
#include "Chroma/StringUtility.h"

using namespace Arca;

class CuratorTestsFixture : public GeneralFixture
{
public:
    struct CuratorCheckpoint
    {
        Curator* curator;

        explicit CuratorCheckpoint(Curator* curator);
    };

    class BasicCurator final : public Curator
    {
    public:
        bool isInitialized = false;

        bool shouldAbort = false;
        std::function<void()> onWork;

        [[nodiscard]] Reliquary& OwnerFromOutside();
        [[nodiscard]] const Reliquary& OwnerFromOutside() const;
    public:
        int value = 0;
    protected:
        void InitializeImplementation() override;
        void WorkImplementation(Stage& stage) override;
    };

    class OtherBasicCurator final : public Curator
    {
    public:
        int value = 0;
    };

    template<size_t id>
    class DifferentiableCurator final : public Curator
    {
    public:
        static std::vector<Curator*>* initializationCheckpoints;
        static std::vector<CuratorCheckpoint>* checkpoints;

        bool shouldAbort = false;
    public:
        void InitializeImplementation() override
        {
            if (initializationCheckpoints)
                initializationCheckpoints->push_back(this);
        }

        void WorkImplementation(Stage& stage) override
        {
            checkpoints->emplace_back(this);

            if (shouldAbort)
            {
                stage.Abort();
                return;
            }
        }
    };
};

template<size_t id>
std::vector<Curator*>* CuratorTestsFixture::DifferentiableCurator<id>::initializationCheckpoints;

template<size_t id>
std::vector<CuratorTestsFixture::CuratorCheckpoint>* CuratorTestsFixture::DifferentiableCurator<id>::checkpoints;

namespace Arca
{
    template<size_t id>
    struct Traits<CuratorTestsFixture::DifferentiableCurator<id>>
    {
        static const ObjectType objectType = ObjectType::Curator;
        static const TypeName typeName;
    };

    template<size_t id>
    const TypeName Traits<CuratorTestsFixture::DifferentiableCurator<id>>::typeName =
        "DifferentiableCurator" + Chroma::ToString(id);
}

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

    template<size_t id>
    class Scribe<CuratorTestsFixture::DifferentiableCurator<id>, BinaryArchive> final :
        public ArcaNullScribe<CuratorTestsFixture::DifferentiableCurator<id>, BinaryArchive>
    {
    private:
        using BaseT = ArcaNullScribe<CuratorTestsFixture::DifferentiableCurator<id>, BinaryArchive>;
    public:
        using ObjectT = typename BaseT::ObjectT;
        using ArchiveT = typename BaseT::ArchiveT;

        using BaseT::Scriven;
    };
}