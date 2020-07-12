//
// Created by imelker on 12.07.2020.
//

#include <iostream>
#include <string>

#include "smart_mutex.hpp"

class XRayMutex : public std::mutex {
  public:
    XRayMutex() : std::mutex() {}

    void lock() {
        std::cout << "\t[Mutex/" << this << "] -  lock" << std::endl;
        std::mutex::lock();
    }

    void unlock() {
        std::mutex::unlock();
        std::cout << "\t[Mutex/" << this << "] -  unlock" << std::endl;
    }
};

int main() {
    SmartMutex<std::string, XRayMutex> test("12");

    // append() in critical section
    test->append("11");
    std::cout << std::endl;

    // c_str() in critical section
    std::cout << ">> " << test->c_str() << " EOF" << std::endl;
    std::cout << std::endl;


    // scope lock for multiple operations
    {
        SmartMutex<std::string, XRayMutex>::ScopedAccess sa(test);
        sa->append("12");
        std::cout << ">> " << sa->c_str() << std::endl;
        sa->append("13");
        std::cout << ">> " << sa->c_str() << std::endl;
        sa->append("42");
        std::cout << ">> " << sa->c_str() << std::endl;
    }
    std::cout << std::endl;

    // copy constructor
    SmartMutex<std::string, XRayMutex> test_copy("1211121342");

    if(test_copy == test) {
        std::cout << ">> Data is equal" << std::endl;
    }
    std::cout << std::endl;
    test_copy->append("11");
    std::cout << std::endl;

    if(test_copy != test) {
        std::cout << ">> Data is not equal" << std::endl;
    }
    std::cout << std::endl;

    std::string testStr = test_copy;
    std::cout << testStr << std::endl;
    std::cout << std::endl;
}


