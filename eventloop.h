#ifndef EVENT_LOOP_H_
#define EVENT_LOOP_H_

#include <stdint.h>
#include <time.h>
#include <sys/epoll.h>

namespace eventloop {

class BaseEvent {
 public:
  static const uint32_t  NONE = 0;
  static const uint32_t  TIMEOUT = 1 << 0;

 public:
  BaseEvent(uint32_t type = 0) { type_ = type; }

  virtual ~BaseEvent() {};

  virtual void Process(uint32_t type) = 0;

 public:
  void SetType(uint32_t type) { type_ = type; }
  uint32_t GetType() { return type_; }

 private:
  uint32_t type_;
};

class BaseFileEvent: public BaseEvent {
 public:
  static const uint32_t  READ = 1 << 1;
  static const uint32_t  WRITE = 1 << 2;
  static const uint32_t  ERROR = 1 << 3;

 public:
  explicit BaseFileEvent(uint32_t type = BaseEvent::NONE) : BaseEvent(type) {}

  virtual ~BaseFileEvent() {};

 public:
  void SetFile(int fd) { fd_ = fd; }
  int GetFile() { return fd_; }

 public:
  virtual void Process(uint32_t type) = 0;

 private:
  int fd_;
};

class BaseSignalEvent: public BaseEvent {
 public:
  static const uint32_t INT = 1 << 1;
  static const uint32_t PIPE = 1 << 2;
  static const uint32_t TERM = 1 << 3;

 public:
  explicit BaseSignalEvent(uint32_t type = BaseEvent::NONE) : BaseEvent(type) {}

  virtual ~BaseSignalEvent() {};
};

class BaseTimerEvent: public BaseEvent {
 public:
  static const uint32_t TIMER = 1 << 1;

 public:
  explicit BaseTimerEvent(uint32_t type = BaseEvent::NONE) : BaseEvent(type) {}
  virtual ~BaseTimerEvent() {};

 public:
  void SetTime(timeval tv) { time_ = tv; }
  timeval GetTime() { return time_; }

 private:
  timeval time_;
};

class EventLoop {
 public:
  explicit EventLoop() {
    epfd_ = epoll_create(256);
  }

 public:
  int AddEvent(BaseFileEvent *e);
  int DeleteEvent(BaseFileEvent *e);
  int UpdateEvent(BaseFileEvent *e);

  int AddEvent(BaseTimerEvent *e);
  int DeleteEvent(BaseTimerEvent *e);
  int UpdateEvent(BaseTimerEvent *e);

  int ProcessEvents(int timeout);

  void Loop();

 private:
  int ProcessFileEvents(int timeout);

 private:
  int epfd_;
  void *timermanager_;
};

int SetNonblocking(int fd);

int BindTo(const char *host, short port);

int ConnectTo(const char *host, short port);

}

#endif // EVENT_LOOP_H_
