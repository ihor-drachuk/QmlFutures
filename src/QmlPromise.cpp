/* License:  MIT
 * Source:   https://github.com/ihor-drachuk/QmlFutures
 * Contact:  ihor-drachuk-libs@pm.me  */

#include <QmlFutures/QmlPromise.h>

#include <QFutureInterface>
#include <QQmlEngine>
#include <QmlFutures/Metatypes.h>

namespace QmlFutures {

struct QmlPromise::impl_t
{
    bool fulfil { false };
    bool cancel { false };
    QVariant result;

    mutable QFutureInterface<QVariant> futureInterface;
};

QmlPromise::QmlPromise(QObject* parent)
    : QObject(parent)
{
    createImpl();
    impl().futureInterface.reportStarted();
}

QmlPromise::~QmlPromise()
{
}

bool QmlPromise::fulfil() const
{
    return impl().fulfil;
}

void QmlPromise::setFulfil(bool value)
{
    if (impl().fulfil == value)
        return;

    impl().fulfil = value;

    if (impl().fulfil && !impl().futureInterface.isFinished()) {
        impl().result = QVariant::fromValue(nullptr);
        impl().futureInterface.reportResult(impl().result);
        impl().futureInterface.reportFinished();
        emit resultChanged(impl().result);
    }

    emit fulfilChanged(impl().fulfil);
}

bool QmlPromise::cancel() const
{
    return impl().cancel;
}

void QmlPromise::setCancel(bool value)
{
    if (impl().cancel == value)
        return;

    impl().cancel = value;

    if (impl().cancel && !impl().futureInterface.isFinished()) {
        impl().futureInterface.reportCanceled();
        impl().futureInterface.reportFinished();
    }

    emit cancelChanged(impl().cancel);
}

const QVariant& QmlPromise::result() const
{
    return impl().result;
}

void QmlPromise::setResult(const QVariant& value)
{
    if (impl().result == value)
        return;

    impl().result = value;

    if (impl().result.isValid() && !impl().result.isNull() && !impl().futureInterface.isFinished()) {
        impl().futureInterface.reportResult(impl().result);
        impl().futureInterface.reportFinished();
    }

    emit resultChanged(impl().result);
}

QFuture<QVariant> QmlPromise::future() const
{
    return impl().futureInterface.future();
}

void QmlPromise::registerTypes()
{
    qmlRegisterType<QmlPromise>("QmlFutures", 1, 0, "QmlPromise");
}

} // namespace QmlFutures
