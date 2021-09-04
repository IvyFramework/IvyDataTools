#ifndef BASEEDMINPUTTREE_HPP
#define BASEEDMINPUTTREE_HPP

#include "SampleHelpersCore.h"
#include "BaseEDMInputTree.h"


#define SIMPLE_DATA_INPUT_DIRECTIVE(name, type, default_value) \
template<> void BaseEDMInputTree::resetEDMBranch<BaseTree::BranchType_##name##_t>(){ BaseTree::resetBranch<BaseTree::BranchType_##name##_t>(); for (auto& it:bridge##name##s){ if (it.second) it.second->reset(); } } \
template<> void BaseEDMInputTree::removeEDMBranch<BaseTree::BranchType_##name##_t>(TString branchname){ BaseTree::removeBranch<BaseTree::BranchType_##name##_t>(branchname); for (auto& it:bridge##name##s){ if (it.first==branchname){ delete it.second; it.second=0; } } bridge##name##s.erase(branchname); } \
template<> bool BaseEDMInputTree::bookEDMBranch<type>(TString branchname, type valdef){ \
  if (val##name##s.find(branchname)==val##name##s.end()) val##name##s[branchname] = new std::pair<type, type>(valdef, valdef); \
  else{ val##name##s[branchname]->first=valdef; val##name##s[branchname]->second=valdef; } \
  if (bridge##name##s.find(branchname)==bridge##name##s.end()) bridge##name##s[branchname] = new CMSEDMWrapperLinker<type>(&(val##name##s[branchname]->first)); \
  SampleHelpers::bookEDMBranch(tree, branchname, &(bridge##name##s[branchname]->getWrapperRef())); \
  SampleHelpers::bookEDMBranch(failedtree, branchname, &(bridge##name##s[branchname]->getWrapperRef())); \
  return true; \
} \
template<> bool BaseEDMInputTree::bookEDMBranch<BaseTree::BranchType_##name##_t>(TString branchname){ return this->bookEDMBranch<type>(branchname, default_value); } \
template<> void CMSEDMWrapperLinker<type>::assignProductToTarget(CMSEDMWrapperLinker<type>::Wrapped_t& product){ *targetVal = product; } \
template<> void CMSEDMWrapperLinker<type, type>::print() const{ \
  using IvyStreamHelpers::IVYout; \
  Wrapped_t const* product = nullptr; if (var) product = var->product(); \
  IVYout << "\t\t- edm product: "; if (product) IVYout << *product; else IVYout << "null"; IVYout << " (address: " << product << ")" << std::endl; \
  IVYout << "\t\t- Target: "; if (targetVal) IVYout << *targetVal; else IVYout << "null"; IVYout << " (address: " << targetVal << ")" << std::endl; \
} \

FUNDAMENTAL_DATA_INPUT_DIRECTIVES

#undef SIMPLE_DATA_INPUT_DIRECTIVE


#define SIMPLE_DATA_INPUT_DIRECTIVE(name, type, default_value) \
template<> void BaseEDMInputTree::resetEDMBranch<BaseTree::BranchType_##name##_t>(){ BaseTree::resetBranch<BaseTree::BranchType_##name##_t>(); for (auto& it:bridge##name##s){ if (it.second) it.second->reset(); } } \
template<> void BaseEDMInputTree::removeEDMBranch<BaseTree::BranchType_##name##_t>(TString branchname){ BaseTree::removeBranch<BaseTree::BranchType_##name##_t>(branchname); for (auto& it:bridge##name##s){ if (it.first==branchname){ delete it.second; it.second=0; } } bridge##name##s.erase(branchname); } \
template<> bool BaseEDMInputTree::bookEDMBranch<type>(TString branchname, type valdef){ \
  if (val##name##s.find(branchname)==val##name##s.end()) val##name##s[branchname] = new std::pair<type*, type>(nullptr, valdef); \
  else{ if (val##name##s[branchname]->first){ *(val##name##s[branchname]->first)=valdef; } val##name##s[branchname]->second=valdef; } \
  if (bridge##name##s.find(branchname)==bridge##name##s.end()) bridge##name##s[branchname] = new CMSEDMWrapperLinker<type, type*>(&(val##name##s[branchname]->first)); \
  SampleHelpers::bookEDMBranch(tree, branchname, &(bridge##name##s[branchname]->getWrapperRef())); \
  SampleHelpers::bookEDMBranch(failedtree, branchname, &(bridge##name##s[branchname]->getWrapperRef())); \
  return true; \
} \
template<> bool BaseEDMInputTree::bookEDMBranch<BaseTree::BranchType_##name##_t>(TString branchname){ return this->bookEDMBranch<type>(branchname, default_value); } \
template<> void CMSEDMWrapperLinker<type, type*>::assignProductToTarget(CMSEDMWrapperLinker<type, type*>::Wrapped_t& product){ *targetVal = &product; } \
template<> void CMSEDMWrapperLinker<type, type*>::print() const{ \
  using IvyStreamHelpers::IVYout; \
  Wrapped_t const* product = nullptr; if (var) product = var->product(); \
  IVYout << "\t\t- edm product: "; if (product) IVYout << *product; else IVYout << "null"; IVYout << " (address: " << product << ")" << std::endl; \
  IVYout << "\t\t- Target: "; if (targetVal && *targetVal) IVYout << **targetVal; else IVYout << "null"; IVYout << " (address: "; if (targetVal) IVYout << *targetVal; else IVYout << "null"; IVYout << ")" << std::endl; \
} \

CLASS_DATA_INPUT_DIRECTIVES

#undef SIMPLE_DATA_INPUT_DIRECTIVE


#define VECTOR_DATA_INPUT_DIRECTIVE(name, type) \
template<> void BaseEDMInputTree::resetEDMBranch<BaseTree::BranchType_v##name##_t>(){ BaseTree::resetBranch<BaseTree::BranchType_v##name##_t>(); for (auto& it:bridgeV##name##s){ if (it.second) it.second->reset(); } } \
template<> void BaseEDMInputTree::removeEDMBranch<BaseTree::BranchType_v##name##_t>(TString branchname){ BaseTree::removeBranch<BaseTree::BranchType_v##name##_t>(branchname); for (auto& it:bridgeV##name##s){ if (it.first==branchname){ delete it.second; it.second=0; } } bridgeV##name##s.erase(branchname); } \
template<> bool BaseEDMInputTree::bookEDMBranch<type*>(TString branchname, type*/* valdef*/){ \
  valV##name##s[branchname] = nullptr; \
  if (bridgeV##name##s.find(branchname)==bridgeV##name##s.end()) bridgeV##name##s[branchname] = new CMSEDMWrapperLinker<type, type*>(&(valV##name##s[branchname])); \
  SampleHelpers::bookEDMBranch(tree, branchname, &(bridgeV##name##s[branchname]->getWrapperRef())); \
  SampleHelpers::bookEDMBranch(failedtree, branchname, &(bridgeV##name##s[branchname]->getWrapperRef())); \
  return true; \
} \
template<> bool BaseEDMInputTree::bookEDMBranch<BaseTree::BranchType_v##name##_t>(TString branchname){ return this->bookEDMBranch<type*>(branchname, nullptr); } \
template<> void CMSEDMWrapperLinker<type, type*>::assignProductToTarget(CMSEDMWrapperLinker<type, type*>::Wrapped_t& product){ *targetVal = &product; } \
template<> void CMSEDMWrapperLinker<type, type*>::print() const{ \
  using IvyStreamHelpers::IVYout; \
  Wrapped_t const* product = nullptr; if (var) product = var->product(); \
  IVYout << "\t\t- edm product: "; if (product) IVYout << *product; else IVYout << "null"; IVYout << " (address: " << product << ")" << std::endl; \
  IVYout << "\t\t- Target: "; if (targetVal && *targetVal) IVYout << **targetVal; else IVYout << "null"; IVYout << " (address: "; if (targetVal) IVYout << *targetVal; else IVYout << "null"; IVYout << ")" << std::endl; \
} \

VECTOR_DATA_INPUT_DIRECTIVES

#undef VECTOR_DATA_INPUT_DIRECTIVE


#define DOUBLEVECTOR_DATA_INPUT_DIRECTIVE(name, type) \
template<> void BaseEDMInputTree::resetEDMBranch<BaseTree::BranchType_vv##name##_t>(){ BaseTree::resetBranch<BaseTree::BranchType_vv##name##_t>(); for (auto& it:bridgeVV##name##s){ if (it.second) it.second->reset(); } } \
template<> void BaseEDMInputTree::removeEDMBranch<BaseTree::BranchType_vv##name##_t>(TString branchname){ BaseTree::removeBranch<BaseTree::BranchType_vv##name##_t>(branchname); for (auto& it:bridgeVV##name##s){ if (it.first==branchname){ delete it.second; it.second=0; } } bridgeVV##name##s.erase(branchname); } \
template<> bool BaseEDMInputTree::bookEDMBranch<type*>(TString branchname, type*/* valdef*/){ \
  valVV##name##s[branchname] = nullptr; \
  if (bridgeVV##name##s.find(branchname)==bridgeVV##name##s.end()) bridgeVV##name##s[branchname] = new CMSEDMWrapperLinker<type, type*>(&(valVV##name##s[branchname])); \
  SampleHelpers::bookEDMBranch(tree, branchname, &(bridgeVV##name##s[branchname]->getWrapperRef())); \
  SampleHelpers::bookEDMBranch(failedtree, branchname, &(bridgeVV##name##s[branchname]->getWrapperRef())); \
  return true; \
} \
template<> bool BaseEDMInputTree::bookEDMBranch<BaseTree::BranchType_vv##name##_t>(TString branchname){ return this->bookEDMBranch<type*>(branchname, nullptr); } \
template<> void CMSEDMWrapperLinker<type, type*>::assignProductToTarget(CMSEDMWrapperLinker<type, type*>::Wrapped_t& product){ *targetVal = &product; } \
template<> void CMSEDMWrapperLinker<type, type*>::print() const{ \
  using IvyStreamHelpers::IVYout; \
  Wrapped_t const* product = nullptr; if (var) product = var->product(); \
  IVYout << "\t\t- edm product: "; if (product) IVYout << *product; else IVYout << "null"; IVYout << " (address: " << product << ")" << std::endl; \
  IVYout << "\t\t- Target: "; if (targetVal && *targetVal) IVYout << **targetVal; else IVYout << "null"; IVYout << " (address: "; if (targetVal) IVYout << *targetVal; else IVYout << "null"; IVYout << ")" << std::endl; \
} \

DOUBLEVECTOR_DATA_INPUT_DIRECTIVES

#undef DOUBLEVECTOR_DATA_INPUT_DIRECTIVE


#endif
