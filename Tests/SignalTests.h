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
    struct Traits<::SignalTestsFixture::BasicSignal>
    {
        static const ObjectType objectType = ObjectType::Signal;
        static inline const TypeName typeName = "SignalTests_BasicSignal";
    };

    template<>
    struct Traits<::SignalTestsFixture::EmptySignal>
    {
        static const ObjectType objectType = ObjectType::Signal;
        static inline const TypeName typeName = "SignalTests_EmptySignal";
    };
}