#ifndef BASEEMPTYCLASS_H
#define BASEEMPTYCLASS_H

/*
This is the simplest class ever. No members exist.
One can inherit from this class to generalize any call structure.
*/

struct BaseEmptyClass{
  BaseEmptyClass(){}
  BaseEmptyClass(BaseEmptyClass const&){}
  BaseEmptyClass(BaseEmptyClass&&){}
  virtual ~BaseEmptyClass(){}
};


typedef BaseEmptyClass BaseIOMessenger;


#endif
