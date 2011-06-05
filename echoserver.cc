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

class ReadEvent : public BaseFileEvent {
 public:
  void Process(uint32_t events) {
    if (events & BaseFileEvent::READ) {
      char a;
      if (read(this->GetFile(), &a, sizeof(char)) == 0) {
        close(this->GetFile());
        delete this;
        return;
      }
      write(this->GetFile(), &a, sizeof(char));
    }

    if (events & BaseFileEvent::ERROR) {
      close(this->GetFile());
      delete this;
    }
  }
};

class AcceptEvent: public BaseFileEvent {
 public:
  void Process(uint32_t events) {
    if (events & BaseFileEvent::READ) {
      uint32_t size = 0;
      struct sockaddr_in addr;

      int fd = accept(this->GetFile(), (struct sockaddr*)&addr, &size);
      ReadEvent *e = new ReadEvent();
      e->SetFile(fd);
      e->SetType(BaseFileEvent::READ | BaseFileEvent::ERROR);
      el.AddEvent(e);
    }

    if (events & BaseFileEvent::ERROR) {
      close(this->GetFile());
    }
  }
};

class Timer : public BaseTimerEvent {
 public:
  void Process(uint32_t events) {
    printf("timer:%u\n", static_cast<uint32_t>(time(0)));
    timeval tv = el.Now();
    tv.tv_sec += 1;
    SetTime(tv);
    el.UpdateEvent(this);
  }
};

class Signal : public BaseSignalEvent {
 public:
  void Process(uint32_t events) {
    printf("shutdown\n");
    el.StopLoop();
  }
};

int main(int argc, char **argv) {
  int fd;
  AcceptEvent e;

  e.SetType(BaseFileEvent::READ | BaseFileEvent::ERROR);

  fd = BindTo("0.0.0.0", 12345);
  if (fd == -1) {
    printf("binding address %s", strerror(errno));
    return -1;
  }

  e.SetFile(fd);

  el.AddEvent(&e);

  Timer t;
  timeval tv;
  gettimeofday(&tv, NULL);
  tv.tv_sec += 3;
  t.SetTime(tv);

  el.AddEvent(&t);

  Signal s;
  s.SetType(BaseSignalEvent::INT);
  el.AddEvent(&s);

  el.StartLoop();

  return 0;
}

