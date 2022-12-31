#ifndef ARRAYWRAPPER_H
#define ARRAYWRAPPER_H

#include <cassert>
#include <vector>
#include "TTree.h"
#include "TBranch.h"
#include "AnalysisDataTypes.hh"
#include "IvyStreamHelpers.hh"


class ArrayWrapperPrimitive{
public:
  typedef unsigned long long ArraySize_t;

protected:
  ArraySize_t max_size;

public:
  ArrayWrapperPrimitive() : max_size(0){}
  ArrayWrapperPrimitive(ArraySize_t const& max_size_) : max_size(max_size_){ assert(this->max_size>0); }
  virtual ~ArrayWrapperPrimitive(){}
};

template<typename T> class ArrayWrapper : public ArrayWrapperPrimitive{
protected:
  typedef T Wrapped_t;

  Wrapped_t valdef;
  Wrapped_t* values;

public:
  ArrayWrapper();
  ArrayWrapper(ArraySize_t const& max_size_, Wrapped_t const& valdef_);
  ArrayWrapper(ArrayWrapper<T> const&) = delete;
  ~ArrayWrapper();

  void setDefaultValue(Wrapped_t const& valdef_){ valdef = valdef_; }
  Wrapped_t* const& get_values() const{ return values; }
  ArraySize_t const& get_max_size() const{ return max_size; }
  void set_values(ArraySize_t n_vals, Wrapped_t* const& vals);

  void print() const;

  void reset();
};

template<typename T> ArrayWrapper<T>::ArrayWrapper() :
  ArrayWrapperPrimitive(),
  valdef(0),
  values(nullptr)
{}
template<typename T> ArrayWrapper<T>::ArrayWrapper(ArraySize_t const& max_size_, Wrapped_t const& valdef_) :
  ArrayWrapperPrimitive(max_size_),
  valdef(valdef_)
{
  values = new Wrapped_t[max_size];
  reset();
}
template<typename T> ArrayWrapper<T>::~ArrayWrapper(){ delete[] values; }

template<typename T> void ArrayWrapper<T>::reset(){ for (ArraySize_t i=0; i<max_size; i++) values[i] = valdef; }

template<typename T> void ArrayWrapper<T>::set_values(ArraySize_t n_vals, Wrapped_t* const& vals){
  ArraySize_t nn = (n_vals<max_size ? n_vals : max_size);
  for (ArraySize_t i=0; i<nn; i++) values[i] = vals[i];
}

template<typename T> void ArrayWrapper<T>::print() const{ for (ArraySize_t i=0; i<this->max_size; i++) IvyStreamHelpers::IVYout << (i>0 ? ", " : "") << values[i]; } \


#endif
