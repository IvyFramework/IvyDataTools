#ifndef IVYNCSPLINEFACTORY_1D
#define IVYNCSPLINEFACTORY_1D

#include <vector>
#include <utility>
#include <algorithm>
#include "TGraph.h"
#include "TTree.h"
#include "IvyNCSpline_1D_fast.h"
#include "IvyFuncPdf.h"


class IvyNCSplineFactory_1D{
protected:
  TString appendName;

  IvyNCSplineCore::BoundaryCondition bcBeginX;
  IvyNCSplineCore::BoundaryCondition bcEndX;

  RooAbsReal* splineVar;
  IvyNCSpline_1D_fast* fcn;
  IvyFuncPdf* PDF;

  const std::vector<std::pair<IvyNCSplineCore::T, IvyNCSplineCore::T>> getPoints(const std::vector<IvyNCSplineCore::T>& XList, const std::vector<IvyNCSplineCore::T>& FcnList);

  void destroyPDF();
  void initPDF(const std::vector<std::pair<IvyNCSplineCore::T, IvyNCSplineCore::T>>& pList);

public:
  IvyNCSplineFactory_1D(
    RooAbsReal& splineVar_, TString appendName_="",
    IvyNCSplineCore::BoundaryCondition const bcBeginX_=IvyNCSplineCore::bcNaturalSpline,
    IvyNCSplineCore::BoundaryCondition const bcEndX_=IvyNCSplineCore::bcNaturalSpline
  );
  ~IvyNCSplineFactory_1D();

  IvyNCSpline_1D_fast* getFunc(){ return fcn; }
  IvyFuncPdf* getPDF(){ return PDF; }

  void setEndConditions(
    IvyNCSplineCore::BoundaryCondition const bcBegin,
    IvyNCSplineCore::BoundaryCondition const bcEnd,
    const unsigned int direction=0
  );

  void setPoints(TTree* tree);
  void setPoints(TGraph* tg);
  void setPoints(const std::vector<std::pair<IvyNCSplineCore::T, IvyNCSplineCore::T>>& pList){ initPDF(pList); }
  template<typename inType> void setPoints(const std::vector<inType>& XList, const std::vector<inType>& FcnList){
    std::vector<IvyNCSplineCore::T> transXList;
    std::vector<IvyNCSplineCore::T> transFcnList;
    for (unsigned int ip=0; ip<XList.size(); ip++) transXList.push_back((IvyNCSplineCore::T)XList.at(ip));
    for (unsigned int ip=0; ip<FcnList.size(); ip++) transFcnList.push_back((IvyNCSplineCore::T)FcnList.at(ip));
    const std::vector<std::pair<IvyNCSplineCore::T, IvyNCSplineCore::T>> pList = getPoints(transXList, transFcnList);
    initPDF(pList);
  }

};

template void IvyNCSplineFactory_1D::setPoints<Float_t>(const std::vector<Float_t>& XList, const std::vector<Float_t>& FcnList);
template void IvyNCSplineFactory_1D::setPoints<Double_t>(const std::vector<Double_t>& XList, const std::vector<Double_t>& FcnList);

#endif



