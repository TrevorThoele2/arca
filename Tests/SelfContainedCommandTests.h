#pragma once

#include "GeneralFixture.h"

#include <Arca/ClosedTypedRelic.h>
#include "BasicShard.h"

class SelfContainedCommandTestsFixture : public GeneralFixture
{
public:
    struct SelfContainedCommand;
    struct SelfContainedCommandWithResult;
};

namespace Arca
{
    template<>
    struct Traits<SelfContainedCommandTestsFixture::SelfContainedCommand>
    {
        static const ObjectType objectType = ObjectType::Command;
        static inline const TypeName typeName = "SelfContainedCommandTestsFixture::SelfContainedCommand";
        static const bool selfContained = true;
    };

    template<>
    struct Traits<SelfContainedCommandTestsFixture::SelfContainedCommandWithResult>
    {
        static const ObjectType objectType = ObjectType::Command;
        static inline const TypeName typeName = "SelfContainedCommandTestsFixture::SelfContainedCommandWithResult";
        using Result = int;
        static const bool selfContained = true;
    };
}

struct SelfContainedCommandTestsFixture::SelfContainedCommand
{
    std::unique_ptr<int> executionCount;

    SelfContainedCommand();

    void Do(Arca::Reliquary& reliquary) const;
};

struct SelfContainedCommandTestsFixture::SelfContainedCommandWithResult
{
    [[nodiscard]] int Do(Arca::Reliquary& reliquary) const;
};