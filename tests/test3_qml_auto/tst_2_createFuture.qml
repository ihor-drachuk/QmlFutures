import QtQuick 2.9
import QtTest 1.0
import QmlFutures 1.0

Item {
    id: root

    QmlFutureWatcher {
        id: futureWatcher
    }

    QmlFutureWatcher {
        id: futureWatcher2
    }

    Item {
        id: signalSpiesHolder
        property QtObject target: futureWatcher

        SignalSpy {
            id: ssUninitialized
            target: signalSpiesHolder.target
            signalName: "uninitialized"
            onTargetChanged: clear();
        }

        SignalSpy {
            id: ssInitialized
            target: signalSpiesHolder.target
            signalName: "initialized"
            onTargetChanged: clear();
        }

        SignalSpy {
            id: ssStarted
            target: signalSpiesHolder.target
            signalName: "started"
            onTargetChanged: clear();
        }

        SignalSpy {
            id: ssPaused
            target: signalSpiesHolder.target
            signalName: "paused"
            onTargetChanged: clear();
        }

        SignalSpy {
            id: ssFinished
            target: signalSpiesHolder.target
            signalName: "finished"
            onTargetChanged: clear();
        }

        SignalSpy {
            id: ssFulfilled
            target: signalSpiesHolder.target
            signalName: "fulfilled"
            onTargetChanged: clear();
        }

        SignalSpy {
            id: ssCanceled
            target: signalSpiesHolder.target
            signalName: "canceled"
            onTargetChanged: clear();
        }
    }

    QtObject {
        id: testObject
        property int value: 20
        property int value2: 50
    }

    TestCase {
        name: "CreateFutureTest"

        function test_00_initial() {
            compare(futureWatcher.future, undefined);
            compare(futureWatcher.state, QF.Uninitialized);
            compare(futureWatcher.result, undefined);
            compare(futureWatcher.resultConverted, undefined);
        }

        function test_01_futureFulfilCondition() {
            compare(ssUninitialized.count, 0);
            compare(ssInitialized.count, 0);
            compare(ssStarted.count, 0);
            compare(ssPaused.count, 0);
            compare(ssFinished.count, 0);
            compare(ssFulfilled.count, 0);
            compare(ssCanceled.count, 0);
            compare(futureWatcher.isFinished, false);

            futureWatcher.future = QF.createFuture(QF.conditionProp(testObject, "value", testObject.value + 1, QF.Equal), null);

            compare(futureWatcher.state, QF.Running);
            compare(futureWatcher.result, undefined);
            compare(futureWatcher.resultConverted, undefined);
            compare(futureWatcher.isFinished, false);
            compare(futureWatcher.isCanceled, false);
            compare(futureWatcher.isFulfilled, false);

            compare(ssUninitialized.count, 0);
            compare(ssInitialized.count, 1);
            compare(ssStarted.count, 1);
            compare(ssPaused.count, 0);
            compare(ssFinished.count, 0);
            compare(ssFulfilled.count, 0);
            compare(ssCanceled.count, 0);

            testObject.value++;
            wait(1);
            wait(1);

            compare(futureWatcher.state, QF.FinishedFulfilled);
            compare(futureWatcher.result, null);
            compare(futureWatcher.resultConverted, null);
            compare(futureWatcher.isFinished, true);
            compare(futureWatcher.isCanceled, false);

            compare(ssUninitialized.count, 0);
            compare(ssInitialized.count, 1);
            compare(ssStarted.count, 1);
            compare(ssPaused.count, 0);
            compare(ssFinished.count, 1);
            compare(ssFulfilled.count, 1);
            compare(ssCanceled.count, 0);
        }

        function test_02_futureFulfilConditionReady() {
            signalSpiesHolder.target = futureWatcher2;

            compare(ssUninitialized.count, 0);
            compare(ssInitialized.count, 0);
            compare(ssStarted.count, 0);
            compare(ssPaused.count, 0);
            compare(ssFinished.count, 0);
            compare(ssFulfilled.count, 0);
            compare(ssCanceled.count, 0);

            futureWatcher2.future = QF.createFuture(QF.conditionProp(testObject, "value", testObject.value, QF.Equal), null);

            compare(futureWatcher2.state, QF.FinishedFulfilled);
            compare(futureWatcher2.result, null);
            compare(futureWatcher2.resultConverted, null);
            compare(futureWatcher2.isFinished, true);
            compare(futureWatcher2.isCanceled, false);
            compare(futureWatcher2.isFulfilled, true);

            compare(ssUninitialized.count, 0);
            compare(ssInitialized.count, 1);
            compare(ssStarted.count, 0);
            compare(ssPaused.count, 0);
            compare(ssFinished.count, 1);
            compare(ssFulfilled.count, 1);
            compare(ssCanceled.count, 0);
        }

        function test_03_futureCancelCondition() {
            futureWatcher.future = null;
            signalSpiesHolder.target = futureWatcher;

            futureWatcher.future = QF.createFuture(QF.conditionProp(testObject, "value",  testObject.value + 1,  QF.Equal),
                                                   QF.conditionProp(testObject, "value2", testObject.value2 + 1, QF.Equal));

            compare(futureWatcher.state, QF.Running);
            compare(futureWatcher.result, undefined);
            compare(futureWatcher.resultConverted, undefined);
            compare(futureWatcher.isFinished, false);
            compare(futureWatcher.isCanceled, false);
            compare(futureWatcher.isFulfilled, false);

            compare(ssUninitialized.count, 0);
            compare(ssInitialized.count, 1);
            compare(ssStarted.count, 1);
            compare(ssPaused.count, 0);
            compare(ssFinished.count, 0);
            compare(ssFulfilled.count, 0);
            compare(ssCanceled.count, 0);

            testObject.value2++;
            wait(1);
            wait(1);

            compare(futureWatcher.state, QF.FinishedCanceled);
            compare(futureWatcher.result, undefined);
            compare(futureWatcher.resultConverted, undefined);
            compare(futureWatcher.isFinished, true);
            compare(futureWatcher.isCanceled, true);
            compare(futureWatcher.isFulfilled, false);

            compare(ssUninitialized.count, 0);
            compare(ssInitialized.count, 1);
            compare(ssStarted.count, 1);
            compare(ssPaused.count, 0);
            compare(ssFinished.count, 1);
            compare(ssFulfilled.count, 0);
            compare(ssCanceled.count, 1);
        }

        function test_04_futureCancelConditionReady() {
            futureWatcher.future = null;
            signalSpiesHolder.target = null;
            signalSpiesHolder.target = futureWatcher;

            futureWatcher.future = QF.createFuture(QF.conditionProp(testObject, "value",  testObject.value + 1,  QF.Equal),
                                                   QF.conditionProp(testObject, "value2", testObject.value2, QF.Equal));

            compare(futureWatcher.state, QF.FinishedCanceled);
            compare(futureWatcher.result, undefined);
            compare(futureWatcher.resultConverted, undefined);
            compare(futureWatcher.isFinished, true);
            compare(futureWatcher.isCanceled, true);
            compare(futureWatcher.isFulfilled, false);

            compare(ssUninitialized.count, 0);
            compare(ssInitialized.count, 1);
            compare(ssStarted.count, 0);
            compare(ssPaused.count, 0);
            compare(ssFinished.count, 1);
            compare(ssFulfilled.count, 0);
            compare(ssCanceled.count, 1);
        }

        function test_05_timedFuture() {
            futureWatcher.future = null;
            signalSpiesHolder.target = null;
            signalSpiesHolder.target = futureWatcher;

            futureWatcher.future = QF.createTimedFuture("val1", 10);

            ssFinished.wait(200);

            compare(futureWatcher.state, QF.FinishedFulfilled);
            compare(futureWatcher.result, "val1");
            compare(futureWatcher.resultConverted, "val1");
            compare(futureWatcher.isFinished, true);
            compare(futureWatcher.isCanceled, false);
            compare(futureWatcher.isFulfilled, true);

            compare(ssUninitialized.count, 0);
            compare(ssInitialized.count, 1);
            compare(ssStarted.count, 1);
            compare(ssPaused.count, 0);
            compare(ssFinished.count, 1);
            compare(ssFulfilled.count, 1);
            compare(ssCanceled.count, 0);
        }

        function test_06_timedFutureReady() {
            futureWatcher.future = null;
            signalSpiesHolder.target = null;
            signalSpiesHolder.target = futureWatcher;

            futureWatcher.future = QF.createTimedFuture("val1", 0);

            ssFinished.wait(200);

            compare(futureWatcher.state, QF.FinishedFulfilled);
            compare(futureWatcher.result, "val1");
            compare(futureWatcher.resultConverted, "val1");
            compare(futureWatcher.isFinished, true);
            compare(futureWatcher.isCanceled, false);
            compare(futureWatcher.isFulfilled, true);

            compare(ssUninitialized.count, 0);
            compare(ssInitialized.count, 1);
            compare(ssStarted.count, 0);
            compare(ssPaused.count, 0);
            compare(ssFinished.count, 1);
            compare(ssFulfilled.count, 1);
            compare(ssCanceled.count, 0);
        }

        function test_07_timedCanceledFuture() {
            futureWatcher.future = null;
            signalSpiesHolder.target = null;
            signalSpiesHolder.target = futureWatcher;

            futureWatcher.future = QF.createTimedCanceledFuture(10);

            ssFinished.wait(200);

            compare(futureWatcher.state, QF.FinishedCanceled);
            compare(futureWatcher.result, undefined);
            compare(futureWatcher.resultConverted, undefined);
            compare(futureWatcher.isFinished, true);
            compare(futureWatcher.isCanceled, true);
            compare(futureWatcher.isFulfilled, false);

            compare(ssUninitialized.count, 0);
            compare(ssInitialized.count, 1);
            compare(ssStarted.count, 1);
            compare(ssPaused.count, 0);
            compare(ssFinished.count, 1);
            compare(ssFulfilled.count, 0);
            compare(ssCanceled.count, 1);
        }

        function test_08_timedCanceledFutureReady() {
            futureWatcher.future = null;
            signalSpiesHolder.target = null;
            signalSpiesHolder.target = futureWatcher;

            futureWatcher.future = QF.createTimedCanceledFuture(0);

            ssFinished.wait(200);

            compare(futureWatcher.state, QF.FinishedCanceled);
            compare(futureWatcher.result, undefined);
            compare(futureWatcher.resultConverted, undefined);
            compare(futureWatcher.isFinished, true);
            compare(futureWatcher.isCanceled, true);
            compare(futureWatcher.isFulfilled, false);

            compare(ssUninitialized.count, 0);
            compare(ssInitialized.count, 1);
            compare(ssStarted.count, 0);
            compare(ssPaused.count, 0);
            compare(ssFinished.count, 1);
            compare(ssFulfilled.count, 0);
            compare(ssCanceled.count, 1);
        }

        function test_09a_futureFulfil() {
            futureWatcher.future = null;
            signalSpiesHolder.target = null;
            signalSpiesHolder.target = futureWatcher;

            futureWatcher.future = QF.createFuture(QF.createTimedFuture("myString", 30),
                                                   QF.createTimedFuture(null, 100));

            ssFinished.wait(200);

            compare(futureWatcher.state, QF.FinishedFulfilled);
            compare(futureWatcher.result, "myString");
            compare(futureWatcher.resultConverted, "myString");
            compare(futureWatcher.isFinished, true);
            compare(futureWatcher.isCanceled, false);
            compare(futureWatcher.isFulfilled, true);

            compare(ssUninitialized.count, 0);
            compare(ssInitialized.count, 1);
            compare(ssStarted.count, 1);
            compare(ssPaused.count, 0);
            compare(ssFinished.count, 1);
            compare(ssFulfilled.count, 1);
            compare(ssCanceled.count, 0);
        }

        function test_09b_futureFulfilReady() {
            futureWatcher.future = null;
            signalSpiesHolder.target = null;
            signalSpiesHolder.target = futureWatcher;

            futureWatcher.future = QF.createFuture(QF.createTimedFuture("myString", 0),
                                                   QF.createTimedFuture(null, 100));

            ssFinished.wait(200);

            compare(futureWatcher.state, QF.FinishedFulfilled);
            compare(futureWatcher.result, "myString");
            compare(futureWatcher.resultConverted, "myString");
            compare(futureWatcher.isFinished, true);
            compare(futureWatcher.isCanceled, false);
            compare(futureWatcher.isFulfilled, true);

            compare(ssUninitialized.count, 0);
            compare(ssInitialized.count, 1);
            compare(ssStarted.count, 0);
            compare(ssPaused.count, 0);
            compare(ssFinished.count, 1);
            compare(ssFulfilled.count, 1);
            compare(ssCanceled.count, 0);
        }

        function test_10_futureCancel() {
            futureWatcher.future = null;
            signalSpiesHolder.target = null;
            signalSpiesHolder.target = futureWatcher;

            futureWatcher.future = QF.createFuture(QF.createTimedFuture("myString", 100),
                                                   QF.createTimedFuture(null, 10));

            ssFinished.wait(200);

            compare(futureWatcher.state, QF.FinishedCanceled);
            compare(futureWatcher.result, undefined);
            compare(futureWatcher.resultConverted, undefined);
            compare(futureWatcher.isFinished, true);
            compare(futureWatcher.isCanceled, true);
            compare(futureWatcher.isFulfilled, false);

            compare(ssUninitialized.count, 0);
            compare(ssInitialized.count, 1);
            compare(ssStarted.count, 1);
            compare(ssPaused.count, 0);
            compare(ssFinished.count, 1);
            compare(ssFulfilled.count, 0);
            compare(ssCanceled.count, 1);
        }

        function test_11_futureCancel2() {
            futureWatcher.future = null;
            signalSpiesHolder.target = null;
            signalSpiesHolder.target = futureWatcher;

            futureWatcher.future = QF.createFuture(QF.createTimedFuture("myString", 100),
                                                   QF.createTimedCanceledFuture(10));

            ssFinished.wait(200);

            compare(futureWatcher.state, QF.FinishedCanceled);
            compare(futureWatcher.result, undefined);
            compare(futureWatcher.resultConverted, undefined);
            compare(futureWatcher.isFinished, true);
            compare(futureWatcher.isCanceled, true);
            compare(futureWatcher.isFulfilled, false);

            compare(ssUninitialized.count, 0);
            compare(ssInitialized.count, 1);
            compare(ssStarted.count, 1);
            compare(ssPaused.count, 0);
            compare(ssFinished.count, 1);
            compare(ssFulfilled.count, 0);
            compare(ssCanceled.count, 1);
        }

        function test_12_futureCancel3() {
            futureWatcher.future = null;
            signalSpiesHolder.target = null;
            signalSpiesHolder.target = futureWatcher;

            futureWatcher.future = QF.createFuture(QF.createTimedCanceledFuture(10),
                                                   QF.createTimedCanceledFuture(100));

            ssFinished.wait(200);

            compare(futureWatcher.state, QF.FinishedCanceled);
            compare(futureWatcher.result, undefined);
            compare(futureWatcher.resultConverted, undefined);
            compare(futureWatcher.isFinished, true);
            compare(futureWatcher.isCanceled, true);
            compare(futureWatcher.isFulfilled, false);

            compare(ssUninitialized.count, 0);
            compare(ssInitialized.count, 1);
            compare(ssStarted.count, 1);
            compare(ssPaused.count, 0);
            compare(ssFinished.count, 1);
            compare(ssFulfilled.count, 0);
            compare(ssCanceled.count, 1);
        }

        function test_13_futureCancel4() {
            futureWatcher.future = null;
            signalSpiesHolder.target = null;
            signalSpiesHolder.target = futureWatcher;

            futureWatcher.future = QF.createFuture(QF.createTimedCanceledFuture(0),
                                                   QF.createTimedCanceledFuture(100));

            ssFinished.wait(200);

            compare(futureWatcher.state, QF.FinishedCanceled);
            compare(futureWatcher.result, undefined);
            compare(futureWatcher.resultConverted, undefined);
            compare(futureWatcher.isFinished, true);
            compare(futureWatcher.isCanceled, true);
            compare(futureWatcher.isFulfilled, false);

            compare(ssUninitialized.count, 0);
            compare(ssInitialized.count, 1);
            compare(ssStarted.count, 0);
            compare(ssPaused.count, 0);
            compare(ssFinished.count, 1);
            compare(ssFulfilled.count, 0);
            compare(ssCanceled.count, 1);
        }

        function test_14_futureCancel5() {
            futureWatcher.future = null;
            signalSpiesHolder.target = null;
            signalSpiesHolder.target = futureWatcher;

            futureWatcher.future = QF.createFuture(QF.createTimedCanceledFuture(100),
                                                   QF.createTimedCanceledFuture(0));

            ssFinished.wait(200);

            compare(futureWatcher.state, QF.FinishedCanceled);
            compare(futureWatcher.result, undefined);
            compare(futureWatcher.resultConverted, undefined);
            compare(futureWatcher.isFinished, true);
            compare(futureWatcher.isCanceled, true);
            compare(futureWatcher.isFulfilled, false);

            compare(ssUninitialized.count, 0);
            compare(ssInitialized.count, 1);
            compare(ssStarted.count, 0);
            compare(ssPaused.count, 0);
            compare(ssFinished.count, 1);
            compare(ssFulfilled.count, 0);
            compare(ssCanceled.count, 1);
        }
    }
}
