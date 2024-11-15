#ifndef HELPERFUNCTIONSCORE_H
#define HELPERFUNCTIONSCORE_H

#include <cassert>
#include <string>
#include <sstream>
#include <vector>
#include <utility>
#include <algorithm>
#include <cctype>
#include <unordered_map>
#include <iterator>
#include <cmath>
#include <type_traits>
#include "TMath.h"
#include "StdExtensions.h"
#include "CMSLorentzVector.h"


// Behold the power of the member checker!
// From https://stackoverflow.com/questions/1005476/how-to-detect-whether-there-is-a-specific-member-variable-in-class
// You need to use the first one to construct the structs and the second one in the if-statement
#define CONSTRUCT_MEMBER_CHECKER(member_type, member) \
template<typename class_type, typename = void> struct has_##member : std::false_type {}; \
template<typename class_type> struct has_##member<class_type, decltype((void)class_type::member, member_type()) > : std::true_type {};
#define HAS_MEMBER(class_type, member_type, member) has_##member<class_type, member_type>::value
// Usage example:
// [Outside of function definitions]
// CONSTRUCT_MEMBER_CHECKER(int, id)
// [Inside the function]
// HAS_MEMBER(SimpleEntry, int, id) -> returns true
// HAS_MEMBER(int, int, id) -> returns false


namespace HelperFunctions{

  // Functions for vectors and maps
  template<typename T> void appendVector(std::vector<T>& a, std::vector<T> const& b);
  template<typename T, typename U> void zipVectors(std::vector<T> const& a, std::vector<U> const& b, std::vector<std::pair<T, U>>& res);

  template<typename T> void addByLowest(std::vector<T>& valArray, T const& val, bool unique);
  template<typename T, typename U> void addByLowest(std::vector<std::pair<T, U>>& valArray, T const& val, U const& index);
  template<typename T, typename U> void addByLowest(std::vector<std::pair<T, U>>& valArray, std::vector<std::pair<T, U>>& inArray, bool consecutive=false, bool inputordered=false);

  template<typename T> void addByHighest(std::vector<T>& valArray, T const& val, bool unique);
  template<typename T, typename U> void addByHighest(std::vector<std::pair<T, U>>& valArray, T const& val, U const& index);
  template<typename T, typename U> void addByHighest(std::vector<std::pair<T, U>>& valArray, std::vector<std::pair<T, U>>& inArray, bool consecutive=false, bool inputordered=false);

  template<typename T> bool checkListVariable(std::vector<T> const& list, T const& var);
  template<typename T> bool hasCommonElements(std::vector<T> const& list1, std::vector<T> const& list2);
  template<typename T> void splitElementsByCommonalty(std::vector<T> const& list1, std::vector<T> const& list2, std::vector<T>* common, std::vector<T>* only_list1, std::vector<T>* only_list2);

  template<typename T> bool removeListVariable(std::vector<T>& list, T const& var);

  template<typename T, typename U> bool getUnorderedMapIterator(T const& theKey, std::unordered_map<T, U> const& theMap, typename std::unordered_map<T, U>::const_iterator& it);
  template<typename T, typename U> bool getUnorderedMapIterator(T const& theKey, std::unordered_map<T, U>& theMap, typename std::unordered_map<T, U>::iterator& it);

  template<typename T, typename U> void cleanUnorderedMap(std::unordered_map<T, U>& um);

  template<typename T> void resetPointer(T*& ptr);

  template<typename T> unsigned long long int strlength(T str);
  template<> unsigned long long int strlength<std::string>(std::string str);
  template<> unsigned long long int strlength<TString>(TString str);
  template<> unsigned long long int strlength<char const*>(char const* str);

  template<typename T> struct replaceStringFcnal;
  template<> struct replaceStringFcnal<TString>{
    template<typename U, typename P> bool operator()(TString& strinput, U strTakeOut, P strPutIn);
  };
  template<> struct replaceStringFcnal<std::string>{
    template<typename U, typename P> bool operator()(std::string& strinput, U strTakeOut, P strPutIn);
  };

  template<typename T, typename U, typename P> bool replaceString(T& strinput, U strTakeOut, P strPutIn);

  template<typename T> void removeNonASCIIChars(T& str);
  template<> void removeNonASCIIChars<std::string>(std::string& str);
  template<> void removeNonASCIIChars<TString>(TString& str);

  template<typename T> void lstrip(T& str, const char* chars=nullptr);
  template<typename T> void rstrip(T& str, const char* chars=nullptr);
  template<typename T> void lrstrip(T& str, const char* chars=nullptr){ lstrip(str, chars); rstrip(str, chars); }
  template<> void lstrip<std::string>(std::string& str, const char* chars);
  template<> void lstrip<TString>(TString& str, const char* chars);
  template<> void rstrip<std::string>(std::string& str, const char* chars);
  template<> void rstrip<TString>(TString& str, const char* chars);

  template<typename T> void castStringToValue(std::string const& name, T& val);
  template<> void castStringToValue(std::string const& name, bool& val);
  template<typename T> void castStringToValue(TString const& name, T& val);
  template<typename T> void castStringToValue(const char* name, T& val);

  // In the following functions, assuming ibegin and iend are both nonnegative,
  // the transformed range is [ibegin, iend).
  template<typename T> void lowercase(T const& name, T& val, int ibegin=-1, int iend=-1);
  template<> void lowercase(std::string const& name, std::string& val, int ibegin, int iend);
  template<> void lowercase(TString const& name, TString& val, int ibegin, int iend);
  template<> void lowercase(const char* const& name, const char*& val, int ibegin, int iend);

  template<typename T> void uppercase(T const& name, T& val, int ibegin=-1, int iend=-1);
  template<> void uppercase(std::string const& name, std::string& val, int ibegin, int iend);
  template<> void uppercase(TString const& name, TString& val, int ibegin, int iend);
  template<> void uppercase(const char* const& name, const char*& val, int ibegin, int iend);

  // Value-to-string casts
  template<typename T> std::string castValueToString(T const& val, unsigned short max_decimals=4, unsigned short precision=4);

  // Non-template functions for string operations
  TString todaysdate();

  void progressbar(unsigned int val, unsigned int tot);

  bool splitOption(const std::string& rawoption, std::string& wish, std::string& value, char delimiter);
  bool splitOption(const TString& rawoption, TString& wish, TString& value, char delimiter);

  void splitOptionRecursive(const std::string& rawoption, std::vector<std::string>& splitoptions, char delimiter, bool uniqueResults=true, bool allowEmpty=false);
  void splitOptionRecursive(const TString& rawoption, std::vector<TString>& splitoptions, char delimiter, bool uniqueResults=true, bool allowEmpty=false);

  bool isASCIICharacter(signed char ch);
  bool isNonASCIICharacter(signed char ch);

  // Find the power of the first significant figure
  template<typename T> int getFirstSignificantDecimalPowerBase10(T const& val);

  // Non-zero and NaN/Inf checkers
  template<typename T> bool checkVarNonNegative(T const& val);
  template<typename T> bool checkNonNegative(std::vector<T> const& vars, int ibegin=-1, int iend=-1);
  template<typename T> bool checkVarNonZero(T const& val);
  template<typename T> bool checkNonZero(std::vector<T> const& vars, int ibegin=-1, int iend=-1);
  template<typename T> bool checkVarPositiveDef(T const& val);
  template<typename T> bool checkPositiveDef(std::vector<T> const& vars, int ibegin=-1, int iend=-1);
  template<typename T> bool checkVarNanInf(T const& val);
  template<typename T> bool checkNanInf(std::vector<T> const& vars);

  // Bit set and test
  template<typename T> void set_bit(T& mask, unsigned int iBit, bool val=true);
  template<typename T> bool test_bit(T const& mask, unsigned int iBit);
  template<typename T> TString displayBitsAsString(T const& mask, unsigned int const& nbits);

  // Math vector helpers
  template<typename T> void deltaR(T const& e1, T const& p1, T const& e2, T const& p2, T& res);
  template<typename T> void deltaEta(T const& v1, T const& v2, T& res);
  template<typename T> void deltaPhi(T const& v1, T const& v2, T& res);
}

template<typename T> void HelperFunctions::appendVector(std::vector<T>& a, std::vector<T> const& b){ a.insert(a.end(), b.cbegin(), b.cend()); }

template<typename T, typename U> void HelperFunctions::zipVectors(std::vector<T> const& a, std::vector<U> const& b, std::vector<std::pair<T, U>>& res){
  if (a.size()!=b.size()) assert(0);
  res.clear();
  res.reserve(a.size());
  typename std::vector<T>::const_iterator it_a = a.cbegin();
  typename std::vector<U>::const_iterator it_b = b.cbegin();
  while (it_a!=a.cend() && it_b!=b.cend()){
    res.emplace_back(*it_a, *it_b);
    ++it_a;
    ++it_b;
  }
}

template<typename T> void HelperFunctions::addByLowest(std::vector<T>& valArray, T const& val, bool unique){
  bool inserted = false;
  if (unique){
    for (typename std::vector<T>::iterator it = valArray.begin(); it!=valArray.end(); ++it){
      if (*it==val){
        inserted=true;
        break;
      }
    }
  }
  if (!inserted){
    for (typename std::vector<T>::iterator it = valArray.begin(); it!=valArray.end(); ++it){
      if (*it>=val){
        inserted=true;
        valArray.insert(it, val);
        break;
      }
    }
  }
  if (!inserted) valArray.push_back(val);
}
template<typename T, typename U> void HelperFunctions::addByLowest(std::vector<std::pair<T, U>>& valArray, T const& val, U const& index){
  bool inserted = false;
  for (typename std::vector<std::pair<T, U>>::iterator it = valArray.begin(); it!=valArray.end(); ++it){
    if ((*it).first>=val){
      inserted=true;
      if ((*it).second!=index) valArray.insert(it, std::pair<T, U>(val, index));
      break;
    }
  }
  if (!inserted) valArray.push_back(std::pair<T, U>(val, index));
}
template<typename T, typename U> void HelperFunctions::addByLowest(std::vector<std::pair<T, U>>& valArray, std::vector<std::pair<T, U>>& inArray, bool consecutive, bool inputordered){
  if (consecutive){
    bool inserted = false;
    typename std::vector<std::pair<T, U>>::iterator inbegin = inArray.begin();
    typename std::vector<std::pair<T, U>>::iterator inend = inArray.end();
    for (typename std::vector<std::pair<T, U>>::iterator it = valArray.begin(); it!=valArray.end(); ++it){
      if ((*it).first>=(*inbegin).first){
        inserted=true;
        if ((*it).second!=(*inbegin).second) valArray.insert(it, inbegin, inend);
        break;
      }
    }
    if (!inserted) appendVector<std::pair<T, U>>(valArray, inArray);
  }
  else if (!inputordered){
    for (typename std::vector<std::pair<T, U>>::iterator init = inArray.begin(); init!=inArray.end(); ++init){
      bool inserted = false;
      for (typename std::vector<std::pair<T, U>>::iterator it = valArray.begin(); it!=valArray.end(); ++it){
        if ((*it).first>=(*init).first){
          inserted=true;
          if ((*it).second!=(*init).second) valArray.insert(it, *init);
          break;
        }
      }
      if (!inserted) valArray.push_back(*init);
    }
  }
  else if (inArray.size()>0){
    typename std::vector<std::pair<T, U>>::iterator infirst = inArray.begin();
    typename std::vector<std::pair<T, U>>::iterator inlast = inArray.end()-1;
    typename std::vector<std::pair<T, U>>::iterator valfirst = valArray.begin();
    typename std::vector<std::pair<T, U>>::iterator vallast = valArray.end()-1;
    while ((*valfirst).first<(*infirst).first) ++valfirst;
    while ((*vallast).first>=(*inlast).first) --vallast;
    ++vallast;
    ++inlast;

    for (typename std::vector<std::pair<T, U>>::iterator init = infirst; init!=inlast; ++init){
      bool inserted = false;
      for (typename std::vector<std::pair<T, U>>::iterator it = valfirst; it!=vallast; ++it){
        if ((*it).first>=(*init).first){
          inserted=true;
          if ((*it).second!=(*init).second) valArray.insert(it, *init);
          break;
        }
      }
      if (!inserted) valArray.insert(vallast, *init);
    }
  }
}

template<typename T> void HelperFunctions::addByHighest(std::vector<T>& valArray, T const& val, bool unique){
  bool inserted = false;
  if (unique){
    for (typename std::vector<T>::iterator it = valArray.begin(); it!=valArray.end(); ++it){
      if (*it==val){
        inserted=true;
        break;
      }
    }
  }
  if (!inserted){
    for (typename std::vector<T>::iterator it = valArray.begin(); it!=valArray.end(); ++it){
      if (*it<=val){
        inserted=true;
        valArray.insert(it, val);
        break;
      }
    }
  }
  if (!inserted) valArray.push_back(val);
}
template<typename T, typename U> void HelperFunctions::addByHighest(std::vector<std::pair<T, U>>& valArray, T const& val, U const& index){
  bool inserted = false;
  for (typename std::vector<std::pair<T, U>>::iterator it = valArray.begin(); it!=valArray.end(); ++it){
    if ((*it).first<=val){
      inserted=true;
      if ((*it).second!=index) valArray.insert(it, std::pair<T, U>(val, index));
      break;
    }
  }
  if (!inserted) valArray.push_back(std::pair<T, U>(val, index));
}
template<typename T, typename U> void HelperFunctions::addByHighest(std::vector<std::pair<T, U>>& valArray, std::vector<std::pair<T, U>>& inArray, bool consecutive, bool inputordered){
  if (consecutive){
    bool inserted = false;
    typename std::vector<std::pair<T, U>>::iterator inbegin = inArray.begin();
    typename std::vector<std::pair<T, U>>::iterator inend = inArray.end();
    for (typename std::vector<std::pair<T, U>>::iterator it = valArray.begin(); it!=valArray.end(); ++it){
      if ((*it).first<=(*inbegin).first){
        inserted=true;
        if ((*it).second!=(*inbegin).second) valArray.insert(it, inbegin, inend);
        break;
      }
    }
    if (!inserted) appendVector<std::pair<T, U>>(valArray, inArray);
  }
  else if (!inputordered){
    for (typename std::vector<std::pair<T, U>>::iterator init = inArray.begin(); init!=inArray.end(); ++init){
      bool inserted = false;
      for (typename std::vector<std::pair<T, U>>::iterator it = valArray.begin(); it!=valArray.end(); ++it){
        if ((*it).first<=(*init).first){
          inserted=true;
          if ((*it).second!=(*init).second) valArray.insert(it, *init);
          break;
        }
      }
      if (!inserted) valArray.push_back(*init);
    }
  }
  else if (inArray.size()>0){
    typename std::vector<std::pair<T, U>>::iterator infirst = inArray.begin();
    typename std::vector<std::pair<T, U>>::iterator inlast = inArray.end()-1;
    typename std::vector<std::pair<T, U>>::iterator valfirst = valArray.begin();
    typename std::vector<std::pair<T, U>>::iterator vallast = valArray.end()-1;
    while ((*valfirst).first>(*infirst).first) ++valfirst;
    while ((*vallast).first<=(*inlast).first) --vallast;
    ++vallast;
    ++inlast;

    for (typename std::vector<std::pair<T, U>>::iterator init = infirst; init!=inlast; ++init){
      bool inserted = false;
      for (typename std::vector<std::pair<T, U>>::iterator it = valfirst; it!=vallast; ++it){
        if ((*it).first<=(*init).first){
          inserted=true;
          if ((*it).second!=(*init).second) valArray.insert(it, *init);
          break;
        }
      }
      if (!inserted) valArray.insert(vallast, *init);
    }
  }
}

template<typename T> bool HelperFunctions::checkListVariable(std::vector<T> const& list, T const& var){ return (std::find(std::begin(list), std::end(list), var)!=std::end(list)); }
template<typename T> bool HelperFunctions::hasCommonElements(std::vector<T> const& list1, std::vector<T> const& list2){
  for (T const& el1:list1){
    for (T const& el2:list2){
      if (el1==el2) return true;
    }
  }
  return false;
}
template<typename T> void HelperFunctions::splitElementsByCommonalty(std::vector<T> const& list1, std::vector<T> const& list2, std::vector<T>* common, std::vector<T>* only_list1, std::vector<T>* only_list2){
  if (common) common->clear();
  if (only_list1) only_list1->clear();
  if (only_list2) only_list2->clear();
  for (T const& el:list1){
    if (checkListVariable(list2, el)){ if (common) common->push_back(el); }
    else{ if (only_list1) only_list1->push_back(el); }
  }
  auto const& l2comp = (common ? *common : list1);
  for (T const& el:list2){
    if (!checkListVariable(l2comp, el)){ if (only_list2) only_list2->push_back(el); }
  }
}

template<typename T> bool HelperFunctions::removeListVariable(std::vector<T>& list, T const& var){
  unsigned long long int nlist_old = list.size();
  auto it_end_eff = std::remove(std::begin(list), std::end(list), var);
  list.erase(it_end_eff, std::end(list));
  return (list.size()!=nlist_old);
}

template<typename T, typename U> bool HelperFunctions::getUnorderedMapIterator(T const& theKey, std::unordered_map<T, U> const& theMap, typename std::unordered_map<T, U>::const_iterator& it){
  it = theMap.find(theKey);
  return (it!=theMap.cend());
}
template<typename T, typename U> bool HelperFunctions::getUnorderedMapIterator(T const& theKey, std::unordered_map<T, U>& theMap, typename std::unordered_map<T, U>::iterator& it){
  it = theMap.find(theKey);
  return (it!=theMap.end());
}

template<typename T, typename U> void HelperFunctions::cleanUnorderedMap(std::unordered_map<T, U>& um){ for (auto& it:um){ delete it.second; it.second=nullptr; } }

template<typename T> void HelperFunctions::resetPointer(T*& ptr){ delete ptr; ptr=nullptr; }

// String functions
template<typename U, typename P> bool HelperFunctions::replaceStringFcnal<TString>::operator()(TString& strinput, U strTakeOut, P strPutIn){
  Ssiz_t ipos = strinput.Index(strTakeOut);
  if (ipos!=-1){ strinput.Replace(ipos, static_cast<Ssiz_t>(strlength<std::remove_const_t<std::remove_reference_t<U>>>(strTakeOut)), strPutIn); return true; }
  else return false;
}
template<typename U, typename P> bool HelperFunctions::replaceStringFcnal<std::string>::operator()(std::string& strinput, U strTakeOut, P strPutIn){
  std::string::size_type ipos = strinput.find(strTakeOut);
  if (ipos!=std::string::npos){ strinput.replace(ipos, static_cast<std::string::size_type>(strlength<std::remove_const_t<std::remove_reference_t<U>>>(strTakeOut)), strPutIn); return true; }
  else return false;
}

template<typename T, typename U, typename P> bool HelperFunctions::replaceString(T& strinput, U strTakeOut, P strPutIn){
  replaceStringFcnal<T> fcn;
  return fcn(strinput, strTakeOut, strPutIn);
}

template<typename T> void HelperFunctions::castStringToValue(std::string const& name, T& val){ std::stringstream ss(name); ss >> val; }
template<typename T> void HelperFunctions::castStringToValue(TString const& name, T& val){ std::string s(name.Data()); HelperFunctions::castStringToValue(s, val); }
template<typename T> void HelperFunctions::castStringToValue(const char* name, T& val){ std::string s(name); HelperFunctions::castStringToValue(s, val); }
template void HelperFunctions::castStringToValue(TString const& name, bool& val);
template void HelperFunctions::castStringToValue(const char* name, bool& val);
template void HelperFunctions::castStringToValue(std::string const& name, short& val);
template void HelperFunctions::castStringToValue(TString const& name, short& val);
template void HelperFunctions::castStringToValue(const char* name, short& val);
template void HelperFunctions::castStringToValue(std::string const& name, unsigned int& val);
template void HelperFunctions::castStringToValue(TString const& name, unsigned int& val);
template void HelperFunctions::castStringToValue(const char* name, unsigned int& val);
template void HelperFunctions::castStringToValue(std::string const& name, int& val);
template void HelperFunctions::castStringToValue(TString const& name, int& val);
template void HelperFunctions::castStringToValue(const char* name, int& val);
template void HelperFunctions::castStringToValue(std::string const& name, float& val);
template void HelperFunctions::castStringToValue(TString const& name, float& val);
template void HelperFunctions::castStringToValue(const char* name, float& val);
template void HelperFunctions::castStringToValue(std::string const& name, double& val);
template void HelperFunctions::castStringToValue(TString const& name, double& val);
template void HelperFunctions::castStringToValue(const char* name, double& val);

template<typename T> std::string HelperFunctions::castValueToString(T const& val, unsigned short max_decimals, unsigned short precision){
  double decimals = std::abs(val - T((long long int) val));
  if (decimals == 0.) return Form("%.0f", double(val));
  decimals += 1e-10; // Machine precision correction...
  unsigned short base10exponent = std::ceil(std::abs(std::log10(decimals)));
  unsigned short iprec = std::min(max_decimals, static_cast<unsigned short>(base10exponent+precision));
  TString strprintf = Form("%s%u%s", "%.", iprec, "f");
  long long unsigned int remainder_prevtoLastDigit = static_cast<long long unsigned int>(decimals*std::pow(10., iprec+1)) % 5;
  double addval = (remainder_prevtoLastDigit==0 ? std::pow(10., -(iprec+1)) : 0.); // Form is smart enough to round 0.00006 to 0.0001, but 0.00005 becomes 0.0000.
  std::string res = Form(strprintf.Data(), static_cast<double>(val)+addval);
  if (res.find('.')!=std::string::npos){ while (res.back()=='0') res.pop_back(); }
  if (res.back()=='.') res.pop_back();
  return res;
}

// Find the power of the first significant figure
template<typename T> int HelperFunctions::getFirstSignificantDecimalPowerBase10(T const& val){ return (val==0. ? 0. : std::floor(std::log10(std::abs(val)))); }

// Non-zero and NaN/Inf checkers
template<typename T> bool HelperFunctions::checkVarNonNegative(T const& val){ return (val>=0.); }
template<typename T> bool HelperFunctions::checkNonNegative(std::vector<T> const& vars, int ibegin, int iend){
  int ipos=0;
  for (T const& v:vars){
    if ((ibegin>=0 && ipos<ibegin) || (iend>=0 && ipos>=iend)) continue;
    ++ipos;
    if (!checkVarNonNegative<T>(v)) return false;
  }
  return true;
}
template<typename T> bool HelperFunctions::checkVarNonZero(T const& val){ return (val!=0.); }
template<typename T> bool HelperFunctions::checkNonZero(std::vector<T> const& vars, int ibegin, int iend){
  int ipos=0;
  for (T const& v:vars){
    if ((ibegin>=0 && ipos<ibegin) || (iend>=0 && ipos>=iend)) continue;
    ++ipos;
    if (!checkVarNonZero<T>(v)) return false;
  }
  return true;
}
template<typename T> bool HelperFunctions::checkVarPositiveDef(T const& val){ return (val>0.); }
template<typename T> bool HelperFunctions::checkPositiveDef(std::vector<T> const& vars, int ibegin, int iend){
  int ipos=0;
  for (T const& v:vars){
    if ((ibegin>=0 && ipos<ibegin) || (iend>=0 && ipos>=iend)) continue;
    ++ipos;
    if (!checkVarPositiveDef<T>(v)) return false;
  }
  return true;
}
// Following https://stackoverflow.com/a/42138465
template<typename T> bool HelperFunctions::checkVarNanInf(T const& val){
  return !(std::isnan(val) || std::isinf(val) || (std::numeric_limits<T>::has_quiet_NaN && val==std::numeric_limits<T>::quiet_NaN()));
}
template<typename T> bool HelperFunctions::checkNanInf(std::vector<T> const& vars){
  for (T const& v:vars){ if (!checkVarNanInf<T>(v)) return false; }
  return true;
}

// Bit set and test
template<typename T> void HelperFunctions::set_bit(T& mask, unsigned int iBit, bool val){
  if (val) mask |= (1<<iBit);
  else if (test_bit(mask, iBit)){
    T tmp_mask = (1<<iBit);
    mask = mask ^ tmp_mask;
  }
}
template<typename T> bool HelperFunctions::test_bit(T const& mask, unsigned int iBit){ return (mask >> iBit) & 1; }
template<typename T> TString HelperFunctions::displayBitsAsString(T const& mask, unsigned int const& nbits){
  assert(nbits>0);
  TString res;
  unsigned int ibit = nbits-1;
  while (true){
    res = res + (test_bit(mask, ibit) ? "1" : "0");
    if (ibit==0) break;
    ibit--;
  }
  return res;
}

// Math vector helpers
template<typename T> void HelperFunctions::deltaR(T const& e1, T const& p1, T const& e2, T const& p2, T& res){
  T deta;
  deltaEta(e1, e2, deta);
  T dphi;
  deltaPhi(p1, p2, dphi);
  res = std::sqrt(std::pow(deta, 2) + std::pow(dphi, 2));
}
template<typename T> void HelperFunctions::deltaEta(T const& v1, T const& v2, T& res){
  res = v1 - v2;
}
template<typename T> void HelperFunctions::deltaPhi(T const& v1, T const& v2, T& res){
  res = v1 - v2;
  T const pi_val = TMath::Pi();
  T const twopi_val = TMath::Pi()*T(2);
  if (res>pi_val){ while (res>pi_val) res -= twopi_val; }
  else if (res<-pi_val){ while (res<-pi_val) res += twopi_val; }
}


#endif
