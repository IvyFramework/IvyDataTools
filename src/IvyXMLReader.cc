#include <cassert>
#include <stdexcept>
#include <fstream>
#include "HelperFunctionsCore.h"
#include "HostHelpersCore.h"
#include "IvyStreamHelpers.hh"
#include "IvyXMLReader.h"


using namespace std;
using namespace IvyStreamHelpers;


IvyXMLReader::IvyXMLReader() :
  entry(nullptr)
{}
IvyXMLReader::IvyXMLReader(std::string const& fname) :
  entry(nullptr)
{
  this->readFile(fname);
}

void IvyXMLReader::readFile(std::string fname){
  HostHelpers::ExpandEnvironmentVariables(fname);
  if (!HostHelpers::FileReadable(fname.data())){
    IVYerr << "IvyXMLReader::readFile: File " << fname << " is not readable." << endl;
    assert(0);
  }
  if (entry){
    IVYout << "IvyXMLReader::readFile: A file was loaded previously. The existing entry is now being discarded." << endl;
    delete entry;
    entry = nullptr;
  }

  std::size_t total_size=0;
  std::vector<std::string> strlist;
  std::string str_in;
  ifstream fin(fname.data());
  if (fin.good()){
    std::size_t pos_xml_spec_begin = std::string::npos;
    bool remove_xml_spec_begin = false;
    while (!fin.eof()){
      getline(fin, str_in);

      {
        pos_xml_spec_begin = str_in.find("<?xml");
        if (pos_xml_spec_begin!=std::string::npos) remove_xml_spec_begin = true;
        if (remove_xml_spec_begin){
          std::size_t pos_xml_spec_end = str_in.find("?>", pos_xml_spec_begin+1);
          bool const found_xml_spec_end = pos_xml_spec_end!=std::string::npos;
          if (found_xml_spec_end) pos_xml_spec_end += 2;
          str_in.erase(str_in.begin()+pos_xml_spec_begin, str_in.begin()+pos_xml_spec_end);
          if (found_xml_spec_end) remove_xml_spec_begin = false;
        }
      }

      HelperFunctions::lrstrip(str_in);
      HelperFunctions::removeNonASCIIChars(str_in);
      if (str_in=="") continue; // Do not increment the line number, just skip.

      strlist.push_back(str_in);
      total_size += str_in.size();
    }
    fin.close();
  }

  {
    // The head entry is a pseudotag with type="xml" to wrap the rest of the XML file.
    // This is needed to contain multiple tags at the first level properly.
    std::string const strbegin = "<xml>";
    std::string const strend = "</xml>";
    std::string const str_dummy;
    std::size_t const max_size = str_dummy.max_size() - strbegin.size() - strend.size();
    if (total_size>max_size){
      IVYerr << "IvyXMLReader::readFile: Implementation is limited to a maximum of " << max_size << " characters in the XML file. The current size is " << total_size << "." << endl;
      assert(0);
    }

    std::string strentry;
    for (auto const& ss:strlist) strentry += ss;
    entry = new IvyXMLCompositeEntry_t(strbegin+strentry+strend);
  }
}
