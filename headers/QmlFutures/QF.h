#pragma once
#include <QObject>
#include <QmlFutures/Tools.h>

namespace QmlFutures {

struct FutureCtx;
struct TimedFutureCtx;

//
// Singleton. Exposed to QML.
// Class for fast access to common things of QmlFutures.
//

class QF : public QObject, public Internal::Singleton<QF>
{
    Q_OBJECT
    friend class Init;
public:
    enum WatcherState {
        Uninitialized,
        Pending,
        Running,
        Paused,
        Finished,
        FinishedFulfilled,
        FinishedCanceled
    };
    Q_ENUM(WatcherState);

    enum Comparison {
        Equal,
        NotEqual
    };
    Q_ENUM(Comparison);

public:
    QF();
    ~QF() override;

    Q_INVOKABLE QVariant conditionObj(QObject* object);
    Q_INVOKABLE QVariant conditionProp(QObject* object, const QString& propertyName, const QVariant& value, Comparison comparison);
    Q_INVOKABLE QVariant createFuture(const QVariant& fulfilTrigger, const QVariant& cancelTrigger);
    Q_INVOKABLE QVariant createTimedFuture(const QVariant& result, int time);
    Q_INVOKABLE QVariant createTimedCanceledFuture(int time);

private:
    static void registerTypes();

private:
    bool isVariantCondition(const QVariant& value) const;
    bool isVariantFuture(const QVariant& value) const;
    void recheckFulfilCond(FutureCtx*);
    void recheckCancelCond(FutureCtx*);
    void finishTimedFuture(TimedFutureCtx*);
    void recheckFutureCond(FutureCtx*);
    void recheckFutureCancelCond(FutureCtx*);

private:
    QF_DECLARE_PIMPL
};

} // namespace QmlFutures
