#include "IvyNCSplineFactory_3D.h"

using namespace std;


IvyNCSplineFactory_3D::IvyNCSplineFactory_3D(
  RooAbsReal& XVar_, RooAbsReal& YVar_, RooAbsReal& ZVar_, TString appendName_,
  IvyNCSplineCore::BoundaryCondition const bcBeginX_,
  IvyNCSplineCore::BoundaryCondition const bcEndX_,
  IvyNCSplineCore::BoundaryCondition const bcBeginY_,
  IvyNCSplineCore::BoundaryCondition const bcEndY_,
  IvyNCSplineCore::BoundaryCondition const bcBeginZ_,
  IvyNCSplineCore::BoundaryCondition const bcEndZ_
) :
  appendName(appendName_),
  bcBeginX(bcBeginX_), bcEndX(bcEndX_),
  bcBeginY(bcBeginY_), bcEndY(bcEndY_),
  bcBeginZ(bcBeginZ_), bcEndZ(bcEndZ_),
  XVar(&XVar_), YVar(&YVar_), ZVar(&ZVar_),
  fcn(0),
  PDF(0)
{}
IvyNCSplineFactory_3D::~IvyNCSplineFactory_3D(){
  destroyPDF();
}

void IvyNCSplineFactory_3D::addUnique(std::vector<IvyNCSplineCore::T>& list, IvyNCSplineCore::T val){
  for (unsigned int ip=0; ip<list.size(); ip++){ if (list.at(ip)==val) return; }
  list.push_back(val);
}
const std::vector<splineQuadruplet_t> IvyNCSplineFactory_3D::getPoints(
  const std::vector<IvyNCSplineCore::T>& XList,
  const std::vector<IvyNCSplineCore::T>& YList,
  const std::vector<IvyNCSplineCore::T>& ZList,
  const std::vector<IvyNCSplineCore::T>& FcnList
  ){
  const unsigned int nX = XList.size();
  const unsigned int nY = YList.size();
  const unsigned int nZ = ZList.size();
  const unsigned int n = FcnList.size();
  if (nX*nY*nZ!=n){
    cerr << "IvyNCSplineFactory_3D::getPoints: nX=" << nX << " x nY=" << nY << " x nZ=" << nZ << " != nFcn=" << n << endl;
    assert(0);
  }

  std::vector<splineQuadruplet_t> pList; pList.reserve(n);
  for (unsigned int ix=0; ix<nX; ix++){
    IvyNCSplineCore::T xval = XList.at(ix);
    for (unsigned int iy=0; iy<nY; iy++){
      IvyNCSplineCore::T yval = YList.at(iy);
      for (unsigned int iz=0; iz<nZ; iz++){
        IvyNCSplineCore::T zval = ZList.at(iz);
        unsigned int ip = nZ*(nY*ix + iy) + iz;
        pList.push_back(splineQuadruplet_t(xval, yval, zval, FcnList.at(ip)));
      }
    }
  }
  return pList;
}

void IvyNCSplineFactory_3D::destroyPDF(){ delete PDF; PDF=0; delete fcn; fcn=0; }
void IvyNCSplineFactory_3D::initPDF(const std::vector<splineQuadruplet_t>& pList){
  destroyPDF();

  const unsigned int n = pList.size();
  vector<IvyNCSplineCore::T> XList;
  vector<IvyNCSplineCore::T> YList;
  vector<IvyNCSplineCore::T> ZList;
  vector<vector<vector<IvyNCSplineCore::T>>> FcnList;
  for (unsigned int ip=0; ip<n; ip++){
    addUnique(XList, (pList.at(ip))[0]);
    addUnique(YList, (pList.at(ip))[1]);
    addUnique(ZList, (pList.at(ip))[2]);
  }
  FcnList.reserve(ZList.size());
  for (unsigned int iz=0; iz<ZList.size(); iz++){
    vector<vector<IvyNCSplineCore::T>> dumz;
    dumz.reserve(YList.size());
    for (unsigned int iy=0; iy<YList.size(); iy++){
      vector<IvyNCSplineCore::T> dumy;
      dumy.reserve(XList.size());
      for (unsigned int ix=0; ix<XList.size(); ix++){
        unsigned int ip = ZList.size()*(YList.size()*ix + iy) + iz;
        dumy.push_back((pList.at(ip))[3]); // Do not use unique here
      }
      dumz.push_back(dumy);
    }
    FcnList.push_back(dumz);
  }

  TString name = "Func";
  if (appendName!="") name = Form("%s_%s", name.Data(), appendName.Data());
  TString title=name;
  fcn = new IvyNCSpline_3D_fast(
    name.Data(),
    title.Data(),
    *XVar, *YVar, *ZVar,
    XList, YList, ZList, FcnList,
    bcBeginX, bcEndX,
    bcBeginY, bcEndY,
    bcBeginZ, bcEndZ
  );

  name.Prepend("PDF_"); title=name;
  PDF = new RooFuncPdf(
    name.Data(),
    title.Data(),
    *fcn
    );
}

void IvyNCSplineFactory_3D::setPoints(TTree* tree){
  vector<splineQuadruplet_t> pList;
  IvyNCSplineCore::T x, y, z, fcn;
  tree->SetBranchAddress("X", &x);
  tree->SetBranchAddress("Y", &y);
  tree->SetBranchAddress("Z", &z);
  tree->SetBranchAddress("Fcn", &fcn);
  int n = tree->GetEntries();
  for (int ip=0; ip<n; ip++){ tree->GetEntry(ip); pList.push_back(splineQuadruplet_t(x, y, z, fcn)); }
  setPoints(pList);
}

void IvyNCSplineFactory_3D::setEndConditions(
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
  case 2:
    bcBeginZ=bcBegin;
    bcEndZ=bcEnd;
    break;
  default:
    // Do nothing
    break;
  }
}
