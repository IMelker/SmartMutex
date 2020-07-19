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
    template<typename U>
    struct Access
    {
        Access(const SmartMutex &smart) : ref(smart) {
            ref.mutex.lock();
        }
        ~Access() {
            ref.mutex.unlock();
        }
        U* operator->() const { return const_cast<U*>(&ref.value); }
      private:
        const SmartMutex& ref;
    };

    typedef Access<T> WriteAccess;
    typedef Access<const T> ReadAccess;

    friend void swap(SmartMutex &lhs, SmartMutex &rhs) {
        std::scoped_lock lock(lhs.mutex, rhs.mutex);
        std::swap(lhs.value, rhs.value);
    }
  public:
    // rule of five
    SmartMutex(SmartMutex& other) {
        std::scoped_lock lock(mutex, other.mutex);
        value = other.value;
    }

    SmartMutex(SmartMutex&& other) noexcept {
        std::scoped_lock lock(mutex, other.mutex);
        value = std::move(other.value);
    }

    SmartMutex& operator=(const SmartMutex& other) {
        std::scoped_lock lock(mutex, other.mutex);
        value = other.value;
        return *this;
    };

    SmartMutex& operator=(SmartMutex&& other) noexcept {
        std::scoped_lock lock(mutex, other.mutex);
        value = std::move(other.value);
        return *this;
    };

    ~SmartMutex() = default;

    // underlying type based constructors
    template<typename ...Args>
    SmartMutex(Args ...args) : value(std::forward<Args>(args)...) {
    }

    SmartMutex(const T& other) {
        std::lock_guard lock(mutex);
        value(other);
    }

    SmartMutex(const T&& other) {
        std::lock_guard lock(mutex);
        value(std::move(other));
    }

    // operators
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
    WriteAccess operator ->() {
        return *this;
    }

    // thread safe access to read only inner functions
    ReadAccess operator->() const {
        return *this;
    }

  private:
    mutable Mutex mutex;
    T value;
};

#endif //SMARTMUTEX__SMART_MUTEX_H_
