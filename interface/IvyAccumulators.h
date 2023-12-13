/*
Adapted from the implementation in https://github.com/cms-analysis/HiggsAnalysis-CombinedLimit,
*/

#ifndef IVYACCUMULATORS_H
#define IVYACCUMULATORS_H

#include <iterator>


namespace NumericUtils{

  template<typename T> class BaseAccumulator{
  protected:
    T sum_;
  public:
    typedef T value_type;

    BaseAccumulator() : sum_(0){}
    BaseAccumulator(const T& value) : sum_(value){}
    BaseAccumulator(const BaseAccumulator<T>& other) : sum_(other.sum_){}

    T sum() const { return sum_; }
    operator T() const { return sum_; }
  };


  template<typename T> class NaiveAccumulator : public BaseAccumulator<T>{
  public:
    NaiveAccumulator() : BaseAccumulator<T>(){}
    NaiveAccumulator(const T& value) : BaseAccumulator<T>(value){}
    NaiveAccumulator(const NaiveAccumulator<T>& other) : BaseAccumulator<T>(other){}
    NaiveAccumulator& operator+=(const T& inc){ this->sum_ += inc; return *this; }
    NaiveAccumulator& operator-=(const T& inc){ this->sum_ -= inc; return *this; }
    NaiveAccumulator& operator*=(const T& inc){ this->sum_ *= inc; return *this; }
    NaiveAccumulator& operator/=(const T& inc){ this->sum_ /= inc; return *this; }
    NaiveAccumulator operator+(const T& inc){ NaiveAccumulator<T> tmp(*this); tmp += inc; return tmp; }
    NaiveAccumulator operator-(const T& inc){ NaiveAccumulator<T> tmp(*this); tmp -= inc; return tmp; }
    NaiveAccumulator operator*(const T& inc){ NaiveAccumulator<T> tmp(*this); tmp *= inc; return tmp; }
    NaiveAccumulator operator/(const T& inc){ NaiveAccumulator<T> tmp(*this); tmp /= inc; return tmp; }
  };

  template<typename T> class KahanAccumulator : public BaseAccumulator<T>{
  protected:
    T compensation_;
  public:
    KahanAccumulator() : BaseAccumulator<T>(), compensation_(0) {}
    KahanAccumulator(const T& value) : BaseAccumulator<T>(value), compensation_(0) {}
    KahanAccumulator(const KahanAccumulator<T>& other) : BaseAccumulator<T>(other), compensation_(other.compensation_){}
    KahanAccumulator& operator+=(const T& inc){
      T y = inc - compensation_;
      T sumnew = this->sum_ + y;
      T sumerr = (sumnew - this->sum_);
      compensation_ = sumerr - y;
      this->sum_ = sumnew;
      return *this;
    }
    KahanAccumulator& operator-=(const T& inc){ this->operator+=(-inc); return *this; }
    KahanAccumulator& operator*=(const T& inc){ this->sum_ *= inc; compensation_ *= inc; return *this; }
    KahanAccumulator& operator/=(const T& inc){ this->sum_ /= inc; compensation_ /= inc; return *this; }
    KahanAccumulator operator+(const T& inc){ KahanAccumulator<T> tmp(*this); tmp += inc; return tmp; }
    KahanAccumulator operator-(const T& inc){ KahanAccumulator<T> tmp(*this); tmp -= inc; return tmp; }
    KahanAccumulator operator*(const T& inc){ KahanAccumulator<T> tmp(*this); tmp *= inc; return tmp; }
    KahanAccumulator operator/(const T& inc){ KahanAccumulator<T> tmp(*this); tmp /= inc; return tmp; }
  };

  template<typename T> using PreciseAccumulator = KahanAccumulator<T>;
  template<typename T> using FastAccumulator = NaiveAccumulator<T>;
  template<typename T> using DefaultAccumulator = PreciseAccumulator<T>;

  template<typename Iterator_T, typename Accumulator_T> inline typename Accumulator_T::value_type sumWith(Iterator_T const& it_begin, Iterator_T const& it_end){
    Accumulator_T ret = 0;
    for (Iterator_T it = it_begin; it != it_end; ++it) ret += *it;
    return ret;
  }
  template<typename Iterable_T, class A, typename T=typename Iterable_T::value_type> inline T sumWith(Iterable_T const& vals){
    return sumWith<typename Iterable_T::const_iterator, A>(std::begin(vals), std::end(vals));
  }

  template<typename Iterable_T, typename T=typename Iterable_T::value_type> inline T sumPrecise(Iterable_T const& vals){
    return sumWith<Iterable_T, PreciseAccumulator<T>, T>(vals);
  }

  template<typename Iterable_T, typename T=typename Iterable_T::value_type> inline T sumFast(Iterable_T const& vals){
    return sumWith<Iterable_T, FastAccumulator<T>, T>(vals);
  }

  template<typename Iterable_T, typename T=typename Iterable_T::value_type> inline T sumDefault(Iterable_T const& vals){
    return sumWith<Iterable_T, DefaultAccumulator<T>, T>(vals);
  }

}

#endif
