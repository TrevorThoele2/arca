#pragma once

namespace Arca
{
    class Reliquary;
};

namespace Inscription
{
    struct ReliquaryUserContext
    {
        Arca::Reliquary* reliquary = nullptr;
    };
}