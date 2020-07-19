//
// Created by imelker on 12.07.2020.
//

#include <iostream>
#include <string>
#include <algorithm>

#include "smart_mutex.hpp"

// BasicLockable class
class XRayMutex : public std::mutex {
  public:
    XRayMutex() : std::mutex() {}

    void lock() {
        std::mutex::lock();
        std::cout << "\t[Mutex/" << this << "] -  lock" << std::endl;
    }

    void unlock() {
        std::cout << "\t[Mutex/" << this << "] -  unlock" << std::endl;
        std::mutex::unlock();
    }
};

int main() {
    using SMString = SmartMutex<std::string, XRayMutex>;
    SMString smString("12");

    // append() in critical section check
    smString->append("11");
    std::cout << std::endl;

    // c_str() in critical section. Temporary RAI object created here
    std::cout << "c_str() \n" << smString->c_str() << " text under mutex because of temporary RAI object" << "\n";
    std::cout << std::endl;


    // scope lock for multiple operations check
    {
        SMString::WriteAccess sa(smString);
        sa->append("12");
        std::cout << "c_str() " << sa->c_str() << "\n";
        sa->append("13");
        std::cout << "c_str() " << sa->c_str() << "\n";
        sa->append("42");
        std::cout << "c_str() " << sa->c_str() << "\n";
    }
    std::cout << std::endl;

    // another way to scope lock for multiple operations
    if (SMString::WriteAccess sa(smString); !sa->empty()) {
        sa->append("12");
        std::cout << "c_str() " << sa->c_str() << "\n";
        sa->append("13");
        std::cout << "c_str() " << sa->c_str() << "\n";
        sa->append("42");
        std::cout << "c_str() " << sa->c_str() << "\n";
    }
    std::cout << std::endl;

    // operator T check
    std::string str = smString;
    std::cout << str << "\n" << std::endl;

    // copy constructor check
    SMString smStringCopy(smString); // "1211121342"
    std::cout << std::endl;

    // equal check
    if (smStringCopy == smString)
        std::cout << ">> Data is equal" << "\n";
    std::cout << std::endl;

    // change copied value
    smStringCopy->append("11");
    std::cout << std::endl;

    // unequal check
    if(smStringCopy != smString)
        std::cout << ">> Data is not equal" << "\n";
    std::cout << std::endl;

    // asign check
    smString = smStringCopy;
    std::cout << std::endl;

    // as read only (const object)
    if (SMString::ReadAccess ra(smString); !ra->empty()) {
        std::cout << "c_str() " << ra->c_str() << "\n";
        std::cout << "c_str() " << ra->c_str() << "\n";
        std::cout << "c_str() " << ra->c_str() << "\n";
        //ra->append("12"); // error
    }
    std::cout << std::endl;

    // std::swap check
    swap(smString, smStringCopy);
    std::cout << std::endl;
}


