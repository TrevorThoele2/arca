#pragma once

#include <Arca/Reliquary.h>

template<class Signal>
class SignalListener
{
public:
    using ExecutionList = std::vector<Signal>;
public:
    explicit SignalListener(Arca::Reliquary& reliquary);

    const ExecutionList& Executions() const;
private:
    ExecutionList executions;

    void On(const Signal& signal);
};

template<class Signal>
SignalListener<Signal>::SignalListener(Arca::Reliquary& reliquary)
{
    reliquary.On<Signal>([this](const Signal& signal) { this->On(signal); });
}

template<class Signal>
auto SignalListener<Signal>::Executions() const -> const ExecutionList&
{
    return executions;
}

template<class Signal>
void SignalListener<Signal>::On(const Signal& signal)
{
    executions.push_back(signal);
}