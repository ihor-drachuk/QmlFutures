#pragma once
#include <QObject>
#include <QVariant>
#include <QmlFutures/Tools.h>
#include <QmlFutures/QF.h>

namespace QmlFutures {

//
// Exposed to QML.
// Allows to create QmlFutureWatcher in QML and handle QFuture<T>
//

class QmlFutureWatcher : public QObject
{
    Q_OBJECT
    friend class Init;
public:
    Q_PROPERTY(QVariant future READ future WRITE setFuture NOTIFY futureChanged)
    Q_PROPERTY(int state READ state NOTIFY stateChanged)
    Q_PROPERTY(QVariant result READ result NOTIFY resultChanged)
    Q_PROPERTY(QVariant resultConverted READ resultConverted NOTIFY resultConvertedChanged)
    Q_PROPERTY(bool isFinished READ isFinished NOTIFY isFinishedChanged)
    Q_PROPERTY(bool isCanceled READ isCanceled NOTIFY isCanceledChanged)
    Q_PROPERTY(bool isFulfilled READ isFulfilled NOTIFY isFulfilledChanged)

    QmlFutureWatcher();
    ~QmlFutureWatcher() override;

signals:
    void uninitialized();
    void initialized();
    void started();
    void paused();
    void finished(bool fulfilled, const QVariant& result, const QVariant& resultConverted);
    void fulfilled(const QVariant& result, const QVariant& resultConverted);
    void canceled();

// --- Properties support ---
public:
    QVariant future() const;
    QF::WatcherState state() const;
    QVariant result() const;
    QVariant resultConverted() const;
    bool isFinished() const;
    bool isCanceled() const;
    bool isFulfilled() const;

public slots:
    void setFuture(const QVariant& value);
    void setFutureImpl(const QVariant& value);

signals:
    void futureChanged(const QVariant& future);
    void stateChanged(QF::WatcherState state);
    void resultChanged(const QVariant& result);
    void resultConvertedChanged(const QVariant& resultConverted);
    void isFinishedChanged(bool isFinished);
    void isCanceledChanged(bool isCanceled);
    void isFulfilledChanged(bool isFulfilled);
// --- ---

private:
    static void registerTypes();
    void onFutureStateChanged();

private:
    QF_DECLARE_PIMPL
};

} // namespace QmlFutures
