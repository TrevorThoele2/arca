#include "DifferentiableCurator.h"

DifferentiableCuratorBase::~DifferentiableCuratorBase() = default;

Reliquary& DifferentiableCuratorBase::TheOwner()
{
    return Owner();
}

DifferentiableCuratorBase::DifferentiableCuratorBase(Reliquary& owner) : Curator(owner)
{
    onWork = [](DifferentiableCuratorBase&) {};
}

void DifferentiableCuratorBase::Work(Stage& stage)
{
    onWork(*this);

    if (shouldAbort)
        stage.Abort();
}