#ifndef EXTENDEDBINNING_H
#define EXTENDEDBINNING_H

#include <vector>
#include <utility>
#include "TString.h"
#include "TH1.h"
#include "BaseEmptyClass.h"


class ExtendedBinning : public BaseEmptyClass{
protected:
  std::vector<double> vbinlow; // Size=Nbins+1
  std::vector<TString> binlabels;
  TString name;
  TString label;

  // These two flags are introduced in order to tell a histogram smoothener that
  // the lower or upper bounds should not be treated as flexible.
  bool hasAbsoluteLowerBound;
  bool hasAbsoluteUpperBound;

  void adjustNameLabel();

public:
  ExtendedBinning(const TString name_="", const TString label_="");
  ExtendedBinning(const unsigned int nbins, const double min, const double max, const TString name="", const TString label_=""); // Uniform constructor
  ExtendedBinning(const std::vector<double>& vbinlow_, const TString name="", const TString label_="");
  ExtendedBinning(const std::vector<TString>& binlabels_, const TString name="", const TString label_="");
  ExtendedBinning(ExtendedBinning const& other);
  ExtendedBinning(ExtendedBinning&& other);

  void swap(ExtendedBinning& other);
  ExtendedBinning& operator=(ExtendedBinning const& other);
  ExtendedBinning& operator=(ExtendedBinning&& other);

  bool isValid() const;

  void setName(const TString name_);
  TString getName() const;

  void setLabel(const TString label_);
  TString getLabel() const;

  void setBinLabels(std::vector<TString> const& labels);

  double* getBinning();
  const double* getBinning() const;
  std::vector<double> const& getBinningVector() const;
  template<typename T> std::vector<std::pair<T, T>> getBoundaryPairsList() const;
  bool hasBinLabels() const;
  std::vector<TString> const& getBinLabels() const;
  unsigned int getNbins() const;

  int getBin(double val) const; // = [ -1,0,...,vbinlow.size() ]
  int getBin(TString binlabel) const; // = [ -1,0,...,vbinlow.size()-1 ]
  double getBinLowEdge(const int bin) const;
  double getBinHighEdge(const int bin) const;
  double getBinCenter(const int bin) const;
  double getBinWidth(const int bin) const;

  double getMin() const;
  double getMax() const;

  void addBinBoundary(double boundary);
  void setBinBoundary(const int bin, double boundary);
  void removeBinLowEdge(const int bin);
  void resetBinning(){ vbinlow.clear(); }

  void setAbsoluteBoundFlags(bool flag_lower, bool flag_upper);
  bool const& checkAbsoluteLowerBound() const{ return hasAbsoluteLowerBound; }
  bool const& checkAbsoluteUpperBound() const{ return hasAbsoluteUpperBound; }

  // Static functions
  static ExtendedBinning extractBinning(TH1 const* histo, unsigned int const direction);

};

template<typename T> std::vector<std::pair<T, T>> ExtendedBinning::getBoundaryPairsList() const{
  std::vector<std::pair<T, T>> res;
  if (vbinlow.size()>1) res.reserve(vbinlow.size()-1);
  for (auto it=vbinlow.begin(); it!=vbinlow.end()-1; it++){
    auto it_next = it+1;
    res.push_back(std::pair<T, T>(T(*it), T(*it_next)));
  }
  return res;
}
template std::vector<std::pair<float, float>> ExtendedBinning::getBoundaryPairsList<float>() const;
template std::vector<std::pair<double, double>> ExtendedBinning::getBoundaryPairsList<double>() const;


#endif

