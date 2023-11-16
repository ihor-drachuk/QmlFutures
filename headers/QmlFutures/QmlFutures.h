/* License:  MIT
 * Source:   https://github.com/ihor-drachuk/QmlFutures
 * Contact:  ihor-drachuk-libs@pm.me  */

#pragma once
#include <QObject>
#include <QVariant>
#include <QJSValue>
#include <QmlFutures/Tools.h>
#include <QmlFutures/QF.h>

namespace QmlFutures {

//
// Singleton. Exposed to QML.
// Allows to handle QFuture<T> in QML.
//

class QmlFutures : public QObject, public Internal::Singleton<QmlFutures>
{
    Q_OBJECT
    friend class Init;
public:
    QmlFutures();
    ~QmlFutures() override;

    Q_INVOKABLE bool isSupportedFuture(const QVariant& value);

    Q_INVOKABLE void onFinished(const QVariant& future, const QVariant& context, const QJSValue& handler);
    Q_INVOKABLE void onResult(const QVariant& future, const QVariant& context, const QJSValue& handler);
    Q_INVOKABLE void onFulfilled(const QVariant& future, const QVariant& context, const QJSValue& handler);
    Q_INVOKABLE void onCanceled(const QVariant& future, const QVariant& context, const QJSValue& handler);
    Q_INVOKABLE void forget(const QVariant& future);
    Q_INVOKABLE void wait(const QVariant& future);

    Q_INVOKABLE bool isRunning(const QVariant& future);
    Q_INVOKABLE bool isFinished(const QVariant& future);
    Q_INVOKABLE bool isFulfilled(const QVariant& future);
    Q_INVOKABLE bool isCanceled(const QVariant& future);
    Q_INVOKABLE QVariant resultRawOf(const QVariant& future);
    Q_INVOKABLE QVariant resultConvOf(const QVariant& future);

    Q_INVOKABLE QF::WatcherState stateOf(const QVariant& future);

private:
    struct Context;
    using ContextPtr = std::shared_ptr<Context>;

private:
    static void registerTypes();

    static bool isConditionCanceled(const QVariant& value);
    static bool isNull(const QVariant& value);
    static bool isCondition(const QVariant& value);

    ContextPtr findFutureCtx(const QVariant& future);
    ContextPtr findFutureCtx(Context* ctx);
    ContextPtr findOrAppendFutureCtx(const QVariant& future, bool append = false);
    ContextPtr createFutureCtx(const QVariant& future);

    void futureChanged(Context* ctxPtr);
    void conditionChanged();

private:
    QF_DECLARE_PIMPL
};

} // namespace QmlFutures
