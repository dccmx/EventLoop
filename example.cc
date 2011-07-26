#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>

#include "eventloop.h"

using namespace eventloop;

EventLoop el;

class BufferEvent : public BufferFileEvent {
 public:
  BufferEvent() {
    state_ = 0;
    Recive(buf_, 1);
  }

  void OnRecived(char *buffer, uint32_t len) {
    switch (state_) {
      case 0:
        len_ = buf_[0] - '0';
        Recive(buf_, len_);
        state_ = 1;
        break;
      case 1:
        Send(buf_, len_);
        state_ = 2;
        break;
      default:
        break;
    }
  }

  void OnSent(char *buffer, uint32_t len) {
    state_ = 0;
    Recive(buf_, 1);
  }

  void OnError() {
  }

 private:
  char buf_[1024];
  int len_;
  int state_;
};

class AcceptEvent: public BaseFileEvent {
 public:
  void OnEvents(uint32_t events) {
    if (events & BaseFileEvent::READ) {
      uint32_t size = 0;
      struct sockaddr_in addr;

      int fd = accept(file, (struct sockaddr*)&addr, &size);
      BufferEvent *e = new BufferEvent();
      e->SetFile(fd);
      el.AddEvent(e);
    }

    if (events & BaseFileEvent::ERROR) {
      close(file);
    }
  }
};

class Periodic : public PeriodicTimerEvent {
 public:
  void OnTimer() {
    printf("Periodic timer\n");
  }
};

class Timer : public BaseTimerEvent {
 public:
  void OnEvents(uint32_t events) {
    printf("timer:%u\n", static_cast<uint32_t>(time(0)));
    timeval tv = el.Now();
    tv.tv_sec += 1;
    SetTime(tv);
    el.UpdateEvent(this);
    if (p->IsRunning()) p->Stop();
    else p->Start();
  }

 public:
  Periodic *p;
};

class Signal : public BaseSignalEvent {
 public:
  void OnEvents(uint32_t events) {
    printf("shutdown\n");
    el.StopLoop();
  }
};

int main(int argc, char **argv) {
  int fd;
  AcceptEvent e;

  e.SetEvents(BaseFileEvent::READ | BaseFileEvent::ERROR);

  fd = BindTo("0.0.0.0", 11112);
  if (fd == -1) {
    printf("binding address %s", strerror(errno));
    return -1;
  }

  e.SetFile(fd);

  el.AddEvent(&e);

  Periodic p;
  timeval tv2;
  tv2.tv_sec = 0;
  tv2.tv_usec = 500 * 1000;
  p.SetInterval(tv2);

  el.AddEvent(&p);

  p.Start();

  Timer t;
  t.p = &p;
  timeval tv;
  gettimeofday(&tv, NULL);
  tv.tv_sec += 3;
  t.SetTime(tv);

  el.AddEvent(&t);

  Signal s;
  s.SetEvents(BaseSignalEvent::INT);
  el.AddEvent(&s);

  el.StartLoop();

  return 0;
}

