
#include "AbstractWorkerThread.hpp"

#include "./internals/WorkerThread.hpp"

std::unique_ptr<AbstractWorkerThread>
AbstractWorkerThread::create() {
  return std::make_unique<WorkerThread>(WorkerThread::LockingModel::lockThreads);
}
