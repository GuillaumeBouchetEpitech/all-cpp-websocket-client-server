
#pragma once

#include <atomic>

class TestAndSetAtomicLock {
public:
  void lock();
  void unlock();

private:
  std::atomic<bool> lock_ = {false};
};

//
//
//

class ScopedAtomicLock {
public:
  ScopedAtomicLock(TestAndSetAtomicLock& atomicLock);
  ~ScopedAtomicLock();

private:
  TestAndSetAtomicLock& _atomicLock;
};
