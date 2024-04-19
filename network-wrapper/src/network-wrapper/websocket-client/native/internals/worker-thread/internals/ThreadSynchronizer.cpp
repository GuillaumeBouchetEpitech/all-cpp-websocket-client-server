
#include "ThreadSynchronizer.hpp"

#include <chrono>
#include <cstdint>

//
//
// ScopedLockedNotifier

ThreadSynchronizer::ScopedLockedNotifier::ScopedLockedNotifier(ThreadSynchronizer& synchronizer)
  : _synchronizer(synchronizer) {
  _synchronizer._mutex.lock(); // scoped lock part
}

ThreadSynchronizer::ScopedLockedNotifier::~ScopedLockedNotifier() {
  // added value compared to a simple scoped lock
  // -> we notify before unlocking the mutex
  _synchronizer.notify();

  _synchronizer._mutex.unlock(); // scoped lock part
}

// ScopedLockedNotifier
//
//

bool ThreadSynchronizer::waitUntilNotified(std::unique_lock<std::mutex>& lock, float seconds /*= 0.0f*/) {
  _isNotified = false;

  // no need to wait for a timeout
  if (seconds <= 0.0f) {
    while (_isNotified == false) // loop to avoid spurious wakeups
      _condVar.wait(lock);

    return true;
  }

  // we need to wait for a timeout
  const int64_t millisecondsToWait = int64_t(seconds * 1000.0f);
  auto timeoutPoint = std::chrono::system_clock::now();
  timeoutPoint += std::chrono::milliseconds(millisecondsToWait);

  while (_isNotified == false) // loop to avoid spurious wakeups
    if (_condVar.wait_until(lock, timeoutPoint) == std::cv_status::timeout)
      return false; // we did time out

  return true; // we did not time out
}

void ThreadSynchronizer::notify() {
  _isNotified = true;
  _condVar.notify_one();
}

std::unique_lock<std::mutex> ThreadSynchronizer::makeScopedLock() { return std::unique_lock<std::mutex>(_mutex); }

ThreadSynchronizer::ScopedLockedNotifier ThreadSynchronizer::makeScopedLockNotifier() {
  return ScopedLockedNotifier(*this);
}

bool ThreadSynchronizer::isNotified() const { return _isNotified; }

