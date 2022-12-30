#include "IvyNCSplineFactory_1D.h"
#include <cassert>


using namespace std;


IvyNCSplineFactory_1D::IvyNCSplineFactory_1D(
  RooAbsReal& splineVar_, TString appendName_,
  IvyNCSplineCore::BoundaryCondition const bcBeginX_,
  IvyNCSplineCore::BoundaryCondition const bcEndX_
) :
  appendName(appendName_),
  bcBeginX(bcBeginX_), bcEndX(bcEndX_),
  splineVar(&splineVar_),
  fcn(0),
  PDF(0)
{}
IvyNCSplineFactory_1D::~IvyNCSplineFactory_1D(){
  destroyPDF();
}
void IvyNCSplineFactory_1D::setPoints(TTree* tree){
  vector<pair<IvyNCSplineCore::T, IvyNCSplineCore::T>> pList;
  IvyNCSplineCore::T x, fcn;
  tree->SetBranchAddress("X", &x);
  tree->SetBranchAddress("Fcn", &fcn);
  int n = tree->GetEntries();
  for (int ip=0; ip<n; ip++){ tree->GetEntry(ip); pList.push_back(pair<IvyNCSplineCore::T, IvyNCSplineCore::T>(x, fcn)); }
  setPoints(pList);
}
void IvyNCSplineFactory_1D::setPoints(TGraph* tg){
  vector<pair<IvyNCSplineCore::T, IvyNCSplineCore::T>> pList;
  double* xx = tg->GetX();
  double* yy = tg->GetY();
  int n = tg->GetN();
  for (int ip=0; ip<n; ip++) pList.push_back(pair<IvyNCSplineCore::T, IvyNCSplineCore::T>(xx[ip], yy[ip]));
  setPoints(pList);
}
const std::vector<std::pair<IvyNCSplineCore::T, IvyNCSplineCore::T>> IvyNCSplineFactory_1D::getPoints(const std::vector<IvyNCSplineCore::T>& XList, const std::vector<IvyNCSplineCore::T>& FcnList){
  const unsigned int nX = XList.size();
  const unsigned int n = FcnList.size();
  if (nX!=n){
    cerr << "IvyNCSplineFactory_1D::getPoints: nX=" << nX << " != nFcn=" << n << endl;
    assert(0);
  }
  std::vector<std::pair<IvyNCSplineCore::T, IvyNCSplineCore::T>> pList; pList.reserve(n);
  for (unsigned int ip=0; ip<n; ip++) pList.push_back(pair<IvyNCSplineCore::T, IvyNCSplineCore::T>(XList.at(ip), FcnList.at(ip)));
  return pList;
}

void IvyNCSplineFactory_1D::destroyPDF(){ delete PDF; PDF=0; delete fcn; fcn=0; }
void IvyNCSplineFactory_1D::initPDF(const std::vector<std::pair<IvyNCSplineCore::T, IvyNCSplineCore::T>>& pList){
  destroyPDF();

  const unsigned int n = pList.size();
  std::vector<IvyNCSplineCore::T> XList;
  std::vector<IvyNCSplineCore::T> FcnList;
  for (unsigned int ip=0; ip<n; ip++){
    XList.push_back(pList.at(ip).first);
    FcnList.push_back(pList.at(ip).second);
  }

  TString name = "Func";
  if (appendName!="") name = Form("%s_%s", name.Data(), appendName.Data());
  TString title=name;
  fcn = new IvyNCSpline_1D_fast(
    name.Data(),
    title.Data(),
    *splineVar,
    XList, FcnList,
    bcBeginX, bcEndX
    );

  name.Prepend("PDF_"); title=name;
  PDF = new IvyFuncPdf(
    name.Data(),
    title.Data(),
    *fcn
    );
}

void IvyNCSplineFactory_1D::setEndConditions(
  IvyNCSplineCore::BoundaryCondition const bcBegin,
  IvyNCSplineCore::BoundaryCondition const bcEnd,
  const unsigned int /*direction*/
){
  bcBeginX=bcBegin;
  bcEndX=bcEnd;
}
