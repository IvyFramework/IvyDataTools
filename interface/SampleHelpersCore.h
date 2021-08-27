#ifndef SAMPLEHELPERSCORE_H
#define SAMPLEHELPERSCORE_H

#include <iostream>
#include <fstream>
#include <iomanip>
#include <memory>
#include <utility>
#include <algorithm>
#include <cmath>
#include <cassert>
#include <string>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <unordered_map>
#include <csignal>
#include "TH1F.h"
#include "TFile.h"
#include "TBranch.h"
#include "TTree.h"
#include "TChain.h"
#include "TList.h"
#include "HostHelpersCore.h"
#include "HelperFunctionsCore.h"
#include "IvyStreamHelpers.hh"


namespace SampleHelpers{
  extern TDirectory* const rootTDirectory;
  extern volatile sig_atomic_t doSignalInterrupt;

  void setSignalInterrupt(int snum);

  std::vector<TString> lsdir(TString const& indir);

  float findPoleMass(const TString samplename);
  TTree* findTree(std::vector<TTree*> const& treeList, int evid);
  bool branchExists(TTree* tree, TString strname);
  bool aliasExists(TTree* tree, TString strname);
  void getEntry(std::vector<TTree*>& treeList, int evid);
  float getEntry(std::vector<std::pair<TTree*, TH1F*>>& treeList, int evid);

  template<typename T> void bookBranch(TTree* tree, TString strname, T* var);
  template<typename T> void bookEDMBranch(TTree* tree, TString strname, T* var);
  template<typename T> void putBranch(TTree* tree, TString strname, T& var, bool forceRelink=false);
}

template<typename T> void SampleHelpers::bookBranch(TTree* tree, TString strname, T* var){
  if (tree){
    TString strbname = strname;
    // First search in aliases and replace strbname
    const TList* aliasList = (const TList*) tree->GetListOfAliases();
    if (aliasList){
      for (int ia=0; ia<aliasList->GetSize(); ia++){
        TObject* alias = aliasList->At(ia);
        if (!alias) continue;
        TString aname = alias->GetName();
        if (aname==strbname){
          strbname = alias->GetTitle();
          IvyStreamHelpers::IVYout << "SampleHelpers::bookBranch: Using branch name " << strbname << " instead of the alias " << strname << "." << std::endl;
          break;
        }
      }
    }
    if (SampleHelpers::branchExists(tree, strbname)){
      TString bnamegen="";
      if (strbname.Contains(".")){
        std::vector<TString> tmplist;
        HelperFunctions::splitOptionRecursive(strbname, tmplist, '.');
        if (!tmplist.empty()) bnamegen = tmplist.front() + "*";
      }
      if (bnamegen!="") tree->SetBranchStatus(bnamegen, 1);
      else tree->SetBranchStatus(strbname, 1);
      tree->SetBranchAddress(strbname, var);
      IvyStreamHelpers::IVYout << "SampleHelpers::bookBranch: Booked " << strbname << " to address " << var << "." << std::endl;
      if (!tree->GetBranchStatus(strbname)) IvyStreamHelpers::IVYerr << "SampleHelpers::bookBranch: Booked branch " << strbname << " has status 0!" << std::endl;
    }
    else IvyStreamHelpers::IVYout << "SampleHelpers::bookBranch: Branch " << strbname << " does not exist in tree " << tree->GetName() << "!" << std::endl;
  }
}
template<typename T> void SampleHelpers::bookEDMBranch(TTree* tree, TString strname, T* var){
  if (tree){
    TString strbname = strname;
    // First search in aliases and replace strbname
    const TList* aliasList = (const TList*) tree->GetListOfAliases();
    if (aliasList){
      for (int ia=0; ia<aliasList->GetSize(); ia++){
        TObject* alias = aliasList->At(ia);
        if (!alias) continue;
        TString aname = alias->GetName();
        if (aname==strbname){
          strbname = alias->GetTitle();
          break;
        }
      }
    }
    // Ensure that the branch name is "edm_product_name."
    if (strbname.Contains(".")){
      std::vector<TString> tmplist;
      HelperFunctions::splitOptionRecursive(strbname, tmplist, '.');
      if (!tmplist.empty()) strbname = tmplist.front() + ".";
    }
    SampleHelpers::bookBranch(tree, strbname, var);
  }
}
template<typename T> void SampleHelpers::putBranch(TTree* tree, TString strname, T& var, bool forceRelink){
  if (tree){
    // Do not check for branch alias
    if (!SampleHelpers::branchExists(tree, strname)){
      //IvyStreamHelpers::IVYout << "SampleHelpers::putBranch: Branching " << strname << std::endl;
      if (!tree->Branch(strname, &var)) IvyStreamHelpers::IVYerr << "SampleHelpers::putBranch: Did not succeed in creating a new branch for " << strname << " in tree " << tree->GetName() << std::endl;
    }
    else if (forceRelink){
      TBranch* br = tree->GetBranch(strname);
      if (br){
        br->ResetAddress();
        br->SetAddress(&var);
      }
    }
  }
}


#endif
