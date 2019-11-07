#pragma once

#include "GeneralFixture.h"

#include <Arca/Curator.h>

#include <Inscription/BinaryArchive.h>
#include "Chroma/StringUtility.h"

using namespace Arca;

class CuratorTestsFixture : public GeneralFixture
{
public:
    CuratorTestsFixture();

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
        bool shouldStart = true;
        std::function<void()> onStartStep;
        std::function<void()> onWork;
        std::function<void()> onStopStep;

        [[nodiscard]] Reliquary& OwnerFromOutside();
        [[nodiscard]] const Reliquary& OwnerFromOutside() const;
    public:
        int value = 0;

        explicit BasicCurator(Reliquary& owner);
    protected:
        bool StartStepImplementation() override;
        void WorkImplementation() override;
        void StopStepImplementation() override;
    };

    class OtherBasicCurator final : public Curator
    {
    public:
        int value = 0;

        explicit OtherBasicCurator(Reliquary& owner);
    };

    template<size_t id>
    class DifferentiableCurator final : public Curator
    {
    public:
        static std::vector<CuratorCheckpoint>* checkpoints;
    public:
        explicit DifferentiableCurator(Reliquary& owner) : Curator(owner)
        {}

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
std::vector<CuratorTestsFixture::CuratorCheckpoint>* CuratorTestsFixture::DifferentiableCurator<id>::checkpoints;

namespace Arca
{
    template<size_t id>
    struct CuratorTraits<CuratorTestsFixture::DifferentiableCurator<id>>
    {
        static const TypeHandle typeHandle;
    };

    template<size_t id>
    const TypeHandle CuratorTraits<CuratorTestsFixture::DifferentiableCurator<id>>::typeHandle =
        "DifferentiableCurator" + Chroma::ToString(id);
}

namespace Inscription
{
    template<>
    class Scribe<CuratorTestsFixture::BasicCurator, BinaryArchive> final :
        public CuratorScribe<CuratorTestsFixture::BasicCurator, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override;
    };

    template<>
    class Scribe<CuratorTestsFixture::OtherBasicCurator, BinaryArchive> final :
        public CuratorScribe<CuratorTestsFixture::OtherBasicCurator, BinaryArchive>
    {
    public:
        static std::vector<TypeHandle> InputTypeHandles(const ArchiveT& archive);
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override;
    };

    template<size_t id>
    class Scribe<CuratorTestsFixture::DifferentiableCurator<id>, BinaryArchive> final :
        public CuratorScribe<CuratorTestsFixture::DifferentiableCurator<id>, BinaryArchive>
    {
    private:
        using BaseT = CuratorScribe<CuratorTestsFixture::DifferentiableCurator<id>, BinaryArchive>;
    public:
        using ObjectT = typename BaseT::ObjectT;
        using ArchiveT = typename BaseT::ArchiveT;

        using BaseT::Scriven;
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {}
    };
}