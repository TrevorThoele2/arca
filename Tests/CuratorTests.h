#pragma once

#include "GeneralFixture.h"

#include <Arca/Curator.h>
#include <Arca/ClosedTypedRelic.h>

#include <Inscription/BinaryArchive.h>

using namespace Arca;

class CuratorTestsFixture : public GeneralFixture
{
public:
    class LocalRelic;
    class GlobalRelic;

    class BasicCurator;
    class OtherBasicCurator;
    class CuratorWithNonDefaultConstructor;
    class CuratorWithLocalRelicConstructor;
    class CuratorWithGlobalRelicConstructor;
};

namespace Arca
{
    template<>
    struct Traits<CuratorTestsFixture::LocalRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "LocalRelic";
    };

    template<>
    struct Traits<CuratorTestsFixture::GlobalRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "GlobalRelic";
        static const Locality locality = Locality::Global;
    };

    template<>
    struct Traits<CuratorTestsFixture::BasicCurator>
    {
        static const ObjectType objectType = ObjectType::Curator;
        static inline const TypeName typeName = "BasicCurator";
    };

    template<>
    struct Traits<CuratorTestsFixture::OtherBasicCurator>
    {
        static const ObjectType objectType = ObjectType::Curator;
        static inline const TypeName typeName = "OtherBasicCurator";
    };

    template<>
    struct Traits<CuratorTestsFixture::CuratorWithNonDefaultConstructor>
    {
        static const ObjectType objectType = ObjectType::Curator;
        static inline const TypeName typeName = "CuratorWithNonDefaultConstructor";
    };

    template<>
    struct Traits<CuratorTestsFixture::CuratorWithLocalRelicConstructor>
    {
        static const ObjectType objectType = ObjectType::Curator;
        static inline const TypeName typeName = "CuratorWithLocalRelicConstructor";
    };

    template<>
    struct Traits<CuratorTestsFixture::CuratorWithGlobalRelicConstructor>
    {
        static const ObjectType objectType = ObjectType::Curator;
        static inline const TypeName typeName = "CuratorWithGlobalRelicConstructor";
    };
}

class CuratorTestsFixture::LocalRelic : public ClosedTypedRelic<LocalRelic>
{
public:
    int value = 0;

    explicit LocalRelic(Init init) : ClosedTypedRelic(init)
    {}

    LocalRelic(Init init, int value) : ClosedTypedRelic(init), value(value)
    {}
};

class CuratorTestsFixture::GlobalRelic : public ClosedTypedRelic<GlobalRelic>
{
public:
    int value = 0;

    explicit GlobalRelic(Init init) : ClosedTypedRelic(init)
    {}

    GlobalRelic(Init init, int value) : ClosedTypedRelic(init), value(value)
    {}
};

class CuratorTestsFixture::BasicCurator final : public Curator
{
public:
    bool shouldAbort = false;
    std::function<void()> onWork = []() {};

    using Curator::Curator;

    void Work();

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
    Index<LocalRelic> localRelic;
    int localRelicValue = 0;

    explicit CuratorWithLocalRelicConstructor(Init init, int localRelicValue);
};

class CuratorTestsFixture::CuratorWithGlobalRelicConstructor final : public Curator
{
public:
    Index<GlobalRelic> globalRelic;
    int globalRelicValue = 0;

    explicit CuratorWithGlobalRelicConstructor(Init init);
};

namespace Inscription
{
    template<>
    class Scribe<CuratorTestsFixture::LocalRelic, BinaryArchive> final :
        public ArcaCompositeScribe<CuratorTestsFixture::LocalRelic, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override;
    };

    template<>
    class Scribe<CuratorTestsFixture::GlobalRelic, BinaryArchive> final :
        public ArcaCompositeScribe<CuratorTestsFixture::GlobalRelic, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override;
    };

    template<>
    class Scribe<CuratorTestsFixture::BasicCurator, BinaryArchive> final :
        public ArcaCompositeScribe<CuratorTestsFixture::BasicCurator, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override;
    };

    template<>
    class Scribe<CuratorTestsFixture::OtherBasicCurator, BinaryArchive> final :
        public ArcaCompositeScribe<CuratorTestsFixture::OtherBasicCurator, BinaryArchive>
    {
    public:
        static std::vector<Type> InputTypes(const ArchiveT& archive);
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override;
    };

    template<>
    class Scribe<CuratorTestsFixture::CuratorWithNonDefaultConstructor, BinaryArchive> final :
        public ArcaNullScribe<CuratorTestsFixture::CuratorWithNonDefaultConstructor, BinaryArchive>
    {};

    template<>
    class Scribe<CuratorTestsFixture::CuratorWithLocalRelicConstructor, BinaryArchive> final :
        public ArcaCompositeScribe<CuratorTestsFixture::CuratorWithLocalRelicConstructor, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override;
    };

    template<>
    class Scribe<CuratorTestsFixture::CuratorWithGlobalRelicConstructor, BinaryArchive> final :
        public ArcaCompositeScribe<CuratorTestsFixture::CuratorWithGlobalRelicConstructor, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override;
    };
}