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

}

#endif

