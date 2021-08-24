#ifndef BASEIOMESSENGER_H
#define BASEIOMESSENGER_H

/*
This is the simplest IO class ever. No members exist.
One can inherit from this class to generalize function IO.
*/

struct BaseIOMessenger{
  BaseIOMessenger(){}
  BaseIOMessenger(BaseIOMessenger const&){}
  BaseIOMessenger(BaseIOMessenger&&){}
  virtual ~BaseIOMessenger(){}
};


#endif
