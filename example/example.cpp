//
// Created by imelker on 12.07.2020.
//

#include <iostream>
#include <string>
#include <algorithm>

#include "smart_mutex.hpp"

// BasicLockable class
class XRayMutex : public std::mutex
{
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
    using sm_string = im::smart_mutex<std::string, XRayMutex>;
    sm_string smString("12"); // forward ctor
    sm_string smString2(std::string("12")); // move ctor
    auto str_test = std::string("12");
    sm_string smString3(str_test); // ref ctor

    // append() in critical section check
    smString->append("11");
    std::cout << std::endl;

    // c_str() in critical section. Temporary RAI object created here
    std::cout << "c_str() \n" << smString->c_str() << " text under mutex because of temporary RAI object" << "\n";
    std::cout << std::endl;


    // scope lock for multiple operations check
    {
        sm_string::write_access sa(smString);
        sa->append("12");
        std::cout << "c_str() " << sa->c_str() << "\n";
        sa->append("13");
        std::cout << "c_str() " << sa->c_str() << "\n";
        sa->append("42");
        std::cout << "c_str() " << sa->c_str() << "\n";
    }
    std::cout << std::endl;

    // another way to scope lock for multiple operations
    if (sm_string::write_access sa(smString); !sa->empty()) {
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
    sm_string smStringCopy(smString); // "1211121342"
    std::cout << std::endl;

    // equal check
    if (smStringCopy == smString)
        std::cout << ">> Data is equal" << "\n";
    std::cout << std::endl;

    // change copied value
    smStringCopy->append("11");
    std::cout << std::endl;

    // unequal check
    if (smStringCopy != smString)
        std::cout << ">> Data is not equal" << "\n";
    std::cout << std::endl;

    // unequal check
    if (smString != smStringCopy)
        std::cout << ">> Data is not equal" << "\n";
    std::cout << std::endl;

    // asign check
    smString = smStringCopy;
    std::cout << std::endl;

    // as read only (const object)
    if (sm_string::read_access ra(smString); !ra->empty()) {
        std::cout << "c_str() " << ra->c_str() << "\n";
        std::cout << "c_str() " << ra->c_str() << "\n";
        std::cout << "c_str() " << ra->c_str() << "\n";
        //ra->append("12"); // error
    }
    std::cout << std::endl;

    // std::swap check
    using std::swap;
    swap(smString, smStringCopy);
    std::cout << smString->c_str() << smStringCopy->c_str() << "\n";
    std::cout << std::endl;

    // std::swap with underlying type check
    std::string getFromMutexRight;
    using std::swap;
    swap(smString, getFromMutexRight);
    std::cout << getFromMutexRight << "\n";
    std::cout << std::endl;

    // std::swap with underlying type check other order
    std::string getFromMutexLeft;
    using std::swap;
    swap(getFromMutexLeft, smStringCopy);
    std::cout << getFromMutexLeft << "\n";
    std::cout << std::endl;
}


