#pragma once

#include "GeneralFixture.h"

#include <Arca/Curator.h>
#include "BasicLocalClosedTypedRelic.h"
#include "BasicGlobalClosedTypedRelic.h"
#include "BasicCommand.h"

using namespace Arca;

class CuratorTestsFixture : public GeneralFixture
{
public:
    class BasicCurator;
    class OtherBasicCurator;
    class CuratorWithNonDefaultConstructor;
    class CuratorWithLocalRelicConstructor;
    class CuratorWithGlobalRelicConstructor;
    class CuratorWithoutCommands;

    class BaseCuratorWithCommand;
    class DerivedCuratorWithCommand;

    class BasicCommand2;
};

namespace Arca
{
    template<>
    struct Traits<CuratorTestsFixture::BasicCurator>
    {
        static const ObjectType objectType = ObjectType::Curator;
        static TypeName TypeName() { return "CuratorTestsFixture::BasicCurator"; }
    };

    template<>
    struct Traits<CuratorTestsFixture::OtherBasicCurator>
    {
        static const ObjectType objectType = ObjectType::Curator;
        static TypeName TypeName() { return "CuratorTestsFixture::OtherBasicCurator"; }
    };

    template<>
    struct Traits<CuratorTestsFixture::CuratorWithNonDefaultConstructor>
    {
        static const ObjectType objectType = ObjectType::Curator;
        static TypeName TypeName() { return "CuratorTestsFixture::CuratorWithNonDefaultConstructor"; }
    };

    template<>
    struct Traits<CuratorTestsFixture::CuratorWithLocalRelicConstructor>
    {
        static const ObjectType objectType = ObjectType::Curator;
        static TypeName TypeName() { return "CuratorTestsFixture::CuratorWithLocalRelicConstructor"; }
    };

    template<>
    struct Traits<CuratorTestsFixture::CuratorWithGlobalRelicConstructor>
    {
        static const ObjectType objectType = ObjectType::Curator;
        static TypeName TypeName() { return "CuratorTestsFixture::CuratorWithGlobalRelicConstructor"; }
    };

    template<>
    struct Traits<CuratorTestsFixture::CuratorWithoutCommands>
    {
        static const ObjectType objectType = ObjectType::Curator;
        static TypeName TypeName() { return "CuratorTestsFixture::CuratorWithoutCommands"; }
    };

    template<>
    struct Traits<CuratorTestsFixture::BaseCuratorWithCommand>
    {
        static const ObjectType objectType = ObjectType::Curator;
        static TypeName TypeName() { return "CuratorTestsFixture::BaseCuratorWithCommand"; }
    };

    template<>
    struct Traits<CuratorTestsFixture::DerivedCuratorWithCommand>
    {
        static const ObjectType objectType = ObjectType::Curator;
        static TypeName TypeName() { return "CuratorTestsFixture::DerivedCuratorWithCommand"; }
        using HandledCommands = Arca::HandledCommands<
            BasicCommand,
            CuratorTestsFixture::BasicCommand2>;
    };

    template<>
    struct Traits<CuratorTestsFixture::BasicCommand2>
    {
        static const ObjectType objectType = ObjectType::Command;
        static TypeName TypeName() { return "CuratorTestsFixture::BasicCommand2"; }
    };
}

class CuratorTestsFixture::BasicCurator final : public Curator
{
public:
    bool shouldAbort = false;
    std::function<void(const BasicCommand&)> onCommand = [](const BasicCommand&) {};

    using Curator::Curator;

    void Handle(const BasicCommand& command);

    [[nodiscard]] Reliquary& OwnerFromOutside();
    [[nodiscard]] const Reliquary& OwnerFromOutside() const;
public:
    int value = 0;
};

class CuratorTestsFixture::OtherBasicCurator final : public Curator
{
public:
    int value = 0;

    using Curator::Curator;
};

class CuratorTestsFixture::CuratorWithNonDefaultConstructor final : public Curator
{
public:
    int myValue = 0;

    CuratorWithNonDefaultConstructor(Init init, int myValue);
};

class CuratorTestsFixture::CuratorWithLocalRelicConstructor final : public Curator
{
public:
    Index<BasicLocalClosedTypedRelic> localRelic;
    int localRelicInteger = 0;
    std::string localRelicString;

    explicit CuratorWithLocalRelicConstructor(
        Init init, int localRelicInteger, const std::string& localRelicString);
};

class CuratorTestsFixture::CuratorWithGlobalRelicConstructor final : public Curator
{
public:
    Index<BasicGlobalClosedTypedRelic> globalRelic;
    int globalRelicInteger = 0;
    std::string globalRelicString;

    explicit CuratorWithGlobalRelicConstructor(Init init);
};

class CuratorTestsFixture::CuratorWithoutCommands final : public Curator
{
public:
    explicit CuratorWithoutCommands(Init init);
};

class CuratorTestsFixture::BaseCuratorWithCommand : public Curator
{
public:
    bool basicCommandIssued = false;

    explicit BaseCuratorWithCommand(Init init);

    void Handle(const BasicCommand& command);
};

class CuratorTestsFixture::DerivedCuratorWithCommand final : public BaseCuratorWithCommand
{
public:
    explicit DerivedCuratorWithCommand(Init init);

    using BaseCuratorWithCommand::Handle;
    void Handle(const BasicCommand2& command);
};

namespace Inscription
{
    template<>
    class Scribe<CuratorTestsFixture::BasicCurator> final
    {
    public:
        using ObjectT = CuratorTestsFixture::BasicCurator;
    public:
        template<class Archive>
        void Scriven(ObjectT& object, Archive& archive);
    };

    template <class Archive>
    void Scribe<CuratorTestsFixture::BasicCurator>::Scriven(ObjectT& object, Archive& archive)
    {
        archive("value", object.value);
    }

    template<class Archive>
    struct ScribeTraits<CuratorTestsFixture::BasicCurator, Archive> final
    {
        using Category = ArcaCompositeScribeCategory<CuratorTestsFixture::BasicCurator>;
    };

    template<>
    class Scribe<CuratorTestsFixture::OtherBasicCurator> final
    {
    public:
        using ObjectT = CuratorTestsFixture::OtherBasicCurator;
    public:
        template<class Archive>
        void Scriven(ObjectT& object, Archive& archive);

        template<class Archive>
        static std::vector<Type> InputTypes(const Archive& archive);
    };

    template<class Archive>
    void Scribe<CuratorTestsFixture::OtherBasicCurator>::Scriven(ObjectT& object, Archive& archive)
    {
        archive("value", object.value);
    }

    template <class Archive>
    std::vector<Type> Scribe<CuratorTestsFixture::OtherBasicCurator>::InputTypes(const Archive&)
    {
        return { "CuratorTestsFixture::BasicCurator" };
    }

    template<class Archive>
    struct ScribeTraits<CuratorTestsFixture::OtherBasicCurator, Archive> final
    {
        using Category = ArcaCompositeScribeCategory<CuratorTestsFixture::OtherBasicCurator>;
    };

    template<class Archive>
    struct ScribeTraits<CuratorTestsFixture::CuratorWithNonDefaultConstructor, Archive> final
    {
        using Category = ArcaNullScribeCategory<CuratorTestsFixture::CuratorWithNonDefaultConstructor>;
    };

    template<>
    class Scribe<CuratorTestsFixture::CuratorWithLocalRelicConstructor> final
    {
    public:
        using ObjectT = CuratorTestsFixture::CuratorWithLocalRelicConstructor;
    public:
        template<class Archive>
        void Scriven(ObjectT& object, Archive& archive);
    };

    template<class Archive>
    void Scribe<CuratorTestsFixture::CuratorWithLocalRelicConstructor>::Scriven(ObjectT& object, Archive& archive)
    {
        archive("localRelic", object.localRelic);

        if (archive.IsInput())
        {
            object.localRelicInteger = object.localRelic->integer;
            object.localRelicString = object.localRelic->string;
        }
    }

    template<class Archive>
    struct ScribeTraits<CuratorTestsFixture::CuratorWithLocalRelicConstructor, Archive>
    {
        using Category = ArcaCompositeScribeCategory<CuratorTestsFixture::CuratorWithLocalRelicConstructor>;
    };

    template<>
    class Scribe<CuratorTestsFixture::CuratorWithGlobalRelicConstructor> final
    {
    public:
        using ObjectT = CuratorTestsFixture::CuratorWithGlobalRelicConstructor;
    public:
        template<class Archive>
        void Scriven(ObjectT& object, Archive& archive);
    };

    template <class Archive>
    void Scribe<CuratorTestsFixture::CuratorWithGlobalRelicConstructor>::Scriven(ObjectT& object, Archive& archive)
    {
        archive("globalRelic", object.globalRelic);

        if (archive.IsInput())
        {
            object.globalRelicInteger = object.globalRelic->integer;
            object.globalRelicString = object.globalRelic->string;
        }
    }

    template<class Archive>
    struct ScribeTraits<CuratorTestsFixture::CuratorWithGlobalRelicConstructor, Archive>
    {
        using Category = ArcaCompositeScribeCategory<CuratorTestsFixture::CuratorWithGlobalRelicConstructor>;
    };

    template<>
    class Scribe<CuratorTestsFixture::CuratorWithoutCommands> final
    {
    public:
        using ObjectT = CuratorTestsFixture::CuratorWithoutCommands;
    public:
        template<class Archive>
        void Scriven(ObjectT& object, Archive& archive)
        {}
    };

    template<class Archive>
    struct ScribeTraits<CuratorTestsFixture::CuratorWithoutCommands, Archive>
    {
        using Category = ArcaNullScribeCategory<CuratorTestsFixture::CuratorWithoutCommands>;
    };

    template<>
    class Scribe<CuratorTestsFixture::BaseCuratorWithCommand> final
    {
    public:
        using ObjectT = CuratorTestsFixture::BaseCuratorWithCommand;
    public:
        template<class Archive>
        void Scriven(ObjectT& object, Archive& archive)
        {}
    };

    template<class Archive>
    struct ScribeTraits<CuratorTestsFixture::BaseCuratorWithCommand, Archive>
    {
        using Category = ArcaNullScribeCategory<CuratorTestsFixture::BaseCuratorWithCommand>;
    };

    template<>
    class Scribe<CuratorTestsFixture::DerivedCuratorWithCommand> final
    {
    public:
        using ObjectT = CuratorTestsFixture::DerivedCuratorWithCommand;
    public:
        template<class Archive>
        void Scriven(ObjectT& object, Archive& archive)
        {}
    };

    template<class Archive>
    struct ScribeTraits<CuratorTestsFixture::DerivedCuratorWithCommand, Archive>
    {
        using Category = ArcaNullScribeCategory<CuratorTestsFixture::DerivedCuratorWithCommand>;
    };
}