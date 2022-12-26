#ifndef IVYNUMERICUTILS_H
#define IVYNUMERICUTILS_H

#include <vector>


namespace NumericUtils{

  template<typename T> struct doublet{
    static constexpr std::size_t size_=2;
    T value[size_];
    doublet(T const& i1, T const& i2){
      value[0]=i1;
      value[1]=i2;
    }
    doublet(T const& i1){
      value[0]=value[1]=i1;
    }
    doublet(){}
    T& operator[](std::size_t ipos){ return value[ipos]; } // Return by reference
    const T& operator[](std::size_t ipos)const{ return value[ipos]; } // Return by const reference
    bool operator == (const doublet<T>& other)const{ bool res = true; for (std::size_t i=0; i<size_; i++) res &= (*this)[i]==other[i]; return res; }
    bool operator != (const doublet<T>& other)const{ return !(*this==other); }
  };
  template<typename T> struct triplet{
    static constexpr std::size_t size_=3;
    T value[size_];
    triplet(T const& i1, T const& i2, T const& i3){
      value[0]=i1;
      value[1]=i2;
      value[2]=i3;
    }
    triplet(T const& i1){
      value[0]=value[1]=value[2]=i1;
    }
    triplet(){}
    T& operator[](std::size_t ipos){ return value[ipos]; } // Return by reference
    const T& operator[](std::size_t ipos)const{ return value[ipos]; } // Return by const reference
    bool operator == (const triplet<T>& other)const{ bool res = true; for (std::size_t i=0; i<size_; i++) res &= (*this)[i]==other[i]; return res; }
    bool operator != (const triplet<T>& other)const{ return !(*this==other);  }
  };
  template<typename T> struct quadruplet{
    static constexpr std::size_t size_=4;
    T value[size_];
    quadruplet(T const& i1, T const& i2, T const& i3, T const& i4){
      value[0]=i1;
      value[1]=i2;
      value[2]=i3;
      value[3]=i4;
    }
    quadruplet(T const& i1){ for (unsigned int idim=0; idim<size_; idim++) value[idim] = i1; }
    quadruplet(){}
    T& operator[](std::size_t ipos){ return value[ipos]; } // Return by reference
    const T& operator[](std::size_t ipos)const{ return value[ipos]; } // Return by const reference
    bool operator == (const quadruplet<T>& other)const{ bool res = true; for (std::size_t i=0; i<size_; i++) res &= (*this)[i]==other[i]; return res; }
    bool operator != (const quadruplet<T>& other)const{ return !(*this==other); }
  };

  typedef doublet<unsigned int> uintDoublet_t;
  typedef doublet<int> intDoublet_t;
  typedef doublet<float> floatDoublet_t;
  typedef doublet<double> doubleDoublet_t;

  typedef triplet<unsigned int> uintTriplet_t;
  typedef triplet<int> intTriplet_t;
  typedef triplet<float> floatTriplet_t;
  typedef triplet<double> doubleTriplet_t;

  typedef quadruplet<unsigned int> uintQuad_t;
  typedef quadruplet<int> intQuad_t;
  typedef quadruplet<float> floatQuad_t;
  typedef quadruplet<double> doubleQuad_t;

  void PermutationGenerator(int n, int k, std::vector<std::vector<int>>& perms, int valbegin=1, int increment=1); // n!/(n-k)! different permutations of {i_1,...,i_k}, i+j in {valbegin,...,valbegin+increment*(n-1)}
  void CombinationGenerator(int n, int k, std::vector<std::vector<int>>& perms, int valbegin=1, int increment=1); // n!/(k!(n-k)!) different combinations (unordered permutations) of {i_1,...,i_k}, i+j in {valbegin,...,valbegin+increment*(n-1)}

  template<typename T> void GreatestCommonDivisor(T const& a, T const& b, T& res);
  template<typename T> void GreatestCommonDivisor(std::vector<T> const& nums, T& res);

  template<typename T> void LeastCommonMultiple(T const& a, T const& b, T& res);
  template<typename T> void LeastCommonMultiple(std::vector<T> const& nums, T& res);

}

template<typename T> void NumericUtils::GreatestCommonDivisor(T const& a, T const& b, T& res){
  if (b==T(0)){
    res = (a!=T(0) ? a : T(1));
    return;
  }
  GreatestCommonDivisor(b, static_cast<T>(a % b), res);
}
template void NumericUtils::GreatestCommonDivisor(unsigned char const& a, unsigned char const& b, unsigned char& res);
template void NumericUtils::GreatestCommonDivisor(char const& a, char const& b, char& res);
template void NumericUtils::GreatestCommonDivisor(unsigned short const& a, unsigned short const& b, unsigned short& res);
template void NumericUtils::GreatestCommonDivisor(short const& a, short const& b, short& res);
template void NumericUtils::GreatestCommonDivisor(unsigned int const& a, unsigned int const& b, unsigned int& res);
template void NumericUtils::GreatestCommonDivisor(int const& a, int const& b, int& res);
template void NumericUtils::GreatestCommonDivisor(unsigned long long int const& a, unsigned long long int const& b, unsigned long long int& res);
template void NumericUtils::GreatestCommonDivisor(long long int const& a, long long int const& b, long long int& res);

template<typename T> void NumericUtils::GreatestCommonDivisor(std::vector<T> const& nums, T& res){
  if (nums.empty()){
    res = 0;
    return;
  }
  else if (nums.size()==1){
    res = nums.front();
    return;
  }
  NumericUtils::GreatestCommonDivisor(std::vector<T>(nums.begin()+1, nums.end()), res);
  NumericUtils::GreatestCommonDivisor(nums.front(), res, res);
}
template void NumericUtils::GreatestCommonDivisor(std::vector<unsigned char> const& nums, unsigned char& res);
template void NumericUtils::GreatestCommonDivisor(std::vector<char> const& nums, char& res);
template void NumericUtils::GreatestCommonDivisor(std::vector<unsigned short> const& nums, unsigned short& res);
template void NumericUtils::GreatestCommonDivisor(std::vector<short> const& nums, short& res);
template void NumericUtils::GreatestCommonDivisor(std::vector<unsigned int> const& nums, unsigned int& res);
template void NumericUtils::GreatestCommonDivisor(std::vector<int> const& nums, int& res);
template void NumericUtils::GreatestCommonDivisor(std::vector<unsigned long long int> const& nums, unsigned long long int& res);
template void NumericUtils::GreatestCommonDivisor(std::vector<long long int> const& nums, long long int& res);

template<typename T> void NumericUtils::LeastCommonMultiple(T const& a, T const& b, T& res){
  T gcd = 0;
  GreatestCommonDivisor(a, b, gcd);
  res = a*b/gcd;
}
template void NumericUtils::LeastCommonMultiple(unsigned char const& a, unsigned char const& b, unsigned char& res);
template void NumericUtils::LeastCommonMultiple(char const& a, char const& b, char& res);
template void NumericUtils::LeastCommonMultiple(unsigned short const& a, unsigned short const& b, unsigned short& res);
template void NumericUtils::LeastCommonMultiple(short const& a, short const& b, short& res);
template void NumericUtils::LeastCommonMultiple(unsigned int const& a, unsigned int const& b, unsigned int& res);
template void NumericUtils::LeastCommonMultiple(int const& a, int const& b, int& res);
template void NumericUtils::LeastCommonMultiple(unsigned long long int const& a, unsigned long long int const& b, unsigned long long int& res);
template void NumericUtils::LeastCommonMultiple(long long int const& a, long long int const& b, long long int& res);

template<typename T> void NumericUtils::LeastCommonMultiple(std::vector<T> const& nums, T& res){
  if (nums.empty()){
    res = 0;
    return;
  }
  else if (nums.size()==1){
    res = nums.front();
    return;
  }
  NumericUtils::LeastCommonMultiple(std::vector<T>(nums.begin()+1, nums.end()), res);
  NumericUtils::LeastCommonMultiple(nums.front(), res, res);
}
template void NumericUtils::LeastCommonMultiple(std::vector<unsigned char> const& nums, unsigned char& res);
template void NumericUtils::LeastCommonMultiple(std::vector<char> const& nums, char& res);
template void NumericUtils::LeastCommonMultiple(std::vector<unsigned short> const& nums, unsigned short& res);
template void NumericUtils::LeastCommonMultiple(std::vector<short> const& nums, short& res);
template void NumericUtils::LeastCommonMultiple(std::vector<unsigned int> const& nums, unsigned int& res);
template void NumericUtils::LeastCommonMultiple(std::vector<int> const& nums, int& res);
template void NumericUtils::LeastCommonMultiple(std::vector<unsigned long long int> const& nums, unsigned long long int& res);
template void NumericUtils::LeastCommonMultiple(std::vector<long long int> const& nums, long long int& res);


#endif
