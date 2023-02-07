#ifndef IVYXMLREADER_H
#define IVYXMLREADER_H

#include "IvyXMLEntry.h"


class IvyXMLReader{
protected:
  IvyXMLCompositeEntry_t* entry;

public:
  IvyXMLReader();
  IvyXMLReader(std::string const& fname);
  ~IvyXMLReader(){ delete entry; }

  void readFile(std::string fname);

  IvyXMLCompositeEntry_t* const& getEntry() const{ return entry; }
  IvyXMLCompositeEntry_t* const& getEntry(){ return entry; }

  void print() const{ if (entry) entry->print(); }
};


#endif
