#pragma once

#include "GeneralFixture.h"

class CommandTestsFixture : public GeneralFixture
{
public:
    struct Command;
    struct CommandWithResult;
    class Curator;
    class CuratorWithSameLink;
};

struct CommandTestsFixture::Command
{
    
};

struct CommandTestsFixture::CommandWithResult
{

};

class CommandTestsFixture::Curator final : public Arca::Curator
{
public:
    std::vector<Command> handledCommands;
    void Handle(const Command& command);
    int Handle(const CommandWithResult& command);
public:
    using Arca::Curator::Curator;
};

class CommandTestsFixture::CuratorWithSameLink final : public Arca::Curator
{
public:
    std::vector<Command> handledCommands;
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
    struct Traits<CommandTestsFixture::CommandWithResult>
    {
        static const ObjectType objectType = ObjectType::Command;
        static inline const TypeName typeName = "CommandTestsFixtureCommandWithResult";
        using Result = int;
    };

    template<>
    struct Traits<CommandTestsFixture::Curator>
    {
        static const ObjectType objectType = ObjectType::Curator;
        static inline const TypeName typeName = "CommandTestsFixtureCurator";
        using HandledCommands = Arca::HandledCommands<
            CommandTestsFixture::Command, CommandTestsFixture::CommandWithResult>;
    };

    template<>
    struct Traits<CommandTestsFixture::CuratorWithSameLink>
    {
        static const ObjectType objectType = ObjectType::Curator;
        static inline const TypeName typeName = "CommandTestsFixtureCuratorWithSameLink";
        using HandledCommands = Arca::HandledCommands<
            CommandTestsFixture::Command>;
    };
}

namespace Inscription
{
    template<>
    class Scribe<CommandTestsFixture::Curator, BinaryArchive> final
        : public ArcaNullScribe<CommandTestsFixture::Curator, BinaryArchive>
    {};

    template<>
    class Scribe<CommandTestsFixture::CuratorWithSameLink, BinaryArchive> final
        : public ArcaNullScribe<CommandTestsFixture::CuratorWithSameLink, BinaryArchive>
    {};
}