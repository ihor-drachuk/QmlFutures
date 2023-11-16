/* License:  MIT
 * Source:   https://github.com/ihor-drachuk/QmlFutures
 * Contact:  ihor-drachuk-libs@pm.me  */

#pragma once
#include <QObject>
#include <QmlFutures/QF.h>
#include <QmlFutures/Tools.h>

//
// class Condition:    used for triggering QFutures from QML.
// class QmlCondition: exposed to QML
//

namespace QmlFutures {

class Condition : public QObject
{
    Q_OBJECT
    friend class Init;
public:
    Q_PROPERTY(bool isActive READ isActive /*WRITE setIsActive*/ NOTIFY isActiveChanged)
    Q_PROPERTY(bool isValid READ isValid WRITE setIsValid NOTIFY isValidChanged)
    Q_PROPERTY(bool triggerOn READ triggerOn CONSTANT)

    Condition(QObject* contextObj);
    Condition(QObject* object, const QString& propertyName, const QVariant& value, QF::Comparison comparison);
    ~Condition() override;

// --- Properties support ---
public:
    bool isActive() const;
    bool isValid() const;
    void setIsValid(bool value);
    bool triggerOn() const;

public slots:
    void setIsActive(bool value);

signals:
    void isActiveChanged(bool isActive);
    void isValidChanged(bool isValid);
// --- ---

private slots:
    void onSrcPropertyChanged();

private:
    static void registerTypes();
    bool compare() const;

private:
    QF_DECLARE_PIMPL
};

using ConditionPtr = std::shared_ptr<Condition>;


class QmlCondition : public QObject
{
    Q_OBJECT
    friend class Init;
public:
    Q_PROPERTY(QmlFutures::ConditionPtr condition READ condition WRITE setCondition NOTIFY conditionChanged)
    Q_PROPERTY(bool isActive READ isActive /*WRITE setIsActive*/ NOTIFY isActiveChanged)
    Q_PROPERTY(bool isValid READ isValid /*WRITE setIsValid*/ NOTIFY isValidChanged)
    Q_PROPERTY(bool triggerOn READ triggerOn CONSTANT)

    explicit QmlCondition(QObject* parent = nullptr)
        : QObject(parent)
    { }

// --- Properties support ---
public:
    const ConditionPtr& condition() const;
    void setCondition(const ConditionPtr& newCondition);
    bool isActive() const;
    bool isValid() const;
    bool triggerOn() const;

signals:
    void conditionChanged(const ConditionPtr& newCondition);
    void isActiveChanged(bool isActive);
    void isValidChanged(bool isValid);
// --- ---

private:
    static void registerTypes();

private:
    ConditionPtr m_condition;
};

} // namespace QmlFutures

Q_DECLARE_METATYPE(QmlFutures::ConditionPtr)
