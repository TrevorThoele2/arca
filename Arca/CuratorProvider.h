#pragma once

#include <tuple>
#include <variant>

#include "Curator.h"
#include "CuratorTraits.h"
#include "TypeHandle.h"

namespace Arca
{
    class CuratorProviderBase
    {
    public:
        using ProvidedCurator = std::variant<std::unique_ptr<Curator>, Curator*>;
        struct Provided
        {
            ProvidedCurator curator;
            TypeHandle typeHandle;
        };
    public:
        CuratorProviderBase(const CuratorProviderBase& arg) = default;
        CuratorProviderBase(CuratorProviderBase&& arg) = delete;

        virtual ~CuratorProviderBase() = 0;

        [[nodiscard]] virtual std::unique_ptr<CuratorProviderBase> Clone() const = 0;

        CuratorProviderBase& operator=(const CuratorProviderBase& arg) = delete;
        CuratorProviderBase& operator=(CuratorProviderBase&& arg) = delete;

        Provided Provide();
    protected:
        explicit CuratorProviderBase(TypeHandle typeHandle);
    protected:
        virtual ProvidedCurator ProvideImpl() = 0;
    private:
        TypeHandle typeHandle;
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
        ProvidedCurator ProvideImpl() override;
    };

    template<class Curator, class... Args>
    CuratorProvider<Curator, Args...>::CuratorProvider(ArgumentTuple&& arguments) :
        CuratorProviderBase(TypeHandleFor<Curator>()), arguments(std::move(arguments))
    {}

    template<class Curator, class... Args>
    std::unique_ptr<CuratorProviderBase> CuratorProvider<Curator, Args...>::Clone() const
    {
        return std::make_unique<CuratorProvider>(*this);
    }

    template<class Curator, class... Args>
    auto CuratorProvider<Curator, Args...>::ProvideImpl() -> ProvidedCurator
    {
        return std::make_unique<Curator>();
    }

    class ExternalCuratorProvider final : public CuratorProviderBase
    {
    public:
        Curator* external;
    public:
        ExternalCuratorProvider(Curator* external, TypeHandle typeHandle);

        [[nodiscard]] std::unique_ptr<CuratorProviderBase> Clone() const override;
    protected:
        ProvidedCurator ProvideImpl() override;
    };
}
