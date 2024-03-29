#ifndef IVYBASE_H
#define IVYBASE_H

#include "VerbosityLevel.h"
#include "BaseEmptyClass.h"
#include "BaseTree.h"
#include "HelperFunctionsCore.h"
#include "IvyStreamHelpers.hh"


class IvyBase : public BaseEmptyClass{
protected:
  MiscUtils::VerbosityLevel verbosity;
  BaseTree* currentTree;

  TTree* eventCache_currentTTree;
  int eventCache_currentEvent;

  // Consumes
  std::vector<TString> sloppyConsumes; // In case some variables are known to be absent in some trees

#define SIMPLE_DATA_INPUT_DIRECTIVE(name, type, default_value) std::unordered_map<TString, type*> val##name##s;
  FUNDAMENTAL_DATA_INPUT_DIRECTIVES
#undef SIMPLE_DATA_INPUT_DIRECTIVE
#define SIMPLE_DATA_INPUT_DIRECTIVE(name, type, default_value) std::unordered_map<TString, type**> val##name##s;
  CLASS_DATA_INPUT_DIRECTIVES
#undef SIMPLE_DATA_INPUT_DIRECTIVE
#define VECTOR_DATA_INPUT_DIRECTIVE(name, type) std::unordered_map<TString, type**> valV##name##s;
  VECTOR_DATA_INPUT_DIRECTIVES
#undef VECTOR_DATA_INPUT_DIRECTIVE
#define DOUBLEVECTOR_DATA_INPUT_DIRECTIVE(name, type) std::unordered_map<TString, type**> valVV##name##s;
  DOUBLEVECTOR_DATA_INPUT_DIRECTIVES
#undef DOUBLEVECTOR_DATA_INPUT_DIRECTIVE
#define ARRAY_DATA_INPUT_DIRECTIVE(name, type, default_value) std::unordered_map<TString, type* const*> valA##name##s;
  ARRAY_DATA_INPUT_DIRECTIVES
#undef ARRAY_DATA_INPUT_DIRECTIVE

  template<typename T> bool linkConsumed(BaseTree* tree);
  bool linkConsumes(BaseTree* tree);

  // Get consumed map
  template<typename T> void getConsumedMap(std::unordered_map<TString, T*>*& theMap);
  template<typename T> void getConsumedMap(std::unordered_map<TString, T*> const*& theMap) const;

  // Get consumed reference
  template<typename T> bool getConsumed(TString name, T*& val) const;
  template<typename T> bool getConsumed(TString name, T const*& val) const;

  // Get consumed value
  template<typename T> bool getConsumedValue(TString name, T& val) const;
  template<typename T> bool getConsumedCIterators(TString name, typename T::const_iterator* it_begin, typename T::const_iterator* it_end=nullptr) const;

  // Cache the event if the daughter supports checking caches
  // Returns true if the event is valid.
  bool cacheEvent();

public:
  // Constructors
  IvyBase();

  // Destructors
  virtual ~IvyBase();

  // Add the necessary objects
  template<typename T> void addConsumed(TString name);
  void defineConsumedSloppy(TString name);

  // Verbosity
  void setVerbosity(MiscUtils::VerbosityLevel flag){ verbosity=flag; }
  MiscUtils::VerbosityLevel getVerbosity() const{ return verbosity; }

  // Tree
  virtual bool wrapTree(BaseTree* tree);
  BaseTree* getWrappedTree() const{ return currentTree; }
  BaseTree* getWrappedTree(){ return currentTree; }

  // Reset the cache
  void resetCache();

  // Check if cache info is the same as current event.
  bool isAlreadyCached() const;

};

template<typename T> bool IvyBase::getConsumedValue(TString name, T& val) const{
  T const* ref;
  bool res = this->getConsumed<T>(name, ref);
  if (res && ref!=nullptr){
    val=*ref;
    return true;
  }
  else{
    if (!res && verbosity>=MiscUtils::ERROR) IvyStreamHelpers::IVYerr << "IvyBase::getConsumedValue: Cannot consume " << name << std::endl;
    return res;
  }
}
template<typename T> bool IvyBase::getConsumedCIterators(TString name, typename T::const_iterator* it_begin, typename T::const_iterator* it_end) const{
  T* vec=nullptr;
  bool res = this->getConsumedValue<T*>(name, vec);
  if (!res || !vec){
    if (verbosity>=MiscUtils::ERROR){
      if (!res) IvyStreamHelpers::IVYerr << "IvyBase::getConsumedCIterators: Cannot consume " << name << std::endl;
      if (!vec) IvyStreamHelpers::IVYerr << "IvyBase::getConsumedCIterators: Vector of " << name << " is null." << std::endl;
    }
    return false;
  }
  else{
    if (it_begin) *it_begin = vec->cbegin();
    if (it_end) *it_end = vec->cend();
    return true;
  }
}


#endif
