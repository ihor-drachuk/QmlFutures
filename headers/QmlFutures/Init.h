/* License:  MIT
 * Source:   https://github.com/ihor-drachuk/QmlFutures
 * Contact:  ihor-drachuk-libs@pm.me  */

#pragma once
#include <QObject>
#include <QVariant>
#include <QFuture>
#include <QFutureWatcher>
#include <cassert>
#include <functional>
#include <QmlFutures/Tools.h>
#include <QmlFutures/QF.h>
#include <QmlFutures/FutureWrapper.h>

class QQmlEngine;

namespace QmlFutures {

//
// Singleton.
// Handles registration of types and type converters for QmlFutures.
//

class Init : public Internal::Singleton<Init>
{
public:
    Init(QQmlEngine& qmlEngine);
    ~Init();

    QQmlEngine* engine();

    template <typename T>
    inline void registerType(const Converter<T>& converter) {
        assert(converter && "Converter should be callable!");
        qRegisterMetaType<T>();
        auto typeId = qRegisterMetaType<QFuture<T>>();

        auto factoryMethod = [converter](const QVariant& future) -> std::shared_ptr<FutureWrapper> {
            auto wrapper = std::make_shared<FutureWrapperT<T>>(future, converter);
            return wrapper;
        };

        registerType(typeId, factoryMethod);
    }

    template <typename T,
              typename std::enable_if<std::is_same<T,void>::value == false>::type* = nullptr>
    inline void registerType() {
        auto converter = [](const T& value) -> QVariant { return QVariant::fromValue(value); };
        registerType<T>(converter);
    }

    template <typename T,
              typename std::enable_if<std::is_same<T,void>::value>::type* = nullptr>
    inline void registerType() {
        auto typeId = qRegisterMetaType<QFuture<T>>();

        auto factoryMethod = [](const QVariant& future) -> std::shared_ptr<FutureWrapper> {
            auto wrapper = std::make_shared<FutureWrapperT<void>>(future);
            return wrapper;
        };

        registerType(typeId, factoryMethod);
    }

    std::shared_ptr<FutureWrapper> createFutureWrapper(const QVariant& unknownFuture);
    bool isSupportedFuture(const QVariant& unknownFuture) const;
    static bool isCondition(const QVariant& value);
    static bool isNull(const QVariant& value);

private:
    using FactoryMethod = std::function<std::shared_ptr<FutureWrapper>(const QVariant& future)>;

    void registerType(int typeId, const FactoryMethod& converter);

private:
    QF_DECLARE_PIMPL
};

} // namespace QmlFutures
