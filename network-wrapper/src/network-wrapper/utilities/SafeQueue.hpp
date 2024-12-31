
#pragma once

#include "./TestAndSetAtomicLock.hpp"

#include <list>
#include <memory>
#include <mutex>



template<typename T>
class SafeQueue {

public:
  SafeQueue() = default;
  ~SafeQueue() = default;

public:
  template< class... Args >
  std::size_t emplaceBack(Args&&... args) {
    // std::lock_guard lock(_writeMutex);
    ScopedAtomicLock lock(this->_writeAtomicLock);

    this->_buffersToSend.emplace_back(std::forward<Args>(args)...);
    return this->_buffersToSend.size();
  }
  void popFront() {
    // std::lock_guard lock(_writeMutex);
    ScopedAtomicLock lock(this->_writeAtomicLock);

    this->_buffersToSend.pop_front();
  }

  const T& peekFront() const {
    return this->_buffersToSend.front();
  }
  T& peekFront() {
    return this->_buffersToSend.front();
  }

  std::size_t size() const {
    return this->_buffersToSend.size();
  }
  bool empty() const {
    return this->_buffersToSend.empty();
  }

private:
  // std::mutex _writeMutex;
  TestAndSetAtomicLock _writeAtomicLock;
  std::list<T> _buffersToSend;

};
