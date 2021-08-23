#include "Math/DistFunc.h"
#include "Math/DistFuncMathMore.h"
#include "TEfficiency.h"
#include "StatisticsHelpers.h"


namespace StatisticsHelpers{
  typedef double(*fcn_chisquared_quantile_t)(double, double);

  const double VAL_CL_1SIGMA = ROOT::Math::chisquared_cdf(1., 1., 0.);
  const double VAL_CL_2SIGMA = ROOT::Math::chisquared_cdf(4., 1., 0.);
  const double VAL_CL_3SIGMA = ROOT::Math::chisquared_cdf(9., 1., 0.);
  const double VAL_CL_4SIGMA = ROOT::Math::chisquared_cdf(16., 1., 0.);
  const double VAL_CL_5SIGMA = ROOT::Math::chisquared_cdf(25., 1., 0.);

  double getConfidenceLevelValue(double chi, double ndof){ return ROOT::Math::chisquared_cdf(std::pow(chi, 2), ndof, 0.); }
  double chisq_quantile(double CL, double ndof){
    //bool const useMathMore = (ndof<1.);
    constexpr bool useMathMore = false;
    fcn_chisquared_quantile_t fcn_chisquared_quantile = (useMathMore ? ROOT::MathMore::chisquared_quantile : ROOT::Math::chisquared_quantile);
    return fcn_chisquared_quantile(CL, ndof);
  }

  void getPoissonCountingConfidenceInterval_Frequentist(double sw_total, double swsq_total, double CL, double& vlow, double& vhigh){
    double const quant = (1. - CL) / 2.;
    double const count = (swsq_total<=0. ? (sw_total==0. ? 0. : sw_total) : std::pow(sw_total, 2)/swsq_total);
    //bool const useMathMore = (count<1.);
    constexpr bool useMathMore = false;
    fcn_chisquared_quantile_t fcn_chisquared_quantile = (useMathMore ? ROOT::MathMore::chisquared_quantile : ROOT::Math::chisquared_quantile);
    vlow = (count == 0. ? 0. : fcn_chisquared_quantile(quant, 2. * count) / 2.);
    vhigh = fcn_chisquared_quantile(1.-quant, 2 * (count + 1.)) / 2.;
    if (count>0.){
      vlow *= sw_total/count;
      vhigh *= sw_total/count;
    }
  }
  void getPoissonCountingConfidenceInterval_Frequentist(double count, double CL, double& vlow, double& vhigh){
    getPoissonCountingConfidenceInterval_Frequentist(count, count, CL, vlow, vhigh);
  }

  void getPoissonEfficiencyConfidenceInterval_Frequentist(double sw_total, double sw_passed, double swsq_total, double CL, double& vlow, double& vhigh){
    double normval = sw_total/swsq_total;
    double total = sw_total*normval;
    double passed = sw_passed*normval;
    vlow = TEfficiency::ClopperPearson(total, passed, CL, false);
    vhigh = TEfficiency::ClopperPearson(total, passed, CL, true);
  }
  void getPoissonEfficiencyConfidenceInterval_Bayesian(double sw_total, double sw_passed, double swsq_total, double CL, double alpha, double beta, double& vlow, double& vhigh){
    double normval = sw_total/swsq_total;
    double passed = sw_passed*normval + alpha;
    double failed = (sw_total - sw_passed)*normval + beta;
    vlow = TEfficiency::BetaCentralInterval(CL, passed, failed, false);
    vhigh = TEfficiency::BetaCentralInterval(CL, passed, failed, true);
  }

}
