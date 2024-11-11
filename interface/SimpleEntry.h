#ifndef SIMPLEENTRY_H
#define SIMPLEENTRY_H

#include <vector>
#include <unordered_map>
#include "AnalysisDataTypes.hh"
#include "TBits.h"
#include "TString.h"
#include "TTree.h"
#include "BaseEmptyClass.h"
#include "HelperFunctionsCore.h"


struct SimpleScalarEntry : public virtual BaseIOMessenger{

#define SIMPLE_DATA_OUTPUT_DIRECTIVE(name_t, type) std::unordered_map<TString, type> named##name_t##s;
  SIMPLE_DATA_OUTPUT_DIRECTIVES
#undef SIMPLE_DATA_OUTPUT_DIRECTIVE

  SimpleScalarEntry();
  SimpleScalarEntry(SimpleScalarEntry const& other);
  SimpleScalarEntry(SimpleScalarEntry&& other);

  template<typename T> std::unordered_map<TString, T> const& getMap() const;
  template<typename T> std::unordered_map<TString, T>& getMap();

  template<typename T> void setVal(TString const& strname, T const& val);
  template<typename T> void getVal(TString const& strname, T& val) const;

  void swap(SimpleScalarEntry& other);
  SimpleScalarEntry& operator=(SimpleScalarEntry const& other);

};


struct SimpleVectorEntry : public virtual BaseIOMessenger{

#define VECTOR_DATA_OUTPUT_DIRECTIVE(name_t, type) std::unordered_map<TString, type> namedV##name_t##s;
  VECTOR_DATA_OUTPUT_DIRECTIVES
#undef VECTOR_DATA_OUTPUT_DIRECTIVE

  SimpleVectorEntry();
  SimpleVectorEntry(SimpleVectorEntry const& other);
  SimpleVectorEntry(SimpleVectorEntry&& other);

  template<typename T> std::unordered_map<TString, T> const& getMap() const;
  template<typename T> std::unordered_map<TString, T>& getMap();

  template<typename T> void setVal(TString const& strname, T const& val);
  template<typename T> void getVal(TString const& strname, T& val) const;

  void swap(SimpleVectorEntry& other);
  SimpleVectorEntry& operator=(SimpleVectorEntry const& other);

};


struct SimpleTensorEntry : public virtual BaseIOMessenger{

#define DOUBLEVECTOR_DATA_OUTPUT_DIRECTIVE(name_t, type) std::unordered_map<TString, type> namedVV##name_t##s;
  DOUBLEVECTOR_DATA_OUTPUT_DIRECTIVES
#undef DOUBLEVECTOR_DATA_OUTPUT_DIRECTIVE

  SimpleTensorEntry();
  SimpleTensorEntry(SimpleTensorEntry const& other);
  SimpleTensorEntry(SimpleTensorEntry&& other);

  template<typename T> std::unordered_map<TString, T> const& getMap() const;
  template<typename T> std::unordered_map<TString, T>& getMap();

  template<typename T> void setVal(TString const& strname, T const& val);
  template<typename T> void getVal(TString const& strname, T& val) const;

  void swap(SimpleTensorEntry& other);
  SimpleTensorEntry& operator=(SimpleTensorEntry const& other);

};


struct SimpleEntry : public SimpleScalarEntry, public SimpleVectorEntry, public SimpleTensorEntry{

  int id;
  float trackingval;
  float weight;
  std::vector<float> recoval;

  SimpleEntry();
  SimpleEntry(int const& id_, float const& trackingval_, float weight_=1);
  SimpleEntry(int const& id_, float const& trackingval_, std::vector<float> const& recoval_, float weight_=1);
  SimpleEntry(SimpleEntry const& other);
  SimpleEntry(SimpleEntry&& other);

  using SimpleScalarEntry::swap;
  using SimpleVectorEntry::swap;
  using SimpleTensorEntry::swap;
  void swap(SimpleEntry& other);
  SimpleEntry& operator=(SimpleEntry const& other);

  bool operator != (SimpleEntry const& other) const;
  bool operator == (SimpleEntry const& other) const;
  bool operator > (SimpleEntry const& other) const;
  bool operator >= (SimpleEntry const& other) const;
  bool operator < (SimpleEntry const& other) const;
  bool operator <= (SimpleEntry const& other) const;

  template<typename T> std::unordered_map<TString, T> const& getNamedMap() const;
  template<typename T> std::unordered_map<TString, T>& getNamedMap();

  template<typename T> void setNamedVal(TString const& strname, T const& val);
  template<typename T> void getNamedVal(TString const& strname, T& val) const;

  static void cropByTrackingVal(std::vector<SimpleEntry>& vec, float minval, float maxval);
  static void writeToTree(std::vector<SimpleEntry>::const_iterator const& vecBegin, std::vector<SimpleEntry>::const_iterator const& vecEnd, TTree* const& tree, bool forceRelink=false);
  void print();

};


struct ExtBin{
  double binlow;
  double binhigh;
  std::vector<SimpleEntry> collection;
  void addEvent(SimpleEntry const& evt);
};


#endif
