#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "eventloop.h"

namespace eventloop {

class SignalManager {
};

class TimerManager {
 public:
  int AddEvent(BaseTimerEvent *e);
  int DeleteEvent(BaseTimerEvent *e);
  int UpdateEvent(BaseTimerEvent *e);
};

int SetNonblocking(int fd) {
  int opts;
  if ((opts = fcntl(fd, F_GETFL)) != -1) {
    opts = opts | O_NONBLOCK;
    if(fcntl(fd, F_SETFL, opts) != -1) {
      return 0;
    }
  }
  return -1;
}

int ConnectTo(const char *host, short port) {
  int fd;
  struct sockaddr_in addr;

  fd = socket(AF_INET, SOCK_STREAM, 0);
  addr.sin_family = PF_INET;
  addr.sin_port = htons(port);
  if (host[0] == '\0' || strcmp(host, "localhost") == 0) {
    inet_aton("127.0.0.1", &addr.sin_addr);
  } else if (!strcmp(host, "any")) {
    addr.sin_addr.s_addr = INADDR_ANY;
  } else {
    inet_aton(host, &addr.sin_addr);
  }

  SetNonblocking(fd);

  if (connect(fd, (struct sockaddr*)&addr, sizeof(struct sockaddr_in)) == -1) {
    if (errno != EINPROGRESS) {
      return -1;
    }
  }

  return fd;
}

int BindTo(const char *host, short port) {
  int fd, on = 1;
  struct sockaddr_in addr;

  if ((fd = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
    return -1;
  }

  setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int));

  memset(&addr, 0, sizeof(struct sockaddr_in));
  addr.sin_family = PF_INET;
  addr.sin_port = htons(port);
  if (host[0] == '\0' || strcmp(host, "localhost") == 0) {
    inet_aton("127.0.0.1", &addr.sin_addr);
  } else if (strcmp(host, "any") == 0) {
    addr.sin_addr.s_addr = INADDR_ANY;
  } else {
    if (inet_aton(host, &addr.sin_addr) == 0) return -1;
  }

  if (bind(fd, (struct sockaddr*)&addr, sizeof(struct sockaddr_in)) == -1
      || listen(fd, 10) == -1) {
    return -1;
  }

  return fd;
}

int EventLoop::ProcessFileEvents(int timeout) {
  int i, n;
  epoll_event evs[256];
  n = epoll_wait(epfd_, evs, 256, timeout);
  for(i = 0; i < n; i++) {
    BaseEvent *e = (BaseEvent *)evs[i].data.ptr;
    uint32_t events = 0;
    if (evs[i].events & EPOLLIN) events |= BaseFileEvent::READ;
    if (evs[i].events & EPOLLOUT) events |= BaseFileEvent::WRITE;
    if (evs[i].events & (EPOLLHUP | EPOLLERR | EPOLLRDHUP)) events |= BaseFileEvent::ERROR;

    e->Process(events);
  }
  return n;
}

int EventLoop::ProcessEvents(int timeout) {
  return ProcessFileEvents(timeout);
}

void EventLoop::Loop() {
  while (true) {
    ProcessEvents(1000);
  }
}

int EventLoop::AddEvent(BaseFileEvent *e) {
  struct epoll_event ev;

  uint32_t type = e->GetType();
  ev.events = 0;
  if (type | BaseFileEvent::READ) ev.events |= EPOLLIN;
  if (type | BaseFileEvent::WRITE) ev.events |= EPOLLOUT;
  if (type | BaseFileEvent::ERROR) ev.events |= EPOLLHUP | EPOLLERR | EPOLLRDHUP;
  ev.data.fd = e->GetFile();
  ev.data.ptr = e;

  SetNonblocking(e->GetFile());

  return epoll_ctl(epfd_, EPOLL_CTL_ADD, e->GetFile(), &ev);
}

int EventLoop::UpdateEvent(BaseFileEvent *e) {
  struct epoll_event ev;
  uint32_t type = e->GetType();

  ev.events = 0;
  if (type | BaseFileEvent::READ) ev.events |= EPOLLIN;
  if (type | BaseFileEvent::WRITE) ev.events |= EPOLLOUT;
  if (type | BaseFileEvent::ERROR) ev.events |= EPOLLHUP | EPOLLERR | EPOLLRDHUP;
  ev.data.fd = e->GetFile();
  ev.data.ptr = e;

  return epoll_ctl(epfd_, EPOLL_CTL_MOD, e->GetFile(), &ev);
}

int EventLoop::DeleteEvent(BaseFileEvent *e) {
  return 0;
}

int TimerManager::AddEvent(BaseTimerEvent *e) {
  return 0;
}

int TimerManager::DeleteEvent(BaseTimerEvent *e) {
  return 0;
}

int TimerManager::UpdateEvent(BaseTimerEvent *e) {
  return 0;
}

}
