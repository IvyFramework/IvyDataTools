#include <iostream>
#include "RooFit.h"
#include "Riostream.h" 
#include "RooAbsReal.h" 
#include "RooAbsCategory.h" 
#include "RooMsgService.h"
#include "RooNameReg.h"
#include <cmath>
#include <memory>
#include <algorithm>
#include "IvyRealFlooredSumPdf.h" 


using namespace RooFit;


// Default constructor
RooRealFlooredSumPdf::RooRealFlooredSumPdf() :
  _extended(kFALSE),
  _doFloor(kTRUE),
  _floorVal(1e-100)
{
  // coverity[UNINIT_CTOR]
#ifdef _IVY_ROOT_HAS_ITERATORS_
  _funcIter  = _funcList.createIterator();
  _coefIter  = _coefList.createIterator();
#endif
}
RooRealFlooredSumPdf::RooRealFlooredSumPdf(const char* name, const char* title) :
  RooAbsPdf(name, title),
  _normIntMgr(this, 10),
  _haveLastCoef(kFALSE),
  _funcList("!funcList", "List of functions", this),
  _coefList("!coefList", "List of coefficients", this),
  _extended(kFALSE),
  _doFloor(kTRUE),
  _floorVal(1e-100)
{
#ifdef _IVY_ROOT_HAS_ITERATORS_
  _funcIter   = _funcList.createIterator();
  _coefIter  = _coefList.createIterator();
#endif
}

// Constructor pdf implementing sum_i [ coef_i * func_i ], if N_coef==N_func
// or sum_i [ coef_i * func_i ] + (1 - sum_i [ coef_i ] )* func_N if Ncoef==N_func-1
// 
// All coefficients and functions are allowed to be negative, but the sum is not, which is enforced at runtime.
RooRealFlooredSumPdf::RooRealFlooredSumPdf(const char* name, const char* title, const RooArgList& inFuncList, const RooArgList& inCoefList, Bool_t extended) :
  RooAbsPdf(name, title),
  _normIntMgr(this, 10),
  _funcList("!funcList", "List of functions", this),
  _coefList("!coefList", "List of coefficients", this),
  _extended(extended),
  _doFloor(kTRUE),
  _floorVal(1e-100)
{
  {
    int const nCoefs = inCoefList.getSize();
    int const nFuncs = inFuncList.getSize();
    if (!(nFuncs == nCoefs + 1 || nCoefs == nFuncs)){
      coutE(InputArguments) << "RooRealFlooredSumPdf::RooRealFlooredSumPdf(" << GetName()
        << ") number of pdfs and coefficients inconsistent, must have Nfunc=Ncoef or Nfunc=Ncoef+1" << std::endl;
      assert(0);
    }
  }

#ifdef _IVY_ROOT_HAS_ITERATORS_
  _funcIter = _funcList.createIterator();
  _coefIter = _coefList.createIterator();

  // Constructor with N functions and N or N-1 coefs
  TIterator* funcIter = inFuncList.createIterator();
  TIterator* coefIter = inCoefList.createIterator();
  RooAbsArg* func;
  RooAbsArg* coef;

  while ((coef = (RooAbsArg*) coefIter->Next())){
    func = (RooAbsArg*) funcIter->Next();

    if (!dynamic_cast<RooAbsReal*>(coef)){
      coutW(InputArguments) << "RooRealFlooredSumPdf::RooRealFlooredSumPdf(" << GetName() << ") coefficient " << coef->GetName() << " is not of type RooAbsReal, ignored" << std::endl;
      continue;
    }
    if (!dynamic_cast<RooAbsReal*>(func)){
      coutW(InputArguments) << "RooRealFlooredSumPdf::RooRealFlooredSumPdf(" << GetName() << ") func " << func->GetName() << " is not of type RooAbsReal, ignored" << std::endl;
      continue;
    }
    _funcList.add(*func);
    _coefList.add(*coef);
  }

  func = (RooAbsReal*) funcIter->Next();
  if (func){
    if (!dynamic_cast<RooAbsReal*>(func)){
      coutE(InputArguments) << "RooRealFlooredSumPdf::RooRealFlooredSumPdf(" << GetName() << ") last func " << func->GetName() << " is not of type RooAbsReal, fatal error" << std::endl;
      assert(0);
    }
    _funcList.add(*func);
  }

  delete funcIter;
  delete coefIter;
#else
  // Constructor with N functions and N or N-1 coefs
  for (RooAbsArg* coef : inCoefList){
    if (!dynamic_cast<RooAbsReal*>(coef)){
      coutW(InputArguments) << "RooRealFlooredSumPdf::RooRealFlooredSumPdf(" << GetName() << ") coefficient " << coef->GetName() << " is not of type RooAbsReal, ignored" << std::endl;
      continue;
    }
    _coefList.add(*coef);
  }
  for (RooAbsArg* func : inFuncList){
    if (!dynamic_cast<RooAbsReal*>(func)){
      coutW(InputArguments) << "RooRealFlooredSumPdf::RooRealFlooredSumPdf(" << GetName() << ") func " << func->GetName() << " is not of type RooAbsReal, ignored" << std::endl;
      continue;
    }
    _funcList.add(*func);
  }
#endif

  _haveLastCoef = (_coefList.getSize() == _funcList.getSize());
}

RooRealFlooredSumPdf::RooRealFlooredSumPdf(const RooRealFlooredSumPdf& other, const char* name) :
  RooAbsPdf(other, name),
  _normIntMgr(other._normIntMgr, this),
  _haveLastCoef(other._haveLastCoef),
  _funcList("!funcList", this, other._funcList),
  _coefList("!coefList", this, other._coefList),
  _extended(other._extended),
  _doFloor(other._doFloor),
  _floorVal(other._floorVal)
{
#ifdef _IVY_ROOT_HAS_ITERATORS_
  _funcIter = _funcList.createIterator();
  _coefIter = _coefList.createIterator();
#endif
}

RooRealFlooredSumPdf::~RooRealFlooredSumPdf(){
#ifdef _IVY_ROOT_HAS_ITERATORS_
  delete _funcIter;
  delete _coefIter;
#endif
}

// Set the floor of the pdf
void RooRealFlooredSumPdf::setFloor(Double_t val){
  _floorVal = val;
}

RooAbsPdf::ExtendMode RooRealFlooredSumPdf::extendMode() const{
  return (_extended && (_funcList.getSize() == _coefList.getSize())) ? CanBeExtended : CanNotBeExtended;
}

// Calculate the current value
Double_t RooRealFlooredSumPdf::evaluate() const{
  Double_t value(0);
  Double_t lastCoef(1);

  // Do running sum of coef/func pairs, calculate lastCoef.
#ifdef _IVY_ROOT_HAS_ITERATORS_
  RooFIter funcIter = _funcList.fwdIterator();
  RooFIter coefIter = _coefList.fwdIterator();
  RooAbsReal* coef;
  RooAbsReal* func;

  // N funcs, N-1 coefficients 
  while ((coef = (RooAbsReal*) coefIter.next())){
    func = (RooAbsReal*) funcIter.next();
    Double_t coefVal = coef->getVal();
    if (coefVal){
      cxcoutD(Eval) << "RooRealFlooredSumPdf::eval(" << GetName() << ") coefVal = " << coefVal << " funcVal = " << func->ClassName() << "::" << func->GetName() << " = " << func->getVal() << std::endl;
      value += func->getVal()*coefVal;
      lastCoef -= coef->getVal();
    }
  }

  if (!_haveLastCoef){
    // Add last func with correct coefficient
    func = (RooAbsReal*) funcIter.next();
#else
  for (int iCoef = 0; iCoef < _coefList.getSize(); ++iCoef){
    RooAbsReal* coef = &static_cast<RooAbsReal&>(_coefList[iCoef]);
    RooAbsReal* func = &static_cast<RooAbsReal&>(_funcList[iCoef]);
    Double_t coefVal = coef->getVal();

    if (coefVal){
      cxcoutD(Eval) << "RooRealFlooredSumPdf::eval(" << GetName() << ") coefVal = " << coefVal << " funcVal = " << func->ClassName() << "::" << func->GetName() << " = " << func->getVal() << std::endl;
      value += func->getVal()*coefVal;
      lastCoef -= coef->getVal();
    }
  }
  if (!_haveLastCoef){
    // Add last func with correct coefficient
    RooAbsReal* func = &static_cast<RooAbsReal&>(_funcList[_coefList.getSize()]);
#endif
    value += func->getVal()*lastCoef;

    cxcoutD(Eval) << "RooRealFlooredSumPdf::eval(" << GetName() << ") lastCoef = " << lastCoef << " funcVal = " << func->getVal() << std::endl;

    // Warn about coefficient degeneration
    if (lastCoef<0. || lastCoef>1.){
      coutW(Eval) << "RooRealFlooredSumPdf::evaluate(" << GetName()
        << " WARNING: sum of FUNC coefficients not in range [0-1], value="
        << (1.-lastCoef) << std::endl;
    }
  }

  // Introduce floor
  if (value <= 0. && _doFloor) value = _floorVal;

  return value;
}

/*
  Check if FUNC is valid for given normalization set.
  Coefficient and FUNC must be non-overlapping, but func-coefficient pairs may overlap each other.

  In the present implementation, coefficients may not be observables or derive from observables.
*/
Bool_t RooRealFlooredSumPdf::checkObservables(const RooArgSet* nset) const{
  Bool_t ret(kFALSE);

#ifdef _IVY_ROOT_HAS_ITERATORS_
  _funcIter->Reset();
  _coefIter->Reset();
  RooAbsReal* coef;
  RooAbsReal* func;
  while ((coef = (RooAbsReal*) _coefIter->Next())){
    func = (RooAbsReal*) _funcIter->Next();
#else
  for (int iCoef = 0; iCoef < _coefList.getSize(); ++iCoef){
    RooAbsReal* coef = &static_cast<RooAbsReal&>(_coefList[iCoef]);
    RooAbsReal* func = &static_cast<RooAbsReal&>(_funcList[iCoef]);
#endif
    if (func->observableOverlaps(nset, *coef)){
      coutE(InputArguments) << "RooRealFlooredSumPdf::checkObservables(" << GetName() << "): ERROR: coefficient " << coef->GetName()
        << " and FUNC " << func->GetName() << " have one or more observables in common" << std::endl;
      ret = kTRUE;
    }
    if (coef->dependsOn(*nset)){
      coutE(InputArguments) << "RooRealPdf::checkObservables(" << GetName() << "): ERROR coefficient " << coef->GetName()
        << " depends on one or more of the following observables"; nset->Print("1");
      ret = kTRUE;
    }
  }

  if (!_haveLastCoef){
#ifdef _IVY_ROOT_HAS_ITERATORS_
    func = (RooAbsReal*) _funcIter->Next();
    _coefIter->Reset();
    while ((coef = (RooAbsReal*) _coefIter->Next())){
#else
    RooAbsReal* func = &static_cast<RooAbsReal&>(_funcList[_coefList.getSize()]);
    for (int iCoef = 0; iCoef < _coefList.getSize(); ++iCoef){
      RooAbsReal* coef = &static_cast<RooAbsReal&>(_coefList[iCoef]);
#endif
      if (func->observableOverlaps(nset, *coef)){
        coutE(InputArguments) << "RooRealFlooredSumPdf::checkObservables(" << GetName() << "): ERROR: coefficient " << coef->GetName()
          << " and FUNC " << func->GetName() << " have one or more observables in common" << std::endl;
        ret = kTRUE;
      }
    }
  }

  return ret;
}

// Advertise that all integrals can be handled internally.
Int_t RooRealFlooredSumPdf::getAnalyticalIntegralWN(RooArgSet& allVars, RooArgSet& analVars, const RooArgSet* normSet2, const char* rangeName) const{
  //std::cout << "RooRealFlooredSumPdf::getAnalyticalIntegralWN:"<<GetName()<<"("<<allVars<<",analVars,"<<(normSet2?*normSet2:RooArgSet())<<","<<(rangeName?rangeName:"<none>") << std::endl;

  // Handle trivial no-integration scenario
  if (allVars.getSize() == 0) return 0;
  if (_forceNumInt) return 0;

  // Select subset of allVars that are actual dependents
  analVars.add(allVars);
  RooArgSet* normSet = normSet2 ? getObservables(normSet2) : 0;


  // Check if this configuration was created before
  Int_t sterileIdx(-1);
  CacheElem* cache = (CacheElem*) _normIntMgr.getObj(normSet, &analVars, &sterileIdx, RooNameReg::ptr(rangeName));
  if (cache){
    //std::cout << "RooRealFlooredSumPdf("<<this<<")::getAnalyticalIntegralWN:"<<GetName()<<"("<<allVars<<","<<analVars<<","<<(normSet2?*normSet2:RooArgSet())<<","<<(rangeName?rangeName:"<none>") << " -> " << _normIntMgr.lastIndex()+1 << " (cached)" << std::endl;
    return _normIntMgr.lastIndex() + 1;
  }

  // Create new cache element
  cache = new CacheElem;

  // Make list of function projection and normalization integrals 
#ifdef _IVY_ROOT_HAS_ITERATORS_
  _funcIter->Reset();
  RooAbsReal* func;
  while ((func = (RooAbsReal*) _funcIter->Next())){
#else
  for (RooAbsArg* funcAbsArg : _funcList){
    RooAbsReal* func = static_cast<RooAbsReal*>(funcAbsArg);
#endif
    RooAbsReal* funcInt = func->createIntegral(analVars, rangeName);
    cache->_funcIntList.addOwned(*funcInt);
    if (normSet && normSet->getSize() > 0){
      RooAbsReal* funcNorm = func->createIntegral(*normSet);
      cache->_funcNormList.addOwned(*funcNorm);
    }
  }

  // Store cache element
  Int_t code = _normIntMgr.setObj(normSet, &analVars, (RooAbsCacheElement*) cache, RooNameReg::ptr(rangeName));

  if (normSet) delete normSet;

  //std::cout << "RooRealFlooredSumPdf("<<this<<")::getAnalyticalIntegralWN:"<<GetName()<<"("<<allVars<<","<<analVars<<","<<(normSet2?*normSet2:RooArgSet())<<","<<(rangeName?rangeName:"<none>") << " -> " << code+1 << std::endl;
  return code + 1;
}

// Implement analytical integrations by deferring integration of component functions to integrators of components
Double_t RooRealFlooredSumPdf::analyticalIntegralWN(Int_t code, const RooArgSet* normSet2, const char* rangeName) const
{
  //std::cout << "RooRealFlooredSumPdf::analyticalIntegralWN:"<<GetName()<<"("<<code<<","<<(normSet2?*normSet2:RooArgSet())<<","<<(rangeName?rangeName:"<none>") << std::endl;

  // Handle trivial passthrough scenario
  if (code == 0) return getVal(normSet2);


  // WVE needs adaptation for rangeName feature
  CacheElem* cache = (CacheElem*) _normIntMgr.getObjByIndex(code - 1);
  if (cache == 0){ // revive the (sterilized) cache
    //std::cout << "RooRealFlooredSumPdf("<<this<<")::analyticalIntegralWN:"<<GetName()<<"("<<code<<","<<(normSet2?*normSet2:RooArgSet())<<","<<(rangeName?rangeName:"<none>") << ": reviving cache "<< std::endl;
    std::unique_ptr<RooArgSet> vars(getParameters(RooArgSet()));
    RooArgSet dummy;
#if ROOT_VERSION_CODE < ROOT_VERSION(6,26,0)
    std::unique_ptr<RooArgSet> iset(_normIntMgr.nameSet2ByIndex(code - 1)->select(*vars));
    std::unique_ptr<RooArgSet> nset(_normIntMgr.nameSet1ByIndex(code - 1)->select(*vars));
    Int_t code2 = getAnalyticalIntegralWN(*iset, dummy, nset.get(), rangeName);
#else
    // In ROOT 6.26, the RooNameSet was removed and the "selectFromSet*"
    // functions were introduced to replace its functionality
    RooArgSet iset{ _normIntMgr.selectFromSet2(*vars, code - 1) };
    RooArgSet nset{ _normIntMgr.selectFromSet1(*vars, code - 1) };
    Int_t code2 = getAnalyticalIntegralWN(iset, dummy, &nset, rangeName);
#endif
    assert(code == code2); // must have revived the right (sterilized) slot...
    cache = (CacheElem*) _normIntMgr.getObjByIndex(code - 1);
    assert(cache != 0);
  }

  Double_t value(0);
  Double_t lastCoef(1);
#ifdef _IVY_ROOT_HAS_ITERATORS_
  RooFIter funcIntIter = cache->_funcIntList.fwdIterator();
  RooFIter coefIter = _coefList.fwdIterator();
  RooFIter funcIter = _funcList.fwdIterator();
  RooAbsReal* coef(nullptr), * funcInt(nullptr), * func(nullptr);
  while ((coef = (RooAbsReal*) coefIter.next())){
    funcInt = (RooAbsReal*) funcIntIter.next();
    func = (RooAbsReal*) funcIter.next();
#else
  for (int iCoef = 0; iCoef < _coefList.getSize(); ++iCoef){
    RooAbsReal* coef = &static_cast<RooAbsReal&>(_coefList[iCoef]);
    RooAbsReal* func = &static_cast<RooAbsReal&>(_funcList[iCoef]);
    RooAbsReal* funcInt = &static_cast<RooAbsReal&>(cache->_funcIntList[iCoef]); 
#endif
    Double_t coefVal = coef->getVal(normSet2);
    if (coefVal){
      assert(func);
      assert(funcInt);
      value += funcInt->getVal()*coefVal;
      lastCoef -= coef->getVal(normSet2);
    }
  }

  if (!_haveLastCoef){
    // Add last func with correct coefficient
#ifdef _IVY_ROOT_HAS_ITERATORS_
    funcInt = (RooAbsReal*) funcIntIter.next();
#else
    RooAbsReal* funcInt = &static_cast<RooAbsReal&>(cache->_funcIntList[_coefList.getSize()]);
#endif
    assert(funcInt);
    value += funcInt->getVal()*lastCoef;

    // Warn about coefficient degeneration
    if (lastCoef<0. || lastCoef>1.){
      coutW(Eval) << "RooRealFlooredSumPdf::integral(" << GetName()
        << ") WARNING: Sum of FUNC coefficients not in range [0-1], value="
        << 1. - lastCoef << std::endl;
    }
  }

  Double_t normVal(1);
  if (normSet2 && normSet2->getSize() > 0){
    normVal = 0;

    // N funcs, N-1 coefficients 
#ifdef _IVY_ROOT_HAS_ITERATORS_
    RooAbsReal* funcNorm;
    RooFIter funcNormIter = cache->_funcNormList.fwdIterator();
    RooFIter coefIter2 = _coefList.fwdIterator();
    while ((coef = (RooAbsReal*) coefIter2.next())){
      funcNorm = (RooAbsReal*) funcNormIter.next();
#else
    for (int iCoef = 0; iCoef < _coefList.getSize(); ++iCoef) {
      RooAbsReal* coef = &static_cast<RooAbsReal&>(_coefList[iCoef]);
      RooAbsReal* funcNorm = &static_cast<RooAbsReal&>(cache->_funcNormList[iCoef]);
#endif
      Double_t coefVal = coef->getVal(normSet2);
      if (coefVal){
        assert(funcNorm);
        normVal += funcNorm->getVal()*coefVal;
      }
    }

    // Add last func with correct coefficient
    if (!_haveLastCoef){
#ifdef _IVY_ROOT_HAS_ITERATORS_
      funcNorm = (RooAbsReal*) funcNormIter.next();
#else
      RooAbsReal* funcNorm = &static_cast<RooAbsReal&>(cache->_funcNormList[_coefList.getSize()]);
#endif
      assert(funcNorm);
      normVal += funcNorm->getVal()*lastCoef;
    }
  }

  Double_t result = 0;
  if (normVal>0.) result = value / normVal;
  if (result<=0. && _doFloor){
    coutW(Eval) << "RooRealFlooredSumPdf::integral(" << GetName()
      << ") WARNING: Integral " << result << " below threshold." << std::endl;
    result = _floorVal;
  }
  return result;
}

Double_t RooRealFlooredSumPdf::expectedEvents(const RooArgSet* nset) const{
  Double_t n = getNorm(nset);
  if (n < 0.) logEvalError("Expected number of events is negative");
  return n;
}

std::list<Double_t>* RooRealFlooredSumPdf::binBoundaries(RooAbsRealLValue& obs, Double_t xlo, Double_t xhi) const{
  std::list<Double_t>* sumBinB = 0;
  Bool_t needClean(kFALSE);

  RooFIter iter = _funcList.fwdIterator();
  RooAbsReal* func;
  // Loop over components pdf
#ifdef _IVY_ROOT_HAS_ITERATORS_
  while ((func = (RooAbsReal*) iter.next())){
#else
  for (RooAbsArg* funcAbsArg : _funcList){
    RooAbsReal* func = static_cast<RooAbsReal*>(funcAbsArg);
#endif
    std::list<Double_t>* funcBinB = func->binBoundaries(obs, xlo, xhi);
    // Process hint
    if (funcBinB){
      if (!sumBinB){
        // If this is the first hint, then just save it
        sumBinB = funcBinB;
      }
      else{
        std::list<Double_t>* newSumBinB = new std::list<Double_t>(sumBinB->size() + funcBinB->size());

        // Merge hints into temporary array
        merge(funcBinB->begin(), funcBinB->end(), sumBinB->begin(), sumBinB->end(), newSumBinB->begin());

        // Copy merged array without duplicates to new sumBinBArrau
        delete sumBinB;
        delete funcBinB;
        sumBinB = newSumBinB;
        needClean = kTRUE;
      }
    }
  }

  // Remove consecutive duplicates
  if (needClean){
    std::list<Double_t>::iterator new_end = std::unique(sumBinB->begin(), sumBinB->end());
    sumBinB->erase(new_end, sumBinB->end());
  }

  return sumBinB;
}

Bool_t RooRealFlooredSumPdf::isBinnedDistribution(const RooArgSet& obs) const{
  // If all components that depend on obs are binned that so is the product
#ifdef _IVY_ROOT_HAS_ITERATORS_
  RooFIter iter = _funcList.fwdIterator();
  RooAbsReal* func;
  while ((func = (RooAbsReal*) iter.next())){
#else
  for (RooAbsArg* funcAbsArg : _funcList) {
    RooAbsReal* func = static_cast<RooAbsReal*>(funcAbsArg);
#endif
    if (func->dependsOn(obs) && !func->isBinnedDistribution(obs)) return kFALSE;
  }
  return kTRUE;
}

std::list<Double_t>* RooRealFlooredSumPdf::plotSamplingHint(RooAbsRealLValue& obs, Double_t xlo, Double_t xhi) const{
  std::list<Double_t>* sumHint = 0;
  Bool_t needClean(kFALSE);

#ifdef _IVY_ROOT_HAS_ITERATORS_
  RooFIter iter = _funcList.fwdIterator();
  RooAbsReal* func;
  // Loop over components pdf
  while ((func = (RooAbsReal*) iter.next())){
#else
  for (RooAbsArg* funcAbsArg : _funcList) {
    RooAbsReal* func = static_cast<RooAbsReal*>(funcAbsArg);
#endif
    std::list<Double_t>* funcHint = func->plotSamplingHint(obs, xlo, xhi);

    // Process hint
    if (funcHint){
      if (!sumHint){
        // If this is the first hint, then just save it
        sumHint = funcHint;
      }
      else{
        std::list<Double_t>* newSumHint = new std::list<Double_t>(sumHint->size() + funcHint->size());

        // Merge hints into temporary array
        merge(funcHint->begin(), funcHint->end(), sumHint->begin(), sumHint->end(), newSumHint->begin());

        // Copy merged array without duplicates to new sumHintArrau
        delete sumHint;
        sumHint = newSumHint;
        needClean = kTRUE;
      }
    }
  }

  // Remove consecutive duplicates
  if (needClean){
    std::list<Double_t>::iterator new_end = unique(sumHint->begin(), sumHint->end());
    sumHint->erase(new_end, sumHint->end());
  }

  return sumHint;
}

// Customized printing of arguments of a RooRealFlooredSumPdf to more intuitively reflect the contents of the product operator construction
void RooRealFlooredSumPdf::printMetaArgs(std::ostream& os) const{
  Bool_t first(kTRUE);

#ifdef _IVY_ROOT_HAS_ITERATORS_
  _funcIter->Reset();
  _coefIter->Reset();
  RooAbsArg* coef, * func;
#endif
  if (_coefList.getSize() != 0){
#ifdef _IVY_ROOT_HAS_ITERATORS_
    while ((coef = (RooAbsArg*) _coefIter->Next())){
      func = (RooAbsArg*) _funcIter->Next();
#else
    for (int iCoef = 0; iCoef < _coefList.getSize(); ++iCoef){
      RooAbsArg* coef = &_coefList[iCoef];
      RooAbsArg* func = &_funcList[iCoef];
#endif
      if (!first)  os << " + ";
      else first = kFALSE;
      os << coef->GetName() << " * " << func->GetName();
    }

    if (!_haveLastCoef){
#ifdef _IVY_ROOT_HAS_ITERATORS_
      func = (RooAbsArg*) _funcIter->Next();
#else
      RooAbsArg* func = &_funcList[_coefList.getSize()];
#endif
      if (func) os << " + [%] * " << func->GetName();
    }
  }
  else{
#ifdef _IVY_ROOT_HAS_ITERATORS_
    while ((func = (RooAbsArg*) _funcIter->Next())){
#else
    for (RooAbsArg* func : _funcList){
#endif
      if (!first) os << " + ";
      else first = kFALSE;
      os << func->GetName();
    }
  }

  os << " ";
}


ClassImp(RooRealFlooredSumPdf)
