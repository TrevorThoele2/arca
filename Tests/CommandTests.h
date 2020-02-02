#pragma once

#include "GeneralFixture.h"

class CommandTestsFixture : public GeneralFixture
{
public:
    struct Command;
    class Curator;
};

struct CommandTestsFixture::Command
{
    
};

class CommandTestsFixture::Curator final : public Arca::Curator
{
public:
    std::vector<Command> handledCommands;
    using HandledCommands = Arca::HandledCommands<Command>;
    void Handle(const Command& command);
public:
    using Arca::Curator::Curator;
};

namespace Arca
{
    template<>
    struct Traits<CommandTestsFixture::Command>
    {
        static const ObjectType objectType = ObjectType::Command;
        static inline const TypeName typeName = "CommandTestsFixtureCommand";
    };

    template<>
    struct Traits<CommandTestsFixture::Curator>
    {
        static const ObjectType objectType = ObjectType::Curator;
        static inline const TypeName typeName = "CommandTestsFixtureCurator";
    };
}

namespace Inscription
{
    template<>
    class Scribe<CommandTestsFixture::Curator, BinaryArchive> final
        : public ArcaNullScribe<CommandTestsFixture::Curator, BinaryArchive>
    {};
}