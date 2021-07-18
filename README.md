## QmlFutures — Handle QFuture&lt;T> in QML

[![Build & test](https://github.com/ihor-drachuk/QmlFutures/actions/workflows/ci.yml/badge.svg?branch=master)](https://github.com/ihor-drachuk/QmlFutures/actions/workflows/ci.yml)

### Features list
 - **Declarative access to QFuture<T> via QmlFutureWatcher**

 - **Scriptable access to QFuture<T> via QmlFutures singleton**

 - **Context/lifetime tracking** - you can be sure that your future-handler will not be called when irrelevant
   - QmlFutures.onFinished(f, QF.conditionObj(root), function(future, result, resultConv) { /* handler */ });
     - Handler will be called only if `root` still exists.
   - QmlFutures.onFinished(f, QF.conditionProp(root, "visible", true, QF.Equal), function(future, result, resultConv) { /* handler */ });
     - Handler will be called only if `root` is visible.

 - **Custom types support**

 - **Create QFuture&lt;QVariant> right from QML**
   - Declarative way: QmlPromise
   - Scriptable:
     - QF.createFuture(fulfilTrigger, cancelTrigger)
     - QF.createTimedFuture(value, delayMs)
     - QF.createTimedCanceledFuture(delayMs)

 - **Wait for multiple events simultaneously**
   - Configurable trigger mode: when all of them are finished or when at least one of them is finished

## API
`QmlFutures` singleton
  - bool isSupportedFuture(future);
  - void onFinished(future, context, handler);
  - void onResult(future, context, handler);
  - void onFulfilled(future, context, handler);
  - void onCanceled(future, context, handler);
  - void forget(future);
  - void wait(future);
  - bool isRunning(future);
  - bool isFinished(future);
  - bool isFulfilled(future);
  - bool isCanceled(future);
  - QVariant resultRawOf(future);
  - QVariant resultConvOf(future);
  - QF::WatcherState stateOf(future);

`QF` singleton
  - enum QF.WatcherState { Uninitialized, Pending, Running, Paused, Finished, FinishedFulfilled, FinishedCanceled}
  - enum QF.Comparison { Equal, NotEqual }
  - enum QF.CombineTrigger { Any, All }
  - QVariant conditionObj(object);
  - QVariant conditionProp(object, propertyName, value, comparison);
  - QVariant createFuture(fulfilTrigger, cancelTrigger);
  - QVariant createTimedFuture(result, delayMs);
  - QVariant createTimedCanceledFuture(delayMs);
  - QVariant combine(combineTrigger, context, list<QFuture_or_Condition>) — combine several futures and conditions to one QFuture

`QmlFutureWatcher` item
  - Property: future (in)
  - Property: state
  - Property: result
  - Property: resultConverted
  - Property: isFinished
  - Property: isCanceled
  - Property: isFulfilled
  - Signal: uninitialized()
  - Signal: initialized()
  - Signal: started()
  - Signal: paused()
  - Signal: finished(isFulfilled, result, resultConverted)
  - Signal: fulfilled(result, resultConverted)
  - Signal: canceled()

`QmlPromise` item
  - Property: fulfil — setting this to `true` finishes the future
  - Property: cancel — setting this to `true` cancels the future
  - Property: result — setting some value to this property finishes the future with that value
  - Property: future (out)

## Setup (CMake)
CMakeLists.txt
```CMake
include(FetchContent)
FetchContent_Declare(QmlFutures
  GIT_REPOSITORY https://github.com/ihor-drachuk/QmlFutures.git
  GIT_TAG        master
)
FetchContent_MakeAvailable(QmlFutures)

target_link_libraries(YourProject PRIVATE QmlFutures)   # Replace "YourProject" !
```

main.cpp
```C++
#include <QmlFutures/Init.h>

int main(int argc, char *argv[])
{
    // ...
    QQmlApplicationEngine engine;

    QmlFutures::Init qmlFuturesInit(engine);
    // ...
}
```

main.qml
```QML
import QmlFutures 1.0

// ...
```

## Examples & short manual

### Example: #1
```QML
import QtQuick 2.9
import QmlFutures 1.0

Item {
    id: root

    QmlFutureWatcher {
        future: FileReader.readAsync("text.txt")  // Assume 'FileReader::readSync' returns 'QFuture<QString>'

        onFinished: {
            if (isFulfilled) {
                console.debug("Result content: " + result);
            } else {
                console.debug("Canceled!");
            }
        }
    }
}
```

### Example: #2
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

### Example: C++ part initialization & custom type registration
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

### Example: Wait for multiple events
```QML
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
        interval: 1500
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
```

For more examples see tests/test3_qml_auto/*.qml
