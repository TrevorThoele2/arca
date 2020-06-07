#pragma once

#include <vector>

namespace Arca
{
    class Reliquary;

    struct InscriptionUserContext
    {
        struct Dependent
        {
            int id = 0;
            Reliquary* reliquary = nullptr;
        };

        std::vector<Dependent> dependents;

        Reliquary* reliquary;

        int IDFor(Reliquary* reliquary);
        [[nodiscard]] Reliquary* ReliquaryFor(int id);
    private:
        static constexpr int nullID = 0;
        static constexpr int selfID = 1;
    };
}