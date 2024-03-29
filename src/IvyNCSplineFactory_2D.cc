#include "IvyNCSplineFactory_2D.h"


using namespace std;


IvyNCSplineFactory_2D::IvyNCSplineFactory_2D(
  RooAbsReal& XVar_, RooAbsReal& YVar_, TString appendName_,
  IvyNCSplineCore::BoundaryCondition const bcBeginX_,
  IvyNCSplineCore::BoundaryCondition const bcEndX_,
  IvyNCSplineCore::BoundaryCondition const bcBeginY_,
  IvyNCSplineCore::BoundaryCondition const bcEndY_
) :
  appendName(appendName_),
  bcBeginX(bcBeginX_), bcEndX(bcEndX_),
  bcBeginY(bcBeginY_), bcEndY(bcEndY_),
  XVar(&XVar_), YVar(&YVar_),
  fcn(0),
  PDF(0)
{}
IvyNCSplineFactory_2D::~IvyNCSplineFactory_2D(){
  destroyPDF();
}

void IvyNCSplineFactory_2D::addUnique(std::vector<IvyNCSplineCore::T>& list, IvyNCSplineCore::T val){
  for (unsigned int ip=0; ip<list.size(); ip++){ if (list.at(ip)==val) return; }
  list.push_back(val);
}
const std::vector<splineTriplet_t> IvyNCSplineFactory_2D::getPoints(
  const std::vector<IvyNCSplineCore::T>& XList,
  const std::vector<IvyNCSplineCore::T>& YList,
  const std::vector<IvyNCSplineCore::T>& FcnList
  ){
  const unsigned int nX = XList.size();
  const unsigned int nY = YList.size();
  const unsigned int n = FcnList.size();
  if (nX*nY!=n){
    cerr << "IvyNCSplineFactory_2D::getPoints: nX=" << nX << " x nY=" << nY << " != nFcn=" << n << endl;
    assert(0);
  }

  std::vector<splineTriplet_t> pList; pList.reserve(n);
  for (unsigned int ix=0; ix<nX; ix++){
    IvyNCSplineCore::T xval = XList.at(ix);
    for (unsigned int iy=0; iy<nY; iy++){
      unsigned int ip = nY*ix + iy;
      IvyNCSplineCore::T yval = YList.at(iy);
      pList.push_back(splineTriplet_t(xval, yval, FcnList.at(ip)));
    }
  }
  return pList;
}

void IvyNCSplineFactory_2D::destroyPDF(){ delete PDF; PDF=0; delete fcn; fcn=0; }
void IvyNCSplineFactory_2D::initPDF(const std::vector<splineTriplet_t>& pList){
  destroyPDF();

  const unsigned int n = pList.size();
  vector<IvyNCSplineCore::T> XList;
  vector<IvyNCSplineCore::T> YList;
  vector<vector<IvyNCSplineCore::T>> FcnList;
  for (unsigned int ip=0; ip<n; ip++){
    addUnique(XList, (pList.at(ip))[0]);
    addUnique(YList, (pList.at(ip))[1]);
  }
  FcnList.reserve(YList.size());
  for (unsigned int iy=0; iy<YList.size(); iy++){
    vector<IvyNCSplineCore::T> dum; dum.reserve(XList.size());
    for (unsigned int ix=0; ix<XList.size(); ix++){
      unsigned int ip = YList.size()*ix + iy;
      dum.push_back((pList.at(ip))[2]); // Do not use unique here
    }
    FcnList.push_back(dum);
  }

  TString name = "Func";
  if (appendName!="") name = Form("%s_%s", name.Data(), appendName.Data());
  TString title=name;
  fcn = new IvyNCSpline_2D_fast(
    name.Data(),
    title.Data(),
    *XVar, *YVar,
    XList, YList, FcnList,
    bcBeginX, bcEndX,
    bcBeginY, bcEndY
    );

  name.Prepend("PDF_"); title=name;
  PDF = new IvyFuncPdf(
    name.Data(),
    title.Data(),
    *fcn
    );
}

void IvyNCSplineFactory_2D::setPoints(TTree* tree){
  vector<splineTriplet_t> pList;
  IvyNCSplineCore::T x, y, fcn;
  tree->SetBranchAddress("X", &x);
  tree->SetBranchAddress("Y", &y);
  tree->SetBranchAddress("Fcn", &fcn);
  int n = tree->GetEntries();
  for (int ip=0; ip<n; ip++){ tree->GetEntry(ip); pList.push_back(splineTriplet_t(x, y, fcn)); }
  setPoints(pList);
}

void IvyNCSplineFactory_2D::setEndConditions(
  IvyNCSplineCore::BoundaryCondition const bcBegin,
  IvyNCSplineCore::BoundaryCondition const bcEnd,
  const unsigned int direction
){
  switch (direction){
  case 0:
    bcBeginX=bcBegin;
    bcEndX=bcEnd;
    break;
  case 1:
    bcBeginY=bcBegin;
    bcEndY=bcEnd;
    break;
  default:
    // Do nothing
    break;
  }
}
