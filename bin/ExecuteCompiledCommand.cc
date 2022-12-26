#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <cstdlib>
#include "HostHelpersCore.h"
#include "SampleHelpersCore.h"
#include "HelperFunctions.h"
#include "StatisticsHelpers.h"
#include "IvyNumericUtils.h"
#include "IvyCSVReader.h"
#include "IvyStreamHelpers.hh"
#include "TDirectory.h"
#include "TFile.h"


using namespace std;
using namespace IvyStreamHelpers;


int main(int argc, char** argv){
  constexpr int iarg_offset=1; // argv[0]==[Executable name]

  TDirectory* curdir = gDirectory;

  int exit_status = 1;
  bool print_help = false;
  TString strCmd;
  std::vector<TString> strArgs;
  for (int iarg=iarg_offset; iarg<argc; iarg++){
    TString strarg = argv[iarg];
    if (strarg == "help"){ print_help = true; exit_status = 0; break; }
    else if (strCmd == "") strCmd = strarg;
    else strArgs.push_back(strarg);
  }

  if (strCmd == "") print_help = true;
  if (!print_help){
    TString strCmdLower = strCmd; strCmdLower.ToLower();
    // Utilities for the shell
    if (strCmdLower == "lstrip" || strCmdLower == "rstrip" || strCmdLower == "lrstrip"){
      if (strArgs.empty() || strArgs.size()>2) print_help = true;
      else{
        auto res = strArgs.front();
        const char* strip_chars = nullptr;
        if (strArgs.size()==2) strip_chars = strArgs.back().Data();
        if (strCmdLower == "lstrip") HelperFunctions::lstrip(res, strip_chars);
        else if (strCmdLower == "rstrip") HelperFunctions::rstrip(res, strip_chars);
        else HelperFunctions::lrstrip(res, strip_chars);
        cout << res << endl;
        exit_status = 0;
      }
    }
    else if (strCmdLower == "lsdir"){
      if (strArgs.size()!=1) print_help = true;
      else{
        auto vres = SampleHelpers::lsdir(strArgs.front());
        for (auto const& res:vres) cout << res << endl;
        exit_status = 0;
      }
    }
    else if (strCmdLower == "directoryexists" || strCmdLower == "fileexists" || strCmdLower == "filereadable"){
      if (strArgs.size()!=1) print_help = true;
      else{
        bool res = false;
        if (strCmdLower == "directoryexists") res = HostHelpers::DirectoryExists(strArgs.front());
        else if (strCmdLower == "fileexists") res = HostHelpers::FileExists(strArgs.front());
        else res = HostHelpers::FileReadable(strArgs.front());
        cout << (res ? "true" : "false") << endl;
        exit_status = 0;
      }
    }
    else if (strCmdLower == "gethostlocalredirector" || strCmdLower == "gethostpathtostore"){
      if (strArgs.size()!=1) print_help = true;
      else{
        HostHelpers::Hosts theHost = HostHelpers::GetHostLocation(strArgs.front());
        if (strCmdLower == "gethostlocalredirector") cout << HostHelpers::GetHostLocalRedirector(theHost, true) << endl;
        else cout << HostHelpers::GetHostPathToStore(theHost) << endl;
        exit_status = 0;
      }
    }
    else if (strCmdLower == "getstandardhostpathtostore"){
      if (strArgs.size()!=2) print_help = true;
      else{
        HostHelpers::Hosts theHost = HostHelpers::GetHostLocation(strArgs.back());
        TString res = HostHelpers::GetStandardHostPathToStore(strArgs.front(), theHost);
        cout << res << endl;
        exit_status = 0;
      }
    }
    // Utilities for quick computations
    else if (strCmdLower == "getnumberorderbase10"){
      if (strArgs.size()!=1) print_help = true;
      else{
        double const val = std::stod(strArgs.front().Data());
        cout << HelperFunctions::getFirstSignificantDecimalPowerBase10(val) << endl;
        exit_status = 0;
      }
    }
    else if (strCmdLower == "computechisqprob"){
      if (strArgs.size()!=2) print_help = true;
      else{
        double chisq = std::stod(strArgs.front().Data());
        double ndof = std::stod(strArgs.back().Data());
        double CL = StatisticsHelpers::getConfidenceLevelValue(std::sqrt(chisq), ndof);
        cout << CL << endl;
        exit_status = 0;
      }
    }
    else if (strCmdLower == "computechisqquantile"){
      if (strArgs.size()!=2) print_help = true;
      else{
        double CL = std::stod(strArgs.front().Data());
        double ndof = std::stod(strArgs.back().Data());
        double quant = StatisticsHelpers::chisq_quantile(CL, ndof);
        cout << quant << endl;
        exit_status = 0;
      }
    }
    else if (strCmdLower == "computepoissoncountinginterval"){
      if (strArgs.size()!=2 && strArgs.size()!=3) print_help = true;
      else{
        double count = std::stod(strArgs.front().Data());
        double CL = std::stod(strArgs.at(1).Data());
        double counterrsq = (strArgs.size()==2 ? count : std::pow(std::stod(strArgs.back().Data()), 2));
        double vlow=0, vhigh=0;
        StatisticsHelpers::getPoissonCountingConfidenceInterval_Frequentist(count, counterrsq, CL, vlow, vhigh);
        int count_precision_low = HelperFunctions::getFirstSignificantDecimalPowerBase10(vlow);
        int count_precision_high = HelperFunctions::getFirstSignificantDecimalPowerBase10(vhigh);
        if (count_precision_low>0) count_precision_low = 0;
        if (count_precision_high>0) count_precision_high = 0;
        cout << HelperFunctions::castValueToString(vlow, -count_precision_low+5, 5)  << " " << HelperFunctions::castValueToString(vhigh, -count_precision_high+5, 5) << endl;
        exit_status = 0;
      }
    }
    else if (strCmdLower == "computepoissonefficienyfrequentistinterval"){
      if (strArgs.size()!=3 && strArgs.size()!=4) print_help = true;
      else{
        double total = std::stod(strArgs.front().Data());
        double pass = std::stod(strArgs.at(1).Data());
        double CL = std::stod(strArgs.at(2).Data());
        double total_errsq = (strArgs.size()==3 ? total : std::pow(std::stod(strArgs.back().Data()), 2));
        double vlow=0, vhigh=0;
        StatisticsHelpers::getPoissonEfficiencyConfidenceInterval_Frequentist(total, pass, total_errsq, CL, vlow, vhigh);
        int count_precision_low = HelperFunctions::getFirstSignificantDecimalPowerBase10(vlow);
        int count_precision_high = HelperFunctions::getFirstSignificantDecimalPowerBase10(vhigh);
        if (count_precision_low>0) count_precision_low = 0;
        if (count_precision_high>0) count_precision_high = 0;
        cout << HelperFunctions::castValueToString(vlow, -count_precision_low+5, 5)  << " " << HelperFunctions::castValueToString(vhigh, -count_precision_high+5, 5) << endl;
        exit_status = 0;
      }
    }
    else if (strCmdLower == "computegreatestcommondivisor"){
      std::vector<unsigned long long int> nums; nums.reserve(strArgs.size());
      for (auto const& ss:strArgs) nums.push_back(std::abs(std::stoll(ss.Data())));
      unsigned long long int res = 0;
      NumericUtils::GreatestCommonDivisor(nums, res);
      cout << res << endl;
      exit_status = 0;
    }
    else if (strCmdLower == "computeleastcommonmultiple"){
      std::vector<unsigned long long int> nums; nums.reserve(strArgs.size());
      for (auto const& ss:strArgs) nums.push_back(std::abs(std::stoll(ss.Data())));
      unsigned long long int res = 0;
      NumericUtils::LeastCommonMultiple(nums, res);
      cout << res << endl;
      exit_status = 0;
    }
    // Other handy helper utilities
    else if (strCmdLower == "convertcsvtottree"){
      if (strArgs.size()!=3 && strArgs.size()!=4) print_help = true;
      else{
        TString const& csvname = strArgs.front();
        TString const& coutput = strArgs.at(1);
        TString const& treename = strArgs.at(2);
        TString strcomment; if (strArgs.size()==4) strcomment = strArgs.back();

        IVYout << "Reading csv " << csvname << " with comment string " << strcomment << "..." << endl;
        IvyCSVReader csvreader(csvname.Data(), strcomment.Data());

        TFile* foutput = TFile::Open(coutput, "recreate");
        foutput->cd();
        BaseTree* tcsv = csvreader.convertToTree(treename.Data());
        tcsv->writeToFile(foutput);
        delete tcsv;
        foutput->Close();
        curdir->cd();

        exit_status = 0;
      }
    }
    else{
      IVYerr << "Command " << strCmd << " with arguments " << strArgs << " is not supported." << endl;
      print_help = true;
    }
  }

  if (print_help){
    cout << "ExecuteCompiledCommand usage (commands are case-insensitive):" << endl;
    cout << endl;
    cout << "  help: Print this help message." << endl;
    cout << endl;
    cout << "*** Helper commands for bash operations ***" << endl;
    cout << endl;
    cout << "  lstrip [line] [characters (optional)]: Strip line off a set of characters from the beginning. If character string is empty, strip off whitespace." << endl;
    cout << "  rstrip [line] [characters (optional)]: Strip line off a set of characters from the end. If character string is empty, strip off whitespace." << endl;
    cout << "  lrstrip [line] [characters (optional)]: lstrip and rstrip combined." << endl;
    cout << "  lsdir [directory]: The directory should already contain local redirectors if needed by an ordinary gfal-ls. Local redirectors should not be put for local files." << endl;
    cout << "  DirectoryExists [path]: Check if directory exists. Path convention is the same as the 'lsdir' command." << endl;
    cout << "  FileExists [path]: Check if file exists. Path convention is the same as the 'lsdir' command." << endl;
    cout << "  FileReadable [path]: Check if file is readable. Path convention is the same as the 'lsdir' command." << endl;
    cout << "  GetHostLocalRedirector [host identifier]: The host identifier should be a common string such as 't2.ucsd.edu', 'iihe.ac.be', 'eos.cms.cern.ch' etc." << endl;
    cout << "  GetHostPathToStore [host identifier]: Same as above." << endl;
    cout << "  GetStandardHostPathToStore [path] [host identifier]: The path can start with '/store', or it can be an absolute path. Same as above for the host identifier." << endl;
    cout << "  GetX509Proxy: This is a call to get the best x509 user proxy, collecting a bunch of checks together." << endl;
    cout << endl;
    cout << "*** Helper functions for quick computations ***" << endl;
    cout << endl;
    cout << "  GetNumberOrderBase10 [number]: Compute the order of a number in base 10 (e.g., 120 -> 2, 0.023 -> -2). 0 returns 0." << endl;
    cout << "  ComputeChisqProb [chisq] [ndof]: Compute the probability corresponding to the CL for a chisq and a certain ndof (e.g., [5.99, 2] -> 0.95)." << endl;
    cout << "  ComputeChisqQuantile [CL] [ndof]: Compute the chi-square quantile corresponding to a CL and a certain ndof." << endl;
    cout << "  ComputePoissonCountingInterval [count] [CL] [count error (optional)]: Compute the Poisson confidence interval for a count of events at a certain CL." << endl;
    cout << "  ComputePoissonEfficienyFrequentistInterval [total] [pass] [CL] [total error (optional)]: Compute the Clopper-Pearson confidence interval for counts of total and passing events at a certain CL." << endl;
    cout << "  ComputeGreatestCommonDivisor [integer 1] [integer 2]... (can pass more than two integers): Compute the greatest common divisor of a list of integers." << endl;
    cout << "  ComputeLeastCommonMultiple [integer 1] [integer 2]... (can pass more than two integers): Compute the least common multiple of a list of integers." << endl;
    cout << "*** Other handy helper utilities ***" << endl;
    cout << endl;
    cout << "  ConvertCSVToTTree [csvfile] [outputfile] [treename] [comment string (optional)]: Convert a CSV file format to a TTree format stored in a TFile." << endl;
  }

  return exit_status;
}
