#ifndef IVYXMLENTRY_H
#define IVYXMLENTRY_H

#include <string>
#include <vector>
#include <unordered_map>


class IvyXMLBasicEntry{
protected:
  std::string type;
  std::string body;
  std::unordered_map<std::string, std::string> attributes;

public:
  IvyXMLBasicEntry(){}
  IvyXMLBasicEntry(std::string const& strentry);
  virtual ~IvyXMLBasicEntry(){}

  void readBasicEntry(std::string strentry);

  std::string const& getType() const{ return this->type; }
  std::string const& getType(){ return this->type; }
  std::unordered_map<std::string, std::string> const& getAttributes() const{ return this->attributes; }
  std::unordered_map<std::string, std::string> const& getAttributes(){ return this->attributes; }

  virtual void print(bool ignore_body=true, unsigned int level=0) const;
};

template<typename T> class IvyXMLEntry : public IvyXMLBasicEntry{
public:
  typedef T EntryType_t;

protected:
  EntryType_t value;

public:
  void readEntry();

public:
  IvyXMLEntry(){}
  IvyXMLEntry(std::string const& strentry) : IvyXMLBasicEntry(strentry){ this->readEntry(); }
  virtual ~IvyXMLEntry(){}

  EntryType_t const& getValue() const{ return this->value; }

  void print(bool ignore_body=true, unsigned int level=0) const;
};


typedef IvyXMLEntry<std::string> IvyXMLStringEntry_t;
typedef IvyXMLEntry<std::vector<IvyXMLBasicEntry*>> IvyXMLCompositeEntry_t;


#endif
