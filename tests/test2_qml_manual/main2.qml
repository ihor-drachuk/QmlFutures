import QtQuick 2.9
import QtQuick.Controls 2.9
import QmlFutures 1.0

ApplicationWindow {
    width: 800
    height: 600
    visible: true

    QtObject {
        id: internal
        property var asyncData: QF.createTimedFuture("some data", 1000);
    }

    Timer {
        id: timeout
        repeat: false
        running: true
        interval: 500
    }

    // Option #1
    QmlFutureWatcher {
        future: QF.combine(QF.Any,
                           QF.conditionProp(timeout, "running", true, QF.NotEqual),
                           [internal.asyncData])

        onFulfilled: console.debug("Data retrieved: " + QmlFutures.resultConvOf(internal.asyncData))
        onCanceled: console.debug("Canceled or timeout occured!");
    }

    // Option #2
    QmlFutureWatcher {
        future: QF.combine(QF.Any,
                           null,
                           [internal.asyncData, QF.conditionProp(timeout, "running", true, QF.NotEqual)])

        onFinished: {
            if (QmlFutures.isFulfilled(internal.asyncData)) {
                console.debug("Data retrieved: " + QmlFutures.resultConvOf(internal.asyncData))
            } else {
                console.debug("Canceled or timeout occured!");
            }
        }
    }
}
