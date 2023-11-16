/* License:  MIT
 * Source:   https://github.com/ihor-drachuk/QmlFutures
 * Contact:  ihor-drachuk-libs@pm.me  */

import QtQuick 2.9
import QtTest 1.0
import QmlFutures 1.0

Item {
    id: root

    TestCase {
        name: "ComplexTypeTest"

        function test_00_initial() {
        }

        function test_01_readComplexValue() {
            var handled = false;

            var f = ComplexStructProvider.provide();
            QmlFutures.onFinished(f, null, function(future, result, resultConv){
                compare(resultConv.value1, 120);
                compare(resultConv.value2, "Hello");
                handled = true;
            });

            compare(handled, true);
        }
    }
}
