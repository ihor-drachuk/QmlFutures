#include <QmlFutures/Condition.h>

#include <QQmlEngine>
#include <QQmlProperty>

//
// Exposed to QML (QmlCondition).
// Non-QML Condition used for triggering QFutures from QML.
//

namespace QmlFutures {

struct Condition::impl_t
{
    bool isActive { false };
    bool isValid { false };
    bool triggerOn { true };
    QF::Comparison comparison { QF::Equal };

    QQmlProperty srcProperty;
    QVariant targetValue;
};

Condition::Condition(QObject* contextObj)
{
    assert(contextObj);

    createImpl();
    impl().isActive = true;
    impl().triggerOn = false;
    impl().isValid = true;
    QObject::connect(contextObj, &QObject::destroyed, this, [this](){
        setIsActive(false);

        QObject c;
        QObject::connect(&c, &QObject::destroyed, this, [this](){ setIsValid(false); }, Qt::QueuedConnection);
    });
}

Condition::Condition(QObject* object, const QString& propertyName, const QVariant& value, QF::Comparison comparison)
{
    assert(object);
    assert(!propertyName.isEmpty());
    assert(QQmlProperty(object, propertyName).isValid());

    createImpl();
    impl().srcProperty = QQmlProperty(object, propertyName);
    impl().srcProperty.connectNotifySignal(this, SLOT(onSrcPropertyChanged()));
    impl().targetValue = value;
    impl().comparison = comparison;
    impl().isActive = compare();
    impl().triggerOn = true;
    impl().isValid = true;

    QObject::connect(object, &QObject::destroyed, this, [this](){ setIsValid(false); });
}

Condition::~Condition()
{
}

bool Condition::isActive() const
{
    return impl().isActive;
}

void Condition::setIsActive(bool value)
{
    if (impl().isActive == value)
        return;

    impl().isActive = value;
    emit isActiveChanged(impl().isActive);
}

bool Condition::isValid() const
{
    return impl().isValid;
}

void Condition::setIsValid(bool value)
{
    if (impl().isValid == value)
        return;
    impl().isValid = value;
    emit isValidChanged(impl().isValid);
}


bool Condition::triggerOn() const
{
    return impl().triggerOn;
}

void Condition::onSrcPropertyChanged()
{
    setIsActive(compare());
}

void Condition::registerTypes()
{
    qRegisterMetaType<QmlFutures::ConditionPtr>();
}

bool Condition::compare() const
{
    bool result;

    auto srcValue = impl().srcProperty.read();
    switch (impl().comparison) {
        case QF::Equal:
            result = (srcValue == impl().targetValue);
            break;

        case QF::NotEqual:
            result = (srcValue != impl().targetValue);
            break;
    }

    return result;
}

//
// --- QmlCondition ---
//

void QmlCondition::registerTypes()
{
    qmlRegisterType<QmlCondition>("QmlFutures", 1, 0, "QmlCondition");
}

const ConditionPtr& QmlCondition::condition() const
{
    return m_condition;
}

void QmlCondition::setCondition(const ConditionPtr& newCondition)
{
    if (m_condition == newCondition)
        return;

    const auto oldActiveValue = isActive();
    const auto oldValidValue = isValid();

    if (m_condition) {
        QObject::disconnect(m_condition.get(), nullptr, this, nullptr);
    }

    m_condition = newCondition;
    emit conditionChanged(m_condition);

    if (m_condition) {
        QObject::connect(m_condition.get(), &Condition::isActiveChanged, this, &QmlCondition::isActiveChanged);
        QObject::connect(m_condition.get(), &Condition::isValidChanged, this, &QmlCondition::isValidChanged);
    }

    if (oldActiveValue != isActive())
        emit isActiveChanged(isActive());

    if (oldValidValue != isValid())
        emit isValidChanged(isValid());
}

bool QmlCondition::isActive() const
{
    return m_condition ? m_condition->isActive() : false;
}

bool QmlCondition::isValid() const
{
    return m_condition ? m_condition->isValid() : false;
}

bool QmlCondition::triggerOn() const
{
    return m_condition ? m_condition->triggerOn() : false;
}

} // namespace QmlFutures
