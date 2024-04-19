
#include "WorkerThread.hpp"

#include <chrono> // std::chrono::milliseconds

WorkerThread::WorkerThread(bool inAvoidBlocking)
  : _avoidBlocking(inAvoidBlocking)
{

  if (_avoidBlocking == false) {
    auto setupLock = _setupSynchronizer.makeScopedLock();

    _isRunning = false; // the WorkerThread's thread will set it to true

    // launch WorkerThread thread

    _thread = std::thread(&WorkerThread::_threadedMethod, this);

    // here we wait for the thread to be running

    // wait -> release the lock for other thread(s)
    _setupSynchronizer.waitUntilNotified(setupLock);
  } else {
    _isRunning = false; // the WorkerThread's thread will set it to true

    // launch WorkerThread thread

    _thread = std::thread(&WorkerThread::_threadedMethod, this);

    // here we wait for the thread to be running

    // avoid blocking on the main thread
    while (_isRunning == false) {
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
  }
}

WorkerThread::~WorkerThread() { quit(); }

//
//

void WorkerThread::execute(const AbstractWorkerThread::WorkCallback& inWorkCallback) {
  auto lockNotifier = _taskSynchronizer.makeScopedLockNotifier();

  // this part is locked and will notify at the end of the scope

  _workCallback = inWorkCallback;
}

void WorkerThread::quit() {
  if (!_isRunning)
    return;

  {
    auto lockNotifier = _taskSynchronizer.makeScopedLockNotifier();

    // this part is locked and will notify at the end of the scope

    _isRunning = false;
  }

  if (_thread.joinable()) {
    _thread.join();
  }
}

//
//

bool WorkerThread::isRunning() const { return _isRunning; }

bool WorkerThread::isAvailable() const { return !_taskSynchronizer.isNotified(); }

//
//

void WorkerThread::_threadedMethod() {

  auto taskLock = _taskSynchronizer.makeScopedLock();

  if (_avoidBlocking == false) {
    auto setupLockNotifier = _setupSynchronizer.makeScopedLockNotifier();

    // this part is locked and will notify at the end of the scope

    _isRunning = true;
  } else {
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

    // _producer._notifyWorkDone(this);
  }
}
