
#pragma once

#include "../AbstractWorkerThread.hpp"

#include "ThreadSynchronizer.hpp"

#include <functional>

#include <thread>

class WorkerThread : public AbstractWorkerThread {
private:
  std::thread _thread;
  ThreadSynchronizer _setupSynchronizer;
  ThreadSynchronizer _taskSynchronizer;

  bool _isRunning = false;
  bool _avoidBlocking = false;

  AbstractWorkerThread::WorkCallback _workCallback;

public:
  explicit WorkerThread(bool inAvoidBlocking);
  ~WorkerThread();

  WorkerThread(const WorkerThread& other) = delete;
  WorkerThread& operator=(const WorkerThread& other) = delete;
  WorkerThread(WorkerThread&& other) = delete;
  WorkerThread& operator=(WorkerThread&& other) = delete;

public:
  void execute(const AbstractWorkerThread::WorkCallback& inWorkCallback) override;
  void quit() override;

public:
  [[nodiscard]] bool isRunning() const override;
  [[nodiscard]] bool isAvailable() const override;

private:
  void _threadedMethod();
};
