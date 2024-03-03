#pragma once

#include <tuple>
#include <variant>

#include "Curator.h"
#include "CuratorTypeDescription.h"
#include "CuratorTraits.h"

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
    public:
        CuratorProviderBase(const CuratorProviderBase& arg) = default;
        CuratorProviderBase(CuratorProviderBase&& arg) = delete;

        virtual ~CuratorProviderBase() = 0;

        [[nodiscard]] virtual std::unique_ptr<CuratorProviderBase> Clone() const = 0;

        CuratorProviderBase& operator=(const CuratorProviderBase& arg) = delete;
        CuratorProviderBase& operator=(CuratorProviderBase&& arg) = delete;

        Provided Provide(Reliquary& reliquary);
    protected:
        explicit CuratorProviderBase(CuratorTypeDescription typeDescription);
    protected:
        virtual ProvidedCurator ProvideImpl(Reliquary& reliquary) = 0;
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

        [[nodiscard]] std::unique_ptr<CuratorProviderBase> Clone() const override;
    protected:
        ProvidedCurator ProvideImpl(Reliquary& reliquary) override;
    };

    template<class Curator, class... Args>
    CuratorProvider<Curator, Args...>::CuratorProvider(ArgumentTuple&& arguments) :
        CuratorProviderBase(CuratorTypeDescriptionFor<Curator>()), arguments(std::move(arguments))
    {}

    template<class Curator, class... Args>
    std::unique_ptr<CuratorProviderBase> CuratorProvider<Curator, Args...>::Clone() const
    {
        return std::make_unique<CuratorProvider>(*this);
    }

    template<class Curator, class... Args>
    auto CuratorProvider<Curator, Args...>::ProvideImpl(Reliquary& reliquary) -> ProvidedCurator
    {
        return std::make_unique<Curator>(reliquary);
    }

    class ExternalCuratorProvider final : public CuratorProviderBase
    {
    public:
        Curator* external;
    public:
        ExternalCuratorProvider(Curator* external, CuratorTypeDescription typeDescription);

        [[nodiscard]] std::unique_ptr<CuratorProviderBase> Clone() const override;
    protected:
        ProvidedCurator ProvideImpl(Reliquary& reliquary) override;
    };
}
