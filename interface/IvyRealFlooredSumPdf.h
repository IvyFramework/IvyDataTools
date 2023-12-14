#ifndef IVYREALFLOOREDSUMPDF_H
#define IVYREALFLOOREDSUMPDF_H

/*
  Originally developed in the CMS Higgs Combine tool,
  this class is consolidated in this package for future development and use.

  This class is a modified version of the RooRealSumPdf class from the RooFit package.
  The original RooRealSumPdf class is a sum of functions (not necessarily pdfs).
  This modified version is still a sum of functions, but with the option to set a floor.
  The class is otherwise identical to RooRealSumPdf.

  The floor can be set using the call RooRealFlooredSumPdf::setFloor.
*/


#include <list>
#include "IvyROOTFlags.h"
#include "RooAbsArg.h"
#include "RooAbsReal.h"
#include "RooAbsPdf.h"
#include "RooArgList.h"
#include "RooArgSet.h"
#include "RooListProxy.h"
#include "RooAbsCacheElement.h"
#include "RooObjCacheManager.h"

#ifdef _IVY_ROOT_HAS_ITERATORS_
#include "TIterator.h"
#endif


class RooRealFlooredSumPdf : public RooAbsPdf{
public:
  RooRealFlooredSumPdf();
  RooRealFlooredSumPdf(const char* name, const char* title);
  RooRealFlooredSumPdf(const char* name, const char* title, const RooArgList& funcList, const RooArgList& coefList, Bool_t extended = kFALSE);
  RooRealFlooredSumPdf(const RooRealFlooredSumPdf& other, const char* name = 0);
  virtual TObject* clone(const char* newname) const { return new RooRealFlooredSumPdf(*this, newname); }
  virtual ~RooRealFlooredSumPdf();

  Double_t evaluate() const;
  virtual Bool_t checkObservables(const RooArgSet* nset) const;

  virtual Bool_t forceAnalyticalInt(const RooAbsArg&) const{ return kTRUE; }
  Int_t getAnalyticalIntegralWN(RooArgSet& allVars, RooArgSet& numVars, const RooArgSet* normSet, const char* rangeName = 0) const;
  Double_t analyticalIntegralWN(Int_t code, const RooArgSet* normSet, const char* rangeName = 0) const;

  const RooArgList& funcList() const{ return _funcList; }
  const RooArgList& coefList() const{ return _coefList; }

  void setFloor(Double_t val);

  virtual ExtendMode extendMode() const;

  virtual Double_t expectedEvents(const RooArgSet* nset) const;
  virtual Double_t expectedEvents(const RooArgSet& nset) const{
    // Return expected number of events for extended likelihood calculation
    // which is the sum of all coefficients
    return expectedEvents(&nset);
  }

  void printMetaArgs(std::ostream& os) const;


  virtual std::list<Double_t>* binBoundaries(RooAbsRealLValue& /*obs*/, Double_t /*xlo*/, Double_t /*xhi*/) const;
  virtual std::list<Double_t>* plotSamplingHint(RooAbsRealLValue& /*obs*/, Double_t /*xlo*/, Double_t /*xhi*/) const;
  Bool_t isBinnedDistribution(const RooArgSet& obs) const;


protected:

  class CacheElem : public RooAbsCacheElement{
  public:
    CacheElem(){};
    virtual ~CacheElem(){};
    virtual RooArgList containedArgs(Action){ RooArgList ret(_funcIntList); ret.add(_funcNormList); return ret; }
    RooArgList _funcIntList;
    RooArgList _funcNormList;
  };
  mutable RooObjCacheManager _normIntMgr; // The integration cache manager


  Bool_t _haveLastCoef;

  RooListProxy _funcList; //  List of component FUNCs
  RooListProxy _coefList; //  List of coefficients
#ifdef _IVY_ROOT_HAS_ITERATORS_
  TIterator* _funcIter; //! Iterator over FUNC list
  TIterator* _coefIter; //! Iterator over coefficient list
#endif
  Bool_t _extended; // Allow use as extended p.d.f.
  Bool_t _doFloor; // Whether to apply a floor to the sum
  Double_t _floorVal; // The value of the floor

private:

  ClassDef(RooRealFlooredSumPdf, 2) // PDF constructed from a sum of (non-pdf) functions
};


// Necessary typedef to ensure compatibility with other IvyFramework uses
typedef RooRealFlooredSumPdf IvyRealFlooredSumPdf;


#endif
