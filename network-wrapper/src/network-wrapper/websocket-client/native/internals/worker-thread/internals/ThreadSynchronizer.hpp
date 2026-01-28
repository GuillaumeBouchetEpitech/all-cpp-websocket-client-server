
#pragma once

#include <condition_variable>
#include <mutex>

// this class handle all locking and conditional variable interactions
class ThreadSynchronizer {
public:
  enum class WaitResult {
    timeoutWasIgnored,
    didNotTimeout,
    hasTimedOut,
  };

public:
  // this class act like a scoped lock but notify before unlocking
  class ScopedLockedNotifier {
    // friendship since we need access to the _mutex
    friend ThreadSynchronizer;

  private:
    ThreadSynchronizer& _synchronizer;

  private:
    ScopedLockedNotifier(ThreadSynchronizer& synchronizer);

  public:
    ~ScopedLockedNotifier();
  };

public:
  ThreadSynchronizer() = default;

  // disable copy
  ThreadSynchronizer(const ThreadSynchronizer& other) = delete;
  ThreadSynchronizer& operator=(const ThreadSynchronizer& other) = delete;

  // disable move
  ThreadSynchronizer(ThreadSynchronizer&& other) = delete;
  ThreadSynchronizer& operator=(ThreadSynchronizer&& other) = delete;

public:
  void waitUntilNotified(std::unique_lock<std::mutex>& lock);
  [[nodiscard]] WaitResult waitUntilNotified(std::unique_lock<std::mutex>& lock, float seconds);

public:
  void notify();

public:
  [[nodiscard]] std::unique_lock<std::mutex> makeScopedLock();
  [[nodiscard]] ScopedLockedNotifier makeScopedLockNotifier();
  [[nodiscard]] bool isNotified() const;

private:
  std::mutex _mutex;
  std::condition_variable _condVar;
  bool _isNotified = false;
};
