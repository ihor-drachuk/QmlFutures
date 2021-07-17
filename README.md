## QmlFutures — Handle QFuture&lt;T> in QML

[![Build & test](https://github.com/ihor-drachuk/QmlFutures/actions/workflows/ci.yml/badge.svg?branch=master)](https://github.com/ihor-drachuk/QmlFutures/actions/workflows/ci.yml)

### Features list
 - **Scriptable access to QFuture<T> via QmlFutures singleton**
   - onFinished / onResult or onFulfilled / onCanceled
   - isRunning / isFinished / isFulfilled / isCanceled
   - resultRawOf / resultConvOf
   - stateOf

 - **Declarative access to QFuture<T> via QmlFutureWatcher**
   - Input property: future
   - Output properties: state, result, resultConverted, isFinished, isCanceled

 - **Context/lifetime tracking** - be sure your future-handler will not be called when irrelevant
   - QmlFutures.onFinished(f, QF.conditionObj(root), function(future, result, resultConv) { /* handler */ });

     Handler will be called only if `root` still exists.

   - QmlFutures.onFinished(f, QF.conditionProp(root, "visible", true, QF.Equal), function(future, result, resultConv) { /* handler */ });

     Handler will be called only if `root` is visible.

 - **Custom types support**

 - **Create QFuture&lt;QVariant> right from QML!**
   - Declarative way: QmlPromise
   - Scriptable:
     - QF.createFuture(fulfilTrigger, cancelTrigger)
     - QF.createTimedFuture(value, delayMs)
     - QF.createTimedCanceledFuture(delayMs)

### Setup (CMake)

```CMake
include(FetchContent)
FetchContent_Declare(QmlFutures
  GIT_REPOSITORY https://github.com/ihor-drachuk/QmlFutures.git
  GIT_TAG        master
)
FetchContent_MakeAvailable(QmlFutures)

target_link_libraries(YourProject PRIVATE QmlFutures)   # Replace "YourProject" !
```

### Example #1
```QML
import QtQuick 2.9
import QmlFutures 1.0

Item {
    id: root

    QmlFutureWatcher {
        future: FileReader.readAsync("text.txt")  // Assume 'FileReader::readSync' returns 'QFuture<QString>'

        onFinished: {
            if (fulfilled) {
                console.debug("Result content: " + result);
            } else {
                console.debug("Canceled!");
            }
        }
    }
}
```

### Example #2
```QML
import QtQuick 2.9
import QmlFutures 1.0

Item {
    id: root
    
    Button {
        onClicked: {
            var f = FileReader.readAsync("text.txt");  // Assume 'FileReader::readSync' returns 'QFuture<QString>'
            
            QmlFutures.onFinished(f, QF.conditionObj(root), function(future, result, resultConv) {
                if (QmlFutures.isFulfilled(future)) {
                    console.debug("Result content: " + result);
                } else {
                    console.debug("Canceled!");
                }
            });
        }
    }
}
```

### C++ part initialization & custom type registration
```C++
#include <QmlFutures/Init.h>  // <---


struct MyComplexType
{
    int value1 { 0 };
    QString value2;
};

Q_DECLARE_METATYPE(MyComplexType);           // <---
Q_DECLARE_METATYPE(QFuture<MyComplexType>);  // <---


int main(int argc, char *argv[])
{
    // ...
    QQmlApplicationEngine engine;

    QmlFutures::Init qmlFuturesInit(engine);  // <---

    QmlFutures::Init::instance()->registerType<MyComplexType>([](const MyComplexType& data) -> QVariant {
        QVariantMap result;
        result["value1"] = data.value1;
        result["value2"] = data.value2;
        return result;
    });

    // ...
}
```

In QML you can work with your QFuture&lt;MyComplexType> like this:
```QML
var f = ComplexStructProvider.provide();
            
QmlFutures.onFinished(f, null, function(future, result, resultConv){
    console.debug(JSON.stringify(resultConv));
    console.debug(resultConv.value1);
    console.debug(resultConv.value2);
});
```
 
Or
```QML
QmlFutureWatcher {
    future: ComplexStructProvider.provide();
    onFinished: {
        console.debug(resultConverted.value1);
        console.debug(resultConverted.value2);
    }
}
```

For more examples see tests/test3_qml_auto/*.qml
