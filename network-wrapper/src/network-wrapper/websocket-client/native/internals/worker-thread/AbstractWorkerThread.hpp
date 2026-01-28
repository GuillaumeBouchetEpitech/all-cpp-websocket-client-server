
#pragma once

#include <functional>

#include <memory>

class AbstractWorkerThread {
public:
  using WorkCallback = std::function<void()>;

public:
  AbstractWorkerThread() = default;
  virtual ~AbstractWorkerThread() = default;

  // disable copy
  AbstractWorkerThread(const AbstractWorkerThread& other) = delete;
  AbstractWorkerThread& operator=(const AbstractWorkerThread& other) = delete;

  // disable move
  AbstractWorkerThread(AbstractWorkerThread&& other) = delete;
  AbstractWorkerThread& operator=(AbstractWorkerThread&& other) = delete;

public:
  virtual void execute(const WorkCallback& inWorkCallback) = 0;
  virtual void shutdown() = 0;

public:
  [[nodiscard]] virtual bool isRunning() const = 0;
  [[nodiscard]] virtual bool isAvailable() const = 0;

public:
  static std::unique_ptr<AbstractWorkerThread> create();
};
