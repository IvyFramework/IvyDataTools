#ifndef IVYASYMQUAD_H
#define IVYASYMQUAD_H

#include "RooFit.h"
#include "Riostream.h"
#include "TIterator.h"
#include "TList.h"
#include <RooAbsReal.h>
#include "RooRealVar.h"
#include <RooRealProxy.h>
#include "RooListProxy.h"
#include "RooMsgService.h"


/*
IvyAsymQuad is helper class for implementing asymmetric additive interpolation.
Consolidated from AsymQuad in the CMS Higgs Combine tool.
*/
class IvyAsymQuad : public RooAbsReal{

public:
  IvyAsymQuad();
  IvyAsymQuad(const char* name, const char* title, const RooArgList& inFuncList, const RooArgList& inCoefList, Double_t smoothRegion=1., Int_t smoothAlgo=0);
  IvyAsymQuad(const IvyAsymQuad& other, const char* name=0);
  ~IvyAsymQuad();

  TObject* clone(const char* newname) const{ return new IvyAsymQuad(*this, newname); }

protected:
  Double_t evaluate() const;

  RooListProxy _funcList;   //  List of component functions
  RooListProxy _coefList;  //  List of coefficients
  Double_t smoothRegion_;
  Int_t smoothAlgo_;
  TIterator* _funcIter;     //! Iterator over FUNC list
  TIterator* _coefIter;    //! Iterator over coefficient list

private:
  Double_t interpolate(Double_t theta_, Double_t valueCenter_, Double_t valueHigh_, Double_t valueLow_) const;

  ClassDef(IvyAsymQuad, 1) // Asymmetric power	
};


typedef IvyAsymQuad AsymQuad;


#endif
