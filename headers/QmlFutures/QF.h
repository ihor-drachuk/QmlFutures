#pragma once
#include <QObject>
#include <memory>
#include <QmlFutures/Tools.h>

namespace QmlFutures {

//
// Singleton. Exposed to QML.
// Class for fast access to common things of QmlFutures.
//

class QF : public QObject, public Internal::Singleton<QF>
{
    Q_OBJECT
    friend class Init;
public:
    enum class WatcherState {
        Uninitialized,
        Pending,
        Running,
        Paused,
        Finished,
        FinishedFulfilled,
        FinishedCanceled
    };
    Q_ENUM(WatcherState);

    enum class Comparison {
        Equal,
        NotEqual
    };
    Q_ENUM(Comparison);

    enum class CombineTrigger {
        One,
        All
    };
    Q_ENUM(CombineTrigger);

public:
    QF();
    ~QF() override;

    Q_INVOKABLE QVariant conditionObj(QObject* object);
    Q_INVOKABLE QVariant conditionProp(QObject* object, const QString& propertyName, const QVariant& value, Comparison comparison);
    Q_INVOKABLE QVariant createFuture(const QVariant& fulfilTrigger, const QVariant& cancelTrigger);
    Q_INVOKABLE QVariant createTimedFuture(const QVariant& result, int time);
    Q_INVOKABLE QVariant createTimedCanceledFuture(int time);
    Q_INVOKABLE QVariant combine(QF::CombineTrigger trigger, const QVariant& sources);

private:
    static void registerTypes();

    struct CombineCtx;
    struct FutureCtx;
    struct TimedFutureCtx;
    using FutureCtxPtr = std::shared_ptr<QF::FutureCtx>;
    using CombineCtxPtr = std::shared_ptr<QF::CombineCtx>;
    using TimedFutureCtxPtr = std::shared_ptr<QF::TimedFutureCtx>;

private:
    static bool isNull(const QVariant& value);
    static bool isCondition(const QVariant& value);
    static bool isFuture(const QVariant& value);
    static bool isCanceled(const QVariant& value);
    static bool isFulfilled(const QVariant& value);

    void recheckFulfilCond(FutureCtx*);
    void recheckCancelCond(FutureCtx*);
    void finishTimedFuture(TimedFutureCtx*);
    void recheckFutureCond(FutureCtx*);
    void recheckFutureCancelCond(FutureCtx*);
    void recheckCombineCtx(CombineCtx*);

private:
    QF_DECLARE_PIMPL
};

} // namespace QmlFutures
