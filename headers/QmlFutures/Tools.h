#pragma once
#include <utility>
#include <memory>
#include <cassert>

//
// Internals for QmlFutures.
//

namespace QmlFutures {
namespace Internal {

#define QF_ONCE_NAMED(flag, code) \
    static bool _flag##flag = true; \
    if (_flag##flag) { \
        _flag##flag = false; \
        code; \
    }

#define QF_ONCE(code) QF_ONCE_NAMED(flag, code)

#define QF_DECLARE_PIMPL  \
    struct impl_t; \
    std::unique_ptr<impl_t> _impl; \
    impl_t& impl() { assert(_impl); return *_impl; } \
    const impl_t& impl() const { assert(_impl); return *_impl; } \
    template<typename... Args> \
    void createImpl(Args&&... args) { assert(!_impl); _impl = std::make_unique<impl_t>(std::forward<Args>(args)...); }

template<class T>
class Singleton
{
public:
    Singleton()
    {
        assert(!m_instance);
        m_instance = static_cast<T*>(this);
    }

    ~Singleton()
    {
        assert(m_instance);
        m_instance = nullptr;
    }

    static T* instance()
    {
        assert(m_instance);
        return m_instance;
    }

    static bool exists()
    {
        return m_instance;
    }

private:
    static T* m_instance;
};

template<class T>
T* Singleton<T>::m_instance = nullptr;

} // namespace Internal
} // namespace QmlFutures
