#pragma once

#include "GeneralFixture.h"

using namespace Arca;

class SignalTestsFixture : public GeneralFixture
{
public:
    class BasicSignal
    {
    public:
        int value = 0;
    };

    class EmptySignal
    {};
};

namespace Arca
{
    template<>
    struct Traits<SignalTestsFixture::BasicSignal>
    {
        static constexpr ObjectType objectType = ObjectType::Signal;
        static const inline TypeName typeName = "SignalTestsFixture::BasicSignal";
    };

    template<>
    struct Traits<SignalTestsFixture::EmptySignal>
    {
        static constexpr ObjectType objectType = ObjectType::Signal;
        static const inline TypeName typeName = "SignalTestsFixture::EmptySignal";
    };
}