
#include "TestAndSetAtomicLock.hpp"

// https://rigtorp.se/spinlock/

void
TestAndSetAtomicLock::lock() {
  while (lock_.exchange(true, std::memory_order_acquire))
    ;
}

void
TestAndSetAtomicLock::unlock() {
  lock_.store(false, std::memory_order_release);
}

//
//
//

ScopedAtomicLock::ScopedAtomicLock(TestAndSetAtomicLock& atomicLock) : _atomicLock(atomicLock) { _atomicLock.lock(); }

ScopedAtomicLock::~ScopedAtomicLock() { _atomicLock.unlock(); }
