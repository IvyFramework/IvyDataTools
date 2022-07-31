#ifndef IVYCSVREADER_H
#define IVYCSVREADER_H

#include <string>
#include <vector>
#include <unordered_map>
#include "BaseTree.h"


class IvyCSVReader{
protected:
  std::vector<std::string> labels;
  std::unordered_map<std::string, std::vector<std::string>> label_entries_map;

public:
  IvyCSVReader(){}
  IvyCSVReader(std::string fname, std::string escape_seq="");
  ~IvyCSVReader(){}

  void readFile(std::string fname, std::string escape_seq="");

  std::vector<std::string> const& getLabels() const{ return labels; }
  std::vector<std::string> const& getColumn(std::string const& label) const;
  std::vector<std::string> const& operator[](std::string const& label) const{ return getColumn(label); }

  unsigned long long getNRows() const;
  std::vector<std::string> getRow(unsigned long long irow) const;

  // This function converts the csv file to an output tree.
  // The branch types are double, int, or std::string in order of precedence.
  BaseTree* convertToTree(std::string const& treename) const;
};


#endif
