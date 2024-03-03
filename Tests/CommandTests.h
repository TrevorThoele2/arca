#pragma once

#include "GeneralFixture.h"

class CommandTestsFixture : public GeneralFixture
{
public:
    struct Command;
    struct ReturnCommand;
    class Curator;
    class CuratorWithSameLink;
};

struct CommandTestsFixture::Command
{
    
};

struct CommandTestsFixture::ReturnCommand
{

};

class CommandTestsFixture::Curator final : public Arca::Curator
{
public:
    std::vector<Command> handledCommands;
    void Handle(const Command& command);
    int Handle(const ReturnCommand& command);
    using HandledCommands = Arca::HandledCommands<Command, ReturnCommand>;
public:
    using Arca::Curator::Curator;
};

class CommandTestsFixture::CuratorWithSameLink final : public Arca::Curator
{
public:
    std::vector<Command> handledCommands;
    void Handle(const Command& command);
    using HandledCommands = Arca::HandledCommands<Command>;
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
    struct Traits<CommandTestsFixture::ReturnCommand>
    {
        static const ObjectType objectType = ObjectType::Command;
        static inline const TypeName typeName = "CommandTestsFixtureReturnCommand";
        using Return = int;
    };

    template<>
    struct Traits<CommandTestsFixture::Curator>
    {
        static const ObjectType objectType = ObjectType::Curator;
        static inline const TypeName typeName = "CommandTestsFixtureCurator";
    };

    template<>
    struct Traits<CommandTestsFixture::CuratorWithSameLink>
    {
        static const ObjectType objectType = ObjectType::Curator;
        static inline const TypeName typeName = "CommandTestsFixtureCuratorWithSameLink";
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