#ifndef IVYNCSPLINEFACTORY_2D
#define IVYNCSPLINEFACTORY_2D

#include <vector>
#include <utility>
#include <algorithm>
#include "TTree.h"
#include "IvyNumericUtils.h"
#include "IvyNCSpline_2D_fast.h"
#include "IvyFuncPdf.h"


typedef NumericUtils::triplet<IvyNCSplineCore::T> splineTriplet_t;


class IvyNCSplineFactory_2D{
protected:
  TString appendName;

  IvyNCSplineCore::BoundaryCondition bcBeginX;
  IvyNCSplineCore::BoundaryCondition bcEndX;
  IvyNCSplineCore::BoundaryCondition bcBeginY;
  IvyNCSplineCore::BoundaryCondition bcEndY;

  RooAbsReal* XVar;
  RooAbsReal* YVar;
  IvyNCSpline_2D_fast* fcn;
  IvyFuncPdf* PDF;

  const std::vector<splineTriplet_t> getPoints(const std::vector<IvyNCSplineCore::T>& XList, const std::vector<IvyNCSplineCore::T>& YList, const std::vector<IvyNCSplineCore::T>& FcnList);

  void destroyPDF();
  void initPDF(const std::vector<splineTriplet_t>& pList);

  void addUnique(std::vector<IvyNCSplineCore::T>& list, IvyNCSplineCore::T val);

public:
  IvyNCSplineFactory_2D(
    RooAbsReal& XVar_, RooAbsReal& YVar_, TString appendName_="",
    IvyNCSplineCore::BoundaryCondition const bcBeginX_=IvyNCSplineCore::bcNaturalSpline,
    IvyNCSplineCore::BoundaryCondition const bcEndX_=IvyNCSplineCore::bcNaturalSpline,
    IvyNCSplineCore::BoundaryCondition const bcBeginY_=IvyNCSplineCore::bcNaturalSpline,
    IvyNCSplineCore::BoundaryCondition const bcEndY_=IvyNCSplineCore::bcNaturalSpline
  );
  ~IvyNCSplineFactory_2D();

  IvyNCSpline_2D_fast* getFunc(){ return fcn; }
  IvyFuncPdf* getPDF(){ return PDF; }

  void setEndConditions(
    IvyNCSplineCore::BoundaryCondition const bcBegin,
    IvyNCSplineCore::BoundaryCondition const bcEnd,
    const unsigned int direction
  );

  void setPoints(TTree* tree);
  void setPoints(const std::vector<splineTriplet_t>& pList){ initPDF(pList); }
  template<typename inType> void setPoints(const std::vector<inType>& XList, const std::vector<inType>& YList, const std::vector<inType>& FcnList){
    std::vector<IvyNCSplineCore::T> transXList;
    std::vector<IvyNCSplineCore::T> transYList;
    std::vector<IvyNCSplineCore::T> transFcnList;
    for (unsigned int ip=0; ip<XList.size(); ip++) transXList.push_back((IvyNCSplineCore::T)XList.at(ip));
    for (unsigned int ip=0; ip<YList.size(); ip++) transYList.push_back((IvyNCSplineCore::T)YList.at(ip));
    for (unsigned int ip=0; ip<FcnList.size(); ip++) transFcnList.push_back((IvyNCSplineCore::T)FcnList.at(ip));
    const std::vector<splineTriplet_t> pList = getPoints(transXList, transYList, transFcnList);
    setPoints(pList);
  }

};

template void IvyNCSplineFactory_2D::setPoints<Float_t>(const std::vector<Float_t>& XList, const std::vector<Float_t>& YList, const std::vector<Float_t>& FcnList);
template void IvyNCSplineFactory_2D::setPoints<Double_t>(const std::vector<Double_t>& XList, const std::vector<Double_t>& YList, const std::vector<Double_t>& FcnList);

#endif



