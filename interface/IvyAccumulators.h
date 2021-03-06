/*
Adapted from the implementation in https://github.com/cms-analysis/HiggsAnalysis-CombinedLimit,
*/

#ifndef IVYACCUMULATORS_H
#define IVYACCUMULATORS_H

#include <vector>


namespace NumericUtils{

  template <typename T> class NaiveAccumulator {
  protected:
    T sum_;
  public:
    NaiveAccumulator() : sum_(0){}
    NaiveAccumulator(const T& value) : sum_(value){}
    NaiveAccumulator(const NaiveAccumulator<T>& other) : sum_(other.sum_){}
    NaiveAccumulator& operator+=(const T& inc){ sum_ += inc; return *this; }
    NaiveAccumulator& operator-=(const T& inc){ sum_ -= inc; return *this; }
    NaiveAccumulator& operator*=(const T& inc){ sum_ *= inc; return *this; }
    NaiveAccumulator& operator/=(const T& inc){ sum_ /= inc; return *this; }
    NaiveAccumulator operator+(const T& inc){ NaiveAccumulator<T> tmp(*this);  tmp += inc; return tmp; }
    NaiveAccumulator operator-(const T& inc){ NaiveAccumulator<T> tmp(*this);  tmp -= inc; return tmp; }
    NaiveAccumulator operator*(const T& inc){ NaiveAccumulator<T> tmp(*this);  tmp *= inc; return tmp; }
    NaiveAccumulator operator/(const T& inc){ NaiveAccumulator<T> tmp(*this);  tmp /= inc; return tmp; }
    T sum() const { return sum_; }
    operator T() const { return sum_; }
  };

  template <typename T> class KahanAccumulator {
  protected:
    T sum_, compensation_;
  public:
    KahanAccumulator() : sum_(0), compensation_(0) {}
    KahanAccumulator(const T& value) : sum_(value), compensation_(0) {}
    KahanAccumulator(const KahanAccumulator<T>& other) : sum_(other.sum_), compensation_(other.compensation_){}
    KahanAccumulator& operator+=(const T& inc){
      T y = inc - compensation_;
      T sumnew = sum_ + y;
      T sumerr = (sumnew - sum_);
      compensation_ = sumerr - y;
      sum_ = sumnew;
      return *this;
    }
    KahanAccumulator& operator-=(const T& inc){ this->operator+=(-inc); return *this; }
    KahanAccumulator& operator*=(const T& inc){ sum_ *= inc; compensation_ *= inc; return *this; }
    KahanAccumulator& operator/=(const T& inc){ sum_ /= inc; compensation_ /= inc; return *this; }
    KahanAccumulator operator+(const T& inc){ KahanAccumulator<T> tmp(*this);  tmp += inc; return tmp; }
    KahanAccumulator operator-(const T& inc){ KahanAccumulator<T> tmp(*this);  tmp -= inc; return tmp; }
    KahanAccumulator operator*(const T& inc){ KahanAccumulator<T> tmp(*this);  tmp *= inc; return tmp; }
    KahanAccumulator operator/(const T& inc){ KahanAccumulator<T> tmp(*this);  tmp /= inc; return tmp; }
    T sum() const { return sum_; }
    operator T() const { return sum_; }
  };

  template<typename T> using PreciseAccumulator = KahanAccumulator<T>;
  template<typename T> using FastAccumulator = NaiveAccumulator<T>;
  template<typename T> using DefaultAccumulator = PreciseAccumulator<T>;

  template<typename T, class A> inline T sumWith(const std::vector<T>& vals){
    A ret = 0;
    for (const T& v : vals) ret += v;
    return ret;
  }

  template<typename T> inline T sumPrecise(const std::vector<T, std::allocator<T>>& vals){
    T ret = sumWith<T, PreciseAccumulator<T>>(vals);
    return ret;
  }

  template<typename T> inline T sumFast(const std::vector<T, std::allocator<T>>& vals){
    T ret = sumWith<T, FastAccumulator<T>>(vals);
    return ret;
  }

  template<typename T> inline T sumDefault(const std::vector<T, std::allocator<T>>& vals){
    T ret = sumWith<T, DefaultAccumulator<T>>(vals);
    return ret;
  }

}

#endif
