#include <utility>
#include "ExtendedBinning.h"
#include "HelperFunctions.h"


ExtendedBinning::ExtendedBinning(const TString name_, const TString label_) :
  BaseEmptyClass(),
  name(name_),
  label(label_),
  hasAbsoluteLowerBound(false),
  hasAbsoluteUpperBound(false)
{
  adjustNameLabel();
}
ExtendedBinning::ExtendedBinning(const unsigned int nbins, const double min, const double max, const TString name_, const TString label_) :
  BaseEmptyClass(),
  name(name_),
  label(label_),
  hasAbsoluteLowerBound(false),
  hasAbsoluteUpperBound(false)
{
  adjustNameLabel();

  if (max>min && nbins>0){
    const double inc = (max-min)/(static_cast<double>(nbins));
    vbinlow.reserve(nbins+1);
    for (unsigned int i=0; i<=nbins; i++) vbinlow.push_back(min+inc*static_cast<double>(i));
  }
}
ExtendedBinning::ExtendedBinning(const std::vector<double>& vbinlow_, const TString name_, const TString label_) :
  BaseEmptyClass(),
  vbinlow(vbinlow_),
  name(name_),
  label(label_),
  hasAbsoluteLowerBound(false),
  hasAbsoluteUpperBound(false)
{
  adjustNameLabel();
}
ExtendedBinning::ExtendedBinning(const std::vector<TString>& binlabels_, const TString name_, const TString label_) :
  BaseEmptyClass(),
  binlabels(binlabels_),
  name(name_),
  label(label_),
  hasAbsoluteLowerBound(false),
  hasAbsoluteUpperBound(false)
{
  if (!binlabels.empty()){
    unsigned int n = binlabels.size()+1;
    vbinlow.reserve(n);
    for (unsigned int i=0; i<n; i++) vbinlow.push_back(i);
  }
  adjustNameLabel();
}
ExtendedBinning::ExtendedBinning(ExtendedBinning const& other) :
  BaseEmptyClass(other),
  vbinlow(other.vbinlow),
  binlabels(other.binlabels),
  name(other.name),
  label(other.label),
  hasAbsoluteLowerBound(other.hasAbsoluteLowerBound),
  hasAbsoluteUpperBound(other.hasAbsoluteUpperBound)
{}
ExtendedBinning::ExtendedBinning(ExtendedBinning&& other) :
  BaseEmptyClass(other),
  vbinlow(std::move(other.vbinlow)),
  binlabels(std::move(other.binlabels)),
  name(std::move(other.name)),
  label(std::move(other.label)),
  hasAbsoluteLowerBound(std::move(other.hasAbsoluteLowerBound)),
  hasAbsoluteUpperBound(std::move(other.hasAbsoluteUpperBound))
{
  other.hasAbsoluteLowerBound = other.hasAbsoluteUpperBound = false;
}

void ExtendedBinning::swap(ExtendedBinning& other){
  std::swap(vbinlow, other.vbinlow);
  std::swap(binlabels, other.binlabels);
  std::swap(name, other.name);
  std::swap(label, other.label);
  std::swap(hasAbsoluteLowerBound, other.hasAbsoluteLowerBound);
  std::swap(hasAbsoluteUpperBound, other.hasAbsoluteUpperBound);
}
ExtendedBinning& ExtendedBinning::operator=(ExtendedBinning const& other){
  ExtendedBinning tmp(other);
  this->swap(tmp);
  return *this;
}
ExtendedBinning& ExtendedBinning::operator=(ExtendedBinning&& other){
  vbinlow = std::move(other.vbinlow);
  binlabels = std::move(other.binlabels);
  name = std::move(other.name);
  label = std::move(other.label);
  hasAbsoluteLowerBound = std::move(other.hasAbsoluteLowerBound);
  hasAbsoluteUpperBound = std::move(other.hasAbsoluteUpperBound);

  other.hasAbsoluteLowerBound = other.hasAbsoluteUpperBound = false;

  return *this;
}

void ExtendedBinning::adjustNameLabel(){ if (this->label=="") this->label = this->name; }

bool ExtendedBinning::isValid() const{ return (vbinlow.size()>1); }

void ExtendedBinning::setName(const TString name_){ name=name_; }
TString ExtendedBinning::getName() const{ return name; }

void ExtendedBinning::setLabel(const TString label_){ label=label_; }
TString ExtendedBinning::getLabel() const{ return label; }

void ExtendedBinning::setBinLabels(std::vector<TString> const& labels){ binlabels = labels; }

double* ExtendedBinning::getBinning(){ return vbinlow.data(); }
const double* ExtendedBinning::getBinning() const{ return vbinlow.data(); }
std::vector<TString> const& ExtendedBinning::getBinLabels() const{ return binlabels; }
bool ExtendedBinning::hasBinLabels() const{ return !binlabels.empty(); }
std::vector<double> const& ExtendedBinning::getBinningVector() const{ return vbinlow; }

unsigned int ExtendedBinning::getNbins() const{
  return (this->isValid() ? vbinlow.size()-1 : 0);
}

int ExtendedBinning::getBin(double val) const{
  if (!this->isValid()) return -1;
  for (int bin=0; bin<(int)(vbinlow.size()-1); bin++){
    if (vbinlow.at(bin)<=val && val<vbinlow.at(bin+1)) return bin;
  }
  if (val>=vbinlow.back()) return this->getNbins();
  else return -1;
}
int ExtendedBinning::getBin(TString binlabel) const{
  if (!this->isValid()) return -1;
  int bin = 0;
  for (auto const& bl:binlabels){
    if (binlabel == bl) return bin;
    bin++;
  }
  return -1;
}
double ExtendedBinning::getBinLowEdge(const int bin) const{
  if (bin>=0 && bin<(int) vbinlow.size()) return vbinlow.at(bin);
  else if (bin<0 && vbinlow.size()>0) return vbinlow.at(0);
  else if (vbinlow.size()>0) return vbinlow.at(vbinlow.size()-1);
  else return -1;
}
double ExtendedBinning::getBinHighEdge(const int bin) const{
  if (bin>=-1 && bin<(int)vbinlow.size()-1) return vbinlow.at(bin+1);
  else if (bin<-1 && vbinlow.size()>0) return vbinlow.at(0);
  else if (vbinlow.size()>0) return vbinlow.at(vbinlow.size()-1);
  else return -1;
}
double ExtendedBinning::getBinCenter(const int bin) const{ return (getBinLowEdge(bin)+getBinHighEdge(bin))/2.; }
double ExtendedBinning::getBinWidth(const int bin) const{
  if (bin>=0 && bin<(int) vbinlow.size()) return (getBinHighEdge(bin)-getBinLowEdge(bin));
  else return 1;
}

double ExtendedBinning::getMin() const{ unsigned int nbins = getNbins(); return (nbins>0 ? getBinLowEdge(0) : 0); }
double ExtendedBinning::getMax() const{ unsigned int nbins = getNbins(); return (nbins>0 ? getBinLowEdge(nbins) : 0); }

void ExtendedBinning::addBinBoundary(double boundary){
  HelperFunctions::addByLowest<double>(vbinlow, boundary, true);
}
void ExtendedBinning::setBinBoundary(const int bin, double boundary){
  if (bin>=0 && bin<(int) vbinlow.size()) vbinlow.at(bin) = boundary;
}
void ExtendedBinning::removeBinLowEdge(const int bin){
  if (bin>=0 && bin<(int) vbinlow.size()) vbinlow.erase(vbinlow.begin()+bin);
}

void ExtendedBinning::setAbsoluteBoundFlags(bool flag_lower, bool flag_upper){ hasAbsoluteLowerBound=flag_lower; hasAbsoluteUpperBound=flag_upper; }

ExtendedBinning ExtendedBinning::extractBinning(TH1 const* histo, unsigned int const direction){
  ExtendedBinning res;
  if (!histo) return res;
  TAxis const* axis;
  switch (direction){
  case 0:
    axis=histo->GetXaxis();
    break;
  case 1:
    axis=histo->GetYaxis();
    break;
  case 2:
    axis=histo->GetZaxis();
    break;
  default:
    return res;
  }
  res.setLabel(axis->GetTitle());
  const int nbins=axis->GetNbins();
  std::vector<TString> binlabels;
  for (int i=0; i<=nbins; i++) res.addBinBoundary(axis->GetBinLowEdge(i+1));

  return res;
}
