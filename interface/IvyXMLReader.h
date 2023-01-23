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

};


#endif
