#include "event.h"

EventEngine *EventEngine::instance_ = NULL;

EventEngine *EventEngine::Instance() {
  if (instance_ == NULL) {
    instance_ = new EventEngine();
  }
  return instance_;
}

int EventEngine::ProcessEvent(int timeout) {
  int i, n;
  n = epoll_wait(epfd, evs, nev, timeout);
  for(i = 0; i < n; i++) {
    Event *e = evs[i].data.ptr;
    e->Process(evs[i].events);
  }
  return n;
}

void EventEngine::Run() {
  while (true) {
    ProcessEvent(300);
  }
}

static int sockbufsize = 32*1024;

int EventEngine::Add(Event *e) {
  struct epoll_event ev;

  ev.events = e->events;
  ev.data.fd = e->fd;
  ev.data.ptr = e;

  setnonblock(e->fd);
  setsockopt(e->fd, SOL_SOCKET, SO_RCVBUF, &sockbufsize, sizeof(sockbufsize));
  setsockopt(e->fd, SOL_SOCKET, SO_SNDBUF, &sockbufsize, sizeof(sockbufsize));

  epoll_ctl(epfd_, EPOLL_CTL_ADD, e->fd, &ev);

  nev++;

  evs = realloc(evs, nev * sizeof(struct epoll_event));

  return 0;
}

int EventEngine::Modify(struct event *e, uint32_t events, event_handler handler, void *ctx) {
  struct epoll_event ev;

  e->handler = handler;
  e->ctx = ctx;

  ev.events = events;
  ev.data.fd = e->fd;
  ev.data.ptr = e;

  return epoll_ctl(epfd, EPOLL_CTL_MOD, e->fd, &ev);
}

