/* License:  MIT
 * Source:   https://github.com/ihor-drachuk/QmlFutures
 * Contact:  ihor-drachuk-libs@pm.me  */

import QtQuick 2.9
import Registrator 1.0

Item {
    Registrator {
        Component.onCompleted: registerAll();
    }
}
