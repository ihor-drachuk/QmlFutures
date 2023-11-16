/* License:  MIT
 * Source:   https://github.com/ihor-drachuk/QmlFutures
 * Contact:  ihor-drachuk-libs@pm.me  */

import QtQuick 2.9
import QtTest 1.0
import QmlFutures 1.0

Item {
    id: root

    Component {
        id: promiseComponent

        QmlPromise { }
    }

    TestCase {
        name: "QmlPromiseTest"

        function test_00_initial() {
        }

        function test_01_finish() {
            var promise = promiseComponent.createObject();
            var future = promise.future;
            compare(QmlFutures.isFinished(future), false);
            promise.fulfil = true;
            compare(QmlFutures.isFinished(future), true);
            compare(QmlFutures.isFulfilled(future), true);
            compare(QmlFutures.resultRawOf(future), null);
            compare(promise.result, null);
            promise.destroy();
        }

        function test_02_cancel() {
            var promise = promiseComponent.createObject();
            var future = promise.future;
            compare(QmlFutures.isFinished(future), false);
            promise.cancel = true;
            compare(QmlFutures.isFinished(future), true);
            compare(QmlFutures.isCanceled(future), true);
            compare(QmlFutures.resultRawOf(future), undefined);
            compare(promise.result, undefined);
            promise.destroy();
        }

        function test_03_finishResult() {
            var promise = promiseComponent.createObject();
            var future = promise.future;
            compare(QmlFutures.isFinished(future), false);
            promise.result = 117;
            compare(QmlFutures.isFinished(future), true);
            compare(QmlFutures.isFulfilled(future), true);
            compare(QmlFutures.resultRawOf(future), 117);
            compare(promise.result, 117);
            promise.destroy();
        }
    }
}
