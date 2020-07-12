//
// Created by imelker on 12.07.2020.
//

#ifndef SMARTMUTEX__SMART_MUTEX_H_
#define SMARTMUTEX__SMART_MUTEX_H_

#include <mutex>

/*
 * Owning object wrapper for thread safety access
 */
template <class T, class Mutex = std::mutex>
class SmartMutex
{
  public:
    /*
     * Proxy object for providing access in critical section
     */
    template<typename U, typename M>
    struct Access
    {
        Access(U& ref, M& mutex) : ref(ref), lg(mutex) {}
        Access(SmartMutex<U, M> &smart) : ref(smart.value), lg(smart.mutex) {}

        // TODO make read/write Access
        U* operator->() { return &ref; }

      private:
        U& ref;
        std::lock_guard<M> lg;
    };

    typedef Access<T, Mutex> ScopedAccess;

    template<typename ...Args>
    SmartMutex(Args ...args) : value(std::forward<Args...>(args...)) {
    }

    ~SmartMutex() = default;

    SmartMutex(const T& other) {
        std::lock_guard lock(mutex);
        value(other);
    }

    SmartMutex(const T&& other) {
        std::lock_guard lock(mutex);
        value(std::move(other));
    }

    SmartMutex& operator=(const SmartMutex& other) = delete;
    SmartMutex& operator=(SmartMutex&& other) noexcept = delete;

    bool operator==(SmartMutex &other) {
        std::scoped_lock lock(mutex, other.mutex);
        return value == other.value;
    }

    bool operator==(const T &other) {
        std::lock_guard lock(mutex);
        return value == other;
    }

    bool operator!=(SmartMutex &other) {
        std::scoped_lock lock(mutex, other.mutex);
        return value != other.value;
    }

    bool operator!=(const T &other) {
        std::lock_guard lock(mutex);
        return value != other;
    }

    operator T() {
        std::lock_guard lock(mutex);
        return value;
    }

    // thread safe access to inner functions
    Access<T, Mutex> operator ->() {
        return Access<T, Mutex>(value, mutex);
    }

    Access<const T, Mutex> operator->() const {
        return Access<const T, Mutex>(value, mutex);
    }

  private:
    Mutex mutex;
    T value;
};

#endif //SMARTMUTEX__SMART_MUTEX_H_
