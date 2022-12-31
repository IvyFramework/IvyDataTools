#ifndef BASETREE_H
#define BASETREE_H

#include <vector>
#include <fstream>
#include <cstdlib>
#include <unordered_map>
#include "StdExtensions.h"
#include "SimpleEntry.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "CMSLorentzVector.h"
#include "AnalysisDataTypes.hh"
#include "ArrayWrapper.h"


class BaseTree{
protected:
  static bool robustSaveWrite;
  static bool doRobustInputCheck;

public:

#define SIMPLE_DATA_INPUT_DIRECTIVE(name, type, default_value) BranchType_##name##_t,
#define VECTOR_DATA_INPUT_DIRECTIVE(name, type) BranchType_v##name##_t,
#define DOUBLEVECTOR_DATA_INPUT_DIRECTIVE(name, type) BranchType_vv##name##_t,
#define ARRAY_DATA_INPUT_DIRECTIVE(name, type, default_value) BranchType_a##name##_t,
  enum BranchType{
    SIMPLE_DATA_INPUT_DIRECTIVES
    VECTOR_DATA_INPUT_DIRECTIVES
    DOUBLEVECTOR_DATA_INPUT_DIRECTIVES
    ARRAY_DATA_INPUT_DIRECTIVES
    BranchType_unknown_t
  };
#undef SIMPLE_DATA_INPUT_DIRECTIVE
#undef VECTOR_DATA_INPUT_DIRECTIVE
#undef DOUBLEVECTOR_DATA_INPUT_DIRECTIVE
#undef ARRAY_DATA_INPUT_DIRECTIVE

  TString sampleIdentifier;

  typedef TH1 HCounters_t;

protected:
  TFile* finput;
  std::vector<TTree*> treelist;
  TTree* tree;
  TTree* failedtree;
  HCounters_t* hCounters;
  bool valid;
  const bool receiver;
  bool acquireTreePossession; // If true, deletes trees and histograms upon destruction.
  bool isTChain;

  int currentEvent; // Index of event in the active tree
  int currentGlobalEvent; // Global index of event over all trees
  TTree* currentTree; // Active tree

#define SIMPLE_DATA_INPUT_DIRECTIVE(name, type, default_value) std::unordered_map<TString, std::pair<type, type>*> val##name##s;
  FUNDAMENTAL_DATA_INPUT_DIRECTIVES
#undef SIMPLE_DATA_INPUT_DIRECTIVE
#define SIMPLE_DATA_INPUT_DIRECTIVE(name, type, default_value) std::unordered_map<TString, std::pair<type*, type>*> val##name##s;
  CLASS_DATA_INPUT_DIRECTIVES
#undef SIMPLE_DATA_INPUT_DIRECTIVE
#define VECTOR_DATA_INPUT_DIRECTIVE(name, type) std::unordered_map<TString, type*> valV##name##s;
  VECTOR_DATA_INPUT_DIRECTIVES
#undef VECTOR_DATA_INPUT_DIRECTIVE
#define DOUBLEVECTOR_DATA_INPUT_DIRECTIVE(name, type) std::unordered_map<TString, type*> valVV##name##s;
  DOUBLEVECTOR_DATA_INPUT_DIRECTIVES
#undef DOUBLEVECTOR_DATA_INPUT_DIRECTIVE
#define ARRAY_DATA_INPUT_DIRECTIVE(name, type, default_value) std::unordered_map<TString, ArrayWrapper<type>*> valA##name##s;
  ARRAY_DATA_INPUT_DIRECTIVES
#undef ARRAY_DATA_INPUT_DIRECTIVE

  BranchType searchBranchType(TString const& branchname) const;

  void getValidBranchNamesWithoutAlias(TTree* t, std::vector<TString>& res, bool check_linked) const;

  template<typename T> bool getBranchCIterator(TString const& branchname, typename std::unordered_map<TString, T>::iterator& it);
  template<typename T> bool getBranchCIterator(TString const& branchname, typename std::unordered_map<TString, T>::const_iterator& it) const;

  template<BranchType T> void resetBranch();

  template<BranchType T> void removeBranch(TString const& branchname);

  // [tree][file] is the order of output vector indices
  bool getValidFilesForTreeList(TString cinput, std::vector<TString> const& treenames, std::vector< std::vector<TString> >& res, std::vector<TString>* all_files = nullptr) const;

  TBranch* getActiveBranch(int ev, TString const& bname, int* ptr_ev_tree = nullptr);

  // The class map is needed to deduce the branch type from an arbitrary branch name.
  // The map is filled only if it is necessary to do so.
  static std::unordered_map< BaseTree::BranchType, std::pair<TString, EDataType> > global_branchtype_class_map;
  static void set_global_branchtype_class_map();

public:
  BaseTree();
  BaseTree(const TString cinput, const TString treename, const TString failedtreename, const TString countersname);
  BaseTree(const TString cinput, std::vector<TString> const& treenames, const TString countersname);
  BaseTree(std::vector<TString> const& strinputfnames, std::vector<TString> const& treenames, const TString countersname);
  BaseTree(const TString treename); // Output constructor
  BaseTree(TFile* finput_, TTree* tree_, TTree* failedtree_, HCounters_t* hCounters_, bool receiver_override); // Mixed definition constructor
  BaseTree(TFile* finput_, std::vector<TTree*> const& treelist_, HCounters_t* hCounters_, bool receiver_override); // Mixed definition constructor
  virtual ~BaseTree();

  template<typename T> bool bookBranch(TString const& branchname, T valdef);
  template<BranchType T> bool bookBranch(TString const& branchname);

  template<typename T> bool bookArrayBranch(TString const& branchname, T valdef, ArrayWrapperPrimitive::ArraySize_t nmax);
  template<BranchType T> bool bookArrayBranch(TString const& branchname, ArrayWrapperPrimitive::ArraySize_t nmax);

  template<typename T> bool putBranch(TString const& branchname, T valdef);
  template<BranchType T> bool putBranch(TString const& branchname);

  virtual void resetBranches();

  TFile* getInputFile();
  TTree* getSelectedTree();
  TTree* getFailedTree();
  HCounters_t* getCountersHistogram();
  std::vector<TTree*>& getValidTrees();
  TFile const* getInputFile() const;
  TTree const* getSelectedTree() const;
  TTree const* getFailedTree() const;
  HCounters_t const* getCountersHistogram() const;
  std::vector<TTree*> const& getValidTrees() const;

  bool getSelectedEvent(int ev);
  bool getFailedEvent(int ev);
  bool getEvent(int ev);
  void refreshCurrentEvent();

  // Update single branch for an event
  // Should work similar to getEvent, but for a single branch.
  bool updateBranch(int ev, TString const& bname, bool check_linked = true);

  // GetNdata info from the leaves of a branch. Returns the number of entries in array types.
  int getBranchNdata(TString const& bname, bool check_linked = true);

  // For caching purposes
  bool getCurrentEventInfo(TTree*& currentTree_, int& currentEvent_) const;
  bool isSameEvent(TTree* const& currentTree_, int const& currentEvent_) const;

  int getSelectedNEvents() const;
  int getFailedNEvents() const;
  int getNEvents() const;

  template<typename T> void getVal(TString const& branchname, T& val) const;
  template<typename T> void setVal(TString const& branchname, T const& val); // For scalars, vectors, and double vectors
  template<typename T> void setVal(TString const& branchname, unsigned long n_vals, T const& val); // For arrays, T is actually a pointer type.
  template<typename T> void getValRef(TString const& branchname, T*& val) const;
  template<typename T> void getValRef(TString const& branchname, T*& val);

  void silenceUnused();
  void unmuteAllBranches();
  void releaseBranch(TString const& branchname);
  // This function below is for expert-use only!
  // The difference of the function below from the one above is that
  // the one above resets branch addresses
  // whereas the one below can be used to suppress branches temporarily.
  void muteAllBranchesExcept(std::vector<TString> const& bnames_excepted);
  // This function is to recover branches temporarily muted using muteAllBranchesExcept.
  void recoverMutedBranches(){ this->silenceUnused(); }

  void setAcquireTreePossession(bool);

  void setAutoSave(Long64_t fsave);
  long long doAutoSave(const char* opts="");

  void setAutoFlush(Long64_t fflush);
  int doFlushBaskets();

  bool isValid() const;
  bool branchExists(TString const& branchname, BranchType* type=nullptr);
  void getValidBranchNamesWithoutAlias(std::vector<TString>& res, bool check_linked) const;

  virtual bool isValidEvent() const;

  void fill();
  void writeToFile(TFile* file);
  void writeToDirectory(TDirectory* dir);

  virtual void print() const;

  static void setRobustSaveWrite(bool flag);
  static void setRobustInputCheck(bool flag);
  static void writeSimpleEntries(std::vector<SimpleEntry>::const_iterator const& vecBegin, std::vector<SimpleEntry>::const_iterator const& vecEnd, BaseTree* const& tree_, bool createBranches=true);

};


#endif
