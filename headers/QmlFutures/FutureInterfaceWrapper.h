/* License:  MIT
 * Source:   https://github.com/ihor-drachuk/QmlFutures
 * Contact:  ihor-drachuk-libs@pm.me  */

#pragma once
#include <QVariant>
#include <QFuture>
#include <QFutureInterface>
#include <QmlFutures/Metatypes.h>

namespace QmlFutures {

//
// Abstraction of QFutureInterface<T> for QmlFutures.
//

class FutureInterfaceWrapper : public QObject
{
    Q_OBJECT
public:
    virtual ~FutureInterfaceWrapper() = default;
    virtual void start() = 0;
    virtual void cancel() = 0;
    virtual void finish(const QVariant& value) = 0;
    virtual QVariant getFuture() = 0;
};


template<typename T>
class FutureInterfaceWrapperT : public FutureInterfaceWrapper
{
public:
    FutureInterfaceWrapperT(const QFutureInterface<T>& futureInterface)
        : m_futureInterface(futureInterface)
    { }

    FutureInterfaceWrapperT(const QVariant& futureInterface)
        : m_futureInterface(futureInterface.value<QFutureInterface<T>>())
    { }

    FutureInterfaceWrapperT() = default;

    void start() override {
        m_futureInterface.reportStarted();
    };

    void cancel() override {
        m_futureInterface.reportCanceled();
        m_futureInterface.reportFinished();
    };

    void finish(const QVariant& value) override {
        m_futureInterface.reportResult(value.value<T>());
        m_futureInterface.reportFinished();
    };

    QVariant getFuture() override {
        return QVariant::fromValue(m_futureInterface.future());
    };

private:
    QFutureInterface<T> m_futureInterface;
};


template<>
class FutureInterfaceWrapperT<void> : public FutureInterfaceWrapper
{
public:
    FutureInterfaceWrapperT(const QFutureInterface<void>& futureInterface)
        : m_futureInterface(futureInterface)
    { }

    FutureInterfaceWrapperT(const QVariant& futureInterface)
        : m_futureInterface(futureInterface.value<QFutureInterface<void>>())
    { }

    FutureInterfaceWrapperT() = default;

    void start() override {
        m_futureInterface.reportStarted();
    };

    void cancel() override {
        m_futureInterface.reportCanceled();
        m_futureInterface.reportFinished();
    };

    void finish(const QVariant&) override {
        m_futureInterface.reportFinished();
    };

    QVariant getFuture() override {
        return QVariant::fromValue(m_futureInterface.future());
    };

private:
    QFutureInterface<void> m_futureInterface;
};

} // namespace QmlFutures
