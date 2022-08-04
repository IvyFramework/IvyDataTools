#include <cassert>
#include <stdexcept>
#include <fstream>
#include <iostream>
#include "HelperFunctionsCore.h"
#include "HostHelpersCore.h"
#include "IvyStreamHelpers.hh"
#include "IvyCSVReader.h"


using namespace std;
using namespace IvyStreamHelpers;


IvyCSVReader::IvyCSVReader(std::string fname, std::string escape_seq){
  this->readFile(fname, escape_seq);
}

void IvyCSVReader::readFile(std::string fname, std::string escape_seq){
  HostHelpers::ExpandEnvironmentVariables(fname);

  std::string str_in;
  ifstream fin(fname.data());
  if (fin.good()){
    unsigned long long idx_line = 0;
    while (!fin.eof()){
      getline(fin, str_in);
      HelperFunctions::lrstrip(str_in);
      HelperFunctions::removeNonASCIIChars(str_in);

      if (str_in=="" || (escape_seq!="" && str_in.find(escape_seq)==0)) continue; // Do not increment the line number, just skip.

      std::vector<std::string> entries;
      HelperFunctions::splitOptionRecursive(str_in, entries, ',', false);
      if (labels.empty()){
        labels = entries;
        for (auto const& lb:labels) label_entries_map[lb] = std::vector<std::string>();
      }
      else if (idx_line==0){
        // This should happen only if we are reading multiple csv files.
        if (labels!=entries){
          IVYerr << "IvyCSVReader::readFile: A label list of " << labels << " already exists, and the new csv file has an incompatible list of labels " << entries << "." << endl;
          assert(0);
        }
      }
      else{
        if (entries.size()!=labels.size()){
          IVYerr << "IvyCSVReader::readFile: The number of columns (" << entries.size() << ") is not the same as number of labels (" << labels.size() << ")." << endl;
          IVYerr << "(Row: '" << str_in << "')" << endl;
          assert(0);
        }
        unsigned int icl = 0;
        for (auto const& lb:labels){
          label_entries_map.find(lb)->second.push_back(entries.at(icl));
          icl++;
        }
      }

      idx_line++;
    }
    fin.close();
  }
}

std::vector<std::string> const& IvyCSVReader::getColumn(std::string const& label) const{
  if (!HelperFunctions::checkListVariable(labels, label)){
    IVYerr << "IvyCSVReader::getEntries: Label" << label << " is not found." << endl;
    assert(0);
  }
  return label_entries_map.find(label)->second;
}

unsigned long long IvyCSVReader::getNRows() const{
  unsigned long long res = 0;
  for (auto const& pp:label_entries_map){
    res = pp.second.size();
    break;
  }
  return res;
}
std::vector<std::string> IvyCSVReader::getRow(unsigned long long irow) const{
  std::vector<std::string> res;
  unsigned long long nrows = this->getNRows();
  if (irow<nrows){ res.reserve(nrows); for (auto const& lb:labels) res.push_back(label_entries_map.find(lb)->second.at(irow)); }
  return res;
}

BaseTree* IvyCSVReader::convertToTree(std::string const& treename) const{
  BaseTree* res = new BaseTree(treename.data());

  std::unordered_map<std::string, BaseTree::BranchType> label_btype_map;
  for (auto const& lb:labels){
    auto const& strvals = label_entries_map.find(lb)->second;
    BaseTree::BranchType btype = BaseTree::BranchType_int_t;
    for (auto const& strval:strvals){
      double dval = 0;
      try{ dval = std::stod(strval); }
      catch (std::invalid_argument const& e){
        btype = BaseTree::BranchType_string_t;
        break;
      }
      int ival = std::stoi(strval);
      if (static_cast<double>(ival)!=dval) btype = BaseTree::BranchType_double_t;
    }
    label_btype_map[lb] = btype;
    switch (btype){
    case BaseTree::BranchType_double_t:
      res->putBranch<BaseTree::BranchType_double_t>(lb.data());
      break;
    case BaseTree::BranchType_int_t:
      res->putBranch<BaseTree::BranchType_int_t>(lb.data());
      break;
    default:
      res->putBranch<BaseTree::BranchType_string_t>(lb.data());
      break;
    }
  }

  for (unsigned long long irow=0; irow<this->getNRows(); irow++){
    for (auto const& lb:labels){
      auto const& btype = label_btype_map.find(lb)->second;
      double dval=0;
      int ival=0;
      std::string const& sval = label_entries_map.find(lb)->second.at(irow);
      switch (btype){
      case BaseTree::BranchType_double_t:
        dval = std::stod(sval);
        res->setVal(lb.data(), dval);
        break;
      case BaseTree::BranchType_int_t:
        ival = std::stoi(sval);
        res->setVal(lb.data(), ival);
        break;
      default:
        res->setVal(lb.data(), sval);
        break;
      }
    }
    res->fill();
  }

  return res;
}
