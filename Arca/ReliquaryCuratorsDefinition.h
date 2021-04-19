#pragma once

#include "ReliquaryCurators.h"
#include "Reliquary.h"

namespace Arca
{
    template<class CuratorT, std::enable_if_t<std::is_same_v<CuratorT, Curator>, int>>
    Curator& ReliquaryCurators::Find(const TypeName& type)
    {
        return static_cast<CuratorT&>(Find(type));
    }

    template<class CuratorT, std::enable_if_t<std::is_same_v<CuratorT, Curator>, int>>
    const Curator& ReliquaryCurators::Find(const TypeName& type) const
    {
        return const_cast<ReliquaryCurators&>(*this).Find<CuratorT>(type);
    }

    template<class CuratorT, std::enable_if_t<is_curator_v<CuratorT>, int>>
    CuratorT& ReliquaryCurators::Find()
    {
        auto found = FindHandler<CuratorT>();
        if (found != nullptr)
            return found->curator;

        throw this->NotRegistered(TypeFor<CuratorT>(), typeid(CuratorT));
    }

    template<class CuratorT, std::enable_if_t<is_curator_v<CuratorT>, int>>
    const CuratorT& ReliquaryCurators::Find() const
    {
        return const_cast<ReliquaryCurators*>(this)->Find<CuratorT>();
    }

    template<class CuratorT>
    template<class... Args>
    ReliquaryCurators::Handler<CuratorT>::Handler(Reliquary& owner, Args&& ... args) :
        HandlerBase(TypeFor<CuratorT>().name), curator(CuratorInit{ owner }, std::forward<Args>(args)...)
    {}

    template<class CuratorT>
    Curator& ReliquaryCurators::Handler<CuratorT>::Value()
    {
        return curator;
    }

    template<class CuratorT>
    bool ReliquaryCurators::Handler<CuratorT>::WillBinarySerialize() const
    {
        return HasScribe<CuratorT, Inscription::Archive::Binary>();
    }

    template<class CuratorT>
    bool ReliquaryCurators::Handler<CuratorT>::WillJsonSerialize() const
    {
        return HasScribe<CuratorT, Inscription::Archive::Json>();
    }

    template<class CuratorT>
    void ReliquaryCurators::Handler<CuratorT>::Serialize(Inscription::Archive::Binary& archive)
    {
        archive(curator);
    }

    template<class CuratorT>
    void ReliquaryCurators::Handler<CuratorT>::Serialize(const std::string& name, Inscription::Archive::Json& archive)
    {
        archive(name, curator);
    }

    template<class CuratorT>
    std::vector<::Inscription::Type> ReliquaryCurators::Handler<CuratorT>::InscriptionTypes(Inscription::Archive::Binary& archive) const
    {
        return Inscription::InputTypesFor<CuratorT>(archive);
    }

    template<class CuratorT>
    std::vector<::Inscription::Type> ReliquaryCurators::Handler<CuratorT>::InscriptionTypes(Inscription::Archive::Json& archive) const
    {
        return Inscription::InputTypesFor<CuratorT>(archive);
    }

    template<class CuratorT, class... Args, std::enable_if_t<is_curator_v<CuratorT>, int>>
    void ReliquaryCurators::CreateHandler(Args&& ... args)
    {
        handlers.push_back(std::make_unique<Handler<CuratorT>>(Owner(), std::forward<Args>(args)...));
    }

    template<class CuratorT, std::enable_if_t<is_curator_v<CuratorT>, int>>
    ReliquaryCurators::Handler<CuratorT>* ReliquaryCurators::FindHandler() const
    {
        auto found = FindHandler(TypeFor<CuratorT>().name);
        if (found == nullptr)
            return nullptr;

        return static_cast<Handler<CuratorT>*>(found);
    }

    template<class CuratorT>
    bool ReliquaryCurators::Contains() const
    {
        return FindHandler<CuratorT>() != nullptr;
    }
}