#include <QmlFutures/QF.h>

#include <QQmlEngine>
#include <QMap>
#include <QTimer>
#include <QJSValueList>
#include <QMetaEnum>
#include <cassert>
#include <optional>
#include <QmlFutures/Init.h>
#include <QmlFutures/Metatypes.h>
#include <QmlFutures/Condition.h>
#include <QmlFutures/FutureInterfaceWrapper.h>
#include <QmlFutures/QmlFutureWatcher.h>

namespace QmlFutures {

struct QF::CombineCtx
{
    QF* master { nullptr };
    QF::CombineTrigger trigger;
    QFutureInterface<QVariant> interface;
    QList<std::shared_ptr<FutureWrapper>> futureWrappers;
    QList<ConditionPtr> conditions;
    QList<QMetaObject::Connection> connections;

    CombineCtx(QF* master)
        : master(master)
    {
        assert(master);
    }

    ~CombineCtx() {
        disconnect();

        if (!interface.isFinished()) {
            interface.reportCanceled();
            interface.reportFinished();
        }
    }

    bool isCanceled() {
        for (const auto& x : qAsConst(futureWrappers))
            if (x->isCanceled())
                return true;

        for (const auto& x : qAsConst(conditions))
            if (!x->isValid())
                return true;

        return false;
    }

    bool isFulfilled() {
        switch (trigger) {
            case QF::CombineTrigger::One:
                for (const auto& x : qAsConst(futureWrappers))
                    if (x->isFulfilled())
                        return true;

                for (const auto& x : qAsConst(conditions))
                    if (x->isActive() == x->triggerOn())
                        return true;

                return false;
                break;

            case QF::CombineTrigger::All: {
                int counter = 0;
                int limit = futureWrappers.size() + conditions.size();

                for (const auto& x : qAsConst(futureWrappers))
                    if (x->isFulfilled())
                        counter++;

                for (const auto& x : qAsConst(conditions))
                    if (x->isActive() == x->triggerOn())
                        counter++;

                return (counter == limit);
                break;
            }
        }

        assert(!"Unexpected flow");
        return true;
    }

    void connect() {
        for (const auto& x : qAsConst(futureWrappers)) {
            auto con = QObject::connect(x.get(), &FutureWrapper::stateChanged, master, [this, master = master](){ master->recheckCombineCtx(this); });
            connections.append(con);
        }

        for (const auto& x : qAsConst(conditions)) {
            auto con1 = QObject::connect(x.get(), &Condition::isActiveChanged, master, [this, master = master](){ master->recheckCombineCtx(this); });
            auto con2 = QObject::connect(x.get(), &Condition::isValidChanged, master, [this, master = master](){ master->recheckCombineCtx(this); });
            connections.append(con1);
            connections.append(con2);
        }
    }

    void disconnect() {
        for (auto& x : connections) {
            QObject::disconnect(x);
        }

        connections.clear();
    }
};

struct QF::FutureCtx
{
    FutureCtx() = delete;

    FutureCtx(QF* master) {
        assert(master);
        this->master = master;
    }

    ~FutureCtx() {
        disconnect();
    }

    QF* master;

    QFutureInterface<QVariant> interface;

    ConditionPtr condition;
    ConditionPtr conditionCancel;

    std::shared_ptr<FutureWrapper> futureWrapper;
    std::shared_ptr<FutureWrapper> futureCancelWrapper;

    void disconnect() {
        if (condition) QObject::disconnect(condition.get(), nullptr, master, nullptr);
        if (conditionCancel) QObject::disconnect(conditionCancel.get(), nullptr, master, nullptr);
    }
};

struct QF::TimedFutureCtx
{
    QFutureInterface<QVariant> interface;
    QTimer timer;
    std::optional<QVariant> value;
};

struct QF::impl_t
{
    QList<FutureCtxPtr> futures;
    QList<TimedFutureCtxPtr> timedFutures;
    QList<CombineCtxPtr> combines;
};


QF::QF()
{
    createImpl();
}

QF::~QF()
{
    for (const auto& x : qAsConst(impl().futures)) {
        x->interface.reportCanceled();
        x->interface.reportFinished();
    }

    for (const auto& x : qAsConst(impl().timedFutures)) {
        x->interface.reportCanceled();
        x->interface.reportFinished();
    }
}

QVariant QF::conditionObj(QObject* object)
{
    auto conditionPtr = std::make_shared<Condition>(object);
    return QVariant::fromValue(conditionPtr);
}

QVariant QF::conditionProp(QObject* object, const QString& propertyName, const QVariant& value, Comparison comparison)
{
    auto conditionPtr = std::make_shared<Condition>(object, propertyName, value, comparison);
    return QVariant::fromValue(conditionPtr);
}

QVariant QF::createFuture(const QVariant& fulfilTrigger, const QVariant& cancelTrigger)
{
    const bool isValid1 = (fulfilTrigger.isValid() && !fulfilTrigger.isNull());
    const bool isCondition1 = isCondition(fulfilTrigger);
    const bool isFuture1 = isFuture(fulfilTrigger);

    const bool isValid2 = (cancelTrigger.isValid() && !cancelTrigger.isNull());
    const bool isCondition2 = isCondition(cancelTrigger);
    const bool isFuture2 = isFuture(cancelTrigger);

    assert(isValid1 && (isCondition1 || isFuture1));
    assert(!isValid2 || isCondition2 || isFuture2);

    FutureCtxPtr ctx = std::make_shared<FutureCtx>(this);

    // Handle 'fulfil' trigger
    if (isCondition1) {
        ctx->condition = fulfilTrigger.value<ConditionPtr>();
        assert(ctx->condition->isValid());
        ctx->interface.reportStarted();

        if (ctx->condition->isActive() == ctx->condition->triggerOn()) {
            ctx->interface.reportResult(QVariant::fromValue(nullptr));
            ctx->interface.reportFinished();
            return QVariant::fromValue(ctx->interface.future());

        } else {
            QObject::connect(ctx->condition.get(), &Condition::isActiveChanged, this, [this, ptr = ctx.get()](){ recheckFulfilCond(ptr); });
            QObject::connect(ctx->condition.get(), &Condition::isValidChanged, this, [this, ptr = ctx.get()](){ recheckFulfilCond(ptr); });
        }

    } else if (isFuture1) {
        ctx->futureWrapper = Init::instance()->createFutureWrapper(fulfilTrigger);

        if (ctx->futureWrapper->isStarted())
            ctx->interface.reportStarted();

        if (ctx->futureWrapper->isFinished()) {
            if (ctx->futureWrapper->isCanceled()) {
                ctx->interface.reportCanceled();
            } else {
                ctx->interface.reportResult(ctx->futureWrapper->resultVariant());
            }

            ctx->interface.reportFinished();
            return QVariant::fromValue(ctx->interface.future());

        } else {
            QObject::connect(ctx->futureWrapper.get(), &FutureWrapper::stateChanged, this, [this, ptr = ctx.get()](){ recheckFutureCond(ptr); });
        }

    } else {
        assert(false);
    }

    // Handle 'cancel' trigger
    if (isCondition2) {
        ctx->conditionCancel = cancelTrigger.value<ConditionPtr>();
        assert(ctx->conditionCancel->isValid());

        if (ctx->conditionCancel->isActive() == ctx->conditionCancel->triggerOn()) {
            ctx->interface.reportCanceled();
            ctx->interface.reportFinished();
            return QVariant::fromValue(ctx->interface.future());

        } else {
            QObject::connect(ctx->conditionCancel.get(), &Condition::isActiveChanged, this, [this, ptr = ctx.get()](){ recheckCancelCond(ptr); });
            QObject::connect(ctx->conditionCancel.get(), &Condition::isValidChanged, this, [this, ptr = ctx.get()](){ recheckCancelCond(ptr); });
        }

    } else if (isFuture2) {
        ctx->futureCancelWrapper = Init::instance()->createFutureWrapper(cancelTrigger);

        if (ctx->futureCancelWrapper->isFinished()) {
            ctx->interface.reportCanceled();
            ctx->interface.reportFinished();
            return QVariant::fromValue(ctx->interface.future());

        } else {
            QObject::connect(ctx->futureCancelWrapper.get(), &FutureWrapper::stateChanged, this, [this, ptr = ctx.get()](){ recheckFutureCancelCond(ptr); });
        }

    } else {
        // Nothing.
    }

    impl().futures.append(ctx);
    return QVariant::fromValue(ctx->interface.future());
}

QVariant QF::createTimedFuture(const QVariant& result, int time)
{
    assert(time >= 0);

    if (time) {
        auto ctx = std::make_shared<TimedFutureCtx>();
        ctx->interface.reportStarted();
        ctx->value = result;

        ctx->timer.setSingleShot(true);
        QObject::connect(&ctx->timer, &QTimer::timeout, this, [this, ctx = ctx.get()](){ finishTimedFuture(ctx); }, Qt::QueuedConnection);
        impl().timedFutures.append(ctx);

        ctx->timer.start(time);

        return QVariant::fromValue(ctx->interface.future());

    } else {
        QFutureInterface<QVariant> interface;
        interface.reportStarted();
        interface.reportResult(result);
        interface.reportFinished();
        return QVariant::fromValue(interface.future());
    }
}

QVariant QF::createTimedCanceledFuture(int time)
{
    assert(time >= 0);

    if (time) {
        auto ctx = std::make_shared<TimedFutureCtx>();
        ctx->interface.reportStarted();

        ctx->timer.setSingleShot(true);
        QObject::connect(&ctx->timer, &QTimer::timeout, this, [this, ctx = ctx.get()](){ finishTimedFuture(ctx); }, Qt::QueuedConnection);
        impl().timedFutures.append(ctx);

        ctx->timer.start(time);

        return QVariant::fromValue(ctx->interface.future());

    } else {
        QFutureInterface<QVariant> interface;
        interface.reportStarted();
        interface.reportCanceled();
        interface.reportFinished();
        return QVariant::fromValue(interface.future());
    }
}

QVariant QF::combine(CombineTrigger trigger, const QVariant& sources)
{
    assert(Internal::isValidEnumValue(trigger));

    if (isNull(sources)) {
        return createTimedFuture(QVariant(), 0);

    } else if (isFuture(sources)) {
        return combine(trigger, QVariantList{sources});

    } else if (isCondition(sources)) {
        return combine(trigger, QVariantList{sources});
    }

    auto list = sources.toList();
    assert(!list.isEmpty());

    for (const auto& x : list)
        assert(isFuture(x) || isCondition(x));

    for (const auto& x : list)
        if (isCanceled(x))
            return createTimedCanceledFuture(0);

    for (const auto& x : list)
        if (isFulfilled(x))
            return createTimedFuture(QVariant(), 0);

    auto ctx = std::make_shared<CombineCtx>(this);
    ctx->trigger = trigger;

    for (const auto& x : list) {
        if (isFuture(x)) {
            ctx->futureWrappers.append(Init::instance()->createFutureWrapper(x));

        } else {
            assert(isCondition(x));
            ctx->conditions.append(x.value<ConditionPtr>());
        }
    }

    ctx->connect();
    impl().combines.append(ctx);
    return QVariant::fromValue(ctx->interface.future());
}

void QF::registerTypes()
{
    qRegisterMetaType<QF::WatcherState>("QF::WatcherState");
    qRegisterMetaType<QF::Comparison>("QF::Comparison");
    qRegisterMetaType<QF::CombineTrigger>("QF::CombineTrigger");

    qmlRegisterSingletonType<QF>("QmlFutures", 1, 0, "QF", [] (QQmlEngine *engine, QJSEngine *) -> QObject* {
        auto ret = QF::instance();
        engine->setObjectOwnership(ret, QQmlEngine::CppOwnership);
        return ret;
    });
}

bool QF::isNull(const QVariant& value)
{
    return Init::instance()->isNull(value);
}

bool QF::isCondition(const QVariant& value)
{
    return Init::instance()->isCondition(value);
}

bool QF::isFuture(const QVariant& value)
{
    return Init::instance()->isSupportedFuture(value);
}

bool QF::isCanceled(const QVariant& value)
{
    if (isCondition(value)) {
        auto condition = value.value<ConditionPtr>();
        return !condition->isValid();

    } else if (isFuture(value)) {
        return Init::instance()->createFutureWrapper(value)->isCanceled();

    } else {
        assert(!"Unexpected value");
        return true;
    }
}

bool QF::isFulfilled(const QVariant& value)
{
    if (isCondition(value)) {
        auto condition = value.value<ConditionPtr>();
        return (condition->isActive() == condition->triggerOn());

    } else if (isFuture(value)) {
        return Init::instance()->createFutureWrapper(value)->isFulfilled();

    } else {
        assert(!"Unexpected value");
        return true;
    }
}

void QF::recheckFulfilCond(FutureCtx* obj)
{
    auto it = std::find_if(impl().futures.begin(), impl().futures.end(),
                           [obj](const FutureCtxPtr& item) -> bool
    {
        return (item.get() == obj);
    });

    assert(it != impl().futures.end());

    if (it->get()->condition->isActive() == it->get()->condition->triggerOn()) {
        it->get()->disconnect();
        it->get()->interface.reportResult(QVariant::fromValue(nullptr));
        it->get()->interface.reportFinished();
        impl().futures.erase(it);

    } else if (!it->get()->condition->isValid()) {
        it->get()->disconnect();
        it->get()->interface.reportCanceled();
        it->get()->interface.reportFinished();
        impl().futures.erase(it);
    }
}

void QF::recheckCancelCond(FutureCtx* obj)
{
    auto it = std::find_if(impl().futures.begin(), impl().futures.end(),
                           [obj](const FutureCtxPtr& item) -> bool
    {
        return (item.get() == obj);
    });

    assert(it != impl().futures.end());

    if (!it->get()->condition->isValid() || it->get()->conditionCancel->isActive() == it->get()->conditionCancel->triggerOn()) {
        it->get()->disconnect();
        it->get()->interface.reportCanceled();
        it->get()->interface.reportFinished();
        impl().futures.erase(it);
    }
}

void QF::finishTimedFuture(TimedFutureCtx* ctx)
{
    auto it = std::find_if(impl().timedFutures.begin(), impl().timedFutures.end(),
                           [ctx](const TimedFutureCtxPtr& item) -> bool
    {
        return (item.get() == ctx);
    });

    assert(it != impl().timedFutures.end());

    if (it->get()->value) {
        it->get()->interface.reportResult(*it->get()->value);
    } else {
        it->get()->interface.reportCanceled();
    }

    it->get()->interface.reportFinished();

    impl().timedFutures.erase(it);
}

void QF::recheckFutureCond(FutureCtx* obj)
{
    auto it = std::find_if(impl().futures.begin(), impl().futures.end(),
                           [obj](const FutureCtxPtr& item) -> bool
    {
        return (item.get() == obj);
    });

    assert(it != impl().futures.end());

    if (obj->futureWrapper->isStarted() &&
        !obj->interface.isStarted())
    {
        obj->interface.reportStarted();
    }

    if (obj->futureWrapper->isFinished()) {
        if (obj->futureWrapper->isCanceled()) {
            obj->interface.reportCanceled();
        } else {
            obj->interface.reportResult(obj->futureWrapper->resultVariant());
        }

        obj->interface.reportFinished();

        impl().futures.erase(it);
    }
}

void QF::recheckFutureCancelCond(FutureCtx* obj)
{
    auto it = std::find_if(impl().futures.begin(), impl().futures.end(),
                           [obj](const FutureCtxPtr& item) -> bool
    {
        return (item.get() == obj);
    });

    assert(it != impl().futures.end());

    if (obj->futureCancelWrapper->isFinished()) {
        obj->interface.reportCanceled();
        obj->interface.reportFinished();
        impl().futures.erase(it);
    }
}

void QF::recheckCombineCtx(CombineCtx* ctx)
{
    auto it = std::find_if(impl().combines.begin(), impl().combines.end(), [ctx](const CombineCtxPtr& item) -> bool {
        return (item.get() == ctx);
    });

    assert(it != impl().combines.end());

    if (it->get()->isCanceled()) {
        it->get()->interface.reportCanceled();
        it->get()->interface.reportFinished();
        impl().combines.erase(it);

    } else if (it->get()->isFulfilled()) {
        it->get()->interface.reportResult(QVariant::fromValue(nullptr));
        it->get()->interface.reportFinished();
        impl().combines.erase(it);
    }
}

} // namespace QmlFutures
