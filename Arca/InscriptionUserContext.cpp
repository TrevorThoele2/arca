#include "InscriptionUserContext.h"

#include "CannotFindDependent.h"

namespace Arca
{
    int InscriptionUserContext::IDFor(Reliquary* reliquary)
    {
        if (reliquary == nullptr)
            return nullID;

        if (reliquary == this->reliquary)
            return selfID;

        for (auto& dependent : dependents)
            if (dependent.reliquary == reliquary)
                return dependent.id;

        throw CannotFindDependent(reliquary);
    }

    Reliquary* InscriptionUserContext::ReliquaryFor(int id)
    {
        if (id == nullID)
            return nullptr;

        if (id == selfID)
            return reliquary;

        for(auto& dependent : dependents)
            if (dependent.id == id)
                return dependent.reliquary;

        throw CannotFindDependent(id);
    }
}