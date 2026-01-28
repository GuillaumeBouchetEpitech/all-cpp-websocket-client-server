
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

void
ThreadSynchronizer::waitUntilNotified(std::unique_lock<std::mutex>& lock) {
  _isNotified = false;

  while (_isNotified == false) { // loop to avoid "spurious wakeups"
    _condVar.wait(lock);
  }
}

ThreadSynchronizer::WaitResult
ThreadSynchronizer::waitUntilNotified(std::unique_lock<std::mutex>& lock, float seconds) {

  // defensive programming -> invalid input timeout value
  if (seconds <= 0.0f) {
    // here assuming the user do not wishes to wait
    return WaitResult::timeoutWasIgnored;
  }

  _isNotified = false;

  // we need to wait for a timeout
  // -> compute the "future point in time" that must be reached for a timeout
  const int64_t millisecondsToWait = int64_t(seconds * 1000.0f);
  auto timeoutTimePoint = std::chrono::system_clock::now();
  timeoutTimePoint += std::chrono::milliseconds(millisecondsToWait);

  while (_isNotified == false) { // loop to avoid "spurious wakeups"
    if (_condVar.wait_until(lock, timeoutTimePoint) == std::cv_status::timeout) {
      return WaitResult::hasTimedOut;
    }
  }

  return WaitResult::didNotTimeout;
}

void
ThreadSynchronizer::notify() {
  _isNotified = true;
  _condVar.notify_one();
}

std::unique_lock<std::mutex>
ThreadSynchronizer::makeScopedLock() {
  return std::unique_lock<std::mutex>(_mutex);
}

ThreadSynchronizer::ScopedLockedNotifier
ThreadSynchronizer::makeScopedLockNotifier() {
  return ScopedLockedNotifier(*this);
}

bool
ThreadSynchronizer::isNotified() const {
  return _isNotified;
}
