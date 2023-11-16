/* License:  MIT
 * Source:   https://github.com/ihor-drachuk/QmlFutures
 * Contact:  ihor-drachuk-libs@pm.me  */

import QtQuick 2.9
import QtTest 1.0
import QmlFutures 1.0

Item {
    id: root

    QmlCondition {
        id: condition1
    }

    QmlCondition {
        id: condition2
    }
    Component {
        id: comp
        Item { }
    }

    QtObject {
        id: testObject
        property int value: 11
    }

    TestCase {
        name: "ConditionTest"

        function test_0_initial() {
            compare(condition1.isActive, false);
            compare(condition2.isActive, false);
            compare(condition1.isValid, false);
            compare(condition2.isValid, false);
        }

        function test_1_lifetime() {
            var obj = comp.createObject(root);
            condition1.condition = QF.conditionObj(obj);
            compare(condition1.isActive, true);
            compare(condition1.isValid, true);

            obj.destroy();
            wait(1);
            wait(1);

            compare(condition1.isActive, false);
            compare(condition1.isValid, false);
        }

        function test_2_property() {
            condition2.condition = QF.conditionProp(testObject, "value", testObject.value, QF.Equal);
            compare(condition2.isActive, true);
            testObject.value++;
            compare(condition2.isActive, false);
            testObject.value--;
            compare(condition2.isActive, true);
        }
    }
}
