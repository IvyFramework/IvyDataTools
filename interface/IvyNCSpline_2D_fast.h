#ifndef IVYNCSPLINE_2D_FAST
#define IVYNCSPLINE_2D_FAST

#include <vector>
#include "RooAbsPdf.h"
#include "RooRealProxy.h"
#include "RooRealVar.h"
#include "RooAbsReal.h"
#include "IvyNCSplineCore.h"


class IvyNCSpline_2D_fast : public IvyNCSplineCore{
protected:
  T rangeYmin;
  T rangeYmax;

  BoundaryCondition const bcBeginX;
  BoundaryCondition const bcEndX;
  BoundaryCondition const bcBeginY;
  BoundaryCondition const bcEndY;

  RooRealProxy theYVar;
  std::vector<T> YList;

  std::vector<std::vector<T>> FcnList;

  std::vector<T> kappaX;
  std::vector<T> kappaY;
  std::vector<std::vector<std::vector<std::vector<T>>>> coefficients; // [ix][A_x,B_x,C_x,D_x][iy][A_x_y,B_x_y,C_x_y,D_x_y]

public:
  IvyNCSpline_2D_fast();
  IvyNCSpline_2D_fast(
    const char* name,
    const char* title
    );
  IvyNCSpline_2D_fast(
    const char* name,
    const char* title,
    RooAbsReal& inXVar,
    RooAbsReal& inYVar,
    const std::vector<T>& inXList,
    const std::vector<T>& inYList,
    const std::vector<std::vector<T>>& inFcnList,
    IvyNCSplineCore::BoundaryCondition const bcBeginX_=IvyNCSplineCore::bcNaturalSpline,
    IvyNCSplineCore::BoundaryCondition const bcEndX_=IvyNCSplineCore::bcNaturalSpline,
    IvyNCSplineCore::BoundaryCondition const bcBeginY_=IvyNCSplineCore::bcNaturalSpline,
    IvyNCSplineCore::BoundaryCondition const bcEndY_=IvyNCSplineCore::bcNaturalSpline,
    Bool_t inUseFloor=true,
    T inFloorEval=0,
    T inFloorInt=0
    );
  IvyNCSpline_2D_fast(const IvyNCSpline_2D_fast& other, const char* name=0);
	virtual TObject* clone(const char* newname) const { return new IvyNCSpline_2D_fast(*this, newname); }
	inline virtual ~IvyNCSpline_2D_fast(){}

  void setRangeValidity(const T valmin, const T valmax, const Int_t whichDirection);

  virtual Int_t getAnalyticalIntegral(RooArgSet& allVars, RooArgSet& analVars, const char* rangeName=0) const;
  virtual Double_t analyticalIntegral(Int_t code, const char* rangeName=0) const;

protected:
  virtual void emptyFcnList(){ std::vector<std::vector<T>> tmp; FcnList.swap(tmp); }

  unsigned int npointsY() const{ return YList.size(); }

  virtual Int_t getWhichBin(const T& val, const Int_t whichDirection) const;
  virtual T getTVar(const std::vector<T>& kappas, const T& val, const Int_t& bin, const Int_t whichDirection) const;
  virtual void getKappas(std::vector<T>& kappas, const Int_t whichDirection);

  Bool_t testRangeValidity(const T& val, const Int_t whichDirection) const;
  void cropValueForRange(T& val, const Int_t whichDirection) const;

  virtual std::vector<std::vector<T>> getCoefficientsPerY(const std::vector<T>& kappaX, const TMatrix_t& xAinv, const Int_t& ybin, IvyNCSplineCore::BoundaryCondition const& bcBegin, IvyNCSplineCore::BoundaryCondition const& bcEnd, const Int_t xbin) const; // xbin can be -1, which means push all of them

  virtual T interpolateFcn(Int_t code, const char* rangeName=0) const;

  virtual Double_t evaluate() const;


  ClassDef(IvyNCSpline_2D_fast, 3)

};
 
#endif
