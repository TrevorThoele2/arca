#include "DifferentiableCurator.h"

DifferentiableCuratorBase::~DifferentiableCuratorBase() = default;

Reliquary& DifferentiableCuratorBase::TheOwner()
{
    return Owner();
}

DifferentiableCuratorBase::DifferentiableCuratorBase(Init init) : Curator(init)
{
    onCommand = [](DifferentiableCuratorBase&) {};
}

void DifferentiableCuratorBase::Handle(const BasicCommand& command, Stage& stage)
{
    onCommand(*this);

    if (shouldAbort)
        stage.Abort();
}