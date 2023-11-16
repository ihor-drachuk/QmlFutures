/* License:  MIT
 * Source:   https://github.com/ihor-drachuk/QmlFutures
 * Contact:  ihor-drachuk-libs@pm.me  */

#include <QtQuickTest>
#include <QCoreApplication>
#include <QQmlEngine>
#include <QQmlContext>
#include <QQuickWindow>
#include <QSGRendererInterface>
#include <QFuture>
#include <QFutureInterface>
#include <cassert>

#include <QmlFutures/Init.h>

// 'Registrator' is created for compatibility with Qt 5.9

struct ComplexStructExample {
    int value1 { 0 };
    QString value2;
};

Q_DECLARE_METATYPE(ComplexStructExample);
Q_DECLARE_METATYPE(QFuture<ComplexStructExample>);

class ComplexStructProvider : public QObject
{
    Q_OBJECT
public:
    Q_INVOKABLE QFuture<ComplexStructExample> provide() {
        ComplexStructExample complexValue {120, "Hello"};

        QFutureInterface<ComplexStructExample> futureInterface;
        futureInterface.reportStarted();
        futureInterface.reportResult(complexValue);
        futureInterface.reportFinished();

        return futureInterface.future();
    }
};

class Registrator : public QObject
{
    Q_OBJECT
public:
    static void registerTypes(const char* url)
    {
        qmlRegisterType<Registrator>(url, 1, 0, "Registrator");
        qputenv("QML2_IMPORT_PATH", QML_FUTURES_ROOT_DIR"/src/Qml");
    }

    explicit Registrator(QObject* parent = nullptr)
        : QObject(parent)
    { }

    Q_INVOKABLE void registerAll()
    {
        auto context = QQmlEngine::contextForObject(this);
        assert(context);

        auto engine = context->engine();
        assert(engine);

        auto init = new QmlFutures::Init(*engine);
        QObject::connect(engine, &QQmlEngine::destroyed, engine, [init](){
            delete init;
        }, Qt::QueuedConnection);

        // Complex struct test
        qmlRegisterSingletonType<ComplexStructProvider>("QmlFutures", 1, 0, "ComplexStructProvider", [] (QQmlEngine*, QJSEngine *) -> QObject* {
            return new ComplexStructProvider();
        });

        QmlFutures::Init::instance()->registerType<ComplexStructExample>([](const ComplexStructExample& item) -> QVariant {
            QVariantMap result;
            result["value1"] = item.value1;
            result["value2"] = item.value2;
            return result;
        });
    }
};

static void Registrator_register()
{
    Registrator::registerTypes("Registrator");
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
    QQuickWindow::setGraphicsApi(QSGRendererInterface::NullRhi);
#elif QT_VERSION >= QT_VERSION_CHECK(5,14,0)
    QQuickWindow::setSceneGraphBackend(QSGRendererInterface::NullRhi);
#endif
}

Q_COREAPP_STARTUP_FUNCTION(Registrator_register)

QUICK_TEST_MAIN(QmlFutures_test3)

#include "main3.moc"
