#include "CuratorProvider.h"

namespace Arca
{
    CuratorProviderBase::~CuratorProviderBase() = default;

    auto CuratorProviderBase::Provide() -> Provided
    {
        return
        {
            ProvideImpl(),
            type
        };
    }

    CuratorProviderBase::CuratorProviderBase(Type type) :
        type(std::move(type))
    {}

    ExternalCuratorProvider::ExternalCuratorProvider(Curator* external, Type type) :
        CuratorProviderBase(std::move(type)), external(external)
    {}

    std::unique_ptr<CuratorProviderBase> ExternalCuratorProvider::Clone() const
    {
        return std::make_unique<ExternalCuratorProvider>(*this);
    }

    auto ExternalCuratorProvider::ProvideImpl() -> ProvidedCurator
    {
        return external;
    }
}