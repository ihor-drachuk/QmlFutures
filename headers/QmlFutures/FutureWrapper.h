#pragma once
#include <QObject>
#include <QVariant>
#include <QFuture>
#include <QFutureWatcher>
#include <functional>
#include <QmlFutures/Metatypes.h>
#include <QmlFutures/QF.h>

namespace QmlFutures {

//
// Abstraction of QFuture<T> for QmlFutures.
//

template<typename T>
using Converter = std::function<QVariant(const T&)>;

class FutureWrapper : public QObject
{
    Q_OBJECT
public:
    virtual ~FutureWrapper() = default;
    virtual bool isStarted() const = 0;
    virtual bool isRunning() const = 0;
    virtual bool isPaused() const = 0;
    virtual bool isFinished() const = 0;
    virtual bool isCanceled() const = 0;
    virtual bool isFulfilled() const { return isFinished() && !isCanceled(); }
    virtual QVariant getFuture() const = 0;
    virtual QVariant resultVariant() const = 0;
    virtual QVariant resultConverted() const = 0;
    virtual std::shared_ptr<QFutureWatcherBase> getWatcher() const = 0;
    QF::WatcherState getState() const;
    virtual void wait() = 0;
    void waitEL();

signals:
    void stateChanged();

protected:
    void connect(QFutureWatcherBase& watcher) const {
        QObject::connect(&watcher, &QFutureWatcherBase::started,  this, &FutureWrapper::onStateChanged);
        QObject::connect(&watcher, &QFutureWatcherBase::finished, this, &FutureWrapper::onStateChanged);
        QObject::connect(&watcher, &QFutureWatcherBase::paused,   this, &FutureWrapper::onStateChanged);
        QObject::connect(&watcher, &QFutureWatcherBase::resumed,  this, &FutureWrapper::onStateChanged);
    }

private:
    void onStateChanged() {
        if (m_lastState != getState()) {
            m_lastState = getState();
            emit stateChanged();
        }
    }

private:
    QF::WatcherState m_lastState { QF::WatcherState::Uninitialized };
};


template<typename T>
class FutureWrapperT : public FutureWrapper
{
public:
    FutureWrapperT(const QVariant& future, const Converter<T>& converter)
        : m_future(future.value<QFuture<T>>()),
          m_converter(converter)
    {
        m_watcher = std::make_shared<QFutureWatcher<T>>();
        connect(*m_watcher);
        m_watcher->setFuture(m_future);
    }

    //~FutureWrapper() override;

    bool isStarted() const override { return m_future.isStarted(); }
    bool isRunning() const override { return m_future.isRunning(); }
    bool isPaused() const override { return m_future.isPaused(); }
    bool isFinished() const override { return m_future.isFinished(); }
    bool isCanceled() const override { return m_future.isCanceled(); }
    T result() const { return m_future.result(); }
    QVariant getFuture() const override { return QVariant::fromValue(m_future); }
    QVariant resultVariant() const override { return isCanceled() ? QVariant() : QVariant::fromValue(m_future.result()); }
    QVariant resultConverted() const override { return isCanceled() ? QVariant() : m_converter(result()); };
    std::shared_ptr<QFutureWatcherBase> getWatcher() const override { return m_watcher; }
    void wait() override { m_future.waitForFinished(); };

private:
    QFuture<T> m_future;
    Converter<T> m_converter;
    std::shared_ptr<QFutureWatcher<T>> m_watcher;
};


template<>
class FutureWrapperT<void> : public FutureWrapper
{
public:
    FutureWrapperT(const QVariant& future)
        : m_future(future.value<QFuture<void>>())
    {
        m_watcher = std::make_shared<QFutureWatcher<void>>();
        connect(*m_watcher);
        m_watcher->setFuture(m_future);
    }

    //~FutureWrapper() override;

    bool isStarted() const override { return m_future.isStarted(); }
    bool isRunning() const override { return m_future.isRunning(); }
    bool isPaused() const override { return m_future.isPaused(); }
    bool isFinished() const override { return m_future.isFinished(); }
    bool isCanceled() const override { return m_future.isCanceled(); }
    QVariant getFuture() const override { return QVariant::fromValue(m_future); }
    QVariant resultVariant() const override { return QVariant::fromValue(nullptr); }
    QVariant resultConverted() const override { return QVariant::fromValue(nullptr); };
    std::shared_ptr<QFutureWatcherBase> getWatcher() const override { return m_watcher; }
    void wait() override { m_future.waitForFinished(); };

private:
    QFuture<void> m_future;
    std::shared_ptr<QFutureWatcher<void>> m_watcher;
};

} // namespace QmlFutures
