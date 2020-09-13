#pragma once

#include "GeneralFixture.h"

#include <Arca/ClosedTypedRelic.h>
#include "BasicShard.h"

class CommandTestsFixture : public GeneralFixture
{
public:
    struct Command;
    struct CommandWithResult;
    class Curator;
    class CuratorWithSameLink;
    class Relic;
    class RelicWithShard;
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

    template<>
    struct Traits<CommandTestsFixture::Relic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "CommandTestsFixtureRelic";
    };

    template<>
    struct Traits<CommandTestsFixture::RelicWithShard>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "CommandTestsFixtureRelicWithShard";
    };
}

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

class CommandTestsFixture::Relic final : public Arca::ClosedTypedRelic<Relic>
{
public:
    int integer = 0;
    std::string string;
public:
    explicit Relic(Init init);
    explicit Relic(Init init, int integer, const std::string& string);
};

class CommandTestsFixture::RelicWithShard final : public Arca::ClosedTypedRelic<RelicWithShard>
{
public:
    Arca::Index<BasicShard> shard;
public:
    RelicWithShard(Init init, int integer, const std::string& string);
};

namespace Inscription
{
    template<class Archive>
    struct ScribeTraits<CommandTestsFixture::Curator, Archive> final
    {
        using Category = ArcaNullScribeCategory<CommandTestsFixture::Curator>;
    };

    template<class Archive>
    struct ScribeTraits<CommandTestsFixture::CuratorWithSameLink, Archive> final
    {
        using Category = ArcaNullScribeCategory<CommandTestsFixture::CuratorWithSameLink>;
    };

    template<class Archive>
    struct ScribeTraits<CommandTestsFixture::Relic, Archive> final
    {
        using Category = ArcaNullScribeCategory<CommandTestsFixture::Relic>;
    };

    template<class Archive>
    struct ScribeTraits<CommandTestsFixture::RelicWithShard, Archive> final
    {
        using Category = ArcaNullScribeCategory<CommandTestsFixture::RelicWithShard>;
    };
}