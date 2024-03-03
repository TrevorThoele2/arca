#pragma once

#include "GeneralFixture.h"

#include <Arca/ClosedTypedRelic.h>

class CommandTestsFixture : public GeneralFixture
{
public:
    struct Command;
    struct CommandWithResult;
    class Curator;
    class CuratorWithSameLink;
    class Relic;
    class Shard;
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
    struct Traits<CommandTestsFixture::Shard>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static inline const TypeName typeName = "CommandTestsFixtureShard";
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

class CommandTestsFixture::Shard final
{
public:
    int integer = 0;
    std::string string;
public:
    explicit Shard() = default;
    explicit Shard(int integer, const std::string& string);
};

class CommandTestsFixture::RelicWithShard final : public Arca::ClosedTypedRelic<RelicWithShard>
{
public:
    Arca::Index<Shard> shard;
public:
    RelicWithShard(Init init, int integer, const std::string& string);
};

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

    template<>
    class Scribe<CommandTestsFixture::Relic, BinaryArchive> final
        : public ArcaNullScribe<CommandTestsFixture::Relic, BinaryArchive>
    {};

    template<>
    class Scribe<CommandTestsFixture::Shard, BinaryArchive> final
        : public ArcaNullScribe<CommandTestsFixture::Shard, BinaryArchive>
    {};

    template<>
    class Scribe<CommandTestsFixture::RelicWithShard, BinaryArchive> final
        : public ArcaNullScribe<CommandTestsFixture::RelicWithShard, BinaryArchive>
    {};
}