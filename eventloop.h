#ifndef EVENT_LOOP_H_
#define EVENT_LOOP_H_

#include <stdint.h>
#include <sys/epoll.h>

namespace eventloop {

class Event {
 public:
  static const uint32_t  NONE = 0;
  static const uint32_t  TIMEOUT = 1 << 0;

 public:
  Event(uint32_t type = 0) { type_ = type; }

  virtual ~Event() {};

  virtual void Process(uint32_t type) = 0;

 public:
  void SetType(uint32_t type) { type_ = type; }
  uint32_t GetType() { return type_; }

 private:
  uint32_t type_;
};

class FileEvent: public Event {
 public:
  static const uint32_t  READ = 1 << 1;
  static const uint32_t  WRITE = 1 << 2;
  static const uint32_t  ERROR = 1 << 3;

 public:
  explicit FileEvent(uint32_t type = Event::NONE) : Event(type) {}

  virtual ~FileEvent() {};

 public:
  void SetFile(int fd) { fd_ = fd; }
  int GetFile() { return fd_; }

 public:
  virtual void Process(uint32_t type) = 0;

 private:
  int fd_;
};

class SignalEvent: public Event {
 public:
  static const uint32_t INT = 1 << 1;
  static const uint32_t PIPE = 1 << 2;
  static const uint32_t TERM = 1 << 3;

 public:
  explicit SignalEvent(uint32_t type = Event::NONE) : Event(type) {}

  virtual ~SignalEvent() {};
};

class TimerEvent: public Event {
 public:
  static const uint32_t TIMER = 1 << 1;

 public:
  explicit TimerEvent(uint32_t type = Event::NONE) : Event(type) {}

  virtual ~TimerEvent() {};
};

class TimerManager {
 public:
  int AddEvent(TimerEvent *e);
  int DeleteEvent(TimerEvent *e);
  int UpdateEvent(TimerEvent *e);
};

class EventLoop {
 public:
  explicit EventLoop() {
    epfd_ = epoll_create(256);
  }

 public:
  int AddEvent(FileEvent *e);
  int DeleteEvent(FileEvent *e);
  int UpdateEvent(FileEvent *e);

  int AddEvent(TimerEvent *e);
  int DeleteEvent(TimerEvent *e);
  int UpdateEvent(TimerEvent *e);

  int ProcessEvents(int timeout);

  void Loop();

 private:
  int ProcessFileEvents(int timeout);

 private:
  int epfd_;
  TimerManager timermanager_;
};

int SetNonblocking(int fd);

int BindTo(const char *host, short port);

int ConnectTo(const char *host, short port);

}

#endif // EVENT_LOOP_H_
