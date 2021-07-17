#include <QmlFutures/QmlFutureWatcher.h>

#include <QQmlEngine>
#include <QmlFutures/Init.h>

namespace QmlFutures {

struct QmlFutureWatcher::impl_t
{
    std::shared_ptr<FutureWrapper> wrapper;
    QVariant future;
    QF::WatcherState state { QF::WatcherState::Uninitialized };
    QVariant result;
    QVariant resultConverted;
    bool isFinished { false };
    bool isCanceled { false };
    bool isFulfilled { false };
};

QmlFutureWatcher::QmlFutureWatcher()
{
    createImpl();
}

QmlFutureWatcher::~QmlFutureWatcher()
{
}

QVariant QmlFutureWatcher::future() const
{
    return impl().future;
}

QF::WatcherState QmlFutureWatcher::state() const
{
    return impl().state;
}

QVariant QmlFutureWatcher::result() const
{
    return impl().result;
}

QVariant QmlFutureWatcher::resultConverted() const
{
    return impl().resultConverted;
}

bool QmlFutureWatcher::isFinished() const
{
    return impl().isFinished;
}

bool QmlFutureWatcher::isCanceled() const
{
    return impl().isCanceled;
}

bool QmlFutureWatcher::isFulfilled() const
{
    return impl().isFulfilled;
}

void QmlFutureWatcher::setFuture(const QVariant& value)
{
    if (impl().future == value)
        return;

    if (impl().state == QF::WatcherState::Uninitialized && (value.isNull() || !value.isValid()))
        return;

    if (impl().state != QF::WatcherState::Uninitialized)
        setFutureImpl(QVariant());

    setFutureImpl(value);
}

void QmlFutureWatcher::setFutureImpl(const QVariant& value)
{
    if (value.isNull() || !value.isValid()) {
        impl().wrapper.reset();
        impl().state = QF::WatcherState::Uninitialized;
        impl().future = QVariant();
        impl().result = QVariant();
        impl().resultConverted = QVariant();
        impl().isFinished = false;
        impl().isCanceled = false;
        impl().isFulfilled = false;

        emit stateChanged(impl().state);
        emit futureChanged(impl().future);
        emit resultChanged(impl().result);
        emit resultConvertedChanged(impl().resultConverted);
        emit isFinishedChanged(impl().isFinished);
        emit isCanceledChanged(impl().isCanceled);
        emit isFulfilledChanged(impl().isFulfilled);
        emit uninitialized();
    } else {
        if (Init::instance()->isFutureSupported(value)) {
            impl().future = value;
            impl().wrapper = Init::instance()->createFutureWrapper(value);
            impl().state = impl().wrapper->getState();
            impl().result = QVariant();
            impl().resultConverted = QVariant();

            QObject::connect(impl().wrapper.get(), &FutureWrapper::stateChanged, this, &QmlFutureWatcher::onFutureStateChanged, Qt::QueuedConnection);

            emit initialized();

            switch (impl().state) {
                case QF::WatcherState::Pending:
                    emit stateChanged(impl().state);
                    break;

                case QF::WatcherState::Running:
                    emit stateChanged(impl().state);
                    emit started();
                    break;

                case QF::WatcherState::Paused:
                    emit stateChanged(impl().state);
                    emit paused();
                    break;

                case QF::WatcherState::FinishedFulfilled:
                    impl().result = impl().wrapper->resultVariant();
                    impl().resultConverted = impl().wrapper->resultConverted();
                    impl().isFinished = true;
                    impl().isFulfilled = true;

                    impl().state = QF::Finished;
                    emit stateChanged(impl().state);
                    emit finished(true, impl().result, impl().resultConverted);

                    impl().state = QF::FinishedFulfilled;
                    emit stateChanged(impl().state);
                    emit fulfilled(impl().result, impl().resultConverted);
                    break;

                case QF::WatcherState::FinishedCanceled:
                    impl().isFinished = true;
                    impl().isCanceled = true;

                    impl().state = QF::Finished;
                    emit stateChanged(impl().state);
                    emit finished(false, QVariant(), QVariant());

                    impl().state = QF::FinishedCanceled;
                    emit stateChanged(impl().state);
                    emit canceled();
                    break;

                case QF::WatcherState::Uninitialized:
                case QF::WatcherState::Finished:
                    assert(!"Unexpected state");
                    break;
            }

            emit futureChanged(impl().future);
            emit resultChanged(impl().result);
            emit resultConvertedChanged(impl().resultConverted);
            emit isFinishedChanged(impl().isFinished);
            emit isCanceledChanged(impl().isCanceled);
            emit isFulfilledChanged(impl().isFulfilled);

        } else {
            // Unsupported QVariant. Is not QFuture<T>? QFuture<T> is not registered?
            assert(!"Unknown QVariant set to 'future' property!");
        }
    }
}

void QmlFutureWatcher::registerTypes()
{
    qmlRegisterType<QmlFutureWatcher>("QmlFutures", 1, 0, "QmlFutureWatcher");
}

void QmlFutureWatcher::onFutureStateChanged()
{
    if (impl().state == impl().wrapper->getState())
        return;

    impl().state = impl().wrapper->getState();

    switch (impl().state) {
        case QF::WatcherState::Running:
            emit stateChanged(impl().state);
            emit started();
            break;

        case QF::WatcherState::Paused:
            emit stateChanged(impl().state);
            emit paused();
            break;

        case QF::WatcherState::FinishedFulfilled:
            impl().result = impl().wrapper->resultVariant();
            impl().resultConverted = impl().wrapper->resultConverted();
            impl().isFinished = true;
            impl().isFulfilled = true;

            impl().state = QF::Finished;
            emit stateChanged(impl().state);
            emit finished(true, impl().result, impl().resultConverted);

            impl().state = QF::FinishedFulfilled;
            emit stateChanged(impl().state);
            emit fulfilled(impl().result, impl().resultConverted);

            emit resultChanged(impl().result);
            emit resultConvertedChanged(impl().resultConverted);

            emit isFinishedChanged(impl().isFinished);
            emit isFulfilledChanged(impl().isFulfilled);
            break;

        case QF::WatcherState::FinishedCanceled:
            impl().isFinished = true;
            impl().isCanceled = true;

            impl().state = QF::Finished;
            emit stateChanged(impl().state);
            emit finished(false, QVariant(), QVariant());

            impl().state = QF::FinishedCanceled;
            emit stateChanged(impl().state);
            emit canceled();

            emit isFinishedChanged(impl().isFinished);
            emit isCanceledChanged(impl().isCanceled);
            break;

        case QF::WatcherState::Pending:
        case QF::WatcherState::Uninitialized:
        case QF::WatcherState::Finished:
            assert(!"Unexpected state");
            break;
    }
}

} // namespace QmlFutures
