#pragma once

#include "GeneralFixture.h"

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
        static constexpr ObjectType objectType = ObjectType::Command;
        static const inline TypeName typeName = "SelfContainedCommandTestsFixture::SelfContainedCommand";
        static constexpr bool selfContained = true;
    };

    template<>
    struct Traits<SelfContainedCommandTestsFixture::SelfContainedCommandWithResult>
    {
        static constexpr ObjectType objectType = ObjectType::Command;
        static const inline TypeName typeName = "SelfContainedCommandTestsFixture::SelfContainedCommandWithResult";
        using Result = int;
        static constexpr bool selfContained = true;
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