#include "Curator.h"

#include "Reliquary.h"

namespace Arca
{
    void Curator::Stage::Abort()
    {
        isAborted = true;
    }

    bool Curator::Stage::IsAborted() const
    {
        return isAborted;
    }

    Curator::~Curator() = default;

    void Curator::PostConstruct(Reliquary& owner)
    {
        this->owner = &owner;
        PostConstructImplementation();
    }

    void Curator::Initialize()
    {
        InitializeImplementation();
    }

    void Curator::Work(Stage& stage)
    {
        WorkImplementation(stage);
    }

    Reliquary& Curator::Owner()
    {
        return *owner;
    }

    const Reliquary& Curator::Owner() const
    {
        return *owner;
    }

    void Curator::PostConstructImplementation()
    {}

    void Curator::InitializeImplementation()
    {}

    void Curator::WorkImplementation(Stage& stage)
    {}
}