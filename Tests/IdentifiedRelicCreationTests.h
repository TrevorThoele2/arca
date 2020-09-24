#pragma once

#include "GeneralFixture.h"

#include "BasicLocalClosedTypedRelic.h"
#include "BasicShard.h"

using namespace Arca;

class IdentifiedRelicCreationTestsFixture : public GeneralFixture
{
public:
    using Relic = BasicLocalClosedTypedRelic;
    using Shard = BasicShard;
};