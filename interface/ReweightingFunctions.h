#ifndef REWEIGHTINGFUNCTIONS_H
#define REWEIGHTINGFUNCTIONS_H

#include <vector>
#include "BaseTree.h"
#include "ExtendedBinning.h"
#include "HelperFunctions.h"
#include "VerbosityLevel.h"


namespace ReweightingFunctions{
  constexpr float invalid_weight_threshold = -99.f;
  constexpr float nonexistent_weight_threshold = -1.f;
  constexpr float zero_weight = 0.f;

  std::vector<float*> getWeightRefs(BaseTree* tree, std::vector<TString> const& strWeights);
  float* getWeightRef(BaseTree* tree, TString const& strWeight);

  // Some generic weight rules
  typedef float(*ReweightingFunction_t)(BaseTree*, std::vector<float*> const&);
  float getSimpleWeight(BaseTree* tree, std::vector<float*> const& vals); // Product of weights
  float getA1PlusB1Weight(BaseTree* tree, std::vector<float*> const& vals); // wgt = wA + wB
  float getA1MinusB1Weight(BaseTree* tree, std::vector<float*> const& vals); // wgt = wA - wB
  float getA1MinusB1MinusC1Weight(BaseTree* tree, std::vector<float*> const& vals); // wgt = wA - wB - wC
  float getOnePlusB1OverA1Weight(BaseTree* tree, std::vector<float*> const& vals); // wgt = 1 + wB/wA
  float getOneMinusB1OverA1Weight(BaseTree* tree, std::vector<float*> const& vals); // wgt = 1 - wB/wA
  float getA1OverB1Weight(BaseTree* tree, std::vector<float*> const& vals); // wgt = wA/wB

  float getAbsWeightThresholdByNeff(BaseTree* tree, std::vector<float*> const& vals, ReweightingFunction_t rule, double thr_Neff, MiscUtils::VerbosityLevel verbosity=MiscUtils::ERROR);

  // Some generic variable and bin retrieval rules
  typedef float(*ReweightingVariableFunction_t)(BaseTree*, std::vector<float*> const&);
  typedef int(*ReweightingVariableBinFunction_t)(BaseTree*, ExtendedBinning const& binning, std::vector<float*> const&);
  float getSimpleVariable(BaseTree* tree, std::vector<float*> const& vals); // A single variable
  int getSimpleVariableBin(BaseTree* tree, ExtendedBinning const& binning, std::vector<float*> const& vals); // Binning for a single variable

  std::vector<double> getSimpleNeffThrsPerBin(
    BaseTree* tree,
    ExtendedBinning const& binning, std::vector<float*> const& var_vals, ReweightingVariableBinFunction_t varbin_rule,
    std::vector<float*> const& wgt_vals, ReweightingFunction_t wgt_rule,
    double thr_Neff,
    MiscUtils::VerbosityLevel verbosity=MiscUtils::ERROR
  );
  std::vector<float> getAbsWeightThresholdsPerBinByNeff(
    BaseTree* tree,
    std::vector<float*> const& wgt_vals, ReweightingFunction_t wgt_rule,
    ExtendedBinning const& binning, std::vector<float*> const& var_vals, ReweightingVariableBinFunction_t varbin_rule,
    double tolerance=-1,
    MiscUtils::VerbosityLevel verbosity=MiscUtils::ERROR
  );
  std::vector<std::vector<float>> getAbsWeightThresholdsPerBinByFixedFractionalThreshold(
    BaseTree* tree,
    std::vector<std::vector<float*>> const& wgt_vals_list, std::vector<ReweightingFunction_t> const& wgt_rule_list, std::vector< std::pair<double, double> > const& frac_tolerance_pair_list,
    ExtendedBinning const& binning, std::vector<float*> const& var_vals, ReweightingVariableBinFunction_t varbin_rule,
    MiscUtils::VerbosityLevel verbosity=MiscUtils::ERROR
  );

}


#endif
