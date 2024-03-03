#pragma once

#include <Arca/Command.h>

struct BasicCommand
{
    int value = 0;
};

namespace Arca
{
    template<>
    struct Traits<BasicCommand>
    {
        static constexpr ObjectType objectType = ObjectType::Command;
        static const inline TypeName typeName = "BasicCommand";
    };
}