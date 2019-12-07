#pragma once

#include "GeneralFixture.h"

#include <Arca/Curator.h>

#include <Inscription/BinaryArchive.h>
#include "Chroma/StringUtility.h"

using namespace Arca;

class CuratorTestsFixture : public GeneralFixture
{
public:
    enum class CuratorState
    {
        Started,
        Worked,
        Stopped
    };

    struct CuratorCheckpoint
    {
        Curator* curator;
        CuratorState state;

        CuratorCheckpoint(Curator* curator, CuratorState state);
    };

    class BasicCurator final : public Curator
    {
    public:
        bool isInitialized = false;

        bool shouldStart = true;
        std::function<void()> onStartStep;
        std::function<void()> onWork;
        std::function<void()> onStopStep;

        [[nodiscard]] Reliquary& OwnerFromOutside();
        [[nodiscard]] const Reliquary& OwnerFromOutside() const;
    public:
        int value = 0;
    protected:
        void InitializeImplementation() override;
        bool StartStepImplementation() override;
        void WorkImplementation() override;
        void StopStepImplementation() override;
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
    public:
        void InitializeImplementation() override
        {
            if (initializationCheckpoints)
                initializationCheckpoints->push_back(this);
        }

        bool StartStepImplementation() override
        {
            checkpoints->emplace_back(this, CuratorState::Started);
            return true;
        }

        void WorkImplementation() override
        {
            checkpoints->emplace_back(this, CuratorState::Worked);
        }

        void StopStepImplementation() override
        {
            checkpoints->emplace_back(this, CuratorState::Stopped);
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