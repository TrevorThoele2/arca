#pragma once

#include "GeneralFixture.h"

#include <Arca/ClosedTypedRelic.h>

class PostulateTestsFixture : public GeneralFixture
{
public:
    class GlobalRelic;
};

namespace Arca
{
    template<>
    struct Traits<PostulateTestsFixture::GlobalRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline TypeName typeName = "PostulateTestsFixture_GlobalRelic";
        static const Locality locality = Locality::Global;
    };
}

class PostulateTestsFixture::GlobalRelic : public Arca::ClosedTypedRelic<GlobalRelic>
{
public:
    int integer = 0;
    std::string string;
public:
    GlobalRelic(Init init, int integer, std::string string);
};

namespace Inscription
{
    template<>
    class Scribe<PostulateTestsFixture::GlobalRelic, BinaryArchive> final
        : public ArcaNullScribe<PostulateTestsFixture::GlobalRelic, BinaryArchive>
    {};
}