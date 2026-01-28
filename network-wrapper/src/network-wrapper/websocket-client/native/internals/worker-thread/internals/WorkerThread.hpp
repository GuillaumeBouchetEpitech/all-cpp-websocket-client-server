
#pragma once

#include "../AbstractWorkerThread.hpp"

#include "ThreadSynchronizer.hpp"

#include <functional>

#include <thread>

class WorkerThread : public AbstractWorkerThread {
public:
  enum class LockingModel {
    lockThreads,
    avoidLocking
  };

public:
  WorkerThread(LockingModel lockingModel);
  ~WorkerThread();

  // disable copy
  WorkerThread(const WorkerThread& other) = delete;
  WorkerThread& operator=(const WorkerThread& other) = delete;

  // disable move
  WorkerThread(WorkerThread&& other) = delete;
  WorkerThread& operator=(WorkerThread&& other) = delete;

public:
  void execute(const AbstractWorkerThread::WorkCallback& inWorkCallback) override;
  void shutdown() override;

public:
  [[nodiscard]] bool isRunning() const override;
  [[nodiscard]] bool isAvailable() const override;

private:
  void _threadedMethod();

private:
  std::thread _thread;
  ThreadSynchronizer _setupSynchronizer;
  ThreadSynchronizer _taskSynchronizer;

  bool _isRunning = false;
  LockingModel _lockingModel;

  AbstractWorkerThread::WorkCallback _workCallback;
};
