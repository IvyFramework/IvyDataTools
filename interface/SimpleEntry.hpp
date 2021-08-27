#ifndef SIMPLEENTRY_HPP
#define SIMPLEENTRY_HPP

#include "SimpleEntry.h"


#define SIMPLEENTRYCOMMANDS \
SIMPLEENTRYCOMMAND(SimpleScalarEntry) \
SIMPLEENTRYCOMMAND(SimpleVectorEntry) \
SIMPLEENTRYCOMMAND(SimpleTensorEntry)

#define SIMPLEENTRYCOMMAND(CLNAME) \
template<typename T> void CLNAME::setVal(TString const& strname, T const& val){ \
  std::unordered_map<TString, T>& theMap = this->getMap<T>(); \
  theMap[strname] = val; \
} \
template<typename T> void CLNAME::getVal(TString const& strname, T& val) const{ \
  typename std::unordered_map<TString, T>::const_iterator it; \
  if (HelperFunctions::getUnorderedMapIterator(strname, this->getMap<T>(), it)) val = it->second; \
}

SIMPLEENTRYCOMMANDS

#undef SIMPLEENTRYCOMMAND
#undef SIMPLEENTRYCOMMANDS


#define SIMPLE_DATA_OUTPUT_DIRECTIVE(name_t, type) \
template<> std::unordered_map<TString, type> const& SimpleScalarEntry::getMap<type>() const{ return named##name_t##s; } \
template<> std::unordered_map<TString, type>& SimpleScalarEntry::getMap<type>(){ return named##name_t##s; } \
template void SimpleScalarEntry::setVal<type>(TString const& strname, type const& val); \
template void SimpleScalarEntry::getVal<type>(TString const& strname, type& val) const; \
template<> std::unordered_map<TString, type> const& SimpleEntry::getNamedMap<type>() const{ return this->SimpleScalarEntry::getMap<type>(); } \
template<> std::unordered_map<TString, type>& SimpleEntry::getNamedMap<type>(){ return this->SimpleScalarEntry::getMap<type>(); } \
template<> void SimpleEntry::setNamedVal<type>(TString const& strname, type const& val){ this->SimpleScalarEntry::setVal<type>(strname, val); } \
template<> void SimpleEntry::getNamedVal<type>(TString const& strname, type& val) const{ this->SimpleScalarEntry::getVal<type>(strname, val); }

#define VECTOR_DATA_OUTPUT_DIRECTIVE(name_t, type) \
template<> std::unordered_map<TString, type> const& SimpleVectorEntry::getMap<type>() const{ return namedV##name_t##s; } \
template<> std::unordered_map<TString, type>& SimpleVectorEntry::getMap<type>(){ return namedV##name_t##s; } \
template void SimpleVectorEntry::setVal<type>(TString const& strname, type const& val); \
template void SimpleVectorEntry::getVal<type>(TString const& strname, type& val) const; \
template<> std::unordered_map<TString, type> const& SimpleEntry::getNamedMap<type>() const{ return this->SimpleVectorEntry::getMap<type>(); } \
template<> std::unordered_map<TString, type>& SimpleEntry::getNamedMap<type>(){ return this->SimpleVectorEntry::getMap<type>(); } \
template<> void SimpleEntry::setNamedVal<type>(TString const& strname, type const& val){ this->SimpleVectorEntry::setVal<type>(strname, val); } \
template<> void SimpleEntry::getNamedVal<type>(TString const& strname, type& val) const{ this->SimpleVectorEntry::getVal<type>(strname, val); }

#define DOUBLEVECTOR_DATA_OUTPUT_DIRECTIVE(name_t, type) \
template<> std::unordered_map<TString, type> const& SimpleTensorEntry::getMap<type>() const{ return namedVV##name_t##s; } \
template<> std::unordered_map<TString, type>& SimpleTensorEntry::getMap<type>(){ return namedVV##name_t##s; } \
template void SimpleTensorEntry::setVal<type>(TString const& strname, type const& val); \
template void SimpleTensorEntry::getVal<type>(TString const& strname, type& val) const; \
template<> std::unordered_map<TString, type> const& SimpleEntry::getNamedMap<type>() const{ return this->SimpleTensorEntry::getMap<type>(); } \
template<> std::unordered_map<TString, type>& SimpleEntry::getNamedMap<type>(){ return this->SimpleTensorEntry::getMap<type>(); } \
template<> void SimpleEntry::setNamedVal<type>(TString const& strname, type const& val){ this->SimpleTensorEntry::setVal<type>(strname, val); } \
template<> void SimpleEntry::getNamedVal<type>(TString const& strname, type& val) const{ this->SimpleTensorEntry::getVal<type>(strname, val); }

SIMPLE_DATA_OUTPUT_DIRECTIVES
VECTOR_DATA_OUTPUT_DIRECTIVES
DOUBLEVECTOR_DATA_OUTPUT_DIRECTIVES

#undef DOUBLEVECTOR_DATA_OUTPUT_DIRECTIVE
#undef VECTOR_DATA_OUTPUT_DIRECTIVE
#undef SIMPLE_DATA_OUTPUT_DIRECTIVE


#endif
