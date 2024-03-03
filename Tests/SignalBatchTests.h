#pragma once

#include "ReliquaryFixture.h"

using namespace Arca;

class SignalBatchFixture : public ReliquaryFixture
{
public:
    class BasicSignal;
};

class SignalBatchFixture::BasicSignal
{
public:
    int myValue = 0;
public:
    BasicSignal() = default;
};

namespace Arca
{
    template<>
    struct Traits<::SignalBatchFixture::BasicSignal>
    {
        static const ObjectType objectType = ObjectType::Signal;
        static inline const TypeName typeName = "SignalBatchBasicSignal";
    };
}