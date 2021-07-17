#pragma once
#include <QObject>
#include <QmlFutures/Metatypes.h>
#include <QmlFutures/Tools.h>

namespace QmlFutures {

//
// Exposed to QML.
// Declarative QFutureInterface
//

class QmlPromise : public QObject
{
    Q_OBJECT
    friend class Init;
public:
    Q_PROPERTY(bool fulfil READ fulfil WRITE setFulfil NOTIFY fulfilChanged)
    Q_PROPERTY(bool cancel READ cancel WRITE setCancel NOTIFY cancelChanged)
    Q_PROPERTY(QVariant result READ result WRITE setResult NOTIFY resultChanged)
    Q_PROPERTY(QFuture<QVariant> future READ future CONSTANT)

    explicit QmlPromise(QObject* parent = nullptr);
    ~QmlPromise() override;

    bool fulfil() const;
    void setFulfil(bool value);
    bool cancel() const;
    void setCancel(bool value);
    const QVariant& result() const;
    void setResult(const QVariant& value);
    QFuture<QVariant> future() const;

signals:
    void fulfilChanged(bool fulfil);
    void cancelChanged(bool cancel);
    void resultChanged(const QVariant& result);

private:
    static void registerTypes();

private:
    QF_DECLARE_PIMPL
};

} // namespace QmlFutures
