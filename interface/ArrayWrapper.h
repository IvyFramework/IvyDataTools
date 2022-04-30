#ifndef ARRAYWRAPPER_H
#define ARRAYWRAPPER_H

#include <cassert>
#include <vector>
#include "TTree.h"
#include "TBranch.h"
#include "AnalysisDataTypes.hh"
#include "IvyStreamHelpers.hh"


template<typename T> class ArrayWrapper{
protected:
  typedef T Wrapped_t;

  unsigned int max_size;
  Wrapped_t valdef;
  Wrapped_t* values;

public:
  ArrayWrapper();
  ArrayWrapper(unsigned long const& max_size_, Wrapped_t const& valdef_);
  ArrayWrapper(ArrayWrapper<T> const&) = delete;
  ~ArrayWrapper();

  void setDefaultValue(Wrapped_t const& valdef_){ valdef = valdef_; }
  Wrapped_t* const& get_values() const{ return values; }
  unsigned int const& get_max_size() const{ return max_size; }
  void set_values(Wrapped_t* const& vals);

  void print() const;

  void reset();
};

template<typename T> ArrayWrapper<T>::ArrayWrapper() :
  max_size(0),
  valdef(0),
  values(nullptr)
{}
template<typename T> ArrayWrapper<T>::ArrayWrapper(unsigned long const& max_size_, Wrapped_t const& valdef_) :
  max_size(max_size_),
  valdef(valdef_)
{
  assert(max_size>0);
  values = new Wrapped_t[max_size];
  reset();
}
template<typename T> ArrayWrapper<T>::~ArrayWrapper(){ delete[] values; }

template<typename T> void ArrayWrapper<T>::reset(){ for (unsigned int i=0; i<max_size; i++) values[i] = valdef; }

template<typename T> void ArrayWrapper<T>::set_values(Wrapped_t* const& vals){
  unsigned int n_vals = sizeof(vals)/sizeof(valdef);
  unsigned int nn = (n_vals<max_size ? n_vals : max_size);
  for (unsigned int i=0; i<nn; i++) values[i] = vals[i];
}

template<typename T> void ArrayWrapper<T>::print() const{ for (unsigned int i=0; i<this->max_size; i++) IvyStreamHelpers::IVYout << (i>0 ? ", " : "") << values[i]; } \


#endif
