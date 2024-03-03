#pragma once

#include "ReliquaryFixture.h"

using namespace Arca;

class SignalBatchFixture : public ReliquaryFixture
{
public:
    class BasicSignal;
    class UnregisteredSignal;
};

class SignalBatchFixture::BasicSignal
{
public:
    int myValue = 0;
public:
    BasicSignal() = default;
};

class SignalBatchFixture::UnregisteredSignal
{};