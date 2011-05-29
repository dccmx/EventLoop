#include "eventloop.h"
#include "util.h"

namespace eventloop {

int EvengLoop::ProcessEvents(int timeout) {
  int i, n;
  epoll_event evs[256];
  n = epoll_wait(epfd_, evs, 256, timeout);
  for(i = 0; i < n; i++) {
    Event *e = (Event *)evs[i].data.ptr;
    e->Process(evs[i].events);
  }
  return n;
}

void EvengLoop::Loop() {
  while (true) {
    ProcessEvents(300);
  }
}

int EvengLoop::AddFileEvent(FileEvent *e) {
  struct epoll_event ev;
  int sockbufsize = 32*1024;

  ev.events = e->GetEvents();
  ev.data.fd = e->GetFile();
  ev.data.ptr = e;

  setnonblock(e->GetFile());
  setsockopt(e->GetFile(), SOL_SOCKET, SO_RCVBUF, &sockbufsize, sizeof(sockbufsize));
  setsockopt(e->GetFile(), SOL_SOCKET, SO_SNDBUF, &sockbufsize, sizeof(sockbufsize));

  epoll_ctl(epfd_, EPOLL_CTL_ADD, e->GetFile(), &ev);

  return 0;
}

int EvengLoop::ModifyFileEvent(FileEvent *e, uint32_t events) {
  struct epoll_event ev;

  ev.events = events;
  ev.data.fd = e->GetFile();
  ev.data.ptr = e;

  return epoll_ctl(epfd_, EPOLL_CTL_MOD, e->GetFile(), &ev);
}

int EvengLoop::DeleteFileEvent(FileEvent *e) {
  return 0;
}

}
