#include <regex>
#include "IvyXMLEntry.hpp"


IvyXMLBasicEntry::IvyXMLBasicEntry(std::string const& strentry){
  readBasicEntry(strentry);
}
void IvyXMLBasicEntry::readBasicEntry(std::string strentry){
  HelperFunctions::lrstrip(strentry);

  std::regex rgx("<(.*?)>(.*)</(.*?)>");
  std::regex rgx_cdata("<\\!\\[CDATA\\[(.*)\\]\\]>");
  std::regex rgx_preproc("<\\?([^\\s]+? )(.*)\\?>");
  std::smatch sm;
  if (std::regex_match(strentry, sm, rgx)){
    this->body = sm[2].str();
    this->type = sm[3].str();
    std::string strattrs = sm[1].str();
    rgx = std::regex(Form("%s (.*=\".*\")+", this->type.data()));
    if (std::regex_match(strattrs, sm, rgx)){
      strattrs = sm[1].str();

      std::vector<std::string> attrs;
      HelperFunctions::splitOptionRecursive(strattrs, attrs, ' ', false);
      {
        std::vector<std::string> attrs_tmp;
        std::string entry_composite;
        bool isComposite = false;
        for (auto entry:attrs){
          if (!isComposite && entry.find('\"')!=std::string::npos) isComposite = true;
          if (isComposite){
            if (entry_composite=="") entry_composite = entry;
            else entry_composite = entry_composite + " " + entry;
          }
          if (isComposite){
            std::size_t const n_quotes = std::count(entry_composite.begin(), entry_composite.end(), '\"');
            if (n_quotes>0 && n_quotes%2==0){
              isComposite = false;
              entry = entry_composite;
              entry_composite = "";
            }
          }
          if (!isComposite) attrs_tmp.push_back(entry);
        }
        std::swap(attrs, attrs_tmp);
      }

      for (auto const& strattr:attrs){
        std::string stra, strv;
        HelperFunctions::splitOption(strattr, stra, strv, '=');
        if (stra==""){
          IVYerr << "IvyXMLBasicEntry::IvyXMLBasicEntry: Attribute in string " << strattr << " is not recognized." << endl;
          assert(0);
        }

        while (strv.find('\"')!=std::string::npos) HelperFunctions::replaceString<std::string, std::string const>(strv, "\"", "");
        while (strv.find("&lt;")!=std::string::npos) HelperFunctions::replaceString<std::string, std::string const>(strv, "&lt;", "<");
        while (strv.find("&gt;")!=std::string::npos) HelperFunctions::replaceString<std::string, std::string const>(strv, "&gt;", ">");
        while (strv.find("&amp;")!=std::string::npos) HelperFunctions::replaceString<std::string, std::string const>(strv, "&amp;", "&");
        while (strv.find("&apos;")!=std::string::npos) HelperFunctions::replaceString<std::string, std::string const>(strv, "&apos;", "'");
        while (strv.find("&quot;")!=std::string::npos) HelperFunctions::replaceString<std::string, std::string const>(strv, "&quot;", "\"");

        this->attributes[stra] = strv;
      }
    }
  }
  else if (std::regex_match(strentry, sm, rgx_cdata)){
    this->body = sm[1].str();
    this->type = "[CDATA[";
  }
  else if (std::regex_match(strentry, sm, rgx_preproc)){
    this->body = sm[2].str();
    this->type = Form("?%s", sm[1].str().data());
  }
  else{
    IVYerr << "IvyXMLBasicEntry::IvyXMLBasicEntry: Failed to recognize entry \"" << strentry << "\"..." << endl;
    assert(0);
  }
}
void IvyXMLBasicEntry::print(unsigned int level) const{
  for (unsigned int i=0; i<level; i++) IVYout << ' ';
  IVYout << "Type: " << this->type << endl;
  for (unsigned int i=0; i<level; i++) IVYout << ' ';
  IVYout << "Attributes: " << this->attributes << endl;
  for (unsigned int i=0; i<level; i++) IVYout << ' ';
  IVYout << "Body: " << this->body << endl;
}
