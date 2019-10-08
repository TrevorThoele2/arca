#pragma once

#include <stdexcept>

namespace Arca
{
    class SignalBatchInvalidated final : public std::runtime_error
    {
    public:
        SignalBatchInvalidated();
    };
}