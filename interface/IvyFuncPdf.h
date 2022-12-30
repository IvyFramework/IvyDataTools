#ifndef IVYFUNCPDF_H
#define IVYFUNCPDF_H

#include "RooRealProxy.h"
#include "RooAbsPdf.h"


class IvyFuncPdf : public RooAbsPdf{
protected:
  RooRealProxy theFunc;

public:
  IvyFuncPdf() : RooAbsPdf(){}
  IvyFuncPdf(
    const char* name,
    const char* title
    ) : RooAbsPdf(name, title), theFunc("theFunc","theFunc",this){}
  IvyFuncPdf(
    const char* name,
    const char* title,
    RooAbsReal& inFunc
    ) : RooAbsPdf(name, title), theFunc("theFunc", "theFunc", this, inFunc){}
  IvyFuncPdf(const IvyFuncPdf& other, const char* name=0) : RooAbsPdf(other, name), theFunc("theFunc", this, other.theFunc){}
  TObject* clone(const char* newname) const{ return new IvyFuncPdf(*this, newname); }
  inline virtual ~IvyFuncPdf(){}

  Int_t getAnalyticalIntegral(RooArgSet& allVars, RooArgSet& analVars, const char* rangeName=0) const{ return dynamic_cast<RooAbsReal*>(theFunc.absArg())->getAnalyticalIntegral(allVars, analVars, rangeName); }
  Double_t analyticalIntegral(Int_t code, const char* rangeName=0) const{ return dynamic_cast<RooAbsReal*>(theFunc.absArg())->analyticalIntegral(code, rangeName); }

  Int_t getAnalyticalIntegralWN(RooArgSet& allVars, RooArgSet& analVars, const RooArgSet *normSet, const char* rangeName=0) const{ return dynamic_cast<RooAbsReal*>(theFunc.absArg())->getAnalyticalIntegralWN(allVars, analVars, normSet, rangeName); }
  Double_t analyticalIntegralWN(Int_t code, const RooArgSet* normSet, const char* rangeName=0) const{
    RooAbsPdf* pdfcast = dynamic_cast<RooAbsPdf*>(theFunc.absArg());
    if (pdfcast!=0) return pdfcast->analyticalIntegralWN(code, normSet, rangeName);
    else return RooAbsPdf::analyticalIntegralWN(code, normSet, rangeName);
  }

protected:
  Double_t evaluate() const{ return theFunc; }


  ClassDef(IvyFuncPdf, 1)

};

ClassImp(IvyFuncPdf)

#endif
