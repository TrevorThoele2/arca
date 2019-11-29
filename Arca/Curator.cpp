#include "Curator.h"

#include "Reliquary.h"

namespace Arca
{
    Curator::~Curator() = default;

    void Curator::Initialize(Reliquary& owner)
    {
        this->owner = &owner;
        InitializeImplementation();
    }

    void Curator::StartStep()
    {
        isStarted = StartStepImplementation();
    }

    void Curator::Work()
    {
        if (!IsStarted())
            return;

        WorkImplementation();
    }

    void Curator::StopStep()
    {
        if (!IsStarted())
            return;

        StopStepImplementation();
        isStarted = false;
    }

    bool Curator::IsStarted() const
    {
        return isStarted;
    }

    Reliquary& Curator::Owner()
    {
        return *owner;
    }

    const Reliquary& Curator::Owner() const
    {
        return *owner;
    }

    void Curator::InitializeImplementation()
    {}

    bool Curator::StartStepImplementation()
    {
        return true;
    }

    void Curator::WorkImplementation()
    {}

    void Curator::StopStepImplementation()
    {}
}

namespace Inscription
{
    void Scribe<Arca::Curator, BinaryArchive>::ScrivenImplementation(ObjectT& object, ArchiveT& archive)
    {}
}