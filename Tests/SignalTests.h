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
};

namespace Arca
{
    template<>
    struct Traits<::SignalTestsFixture::BasicSignal>
    {
        static const ObjectType objectType = ObjectType::Signal;
        static inline const TypeName typeName = "SignalTestsBasicSignal";
    };
}