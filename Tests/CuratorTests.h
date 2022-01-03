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
        static constexpr ObjectType objectType = ObjectType::Curator;
        static const inline TypeName typeName = "CuratorTestsFixture::BasicCurator";
    };

    template<>
    struct Traits<CuratorTestsFixture::OtherBasicCurator>
    {
        static constexpr ObjectType objectType = ObjectType::Curator;
        static const inline TypeName typeName = "CuratorTestsFixture::OtherBasicCurator";
    };

    template<>
    struct Traits<CuratorTestsFixture::CuratorWithNonDefaultConstructor>
    {
        static constexpr ObjectType objectType = ObjectType::Curator;
        static const inline TypeName typeName = "CuratorTestsFixture::CuratorWithNonDefaultConstructor";
    };

    template<>
    struct Traits<CuratorTestsFixture::CuratorWithLocalRelicConstructor>
    {
        static constexpr ObjectType objectType = ObjectType::Curator;
        static const inline TypeName typeName = "CuratorTestsFixture::CuratorWithLocalRelicConstructor";
    };

    template<>
    struct Traits<CuratorTestsFixture::CuratorWithGlobalRelicConstructor>
    {
        static constexpr ObjectType objectType = ObjectType::Curator;
        static const inline TypeName typeName = "CuratorTestsFixture::CuratorWithGlobalRelicConstructor";
    };

    template<>
    struct Traits<CuratorTestsFixture::CuratorWithoutCommands>
    {
        static constexpr ObjectType objectType = ObjectType::Curator;
        static const inline TypeName typeName = "CuratorTestsFixture::CuratorWithoutCommands";
    };

    template<>
    struct Traits<CuratorTestsFixture::BaseCuratorWithCommand>
    {
        static constexpr ObjectType objectType = ObjectType::Curator;
        static const inline TypeName typeName = "CuratorTestsFixture::BaseCuratorWithCommand";
    };

    template<>
    struct Traits<CuratorTestsFixture::DerivedCuratorWithCommand>
    {
        static constexpr ObjectType objectType = ObjectType::Curator;
        static const inline TypeName typeName = "CuratorTestsFixture::DerivedCuratorWithCommand";
        using HandledCommands = Arca::HandledCommands<
            BasicCommand,
            CuratorTestsFixture::BasicCommand2>;
    };

    template<>
    struct Traits<CuratorTestsFixture::BasicCommand2>
    {
        static constexpr ObjectType objectType = ObjectType::Command;
        static const inline TypeName typeName = "CuratorTestsFixture::BasicCommand2";
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
        template<class Format>
        void Scriven(ObjectT& object, Format& format);
    };

    template <class Format>
    void Scribe<CuratorTestsFixture::BasicCurator>::Scriven(ObjectT& object, Format& format)
    {
        format("value", object.value);
    }

    template<class Format>
    struct ScribeTraits<CuratorTestsFixture::BasicCurator, Format> final
    {
        using Category = ArcaCompositeScribeCategory<CuratorTestsFixture::BasicCurator>;
    };

    template<>
    class Scribe<CuratorTestsFixture::OtherBasicCurator> final
    {
    public:
        using ObjectT = CuratorTestsFixture::OtherBasicCurator;
    public:
        template<class Format>
        void Scriven(ObjectT& object, Format& format);

        template<class Format>
        static std::vector<Type> InputTypes(const Format& format);
    };

    template<class Format>
    void Scribe<CuratorTestsFixture::OtherBasicCurator>::Scriven(ObjectT& object, Format& format)
    {
        format("value", object.value);
    }

    template <class Format>
    std::vector<Type> Scribe<CuratorTestsFixture::OtherBasicCurator>::InputTypes(const Format&)
    {
        return { "CuratorTestsFixture::BasicCurator" };
    }

    template<class Format>
    struct ScribeTraits<CuratorTestsFixture::OtherBasicCurator, Format> final
    {
        using Category = ArcaCompositeScribeCategory<CuratorTestsFixture::OtherBasicCurator>;
    };

    template<class Format>
    struct ScribeTraits<CuratorTestsFixture::CuratorWithNonDefaultConstructor, Format> final
    {
        using Category = ArcaNullScribeCategory<CuratorTestsFixture::CuratorWithNonDefaultConstructor>;
    };

    template<>
    class Scribe<CuratorTestsFixture::CuratorWithLocalRelicConstructor> final
    {
    public:
        using ObjectT = CuratorTestsFixture::CuratorWithLocalRelicConstructor;
    public:
        template<class Format>
        void Scriven(ObjectT& object, Format& format);
    };

    template<class Format>
    void Scribe<CuratorTestsFixture::CuratorWithLocalRelicConstructor>::Scriven(ObjectT& object, Format& format)
    {
        format("localRelic", object.localRelic);

        if (format.IsInput())
        {
            object.localRelicInteger = object.localRelic->integer;
            object.localRelicString = object.localRelic->string;
        }
    }

    template<class Format>
    struct ScribeTraits<CuratorTestsFixture::CuratorWithLocalRelicConstructor, Format>
    {
        using Category = ArcaCompositeScribeCategory<CuratorTestsFixture::CuratorWithLocalRelicConstructor>;
    };

    template<>
    class Scribe<CuratorTestsFixture::CuratorWithGlobalRelicConstructor> final
    {
    public:
        using ObjectT = CuratorTestsFixture::CuratorWithGlobalRelicConstructor;
    public:
        template<class Format>
        void Scriven(ObjectT& object, Format& format);
    };

    template <class Format>
    void Scribe<CuratorTestsFixture::CuratorWithGlobalRelicConstructor>::Scriven(ObjectT& object, Format& format)
    {
        format("globalRelic", object.globalRelic);

        if (format.IsInput())
        {
            object.globalRelicInteger = object.globalRelic->integer;
            object.globalRelicString = object.globalRelic->string;
        }
    }

    template<class Format>
    struct ScribeTraits<CuratorTestsFixture::CuratorWithGlobalRelicConstructor, Format>
    {
        using Category = ArcaCompositeScribeCategory<CuratorTestsFixture::CuratorWithGlobalRelicConstructor>;
    };

    template<>
    class Scribe<CuratorTestsFixture::CuratorWithoutCommands> final
    {
    public:
        using ObjectT = CuratorTestsFixture::CuratorWithoutCommands;
    public:
        template<class Format>
        void Scriven(ObjectT& object, Format& format)
        {}
    };

    template<class Format>
    struct ScribeTraits<CuratorTestsFixture::CuratorWithoutCommands, Format>
    {
        using Category = ArcaNullScribeCategory<CuratorTestsFixture::CuratorWithoutCommands>;
    };

    template<>
    class Scribe<CuratorTestsFixture::BaseCuratorWithCommand> final
    {
    public:
        using ObjectT = CuratorTestsFixture::BaseCuratorWithCommand;
    public:
        template<class Format>
        void Scriven(ObjectT& object, Format& format)
        {}
    };

    template<class Format>
    struct ScribeTraits<CuratorTestsFixture::BaseCuratorWithCommand, Format>
    {
        using Category = ArcaNullScribeCategory<CuratorTestsFixture::BaseCuratorWithCommand>;
    };

    template<>
    class Scribe<CuratorTestsFixture::DerivedCuratorWithCommand> final
    {
    public:
        using ObjectT = CuratorTestsFixture::DerivedCuratorWithCommand;
    public:
        template<class Format>
        void Scriven(ObjectT& object, Format& format)
        {}
    };

    template<class Format>
    struct ScribeTraits<CuratorTestsFixture::DerivedCuratorWithCommand, Format>
    {
        using Category = ArcaNullScribeCategory<CuratorTestsFixture::DerivedCuratorWithCommand>;
    };
}