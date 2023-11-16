/* License:  MIT
 * Source:   https://github.com/ihor-drachuk/QmlFutures
 * Contact:  ihor-drachuk-libs@pm.me  */

#pragma once
#include <QObject>
#include <QVariant>
#include <QFuture>
#include <QFutureInterface>

Q_DECLARE_METATYPE(QFuture<void>)
Q_DECLARE_METATYPE(QFutureInterface<void>)
Q_DECLARE_METATYPE(QFuture<QVariant>)
Q_DECLARE_METATYPE(QFutureInterface<QVariant>)

class QQmlEngine;

namespace QmlFutures {

} // namespace QmlFutures
