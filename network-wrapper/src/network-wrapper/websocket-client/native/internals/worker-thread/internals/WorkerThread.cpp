
#include "WorkerThread.hpp"

#include <chrono> // std::chrono::milliseconds

WorkerThread::WorkerThread(LockingModel lockingModel) : _lockingModel(lockingModel) {

  _isRunning = false; // the WorkerThread's thread will set it to true

  if (_lockingModel == LockingModel::avoidLocking) {
    // launch WorkerThread thread
    _thread = std::thread(&WorkerThread::_threadedMethod, this);

    // here we wait for the thread to be running

    // avoid locking on this (main?) thread
    // -> we just to observe the value of the "_isRunning" attribute
    // ---> the new thread will change its value to "true"
    while (_isRunning == false) {
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
  }
  else {
    // the worker thread is being create -> we need a lock that can be awaited
    auto setupLock = _setupSynchronizer.makeScopedLock();

    // launch worker thread
    _thread = std::thread(&WorkerThread::_threadedMethod, this);

    // here we wait for the thread to be running

    // wait -> release the lock for other thread(s)
    _setupSynchronizer.waitUntilNotified(setupLock);
  }
}

WorkerThread::~WorkerThread() { shutdown(); }

//
//

void
WorkerThread::execute(const AbstractWorkerThread::WorkCallback& inWorkCallback) {
  // the worker thread is waiting -> lock, apply the logic, then notify
  auto lockNotifier = _taskSynchronizer.makeScopedLockNotifier();

  // this part is locked and will notify at the end of the scope

  _workCallback = inWorkCallback;
}

void
WorkerThread::shutdown() {
  // defensive programming
  // -> especially since this method is called by the destructor
  if (!_isRunning) {
    return;
  }

  {
    // the worker thread is waiting -> lock, apply the logic, then notify
    auto lockNotifier = _taskSynchronizer.makeScopedLockNotifier();
    // now locked

    // tells the worker thread to stop its internal loop
    _isRunning = false;
  }

  // std::thread::joinable() -> defensive programming
  if (_thread.joinable()) {
    _thread.join();
  }
}

//
//

bool
WorkerThread::isRunning() const {
  return _isRunning;
}

bool
WorkerThread::isAvailable() const {
  return !_taskSynchronizer.isNotified();
}

//
//

void
WorkerThread::_threadedMethod() {

  auto taskLock = _taskSynchronizer.makeScopedLock();

  if (_lockingModel == LockingModel::avoidLocking) {
    // the calling thread is waiting using a "crude sleeping loop"
    // -> all we need is to set the "_isRunning" attribute to true
    _isRunning = true;
  }
  else {
    // the calling thread is waiting -> lock, apply the logic, then notify
    auto setupLockNotifier = _setupSynchronizer.makeScopedLockNotifier();
    // now locked


    //
    _isRunning = true;
  }

  // this part is locked

  while (_isRunning) {
    // wait -> release the lock for other thread(s)
    _taskSynchronizer.waitUntilNotified(taskLock);

    // this part is locked

    if (!_isRunning) {
      break; // quit scenario
    }

    _workCallback();
  }
}
