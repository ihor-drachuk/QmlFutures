/* License:  MIT
 * Source:   https://github.com/ihor-drachuk/QmlFutures
 * Contact:  ihor-drachuk-libs@pm.me  */

import QtQuick 2.9
import QtTest 1.0
import QmlFutures 1.0

Item {
    id: root

    Component {
        id: comp

        Item { }
    }

    QtObject {
        id: testObject
        property int value: 20
    }

    TestCase {
        name: "QmlFuturesTest"

        function test_00_initial() {
        }

        function test_01_onFinished() {
            var value = false;

            var f = QF.createTimedFuture(1, 17);
            QmlFutures.onFinished(f, null, function(future, raw, conv){
                value = true;
                compare(QmlFutures.resultRawOf(f), 1);
                compare(QmlFutures.resultConvOf(f), 1);
                compare(future, f);
                compare(raw, 1);
                compare(conv, 1);
            });

            QmlFutures.wait(f);

            compare(value, true);
        }

        function test_02_onFinishedReady() {
            var value = false;

            var f = QF.createTimedCanceledFuture(0);
            QmlFutures.onFinished(f, null, function(future, raw, conv){
                value = true;
                compare(QmlFutures.resultRawOf(f), undefined);
                compare(QmlFutures.resultConvOf(f), undefined);
                compare(future, f);
                compare(raw, undefined);
                compare(conv, undefined);
            });

            QmlFutures.wait(f);

            compare(value, true);
        }

        function test_03_onFinishedCanceled() {
            var value = false;

            var f = QF.createTimedCanceledFuture(17);
            QmlFutures.onFinished(f, null, function(future, raw, conv){
                value = true;
                compare(QmlFutures.resultRawOf(f), undefined);
                compare(QmlFutures.resultConvOf(f), undefined);
                compare(future, f);
                compare(raw, undefined);
                compare(conv, undefined);
            });

            QmlFutures.wait(f);

            compare(value, true);
        }

        function test_04_onFinishedCanceledReady() {
            var value = false;

            var f = QF.createTimedCanceledFuture(0);
            QmlFutures.onFinished(f, null, function(future, raw, conv){
                value = true;
                compare(QmlFutures.resultRawOf(f), undefined);
                compare(QmlFutures.resultConvOf(f), undefined);
                compare(future, f);
                compare(raw, undefined);
                compare(conv, undefined);
            });

            QmlFutures.wait(f);

            compare(value, true);
        }

        function test_05_onFinishedForget() {
            var value = false;

            var f = QF.createTimedFuture(1, 17);
            QmlFutures.onFinished(f, null, function(future, raw, conv){
                value = true;
            });

            compare(value, false);
            QmlFutures.forget(f);

            compare(value, false);
            QmlFutures.wait(f);

            compare(value, false);
        }

        function test_06_onFinishedConditionOn() {
            var value = 0;
            var obj = comp.createObject(root);

            var f = QF.createTimedFuture(1, 17);
            QmlFutures.onFinished(f,  QF.conditionObj(obj), function(){ value++; });
            QmlFutures.onFulfilled(f, QF.conditionObj(obj), function(){ value++; });
            QmlFutures.onCanceled(f,  QF.conditionObj(obj), function(){ value++; });

            QmlFutures.wait(f);

            obj.destroy();
            compare(value, 2);
        }

        function test_07_onFinishedConditionOff() {
            var value = 0;
            var obj = comp.createObject(root);

            var f = QF.createTimedFuture(1, 17);
            QmlFutures.onFinished(f,  QF.conditionObj(obj), function(){ value++; });
            QmlFutures.onFulfilled(f, QF.conditionObj(obj), function(){ value++; });
            QmlFutures.onCanceled(f,  QF.conditionObj(obj), function(){ value++; });

            obj.destroy();
            QmlFutures.wait(f);

            compare(value, 0);
        }

        function test_08_onFinishedConditionOn() {
            var value = 0;

            var f = QF.createTimedFuture(1, 17);
            QmlFutures.onFinished(f,  QF.conditionProp(testObject, "value", 20, QF.Equal), function(){ value++; });
            QmlFutures.onFulfilled(f, QF.conditionProp(testObject, "value", 20, QF.Equal), function(){ value++; });
            QmlFutures.onCanceled(f,  QF.conditionProp(testObject, "value", 20, QF.Equal), function(){ value++; });

            QmlFutures.wait(f);

            compare(value, 2);
        }

        function test_09_onFinishedConditionOff() {
            var value = 0;

            var f = QF.createTimedFuture(1, 17);
            QmlFutures.onFinished(f,  QF.conditionProp(testObject, "value", 20, QF.Equal), function(){ value++; });
            QmlFutures.onFulfilled(f, QF.conditionProp(testObject, "value", 20, QF.Equal), function(){ value++; });
            QmlFutures.onCanceled(f,  QF.conditionProp(testObject, "value", 20, QF.Equal), function(){ value++; });

            testObject.value++;
            testObject.value--;

            QmlFutures.wait(f);

            compare(value, 0);
        }

        function test_10_onFinishedConditionOff2() {
            var value = 0;

            var f = QF.createTimedFuture(1, 17);
            QmlFutures.onFinished(f,  QF.conditionProp(testObject, "value", 25, QF.Equal), function(){ value++; });
            QmlFutures.onFulfilled(f, QF.conditionProp(testObject, "value", 25, QF.Equal), function(){ value++; });
            QmlFutures.onCanceled(f,  QF.conditionProp(testObject, "value", 25, QF.Equal), function(){ value++; });

            QmlFutures.wait(f);

            compare(value, 0);
        }
    }
}
