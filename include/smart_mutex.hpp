// Implementation of the smart mutex wrapper.
//
// Created by imelker on 12.07.2020.
//

#ifndef SMARTMUTEX__SMART_MUTEX_H_
#define SMARTMUTEX__SMART_MUTEX_H_

#include <mutex>

namespace im
{

/**
 * @class smart_mutex
 * @brief Smart mutex - basic lockable object wrapper for concurrency.
 * @tparam T The type of the element stored in the smart_mutex.
 * @tparam Mutex The Mutex type used for wrapped data protection from being
 * simultaneously accessed by multiple threads. It has to meet the
 * BasicLockable requirements imposed by STL.
 */
template<class T, class Mutex = std::mutex>
class smart_mutex
{
  public:
    // Container specific types
    /**
     * @class access
     * @brief Proxy object for providing access in critical section.
     * @tparam U The type of the element stored in the <code>smart_mutex</code>.
     */
    template<typename U>
    struct access
    {
        explicit access(const smart_mutex &smart) : ref(smart), lg(ref.mutex) {
        }

        /**
         * @brief Get the underlying type pointer.
         * @return The underlying type pointer.
         */
        U *operator->() const noexcept { return const_cast<U *>(&ref.value); }

      private:
        // The internal reference to overlying smart_mutex object.
        const smart_mutex &ref;
        // The internal mutex wrapper with RAII mechanism for owning a mutex.
        std::lock_guard<Mutex> lg;
    };

    //! A type representing the write access to underlying type in critical section.
    typedef access<T> write_access;
    //! A type representing the read access to underlying type in critical section.
    typedef access<const T> read_access;

  public:
    // Construction/Destruction
    /**
     * @brief  Create smart_mutex with forwarded arguments for the specified underlying type.
     * @tparam Args Types of elements passed to the underlying value constructor.
     * @param args Elements passed to the underlying value constructor.
     */
    template<typename ...Args>
    explicit smart_mutex(Args ...args) : value(std::forward<Args>(args)...) {
    }

    /**
     * @brief Crate smart_mutex with copying value of underlying type into it.
     * @param other Element of underlying type
     */
    explicit smart_mutex(const T &other) : value(other) {
    }

    /**
     * @brief Create smart_mutex with moving value of underlying type into it.
     * @param other Element of underlying type
     */
    explicit smart_mutex(const T &&other) : value(std::move(other)) {
    }

    /**
     * The destructor.
     * @brief Destroys the smart_mutex.
     */
    ~smart_mutex() = default;

    // rule of five
    /**
     * The copy constructor.
     * @brief Creates a copy of the specified circular_buffer.
     * @param other The smart_mutex to be copied.
     */
    smart_mutex(smart_mutex &other) {
        std::scoped_lock lock(mutex, other.mutex);
        value = other.value;
    }

    /**
     * The move constructor.
     * @brief Move constructs a smart_mutex from other, leaving other empty.
     * @param other smart_mutex to 'steal' value from.
     */
    smart_mutex(smart_mutex &&other) noexcept {
        std::scoped_lock lock(mutex, other.mutex);
        value = std::move(other.value);
    }

    // Assign methods
    /**
     * The assign operator.
     * @brief Makes this smart_mutex to become a copy of the specified smart_mutex.
     * @param other The smart_mutex to be copied.
     * @return This smart_mutex.
     */
    smart_mutex &operator=(const smart_mutex &other) {
        std::scoped_lock lock(mutex, other.mutex);
        value = other.value;
        return *this;
    };

    /**
     * @brief Move assigns content of other to *this, leaving other empty.
     * C++ compiler with rvalue references support.
     * @param other smart_mutex to 'steal' value from.
     * @return This smart_mutex.
     */
    smart_mutex &operator=(smart_mutex &&other) noexcept {
        std::scoped_lock lock(mutex, other.mutex);
        value = std::move(other.value);
        return *this;
    };

    /**
     * @brief Compare two smart_mutexes to determine if they are equal.
     * @param other smart_mutex to compare with.
     * @return Comparison result.
     */
    bool operator==(const smart_mutex &other) {
        std::scoped_lock lock(mutex, other.mutex);
        return value == other.value;
    }

    /**
     * @brief Compare other value of underlying type with this smart_mutex
     * to determine if they are equal.
     * @param lhs smart_mutex to compare with.
     * @param rhs value of underlying type to compare with.
     * @return Equal comparison result.
     */
    friend bool operator==(const smart_mutex &lhs, const T &rhs) {
        std::lock_guard lock(lhs.mutex);
        return lhs.value == rhs;
    }

    /**
     * @brief Compare two smart_mutexes to determine if they are non-equal.
     * @param other smart_mutex to compare with.
     * @return Unequal comparison result.
     */
    bool operator!=(const smart_mutex &other) {
        std::scoped_lock lock(mutex, other.mutex);
        return value != other.value;
    }

    /**
     * @brief Compare other value of underlying type with this smart_mutex.
     * @param lhs smart_mutex to compare with.
     * @param rhs value of underlying type to compare with.
     * @return Unequal comparison result.
     */
    friend bool operator!=(const smart_mutex &lhs, const T &rhs) {
        std::lock_guard lock(lhs.mutex);
        return lhs.value != rhs;
    }

    // Element access
    /**
     * @brief Get copy of the underlying value in smart_mutex
     * @return Underlying value
     */
    operator T() const {
        std::lock_guard lock(mutex);
        return value;
    }

    /**
     * @brief Thread safe access to inner functions
     * @return write_access lockable wrapper for underlying value
     */
    write_access operator->() {
        return write_access(*this);
    }

    /**
     * @brief Thread safe access to read only inner functions
     * @return read_access lockable wrapper for underlying value
     */
    read_access operator->() const {
        return read_access(*this);
    }

  public:
    // STL like helpers
    /**
     * @brief Swap the contents of two smart_mutexes.
     * @param lhs The smart_mutex whose content will be swapped.
     * @param rhs The smart_mutex whose content will be swapped.
     */
    friend void swap(smart_mutex &lhs, smart_mutex &rhs) noexcept {
        std::scoped_lock lock(lhs.mutex, rhs.mutex);
        std::swap(lhs.value, rhs.value);
    }

    /**
     * @brief Swap the contents of smart_mutex and the value of underlying type.
     * @param lhs The smart_mutex whose content will be swapped.
     * @param rhs The value of underlying type.
     */
    friend void swap(smart_mutex &lhs, T &rhs) noexcept {
        std::lock_guard lock(lhs.mutex);
        std::swap(lhs.value, rhs);
    }

    /**
     * @brief Swap the contents of value of underlying type and the smart_mutex.
     * @param lhs The value of underlying type.
     * @param rhs The smart_mutex whose content will be swapped.
     */
    friend void swap(T &lhs, smart_mutex &rhs) noexcept {
        std::lock_guard lock(rhs.mutex);
        std::swap(lhs, rhs.value);
    }

  private:
    // Member variables
    // The internal Mutex object for providing threadsafety access.
    mutable Mutex mutex;
    // The internal value protected by mutex.
    T value;
};

} // end namespace im

#endif //SMARTMUTEX__SMART_MUTEX_H_
