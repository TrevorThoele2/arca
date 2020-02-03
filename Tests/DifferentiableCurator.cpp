#include "DifferentiableCurator.h"

DifferentiableCuratorBase::~DifferentiableCuratorBase() = default;

Reliquary& DifferentiableCuratorBase::TheOwner()
{
    return Owner();
}

DifferentiableCuratorBase::DifferentiableCuratorBase(Init init) : Curator(init)
{
    onWork = [](DifferentiableCuratorBase&) {};
}

void DifferentiableCuratorBase::Work(Stage& stage)
{
    onWork(*this);

    if (shouldAbort)
        stage.Abort();
}