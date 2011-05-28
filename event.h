#ifndef _EVENT_H_
#define _EVENT_H_

#include "util.h"

class Event {
  public:
    enum Type{
      READ = EPOLLIN,
      WRITE = EPOLLOUT,
      ERROR = EPOLLHUP | EPOLLERR | EPOLLRDHUP,
    };

  public:
    Event(Type events) {
      events_ = events;
    }

  public:
    virtual void Process(Type events);

  private:
    int fd_;
    Type events_;
};

class EventEngine {
  public:
    static EventEngine *Instance();

  public:
    int Add(Event *e);
    int Delete(Event *e);
    int Modify(Event *e, Event::Type events);

    int ProcessEvent(int timeout);
    void Run();

  protected:
    EventEngine() {
      nev = 0;
      evs = NULL;
      epfd = epoll_create(256);
    }

  private:
    static EventEngine *instance_;

    int epfd_;
    int nev = 0;
    epoll_event *evs_ = NULL;
};

#endif /* _EVENT_H_ */
