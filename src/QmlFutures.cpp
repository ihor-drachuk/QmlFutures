#include <QmlFutures/QmlFutures.h>

#include <QQmlEngine>
#include <QList>
#include <QJSValueList>
#include <QmlFutures/Init.h>
#include <QmlFutures/Condition.h>
#include <QmlFutures/FutureWrapper.h>

namespace QmlFutures {

struct HandlerCtx
{
    ConditionPtr condition;
    QJSValue handler;
};

struct QmlFutures::Context
{
    QVariant future;
    std::shared_ptr<FutureWrapper> wrapper;

    QList<HandlerCtx> finishedHandlers;
    QList<HandlerCtx> resultHandlers;
    QList<HandlerCtx> canceledHandlers;
};

struct QmlFutures::impl_t
{
    QList<QmlFutures::ContextPtr> contexts;
};


namespace {

void callJsValue(QJSValue value, QJSValueList& params) {
    value.call(params);
}

template<typename Arg0, typename... Args>
void callJsValue(QJSValue value, QJSValueList& params, const Arg0& arg0, const Args&... args) {
    auto param0 = Init::instance()->engine()->toScriptValue(arg0);
    params.append(param0);
    callJsValue(value, params, args...);
}

template<typename... Args>
void callJsValue(QJSValue value, const Args&... args) {
    if (!value.isCallable())
        return;

    QJSValueList list;
    callJsValue(value, list, args...);
}

} // namespace


QmlFutures::QmlFutures()
{
    createImpl();
}

QmlFutures::~QmlFutures()
{
}

bool QmlFutures::isSupportedFuture(const QVariant& value)
{
    return Init::instance()->isFutureSupported(value);
}

void QmlFutures::onFinished(const QVariant& future, const QVariant& context, const QJSValue& handler)
{
    assert(isSupportedFuture(future));
    assert(isNull(context) || isCondition(context));
    assert(handler.isUndefined() || handler.isNull() || handler.isCallable());

    if (isConditionCanceled(context))
        return;

    if (isFinished(future)) {
        if (isCanceled(future)) {
            callJsValue(handler, future);
        } else {
            callJsValue(handler, future, resultRawOf(future), resultConvOf(future));
        }
    } else {
        auto ctx = findOrAppendFutureCtx(future, true);
        ConditionPtr condition = isNull(context) ? ConditionPtr() : context.value<ConditionPtr>();
        ctx->finishedHandlers.append({condition, handler});

        if (condition) {
            QObject::connect(condition.get(), &Condition::isActiveChanged, this, &QmlFutures::conditionChanged);
            QObject::connect(condition.get(), &Condition::isValidChanged, this, &QmlFutures::conditionChanged);
        }
    }
}

void QmlFutures::onResult(const QVariant& future, const QVariant& context, const QJSValue& handler)
{
    onFulfilled(future, context, handler);
}

void QmlFutures::onFulfilled(const QVariant& future, const QVariant& context, const QJSValue& handler)
{
    assert(isSupportedFuture(future));
    assert(isNull(context) || isCondition(context));
    assert(handler.isUndefined() || handler.isNull() || handler.isCallable());

    if (isConditionCanceled(context))
        return;

    if (isFulfilled(future)) {
        callJsValue(handler, future, resultRawOf(future), resultConvOf(future));
    } else {
        auto ctx = findOrAppendFutureCtx(future, true);
        ConditionPtr condition = isNull(context) ? ConditionPtr() : context.value<ConditionPtr>();
        ctx->resultHandlers.append({condition, handler});

        if (condition) {
            QObject::connect(condition.get(), &Condition::isActiveChanged, this, &QmlFutures::conditionChanged);
            QObject::connect(condition.get(), &Condition::isValidChanged, this, &QmlFutures::conditionChanged);
        }
    }
}

void QmlFutures::onCanceled(const QVariant& future, const QVariant& context, const QJSValue& handler)
{
    assert(isSupportedFuture(future));
    assert(isNull(context) || isCondition(context));
    assert(handler.isUndefined() || handler.isNull() || handler.isCallable());

    if (isConditionCanceled(context))
        return;

    if (isCanceled(future)) {
        callJsValue(handler, future);
    } else {
        auto ctx = findOrAppendFutureCtx(future, true);
        ConditionPtr condition = isNull(context) ? ConditionPtr() : context.value<ConditionPtr>();
        ctx->canceledHandlers.append({condition, handler});

        if (condition) {
            QObject::connect(condition.get(), &Condition::isActiveChanged, this, &QmlFutures::conditionChanged);
            QObject::connect(condition.get(), &Condition::isValidChanged, this, &QmlFutures::conditionChanged);
        }
    }
}

void QmlFutures::forget(const QVariant& future)
{
    impl().contexts.removeOne(findFutureCtx(future));
}

void QmlFutures::wait(const QVariant& future)
{
    Init::instance()->createFutureWrapper(future)->waitEL();
}

bool QmlFutures::isRunning(const QVariant& future)
{
    return Init::instance()->createFutureWrapper(future)->isRunning();
}

bool QmlFutures::isFinished(const QVariant& future)
{
    return Init::instance()->createFutureWrapper(future)->isFinished();
}

bool QmlFutures::isFulfilled(const QVariant& future)
{
    return  Init::instance()->createFutureWrapper(future)->isFinished() &&
           !Init::instance()->createFutureWrapper(future)->isCanceled();
}

bool QmlFutures::isCanceled(const QVariant& future)
{
    return Init::instance()->createFutureWrapper(future)->isCanceled();
}

QVariant QmlFutures::resultRawOf(const QVariant& future)
{
    return Init::instance()->createFutureWrapper(future)->resultVariant();
}

QVariant QmlFutures::resultConvOf(const QVariant& future)
{
    return Init::instance()->createFutureWrapper(future)->resultConverted();
}

QF::WatcherState QmlFutures::stateOf(const QVariant& future)
{
    return Init::instance()->createFutureWrapper(future)->getState();
}

bool QmlFutures::isCondition(const QVariant& value)
{
    return (value.userType() == qMetaTypeId<ConditionPtr>());
}

bool QmlFutures::isNull(const QVariant& value)
{
    return (value.isNull() || !value.isValid());
}

bool QmlFutures::isConditionCanceled(const QVariant& value)
{
    if (isNull(value)) return false;

    assert(isCondition(value));
    auto cond = value.value<ConditionPtr>();
    assert(cond);

    return (!cond->isActive() || !cond->isValid());
}

QmlFutures::ContextPtr QmlFutures::findFutureCtx(const QVariant& future)
{
    auto it = std::find_if(impl().contexts.begin(), impl().contexts.end(),
                           [future](const QmlFutures::ContextPtr& item) ->bool
    {
        return (item->future == future);
    });

    return (it == impl().contexts.end()) ? ContextPtr() : *it;
}

QmlFutures::ContextPtr QmlFutures::findFutureCtx(Context* ctx)
{
    auto it = std::find_if(impl().contexts.begin(), impl().contexts.end(), [ctx](const ContextPtr& item){ return item.get() == ctx; });
    return (it == impl().contexts.end()) ? ContextPtr() : *it;
}

QmlFutures::ContextPtr QmlFutures::findOrAppendFutureCtx(const QVariant& future, bool append)
{
    auto ctx = findFutureCtx(future);

    if (ctx)
        return ctx;

    if (append) {
        impl().contexts.append(createFutureCtx(future));
        return impl().contexts.last();
    } else {
        return {};
    }
}

QmlFutures::ContextPtr QmlFutures::createFutureCtx(const QVariant& future)
{
    auto ctx = std::make_shared<QmlFutures::Context>();
    ctx->future = future;
    ctx->wrapper = Init::instance()->createFutureWrapper(future);
    QObject::connect(ctx->wrapper.get(), &FutureWrapper::stateChanged,
                     this, [this, ctx = ctx.get()]()
    {
        futureChanged(ctx);
    });

    return ctx;
}

void QmlFutures::futureChanged(Context* ctxPtr)
{
    auto ctx = findFutureCtx(ctxPtr);
    assert(ctx);

    if (ctx->wrapper->isFinished()) {
        forget(ctx->future);

        if (ctx->wrapper->isCanceled()) {
            for (const auto& x : qAsConst(ctx->finishedHandlers)) {
                assert(!x.condition || x.condition->isActive());
                callJsValue(x.handler, ctx->future);
            }
        } else {
            for (const auto& x : qAsConst(ctx->finishedHandlers)) {
                assert(!x.condition || x.condition->isActive());
                callJsValue(x.handler, ctx->future, resultRawOf(ctx->future), resultConvOf(ctx->future));
            }
        }
    }

    if (ctx->wrapper->isCanceled()) {
        for (const auto& x : qAsConst(ctx->canceledHandlers)) {
            assert(!x.condition || x.condition->isActive());
            callJsValue(x.handler, ctx->future);
        }
    }

    if (ctx->wrapper->isFulfilled()) {
        for (const auto& x : qAsConst(ctx->resultHandlers)) {
            assert(!x.condition || x.condition->isActive());
            callJsValue(x.handler, ctx->future, resultRawOf(ctx->future), resultConvOf(ctx->future));
        }
    }
}

void QmlFutures::conditionChanged()
{
    auto it = impl().contexts.begin();
    const auto itEnd = impl().contexts.end();
    while (it != itEnd) {
        auto hIt = it->get()->finishedHandlers.begin();
        auto hItEnd = it->get()->finishedHandlers.end();

        while (hIt != hItEnd) {
            auto remove = (hIt->condition && (!hIt->condition->isActive() || !hIt->condition->isValid()));
            if (remove) hIt->condition->disconnect(this);
            hIt = remove ? hIt = it->get()->finishedHandlers.erase(hIt) : hIt + 1;
        }

        hIt = it->get()->resultHandlers.begin();
        hItEnd = it->get()->resultHandlers.end();

        while (hIt != hItEnd) {
            auto remove = (hIt->condition && (!hIt->condition->isActive() || !hIt->condition->isValid()));
            if (remove) hIt->condition->disconnect(this);
            hIt = remove ? hIt = it->get()->resultHandlers.erase(hIt) : hIt + 1;
        }

        hIt = it->get()->canceledHandlers.begin();
        hItEnd = it->get()->canceledHandlers.end();

        while (hIt != hItEnd) {
            auto remove = (hIt->condition && (!hIt->condition->isActive() || !hIt->condition->isValid()));
            if (remove) hIt->condition->disconnect(this);
            hIt = remove ? hIt = it->get()->canceledHandlers.erase(hIt) : hIt + 1;
        }

        auto remove = it->get()->finishedHandlers.isEmpty() &&
                      it->get()->resultHandlers.isEmpty() &&
                      it->get()->canceledHandlers.isEmpty();

        it = remove ? impl().contexts.erase(it) : it + 1;
    }
}

void QmlFutures::registerTypes()
{
    qmlRegisterSingletonType<QmlFutures>("QmlFutures", 1, 0, "QmlFutures", [] (QQmlEngine *engine, QJSEngine *) -> QObject* {
        auto ret = QmlFutures::instance();
        engine->setObjectOwnership(ret, QQmlEngine::CppOwnership);
        return ret;
    });
}

} // namespace QmlFutures
