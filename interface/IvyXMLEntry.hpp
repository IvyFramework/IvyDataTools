#ifndef IVYXMLENTRY_HPP
#define IVYXMLENTRY_HPP

#include <cassert>
#include "IvyXMLEntry.h"
#include "HelperFunctionsCore.h"
#include "IvyStreamHelpers.hh"


using namespace std;
using namespace IvyStreamHelpers;


template<> void IvyXMLStringEntry_t::readEntry(){
  this->value = this->body;

  while (this->value.find("&lt;")!=std::string::npos) HelperFunctions::replaceString(this->value, "&lt;", "<");
  while (this->value.find("&gt;")!=std::string::npos) HelperFunctions::replaceString(this->value, "&gt;", ">");
  while (this->value.find("&amp;")!=std::string::npos) HelperFunctions::replaceString(this->value, "&amp;", "&");
  while (this->value.find("&apos;")!=std::string::npos) HelperFunctions::replaceString(this->value, "&apos;", "'");
  while (this->value.find("&quot;")!=std::string::npos) HelperFunctions::replaceString(this->value, "&quot;", "\"");
}
template<> void IvyXMLCompositeEntry_t::readEntry(){
  std::string strentry = this->body;
  // Clean the entry from comment blocks
  {
    std::size_t pos_first_comment_begin = strentry.find("<!--");
    while (pos_first_comment_begin!=std::string::npos){
      std::size_t pos_first_comment_end = strentry.find("-->", pos_first_comment_begin);
      if (pos_first_comment_end==std::string::npos){
        IVYerr << "IvyXMLCompositeEntry_t::IvyXMLEntry: Comment block is not closed." << endl;
        assert(0);
      }
      strentry.erase(strentry.begin()+pos_first_comment_begin, strentry.begin()+pos_first_comment_end);
      pos_first_comment_begin = strentry.find("<!--");
    }
  }

  // Process entries
  std::vector<std::pair<std::pair<std::string, std::string>, std::string>> entries;
  {
    std::size_t pos_first_lt = strentry.find('<');
    while (pos_first_lt!=std::string::npos){
      bool const is_cdata = strentry.find("<![CDATA[")==pos_first_lt;
      bool const is_preproc = strentry.find("<?")==pos_first_lt;
      if (!is_cdata && !is_preproc){
        std::size_t pos_first_gt = strentry.find('>', pos_first_lt);
        std::string tmp_block(strentry.cbegin()+pos_first_lt+1, strentry.cbegin()+pos_first_gt);
        tmp_block = tmp_block.substr(0, tmp_block.find(' '));

        std::string tmp_block_end = Form("</%s>", tmp_block.data());
        std::size_t pos_block_end_begin = strentry.find(tmp_block_end);
        std::size_t pos_block_end = pos_block_end_begin + tmp_block_end.size();

        std::string tmp_body(strentry.cbegin()+pos_first_gt+1, strentry.cbegin()+pos_block_end_begin);
        std::string tmp_block_begin(strentry.cbegin()+pos_first_lt, strentry.cbegin()+pos_first_gt+1);
        entries.push_back(std::pair<std::pair<std::string, std::string>, std::string>(std::pair<std::string, std::string>(tmp_block_begin, tmp_block_end), tmp_body));
        strentry = strentry.substr(pos_block_end);
      }
      else if (is_preproc){
        std::size_t pos_first_gt = strentry.find("?>", pos_first_lt);
        std::string tmp_block(strentry.cbegin()+pos_first_lt, strentry.cbegin()+pos_first_gt);
        tmp_block = tmp_block.substr(0, tmp_block.find(' ')+1); // Block is in fact 'xml ', ', ' etc.
        std::size_t pos_block_end = pos_first_gt + 2;

        std::string tmp_body(strentry.cbegin()+pos_first_lt+tmp_block.size(), strentry.cbegin()+pos_first_gt);
        entries.push_back(std::pair<std::pair<std::string, std::string>, std::string>(std::pair<std::string, std::string>(tmp_block, "?>"), tmp_body));
        strentry = strentry.substr(pos_block_end);
      }
      else if (is_cdata){
        std::size_t pos_first_gt = strentry.find("]]>", pos_first_lt);
        std::string tmp_block = "<![CDATA[";
        std::size_t pos_block_end = pos_first_gt + 3;

        std::string tmp_body(strentry.cbegin()+pos_first_lt+tmp_block.size(), strentry.cbegin()+pos_first_gt);
        entries.push_back(std::pair<std::pair<std::string, std::string>, std::string>(std::pair<std::string, std::string>(tmp_block, "]]>"), tmp_body));
        strentry = strentry.substr(pos_block_end);
      }

      pos_first_lt = strentry.find('<');
    }
  }

  this->value.reserve(entries.size());
  for (auto const& pp:entries){
    std::string strfull = pp.first.first + pp.second + pp.first.second;
    if (pp.second.find('<')==std::string::npos) this->value.push_back(new IvyXMLStringEntry_t(strfull));
    else this->value.push_back(new IvyXMLCompositeEntry_t(strfull));
  }
}
template<> IvyXMLCompositeEntry_t::~IvyXMLEntry(){ for (auto& v:this->value) delete v; }

template<> void IvyXMLStringEntry_t::print(bool ignore_body, unsigned int level) const{
  IvyXMLBasicEntry::print(ignore_body, level);
  for (unsigned int i=0; i<level; ++i) IVYout << ' ';
  IVYout << "Value: " << this->value << endl;
}
template<> void IvyXMLCompositeEntry_t::print(bool ignore_body, unsigned int level) const{
  IvyXMLBasicEntry::print(ignore_body, level);
  for (unsigned int i=0; i<level; ++i) IVYout << ' ';
  IVYout << "Value: {" << endl;
  for (auto const& vv:this->value) vv->print(ignore_body, level+1);
  for (unsigned int i = 0; i<level; ++i) IVYout << ' ';
  IVYout << "}" << endl;
}


#endif
