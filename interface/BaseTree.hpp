#ifndef BASETREE_HPP
#define BASETREE_HPP

#include "SampleHelpersCore.h"
#include "BaseTree.h"


#define SIMPLE_DATA_INPUT_DIRECTIVE(name, type, default_value) \
template<> bool BaseTree::getBranchCIterator<std::pair<type, type>*>(TString const& branchname, std::unordered_map<TString, std::pair<type, type>*>::iterator& it){ \
  auto& theMap = val##name##s; \
  it = theMap.find(branchname); \
  return (it!=theMap.end()); \
} \
template<> bool BaseTree::getBranchCIterator<std::pair<type, type>*>(TString const& branchname, std::unordered_map<TString, std::pair<type, type>*>::const_iterator& it) const{ \
  auto const& theMap = val##name##s; \
  it = theMap.find(branchname); \
  return (it!=theMap.cend()); \
} \
template<> void BaseTree::resetBranch<BaseTree::BranchType_##name##_t>(){ for (auto& it:val##name##s){ if (it.second){ it.second->first=it.second->second; } } } \
template<> void BaseTree::removeBranch<BaseTree::BranchType_##name##_t>(TString const& branchname){ for (auto& it:val##name##s){ if (it.first==branchname){ delete it.second; it.second=0; } } val##name##s.erase(branchname); } \
template<> bool BaseTree::bookBranch<type>(TString const& branchname, type valdef){ \
  if (val##name##s.find(branchname)==val##name##s.end()) val##name##s[branchname] = new std::pair<type, type>(valdef, valdef); \
  else{ val##name##s[branchname]->first=valdef; val##name##s[branchname]->second=valdef; } \
  for (auto& tt:treelist) SampleHelpers::bookBranch(tt, branchname, &(val##name##s[branchname]->first)); \
  return true; \
} \
template<> bool BaseTree::bookBranch<BaseTree::BranchType_##name##_t>(TString const& branchname){ return this->bookBranch<type>(branchname, default_value); } \
template<> bool BaseTree::putBranch<type>(TString const& branchname, type valdef){ \
  if (val##name##s.find(branchname)==val##name##s.end()) val##name##s[branchname] = new std::pair<type, type>(valdef, valdef); \
  else{ val##name##s[branchname]->first=valdef; val##name##s[branchname]->second=valdef; } \
  for (auto& tt:treelist) SampleHelpers::putBranch(tt, branchname, val##name##s[branchname]->first); \
  return true; \
} \
template<> bool BaseTree::putBranch<BaseTree::BranchType_##name##_t>(TString const& branchname){ return this->putBranch<type>(branchname, default_value); } \
template<> void BaseTree::getVal<type>(TString const& branchname, type& val) const{ \
  typedef type itType; \
  std::unordered_map<TString, std::pair<itType, itType>*>::const_iterator it; \
  if (this->getBranchCIterator<std::pair<itType, itType>*>(branchname, it)){ auto& tmp = it->second; if (tmp) val=tmp->first; } \
} \
template<> void BaseTree::setVal<type>(TString const& branchname, type const& val){ \
  typedef type itType; \
  std::unordered_map<TString, std::pair<itType, itType>*>::iterator it; \
  if (this->getBranchCIterator<std::pair<itType, itType>*>(branchname, it)){ auto& tmp = it->second; if (tmp) tmp->first=val; } \
} \
template<> void BaseTree::getValRef<type>(TString const& branchname, type*& val) const{ \
  typedef type itType; \
  std::unordered_map<TString, std::pair<itType, itType>*>::const_iterator it; \
  if (this->getBranchCIterator<std::pair<itType, itType>*>(branchname, it)){ auto& tmp = it->second; if (tmp) val=&(tmp->first); } \
} \
template<> void BaseTree::getValRef<type>(TString const& branchname, type*& val){ \
  typedef type itType; \
  std::unordered_map<TString, std::pair<itType, itType>*>::iterator it; \
  if (this->getBranchCIterator<std::pair<itType, itType>*>(branchname, it)){ auto& tmp = it->second; if (tmp) val=&(tmp->first); } \
} \

FUNDAMENTAL_DATA_INPUT_DIRECTIVES

#undef SIMPLE_DATA_INPUT_DIRECTIVE


#define SIMPLE_DATA_INPUT_DIRECTIVE(name, type, default_value) \
template<> bool BaseTree::getBranchCIterator<std::pair<type*, type>*>(TString const& branchname, std::unordered_map<TString, std::pair<type*, type>*>::iterator& it){ \
  auto& theMap = val##name##s; \
  it = theMap.find(branchname); \
  return (it!=theMap.end()); \
} \
template<> bool BaseTree::getBranchCIterator<std::pair<type*, type>*>(TString const& branchname, std::unordered_map<TString, std::pair<type*, type>*>::const_iterator& it) const{ \
  auto const& theMap = val##name##s; \
  it = theMap.find(branchname); \
  return (it!=theMap.cend()); \
} \
template<> void BaseTree::resetBranch<BaseTree::BranchType_##name##_t>(){ for (auto& it:val##name##s){ if (it.second){ *(it.second->first)=it.second->second; } } } \
template<> void BaseTree::removeBranch<BaseTree::BranchType_##name##_t>(TString const& branchname){ for (auto& it:val##name##s){ if (it.first==branchname){ delete it.second->first; delete it.second; it.second=0; } } val##name##s.erase(branchname); } \
template<> bool BaseTree::bookBranch<type>(TString const& branchname, type valdef){ \
  if (val##name##s.find(branchname)==val##name##s.end()) val##name##s[branchname] = new std::pair<type*, type>(nullptr, valdef); \
  else{ if (val##name##s[branchname]->first){ *(val##name##s[branchname]->first)=valdef; } val##name##s[branchname]->second=valdef; } \
  for (auto& tt:treelist) SampleHelpers::bookBranch(tt, branchname, &(val##name##s[branchname]->first)); \
  return true; \
} \
template<> bool BaseTree::bookBranch<BaseTree::BranchType_##name##_t>(TString const& branchname){ return this->bookBranch<type>(branchname, default_value); } \
template<> bool BaseTree::putBranch<type>(TString const& branchname, type valdef){ \
  if (val##name##s.find(branchname)==val##name##s.end()) val##name##s[branchname] = new std::pair<type*, type>(new type(valdef), valdef); \
  else{ *(val##name##s[branchname]->first)=valdef; val##name##s[branchname]->second=valdef; } \
  for (auto& tt:treelist) SampleHelpers::putBranch(tt, branchname, *(val##name##s[branchname]->first)); \
  return true; \
} \
template<> bool BaseTree::putBranch<BaseTree::BranchType_##name##_t>(TString const& branchname){ return this->putBranch<type>(branchname, default_value); } \
template<> void BaseTree::getVal<type>(TString const& branchname, type& val) const{ \
  typedef type itType; \
  std::unordered_map<TString, std::pair<itType*, itType>*>::const_iterator it; \
  if (this->getBranchCIterator<std::pair<itType*, itType>*>(branchname, it)){ auto& tmp = it->second; if (tmp) val=*(tmp->first); } \
} \
template<> void BaseTree::setVal<type>(TString const& branchname, type const& val){ \
  typedef type itType; \
  std::unordered_map<TString, std::pair<itType*, itType>*>::iterator it; \
  if (this->getBranchCIterator<std::pair<itType*, itType>*>(branchname, it)){ auto& tmp = it->second; if (tmp) *(tmp->first)=val; } \
} \
template<> void BaseTree::getValRef<type>(TString const& branchname, type*& val) const{ \
  typedef type itType; \
  std::unordered_map<TString, std::pair<itType*, itType>*>::const_iterator it; \
  if (this->getBranchCIterator<std::pair<itType*, itType>*>(branchname, it)){ auto& tmp = it->second; if (tmp) val=tmp->first; } \
} \
template<> void BaseTree::getValRef<type* const>(TString const& branchname, type* const*& val) const{ \
  typedef type itType; \
  std::unordered_map<TString, std::pair<itType*, itType>*>::const_iterator it; \
  if (this->getBranchCIterator<std::pair<itType*, itType>*>(branchname, it)){ auto& tmp = it->second; if (tmp && tmp->first) val=&(tmp->first); } \
} \
template<> void BaseTree::getValRef<type*>(TString const& branchname, type**& val){ \
  typedef type itType; \
  std::unordered_map<TString, std::pair<itType*, itType>*>::iterator it; \
  if (this->getBranchCIterator<std::pair<itType*, itType>*>(branchname, it)){ auto& tmp = it->second; if (tmp && tmp->first) val=&(tmp->first); } \
} \
template<> void BaseTree::getValRef<type* const>(TString const& branchname, type* const*& val){ \
  typedef type itType; \
  std::unordered_map<TString, std::pair<itType*, itType>*>::const_iterator it; \
  if (this->getBranchCIterator<std::pair<itType*, itType>*>(branchname, it)){ auto& tmp = it->second; if (tmp && tmp->first) val=&(tmp->first); } \
} \

CLASS_DATA_INPUT_DIRECTIVES

#undef SIMPLE_DATA_INPUT_DIRECTIVE


#define VECTOR_DATA_INPUT_DIRECTIVE(name, type) \
template<> bool BaseTree::getBranchCIterator<type*>(TString const& branchname, std::unordered_map<TString, type*>::iterator& it){ \
  auto& theMap = valV##name##s; \
  it = theMap.find(branchname); \
  return (it!=theMap.end()); \
} \
template<> bool BaseTree::getBranchCIterator<type*>(TString const& branchname, std::unordered_map<TString, type*>::const_iterator& it) const{ \
  auto const& theMap = valV##name##s; \
  it = theMap.find(branchname); \
  return (it!=theMap.cend()); \
} \
template<> void BaseTree::resetBranch<BaseTree::BranchType_v##name##_t>(){ for (auto& it:valV##name##s){ if (it.second) it.second->clear(); } } \
template<> void BaseTree::removeBranch<BaseTree::BranchType_v##name##_t>(TString const& branchname){ for (auto& it:valV##name##s){ if (it.first==branchname){ delete it.second; it.second=0; } } valV##name##s.erase(branchname); } \
template<> bool BaseTree::bookBranch<type*>(TString const& branchname, type*/* valdef*/){ \
  valV##name##s[branchname] = nullptr; \
  for (auto& tt:treelist) SampleHelpers::bookBranch(tt, branchname, &(valV##name##s[branchname])); \
  return true; \
} \
template<> bool BaseTree::bookBranch<BaseTree::BranchType_v##name##_t>(TString const& branchname){ return this->bookBranch<type*>(branchname, nullptr); } \
template<> bool BaseTree::putBranch<type*>(TString const& branchname, type*/* valdef*/){ \
  if (valV##name##s.find(branchname)==valV##name##s.end()) valV##name##s[branchname] = new type(); \
  else valV##name##s[branchname]->clear(); \
  for (auto& tt:treelist) SampleHelpers::putBranch(tt, branchname, *(valV##name##s[branchname])); \
  return true; \
} \
template<> bool BaseTree::putBranch<type const*>(TString const& branchname, type const*/* valdef*/){ \
  if (valV##name##s.find(branchname)==valV##name##s.end()) valV##name##s[branchname] = new type(); \
  else valV##name##s[branchname]->clear(); \
  for (auto& tt:treelist) SampleHelpers::putBranch(tt, branchname, *(valV##name##s[branchname])); \
  return true; \
} \
template<> bool BaseTree::putBranch<BaseTree::BranchType_v##name##_t>(TString const& branchname){ return this->putBranch<type*>(branchname, nullptr); } \
template<> void BaseTree::getVal<type const*>(TString const& branchname, type const*& val) const{ \
  typedef type itType; \
  std::unordered_map<TString, itType*>::const_iterator it; \
  if (this->getBranchCIterator<itType*>(branchname, it)) val = it->second; \
} \
template<> void BaseTree::setVal<type*>(TString const& branchname, type* const& val){ \
  typedef type itType; \
  std::unordered_map<TString, itType*>::iterator it; \
  if (this->getBranchCIterator<itType*>(branchname, it) && it->second && val) it->second->assign(val->begin(), val->end()); \
} \
template<> void BaseTree::setVal<type const*>(TString const& branchname, type const* const& val){ \
  typedef type itType; \
  std::unordered_map<TString, itType*>::iterator it; \
  if (this->getBranchCIterator<itType*>(branchname, it) && it->second && val) it->second->assign(val->cbegin(), val->cend()); \
} \
template<> void BaseTree::getValRef<type>(TString const& branchname, type*& val) const{ \
  typedef type itType; \
  std::unordered_map<TString, itType*>::const_iterator it; \
  if (this->getBranchCIterator<itType*>(branchname, it)) val = it->second; \
} \
template<> void BaseTree::getValRef<type* const>(TString const& branchname, type* const*& val) const{ \
  typedef type itType; \
  std::unordered_map<TString, itType*>::const_iterator it; \
  if (this->getBranchCIterator<itType*>(branchname, it)){ if (it->second) val = &(it->second); } \
} \
template<> void BaseTree::getValRef<type*>(TString const& branchname, type**& val){ \
  typedef type itType; \
  std::unordered_map<TString, itType*>::iterator it; \
  if (this->getBranchCIterator<itType*>(branchname, it)){ if (it->second) val = &(it->second); } \
} \
template<> void BaseTree::getValRef<type* const>(TString const& branchname, type* const*& val){ \
  typedef type itType; \
  std::unordered_map<TString, itType*>::const_iterator it; \
  if (this->getBranchCIterator<itType*>(branchname, it)){ if (it->second) val = &(it->second); } \
} \

VECTOR_DATA_INPUT_DIRECTIVES

#undef VECTOR_DATA_INPUT_DIRECTIVE


#define DOUBLEVECTOR_DATA_INPUT_DIRECTIVE(name, type) \
template<> bool BaseTree::getBranchCIterator<type*>(TString const& branchname, std::unordered_map<TString, type*>::iterator& it){ \
  auto& theMap = valVV##name##s; \
  it = theMap.find(branchname); \
  return (it!=theMap.end()); \
} \
template<> bool BaseTree::getBranchCIterator<type*>(TString const& branchname, std::unordered_map<TString, type*>::const_iterator& it) const{ \
  auto const& theMap = valVV##name##s; \
  it = theMap.find(branchname); \
  return (it!=theMap.cend()); \
} \
template<> void BaseTree::resetBranch<BaseTree::BranchType_vv##name##_t>(){ for (auto& it:valVV##name##s){ if (it.second) it.second->clear(); } } \
template<> void BaseTree::removeBranch<BaseTree::BranchType_vv##name##_t>(TString const& branchname){ for (auto& it:valVV##name##s){ if (it.first==branchname){ delete it.second; it.second=0; } } valVV##name##s.erase(branchname); } \
template<> bool BaseTree::bookBranch<type*>(TString const& branchname, type*/* valdef*/){ \
  valVV##name##s[branchname] = nullptr; \
  for (auto& tt:treelist) SampleHelpers::bookBranch(tt, branchname, &(valVV##name##s[branchname])); \
  return true; \
} \
template<> bool BaseTree::bookBranch<BaseTree::BranchType_vv##name##_t>(TString const& branchname){ return this->bookBranch<type*>(branchname, nullptr); } \
template<> bool BaseTree::putBranch<type*>(TString const& branchname, type*/* valdef*/){ \
  if (valVV##name##s.find(branchname)==valVV##name##s.end()) valVV##name##s[branchname] = new type(); \
  else valVV##name##s[branchname]->clear(); \
  for (auto& tt:treelist) SampleHelpers::putBranch(tt, branchname, *(valVV##name##s[branchname])); \
  return true; \
} \
template<> bool BaseTree::putBranch<type const*>(TString const& branchname, type const*/* valdef*/){ \
  if (valVV##name##s.find(branchname)==valVV##name##s.end()) valVV##name##s[branchname] = new type(); \
  else valVV##name##s[branchname]->clear(); \
  for (auto& tt:treelist) SampleHelpers::putBranch(tt, branchname, *(valVV##name##s[branchname])); \
  return true; \
} \
template<> bool BaseTree::putBranch<BaseTree::BranchType_vv##name##_t>(TString const& branchname){ return this->putBranch<type*>(branchname, nullptr); } \
template<> void BaseTree::getVal<type const*>(TString const& branchname, type const*& val) const{ \
  typedef type itType; \
  std::unordered_map<TString, itType*>::const_iterator it; \
  if (this->getBranchCIterator<itType*>(branchname, it)) val = it->second; \
} \
template<> void BaseTree::setVal<type*>(TString const& branchname, type* const& val){ \
  typedef type itType; \
  std::unordered_map<TString, itType*>::iterator it; \
  if (this->getBranchCIterator<itType*>(branchname, it) && it->second && val) it->second->assign(val->begin(), val->end()); \
} \
template<> void BaseTree::setVal<type const*>(TString const& branchname, type const* const& val){ \
  typedef type itType; \
  std::unordered_map<TString, itType*>::iterator it; \
  if (this->getBranchCIterator<itType*>(branchname, it) && it->second && val) it->second->assign(val->cbegin(), val->cend()); \
} \
template<> void BaseTree::getValRef<type>(TString const& branchname, type*& val) const{ \
  typedef type itType; \
  std::unordered_map<TString, itType*>::const_iterator it; \
  if (this->getBranchCIterator<itType*>(branchname, it)) val = it->second; \
} \
template<> void BaseTree::getValRef<type* const>(TString const& branchname, type* const*& val) const{ \
  typedef type itType; \
  std::unordered_map<TString, itType*>::const_iterator it; \
  if (this->getBranchCIterator<itType*>(branchname, it)){ if (it->second) val = &(it->second); } \
} \
template<> void BaseTree::getValRef<type*>(TString const& branchname, type**& val){ \
  typedef type itType; \
  std::unordered_map<TString, itType*>::iterator it; \
  if (this->getBranchCIterator<itType*>(branchname, it)){ if (it->second) val = &(it->second); } \
} \
template<> void BaseTree::getValRef<type* const>(TString const& branchname, type* const*& val){ \
  typedef type itType; \
  std::unordered_map<TString, itType*>::const_iterator it; \
  if (this->getBranchCIterator<itType*>(branchname, it)){ if (it->second) val = &(it->second); } \
} \

DOUBLEVECTOR_DATA_INPUT_DIRECTIVES

#undef DOUBLEVECTOR_DATA_INPUT_DIRECTIVE


#endif
