#ifndef EVENT_LOOP_H_
#define EVENT_LOOP_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <sys/socket.h>

namespace eventloop {

class Event {
 public:
  virtual void Process(uint32_t events) = 0;
};

class FileEvent: public Event {
 public:
  enum Type{
    READ = EPOLLIN,
    WRITE = EPOLLOUT,
    ERROR = EPOLLHUP | EPOLLERR | EPOLLRDHUP,
  };

 public:
  explicit FileEvent() {
  }

  explicit FileEvent(Type events) {
    events_ = events;
  }

  int GetFile() {
    return fd_;
  }

  int GetEvents() {
    return events_;
  }

 public:
  virtual void Process(uint32_t events) = 0;

 private:
  int fd_;
  Type events_;
};

class TimerEvent: public Event {
};

class TimerManager {
};

class EvengLoop {
 public:
  EvengLoop() {
    epfd_ = epoll_create(256);
  }

 public:
  int AddFileEvent(FileEvent *e);
  int DeleteFileEvent(FileEvent *e);
  int ModifyFileEvent(FileEvent *e, uint32_t events);

  int ProcessEvents(int timeout);
  void Loop();

 private:
  int epfd_;
};

}

#endif // EVENT_LOOP_H_

