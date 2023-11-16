/* License:  MIT
 * Source:   https://github.com/ihor-drachuk/QmlFutures
 * Contact:  ihor-drachuk-libs@pm.me  */

#include <QmlFutures/Qml.h>
#include <QDir>
#include <QQmlEngine>

inline void initResource() {
    Q_INIT_RESOURCE(Qml);
}

namespace QmlFutures {
namespace Qml {

void init(QQmlEngine& qmlEngine) {
    initResource();
    qmlEngine.addImportPath(":/QmlFutures");
    qmlEngine.addImportPath("qrc:/QmlFutures");
}

} // namespace Qml
} // namespace QmlFutures
