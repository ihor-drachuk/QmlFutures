import QtQuick 2.9
import QtTest 1.0
import QmlFutures 1.0

Item {
    id: root

    Component {
        id: comp

        Item { property int value: 0 }
    }

    TestCase {
        name: "CombineTest"

        function test_00_initial() {
        }

        function test_01_futures_N2_One() {
            var f1 = QF.createTimedFuture(null, 10);
            var f2 = QF.createTimedFuture(null, 150);
            var f3 = QF.combine(QF.One, [f1, f2]);
            compare(QmlFutures.isFinished(f1), false);
            compare(QmlFutures.isFinished(f2), false);
            compare(QmlFutures.isFinished(f3), false);
            QmlFutures.wait(f1);
            wait(1);
            wait(1);
            compare(QmlFutures.isFinished(f1), true);
            //compare(QmlFutures.isFinished(f2), false);
            compare(QmlFutures.isFulfilled(f3), true);
        }

        function test_02_futures_N2_All() {
            var f1 = QF.createTimedFuture(null, 10);
            var f2 = QF.createTimedFuture(null, 150);
            var f3 = QF.combine(QF.All, [f1, f2]);
            compare(QmlFutures.isFinished(f1), false);
            compare(QmlFutures.isFinished(f2), false);
            compare(QmlFutures.isFinished(f3), false);
            QmlFutures.wait(f1);
            wait(1);
            wait(1);
            compare(QmlFutures.isFinished(f1), true);
            compare(QmlFutures.isFinished(f2), false);
            compare(QmlFutures.isFinished(f3), false);
            QmlFutures.wait(f2);
            wait(1);
            wait(1);
            compare(QmlFutures.isFinished(f1), true);
            compare(QmlFutures.isFinished(f2), true);
            compare(QmlFutures.isFulfilled(f3), true);
        }

        function test_03_conditions_N2_One() {
            var obj1 = comp.createObject();
            var obj2 = comp.createObject();
            var cond1 = QF.conditionProp(obj1, "value", 1, QF.Equal);
            var cond2 = QF.conditionProp(obj2, "value", 1, QF.Equal);

            var f = QF.combine(QF.One, [cond1, cond2]);
            compare(QmlFutures.isFinished(f), false);

            obj1.value = 1;

            wait(1);
            wait(1);

            compare(QmlFutures.isFulfilled(f), true);

            obj1.destroy();
            obj2.destroy();
        }

        function test_04_conditions_N2_All() {
            var obj1 = comp.createObject();
            var obj2 = comp.createObject();
            var cond1 = QF.conditionProp(obj1, "value", 1, QF.Equal);
            var cond2 = QF.conditionProp(obj2, "value", 1, QF.Equal);

            var f = QF.combine(QF.All, [cond1, cond2]);
            compare(QmlFutures.isFinished(f), false);

            obj1.value = 1;

            wait(1);
            wait(1);
            compare(QmlFutures.isFinished(f), false);

            obj2.value = 1;

            wait(1);
            wait(1);
            compare(QmlFutures.isFulfilled(f), true);

            obj1.destroy();
            obj2.destroy();
        }

        function test_05_future_condition_N3_One() {
            var obj1 = comp.createObject();
            var obj2 = comp.createObject();
            var cond1 = QF.conditionProp(obj1, "value", 1, QF.Equal);
            var cond2 = QF.conditionProp(obj2, "value", 1, QF.Equal);
            var f3 = QF.createTimedFuture(null, 100);

            var f = QF.combine(QF.One, [cond1, cond2, f3]);
            compare(QmlFutures.isFinished(f), false);

            obj1.value = 1;

            wait(1);
            wait(1);
            compare(QmlFutures.isFulfilled(f), true);

            obj1.destroy();
            obj2.destroy();
        }

        function test_06_future_condition_N3_One2() {
            var obj1 = comp.createObject();
            var obj2 = comp.createObject();
            var cond1 = QF.conditionProp(obj1, "value", 1, QF.Equal);
            var cond2 = QF.conditionProp(obj2, "value", 1, QF.Equal);
            var f3 = QF.createTimedFuture(null, 100);

            var f = QF.combine(QF.One, [cond1, cond2, f3]);
            compare(QmlFutures.isFinished(f), false);

            QmlFutures.wait(f3);

            wait(1);
            wait(1);
            compare(QmlFutures.isFulfilled(f), true);

            obj1.destroy();
            obj2.destroy();
        }

        function test_06_future_condition_N3_All() {
            var obj1 = comp.createObject();
            var obj2 = comp.createObject();
            var cond1 = QF.conditionProp(obj1, "value", 1, QF.Equal);
            var cond2 = QF.conditionProp(obj2, "value", 1, QF.Equal);
            var f3 = QF.createTimedFuture(null, 100);

            var f = QF.combine(QF.All, [cond1, cond2, f3]);
            compare(QmlFutures.isFinished(f), false);

            obj1.value = 1;
            QmlFutures.wait(f3);

            wait(1);
            wait(1);
            compare(QmlFutures.isFinished(f), false);

            obj2.value = 1;

            wait(1);
            wait(1);
            compare(QmlFutures.isFulfilled(f), true);

            obj1.destroy();
            obj2.destroy();
        }

        function test_07_future_condition_N3_All_Cancel() {
            var obj1 = comp.createObject();
            var obj2 = comp.createObject();
            var cond1 = QF.conditionProp(obj1, "value", 1, QF.Equal);
            var cond2 = QF.conditionProp(obj2, "value", 1, QF.Equal);
            var f3 = QF.createTimedFuture(null, 100);

            var f = QF.combine(QF.All, [cond1, cond2, f3]);
            compare(QmlFutures.isFinished(f), false);

            obj1.value = 1;
            QmlFutures.wait(f3);

            wait(1);
            wait(1);
            compare(QmlFutures.isFinished(f), false);

            obj2.destroy();

            wait(1);
            wait(1);
            compare(QmlFutures.isCanceled(f), true);

            obj1.destroy();
        }

        function test_08_future_condition_N3_All_Cancel2() {
            var obj1 = comp.createObject();
            var obj2 = comp.createObject();
            var cond1 = QF.conditionProp(obj1, "value", 1, QF.Equal);
            var cond2 = QF.conditionProp(obj2, "value", 1, QF.Equal);
            var f3 = QF.createTimedCanceledFuture(100);

            var f = QF.combine(QF.All, [cond1, cond2, f3]);
            compare(QmlFutures.isFinished(f), false);

            obj1.value = 1;

            wait(1);
            wait(1);
            compare(QmlFutures.isFinished(f), false);

            QmlFutures.wait(f3);

            wait(1);
            wait(1);
            compare(QmlFutures.isCanceled(f), true);

            obj1.destroy();
            obj2.destroy();
        }
    }
}
