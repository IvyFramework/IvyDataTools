#ifndef IVYOUTPUTSTREAMER_H
#define IVYOUTPUTSTREAMER_H

#include <ios>
#include <iostream>
#include <istream>
#include <ostream>
#include <fstream>
#include <streambuf>
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>
#include <utility>
#include "TString.h"
#include "TLorentzVector.h"
#include "Math/LorentzVector.h"


class IvyOutputStreamer{
protected:
  std::ofstream theFile;
  std::ostream* stdout_ptr;

public:
  IvyOutputStreamer(const char* fname, std::ios_base::openmode fmode = std::ios_base::out, bool printError=false);
  ~IvyOutputStreamer();

  template<typename T> IvyOutputStreamer& operator<<(T const& val);
  template<typename T> IvyOutputStreamer& operator<<(ROOT::Math::LorentzVector<T> const& val);
  template<typename T, typename U> IvyOutputStreamer& operator<<(std::pair<T, U> const& val);
  template<typename T> IvyOutputStreamer& operator<<(std::vector<T> const& val);
  IvyOutputStreamer& operator<<(std::ostream& (*fcn)(std::ostream&));
  IvyOutputStreamer& operator<<(std::ios& (*fcn)(std::ios&));
  IvyOutputStreamer& operator<<(std::ios_base& (*fcn)(std::ios_base&));

  std::streamsize width() const;
  std::streamsize width(std::streamsize wide);

  char fill() const;
  char fill(char fillch);

  std::ofstream& getFileStream(){ return this->theFile; }
  std::ofstream const& getFileStream() const{ return this->theFile; }
  std::ostream* const& getStdOutPtr() const{ return this->stdout_ptr; }

  void open(const char* fname, std::ios_base::openmode fmode = std::ios_base::out);
  void close();

  template<typename T> void writeCentered(const T& val, char fillch=' ', std::streamsize gapsize=0);

};

namespace IvyStreamHelpers{
  template<typename T> void print_generic_output(IvyOutputStreamer& ivy_os, T const& val){
    auto& theFile = ivy_os.getFileStream();
    auto& stdout_ptr = ivy_os.getStdOutPtr();
    theFile << val;
    if (stdout_ptr) *stdout_ptr << val;
  }
}

template<typename T> IvyOutputStreamer& IvyOutputStreamer::operator<<(T const& val){
  IvyStreamHelpers::print_generic_output(*this, val);
  return *this;
}
template IvyOutputStreamer& IvyOutputStreamer::operator<< <bool>(bool const& val);
template IvyOutputStreamer& IvyOutputStreamer::operator<< <unsigned short>(unsigned short const& val);
template IvyOutputStreamer& IvyOutputStreamer::operator<< <short>(short const& val);
template IvyOutputStreamer& IvyOutputStreamer::operator<< <unsigned int>(unsigned int const& val);
template IvyOutputStreamer& IvyOutputStreamer::operator<< <int>(int const& val);
template IvyOutputStreamer& IvyOutputStreamer::operator<< <unsigned long>(unsigned long const& val);
template IvyOutputStreamer& IvyOutputStreamer::operator<< <long>(long const& val);
template IvyOutputStreamer& IvyOutputStreamer::operator<< <unsigned long long>(unsigned long long const& val);
template IvyOutputStreamer& IvyOutputStreamer::operator<< <long long>(long long const& val);
template IvyOutputStreamer& IvyOutputStreamer::operator<< <float>(float const& val);
template IvyOutputStreamer& IvyOutputStreamer::operator<< <double>(double const& val);
template IvyOutputStreamer& IvyOutputStreamer::operator<< <long double>(long double const& val);

template IvyOutputStreamer& IvyOutputStreamer::operator<< <char*>(char* const& val);
template IvyOutputStreamer& IvyOutputStreamer::operator<< <char const*>(char const* const& val);
template IvyOutputStreamer& IvyOutputStreamer::operator<< <char>(char const& val);
template IvyOutputStreamer& IvyOutputStreamer::operator<< <signed char*>(signed char* const& val);
template IvyOutputStreamer& IvyOutputStreamer::operator<< <signed char const*>(signed char const* const& val);
template IvyOutputStreamer& IvyOutputStreamer::operator<< <signed char>(signed char const& val);
template IvyOutputStreamer& IvyOutputStreamer::operator<< <unsigned char*>(unsigned char* const& val);
template IvyOutputStreamer& IvyOutputStreamer::operator<< <unsigned char const*>(unsigned char const* const& val);
template IvyOutputStreamer& IvyOutputStreamer::operator<< <unsigned char>(unsigned char const& val);

template<> IvyOutputStreamer& IvyOutputStreamer::operator<< <TLorentzVector>(TLorentzVector const& val);

template IvyOutputStreamer& IvyOutputStreamer::operator<< <std::string>(std::string const& val);
template IvyOutputStreamer& IvyOutputStreamer::operator<< <TString>(TString const& val);
template IvyOutputStreamer& IvyOutputStreamer::operator<< <std::streambuf*>(std::streambuf* const& val);
template IvyOutputStreamer& IvyOutputStreamer::operator<< <void*>(void* const& val);

template<typename T> IvyOutputStreamer& IvyOutputStreamer::operator<<(ROOT::Math::LorentzVector<T> const& val){
  *this << "[ " << val.X() << ", " << val.Y() << ", " << val.Z() << ", " << val.T() << ", " << val.M() << " ]";
  return *this;
}

template<typename T, typename U> IvyOutputStreamer& IvyOutputStreamer::operator<<(std::pair<T, U> const& val){
  *this << "{ " << val.first << ", " << val.second << " }";
  return *this;
}

template<typename T> IvyOutputStreamer& IvyOutputStreamer::operator<<(std::vector<T> const& val){
  *this << "{ ";
  for (typename std::vector<T>::const_iterator it=val.cbegin(); it!=val.cend(); it++){
    *this << *it;
    if (it!=val.cend()-1) *this << ", ";
  }
  *this << " }";
  return *this;
}
template IvyOutputStreamer& IvyOutputStreamer::operator<< <bool>(std::vector<bool> const& val);
template IvyOutputStreamer& IvyOutputStreamer::operator<< <unsigned short>(std::vector<unsigned short> const& val);
template IvyOutputStreamer& IvyOutputStreamer::operator<< <short>(std::vector<short> const& val);
template IvyOutputStreamer& IvyOutputStreamer::operator<< <unsigned int>(std::vector<unsigned int> const& val);
template IvyOutputStreamer& IvyOutputStreamer::operator<< <int>(std::vector<int> const& val);
template IvyOutputStreamer& IvyOutputStreamer::operator<< <unsigned long>(std::vector<unsigned long> const& val);
template IvyOutputStreamer& IvyOutputStreamer::operator<< <long>(std::vector<long> const& val);
template IvyOutputStreamer& IvyOutputStreamer::operator<< <unsigned long long>(std::vector<unsigned long long> const& val);
template IvyOutputStreamer& IvyOutputStreamer::operator<< <long long>(std::vector<long long> const& val);
template IvyOutputStreamer& IvyOutputStreamer::operator<< <float>(std::vector<float> const& val);
template IvyOutputStreamer& IvyOutputStreamer::operator<< <double>(std::vector<double> const& val);
template IvyOutputStreamer& IvyOutputStreamer::operator<< <long double>(std::vector<long double> const& val);

template IvyOutputStreamer& IvyOutputStreamer::operator<< <char*>(std::vector<char*> const& val);
template IvyOutputStreamer& IvyOutputStreamer::operator<< <char const*>(std::vector<char const*> const& val);
template IvyOutputStreamer& IvyOutputStreamer::operator<< <char>(std::vector<char> const& val);
template IvyOutputStreamer& IvyOutputStreamer::operator<< <signed char*>(std::vector<signed char*> const& val);
template IvyOutputStreamer& IvyOutputStreamer::operator<< <signed char const*>(std::vector<signed char const*> const& val);
template IvyOutputStreamer& IvyOutputStreamer::operator<< <signed char>(std::vector<signed char> const& val);
template IvyOutputStreamer& IvyOutputStreamer::operator<< <unsigned char*>(std::vector<unsigned char*> const& val);
template IvyOutputStreamer& IvyOutputStreamer::operator<< <unsigned char const*>(std::vector<unsigned char const*> const& val);
template IvyOutputStreamer& IvyOutputStreamer::operator<< <unsigned char>(std::vector<unsigned char> const& val);

template IvyOutputStreamer& IvyOutputStreamer::operator<< <std::string>(std::vector<std::string> const& val);
template IvyOutputStreamer& IvyOutputStreamer::operator<< <TString>(std::vector<TString> const& val);


template<typename T> void IvyOutputStreamer::writeCentered(const T& val, char fillch, std::streamsize gapsize){
  char deffillch = this->fill(fillch);

  std::stringstream tmpss;
  tmpss << val;
  std::string tmpstr = tmpss.str();
  std::streamsize strlength = (std::streamsize) tmpstr.length();

  if (strlength>gapsize) *this << std::setw(gapsize) << "";
  else{
    std::streamsize leftgap = (gapsize+strlength)/2;
    std::streamsize rightgap = gapsize-leftgap;
    *this << std::setw(leftgap) << tmpstr << std::setw(rightgap) << "";
  }

  this->fill(deffillch);
}
template void IvyOutputStreamer::writeCentered<bool>(const bool& val, char fillch, std::streamsize gapsize);
template void IvyOutputStreamer::writeCentered<unsigned short>(const unsigned short& val, char fillch, std::streamsize gapsize);
template void IvyOutputStreamer::writeCentered<short>(const short& val, char fillch, std::streamsize gapsize);
template void IvyOutputStreamer::writeCentered<unsigned int>(const unsigned int& val, char fillch, std::streamsize gapsize);
template void IvyOutputStreamer::writeCentered<int>(const int& val, char fillch, std::streamsize gapsize);
template void IvyOutputStreamer::writeCentered<unsigned long>(const unsigned long& val, char fillch, std::streamsize gapsize);
template void IvyOutputStreamer::writeCentered<long>(const long& val, char fillch, std::streamsize gapsize);
template void IvyOutputStreamer::writeCentered<unsigned long long>(const unsigned long long& val, char fillch, std::streamsize gapsize);
template void IvyOutputStreamer::writeCentered<long long>(const long long& val, char fillch, std::streamsize gapsize);
template void IvyOutputStreamer::writeCentered<float>(const float& val, char fillch, std::streamsize gapsize);
template void IvyOutputStreamer::writeCentered<double>(const double& val, char fillch, std::streamsize gapsize);
template void IvyOutputStreamer::writeCentered<long double>(const long double& val, char fillch, std::streamsize gapsize);

template void IvyOutputStreamer::writeCentered<char*>(char* const& val, char fillch, std::streamsize gapsize);
template void IvyOutputStreamer::writeCentered<char const*>(char const* const& val, char fillch, std::streamsize gapsize);
template void IvyOutputStreamer::writeCentered<char>(char const& val, char fillch, std::streamsize gapsize);
template void IvyOutputStreamer::writeCentered<signed char*>(signed char* const& val, char fillch, std::streamsize gapsize);
template void IvyOutputStreamer::writeCentered<signed char const*>(signed char const* const& val, char fillch, std::streamsize gapsize);
template void IvyOutputStreamer::writeCentered<signed char>(signed char const& val, char fillch, std::streamsize gapsize);
template void IvyOutputStreamer::writeCentered<unsigned char*>(unsigned char* const& val, char fillch, std::streamsize gapsize);
template void IvyOutputStreamer::writeCentered<unsigned char const*>(unsigned char const* const& val, char fillch, std::streamsize gapsize);
template void IvyOutputStreamer::writeCentered<unsigned char>(unsigned char const& val, char fillch, std::streamsize gapsize);

template void IvyOutputStreamer::writeCentered<std::string>(std::string const& val, char fillch, std::streamsize gapsize);
template void IvyOutputStreamer::writeCentered<TString>(TString const& val, char fillch, std::streamsize gapsize);



#endif
