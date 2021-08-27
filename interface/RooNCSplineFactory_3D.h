#ifndef ROONCSPLINEFACTORY_3D
#define ROONCSPLINEFACTORY_3D

#include <vector>
#include <utility>
#include <algorithm>
#include "TTree.h"
#include "IvyNumericUtils.h"
#include "RooNCSpline_3D_fast.h"
#include "RooFuncPdf.h"


typedef NumericUtils::quadruplet<RooNCSplineCore::T> splineQuadruplet_t;


class RooNCSplineFactory_3D{
protected:
  TString appendName;

  RooNCSplineCore::BoundaryCondition bcBeginX;
  RooNCSplineCore::BoundaryCondition bcEndX;
  RooNCSplineCore::BoundaryCondition bcBeginY;
  RooNCSplineCore::BoundaryCondition bcEndY;
  RooNCSplineCore::BoundaryCondition bcBeginZ;
  RooNCSplineCore::BoundaryCondition bcEndZ;

  RooAbsReal* XVar;
  RooAbsReal* YVar;
  RooAbsReal* ZVar;
  RooNCSpline_3D_fast* fcn;
  RooFuncPdf* PDF;

  const std::vector<splineQuadruplet_t> getPoints(const std::vector<RooNCSplineCore::T>& XList, const std::vector<RooNCSplineCore::T>& YList, const std::vector<RooNCSplineCore::T>& ZList, const std::vector<RooNCSplineCore::T>& FcnList);

  void destroyPDF();
  void initPDF(const std::vector<splineQuadruplet_t>& pList);

  void addUnique(std::vector<RooNCSplineCore::T>& list, RooNCSplineCore::T val);

public:
  RooNCSplineFactory_3D(
    RooAbsReal& XVar_, RooAbsReal& YVar_, RooAbsReal& ZVar_, TString appendName_="",
    RooNCSplineCore::BoundaryCondition const bcBeginX_=RooNCSplineCore::bcNaturalSpline,
    RooNCSplineCore::BoundaryCondition const bcEndX_=RooNCSplineCore::bcNaturalSpline,
    RooNCSplineCore::BoundaryCondition const bcBeginY_=RooNCSplineCore::bcNaturalSpline,
    RooNCSplineCore::BoundaryCondition const bcEndY_=RooNCSplineCore::bcNaturalSpline,
    RooNCSplineCore::BoundaryCondition const bcBeginZ_=RooNCSplineCore::bcNaturalSpline,
    RooNCSplineCore::BoundaryCondition const bcEndZ_=RooNCSplineCore::bcNaturalSpline
  );
  ~RooNCSplineFactory_3D();

  RooNCSpline_3D_fast* getFunc(){ return fcn; }
  RooFuncPdf* getPDF(){ return PDF; }

  void setEndConditions(
    RooNCSplineCore::BoundaryCondition const bcBegin,
    RooNCSplineCore::BoundaryCondition const bcEnd,
    const unsigned int direction
  );

  void setPoints(TTree* tree);
  void setPoints(const std::vector<splineQuadruplet_t>& pList){ initPDF(pList); }
  template<typename inType> void setPoints(const std::vector<inType>& XList, const std::vector<inType>& YList, const std::vector<inType>& ZList, const std::vector<inType>& FcnList){
    std::vector<RooNCSplineCore::T> transXList;
    std::vector<RooNCSplineCore::T> transYList;
    std::vector<RooNCSplineCore::T> transZList;
    std::vector<RooNCSplineCore::T> transFcnList;
    for (unsigned int ip=0; ip<XList.size(); ip++) transXList.push_back((RooNCSplineCore::T)XList.at(ip));
    for (unsigned int ip=0; ip<YList.size(); ip++) transYList.push_back((RooNCSplineCore::T)YList.at(ip));
    for (unsigned int ip=0; ip<ZList.size(); ip++) transZList.push_back((RooNCSplineCore::T)ZList.at(ip));
    for (unsigned int ip=0; ip<FcnList.size(); ip++) transFcnList.push_back((RooNCSplineCore::T)FcnList.at(ip));
    const std::vector<splineQuadruplet_t> pList = getPoints(transXList, transYList, transZList, transFcnList);
    setPoints(pList);
  }

};

template void RooNCSplineFactory_3D::setPoints<Float_t>(const std::vector<Float_t>& XList, const std::vector<Float_t>& YList, const std::vector<Float_t>& ZList, const std::vector<Float_t>& FcnList);
template void RooNCSplineFactory_3D::setPoints<Double_t>(const std::vector<Double_t>& XList, const std::vector<Double_t>& YList, const std::vector<Double_t>& ZList, const std::vector<Double_t>& FcnList);


#endif



