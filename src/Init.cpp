#include <QmlFutures/Init.h>

#include <QObject>
#include <QMap>
#include <QQmlEngine>
#include <memory>
#include <QmlFutures/QF.h>
#include <QmlFutures/QmlFutures.h>
#include <QmlFutures/QmlFutureWatcher.h>
#include <QmlFutures/Condition.h>
#include <QmlFutures/QmlPromise.h>
#include <QmlFutures/Qml.h>

// -- Register default types --
#include <QString>
#include <QByteArray>
#include <QVariant>
#include <QVariantMap>
#include <QSize>

Q_DECLARE_METATYPE(QFuture<QString>)
Q_DECLARE_METATYPE(QFuture<int>)
Q_DECLARE_METATYPE(QFuture<bool>)
Q_DECLARE_METATYPE(QFuture<float>)
Q_DECLARE_METATYPE(QFuture<double>)
Q_DECLARE_METATYPE(QFuture<QByteArray>)
Q_DECLARE_METATYPE(QFuture<QVariantMap>)
Q_DECLARE_METATYPE(QFuture<QSize>)
// ----

namespace QmlFutures {

struct Init::impl_t
{
    QObject context;
    QQmlEngine* engine { nullptr };
    QmlFutures qmlFuturesSingleton;
    QF qfSingleton;
    QMap<int, FactoryMethod> futureWrappersFactory;
};

Init::Init(QQmlEngine& qmlEngine)
{
    createImpl();

    impl().engine = &qmlEngine;
    QObject::connect(impl().engine, &QQmlEngine::quit, &impl().context, [this](){ impl().engine = nullptr; });

    QF::registerTypes();
    QmlFutureWatcher::registerTypes();
    Condition::registerTypes();
    QmlCondition::registerTypes();
    Qml::init(qmlEngine);
    QmlFutures::registerTypes();
    QmlPromise::registerTypes();

    registerType<QString>();
    registerType<int>();
    registerType<void>();
    registerType<bool>();
    registerType<float>();
    registerType<double>();
    registerType<QByteArray>();
    registerType<QVariant>();
    registerType<QVariantMap>();
    registerType<QSize>();
}

Init::~Init()
{
}

QQmlEngine* Init::engine()
{
    return impl().engine;
}

std::shared_ptr<FutureWrapper> Init::createFutureWrapper(const QVariant& unknownFuture)
{
    auto typeId = unknownFuture.userType();
    assert(impl().futureWrappersFactory.contains(typeId) && "Have you registered this type?");
    auto wrapper = impl().futureWrappersFactory.value(typeId)(unknownFuture);
    return wrapper;
}

bool Init::isSupportedFuture(const QVariant& unknownFuture) const
{
    return impl().futureWrappersFactory.contains(unknownFuture.userType());
}

bool Init::isCondition(const QVariant& value)
{
    return (value.userType() == qMetaTypeId<ConditionPtr>());
}

bool Init::isNull(const QVariant& value)
{
    return (value.isNull() || !value.isValid());
}

void Init::registerType(int typeId, const FactoryMethod& converter)
{
    assert(!impl().futureWrappersFactory.contains(typeId) && "Already registered");
    assert(converter);
    impl().futureWrappersFactory.insert(typeId, converter);
}

} // namespace QmlFutures
