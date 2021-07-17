#include <QmlFutures/FutureWrapper.h>

#include <QEventLoop>

namespace QmlFutures {

QF::WatcherState FutureWrapper::getState() const
{
    if (isFinished()) {
        if (isCanceled()) {
            return QF::WatcherState::FinishedCanceled;
        } else {
            return QF::WatcherState::FinishedFulfilled;
        }
    } else if (isPaused()) {
        return QF::WatcherState::Paused;
    } else if (isStarted()) {
        return QF::WatcherState::Running;
    } else {
        return QF::WatcherState::Pending;
    }
}

void FutureWrapper::waitEL()
{
    if (isFinished())
        return;

    QEventLoop loop;
    QObject::connect(this, &FutureWrapper::stateChanged, &loop,
                     [this, &loop]()
    {
        if (isFinished())
            loop.quit();
    });

    loop.exec();
}

} // namespace QmlFutures
