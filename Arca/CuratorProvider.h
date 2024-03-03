#pragma once

#include <tuple>
#include <variant>

#include "Curator.h"
#include "CuratorTypeDescription.h"
#include "ProcessedCuratorTraits.h"

namespace Arca
{
    class CuratorProviderBase
    {
    public:
        using ProvidedCurator = std::variant<std::unique_ptr<Curator>, Curator*>;
        struct Provided
        {
            ProvidedCurator curator;
            CuratorTypeDescription description;
        };

        using CuratorMap = std::unordered_map<TypeHandle, Curator*>;
        using CuratorProviderMap = std::unordered_map<TypeHandle, CuratorProviderBase*>;
    public:
        CuratorProviderBase(const CuratorProviderBase& arg) = delete;
        CuratorProviderBase(CuratorProviderBase&& arg) = delete;

        virtual ~CuratorProviderBase() = 0;

        CuratorProviderBase& operator=(const CuratorProviderBase& arg) = delete;
        CuratorProviderBase& operator=(CuratorProviderBase&& arg) = delete;

        Provided Provide(CuratorMap& curators, CuratorProviderMap& curatorProviders, Reliquary& reliquary);
    protected:
        explicit CuratorProviderBase(CuratorTypeDescription typeDescription);
    protected:
        virtual ProvidedCurator ProvideImpl(CuratorMap& curators, CuratorProviderMap& curatorProviders, Reliquary& reliquary) = 0;
    private:
        CuratorTypeDescription typeDescription;
    };

    template<class Curator, class... Args>
    class CuratorProvider : public CuratorProviderBase
    {
    public:
        using ArgumentTuple = std::tuple<Args...>;
        ArgumentTuple arguments;
    public:
        explicit CuratorProvider(ArgumentTuple&& arguments);
    protected:
        ProvidedCurator ProvideImpl(CuratorMap& curators, CuratorProviderMap& curatorProviders, Reliquary& reliquary) override;
    };

    template<class Curator, class... Args>
    CuratorProvider<Curator, Args...>::CuratorProvider(ArgumentTuple&& arguments) :
        CuratorProviderBase(ProcessedCuratorTraits<Curator>::TypeDescription()), arguments(std::move(arguments))
    {}

    template<class Curator, class... Args>
    auto CuratorProvider<Curator, Args...>::ProvideImpl(
        CuratorMap& curators, CuratorProviderMap& curatorProviders, Reliquary& reliquary)
        -> ProvidedCurator
    {
        using ProcessedTraits = ProcessedCuratorTraits<Curator>;

        std::unique_ptr<Curator> created;
        ProcessedTraits::DoWithDependencies
        (
            [&created, &reliquary, this](typename ProcessedTraits::DependencyTuple& dependencies)
            {
                auto tuple = std::tuple_cat
                (
                    std::forward_as_tuple(reliquary),
                    dependencies,
                    arguments
                );

                created = std::move(std::apply(
                    [](auto&& ... args)
                    {
                        return std::make_unique<Curator>(std::forward<decltype(args)>(args)...);
                    }, tuple));
            },
            curators
        );
        return created;
    }

    class ExternalCuratorProvider final : public CuratorProviderBase
    {
    public:
        Curator* external;
    public:
        ExternalCuratorProvider(Curator* external, CuratorTypeDescription typeDescription);
    protected:
        ProvidedCurator ProvideImpl(CuratorMap& curators, CuratorProviderMap& curatorProviders, Reliquary& reliquary) override;
    };
}
