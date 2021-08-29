#include <sys/types.h>
#include <dirent.h>
#include <regex>
#include "SampleHelpersCore.h"
#include "IvyStreamHelpers.hh"


using namespace std;
using namespace IvyStreamHelpers;


namespace SampleHelpers{
  TDirectory* const rootTDirectory = gDirectory;
  volatile sig_atomic_t doSignalInterrupt = 0;
}

void SampleHelpers::setSignalInterrupt(int snum){
  if (snum==SIGINT) doSignalInterrupt = 1;
}

std::vector<TString> SampleHelpers::lsdir(TString const& indir){
  std::vector<TString> res;

  if (!HostHelpers::CheckContainsURL(indir.Data())){
    struct dirent* ep;
    DIR* dp = opendir(indir.Data());
    if (dp != NULL){
      while ((ep = readdir(dp))){
        TString strdir = ep->d_name;
        if (strdir == "." || strdir == "..") continue;
        res.push_back(strdir);
      }
      closedir(dp);
    }
    else IVYerr << "SampleHelpers::lsdir: Could not open the directory " << indir << "." << endl;
  }
  else{
    TString tmpfname = Form("tmpfile_SampleHelpers_lsdir_%s.tmp", indir.Data());
    while (tmpfname.Contains("/")) HelperFunctions::replaceString(tmpfname, "/", "_");
    while (tmpfname.Contains(":")) HelperFunctions::replaceString(tmpfname, ":", "_");

    TString x509_proxy = HostHelpers::GetX509Proxy();

    TString strcmd = Form("env -i X509_USER_PROXY=%s gfal-ls %s > %s", x509_proxy.Data(), indir.Data(), tmpfname.Data());
    int status_cmd = HostHelpers::ExecuteCommand(strcmd);
    if (status_cmd==0){
      TUrl tmpUrl(indir);

      TString strRemoveMaindir = tmpUrl.GetFile();
      TString strRemoveUrlRoot = indir;
      HelperFunctions::replaceString<TString, TString const>(strRemoveUrlRoot, strRemoveMaindir, "");
      if (!strRemoveMaindir.EndsWith("/")) strRemoveMaindir = strRemoveMaindir + '/';

      ifstream fin;
      fin.open(tmpfname.Data(), std::ios_base::in);
      if (fin.good()){
        std::string str_in;
        while (!fin.eof()){
          getline(fin, str_in);
          HelperFunctions::lstrip(str_in);
          HelperFunctions::rstrip(str_in);
          if (str_in!=""){
            HelperFunctions::replaceString<std::string, const char*>(str_in, strRemoveUrlRoot.Data(), "");
            HelperFunctions::replaceString<std::string, const char*>(str_in, strRemoveMaindir.Data(), "");
            while (str_in.find("/")==0) str_in = str_in.substr(1);
            res.push_back(str_in.data());
          }
        }
      }
      fin.close();
      std::remove(tmpfname.Data());
    }
    else{
      IVYerr << "SampleHelpers::lsdir: Command '" << strcmd << "' returned exit status " << status_cmd << "." << endl;
      assert(0);
    }
  }

  return res;
}


float SampleHelpers::findPoleMass(const TString samplename){
  float mass = -1;
  if (samplename=="") return mass;

  std::string strSampleName = samplename.Data();
  std::regex strMatch(".*_M([0-9]*p*[0-9]*)_.*");
  std::smatch matches_MXYZ;
  std::regex_match(strSampleName, matches_MXYZ, strMatch);
  if (matches_MXYZ.size()==2){
    std::string strmass = matches_MXYZ[1].str();
    HelperFunctions::replaceString(strmass, "p", ".");
    try{
      mass = std::stof(strmass);
    }
    catch (std::invalid_argument& e){
      IVYerr << "SampleHelpers::findPoleMass: Sample '" << samplename << "' contains the mass string '" << strmass << "', but parsing of this mass string failed!" << endl;
      assert(0);
    }
  }
  return mass;
}
TTree* SampleHelpers::findTree(std::vector<TTree*> const& treeList, int evid){
  int ev = evid;
  for (auto const& tree:treeList){
    int nevts = tree->GetEntries();
    if (ev<nevts) return tree;
    else ev -= nevts;
    if (ev<0) cerr << "findTree::ERROR: Could not find the event " << evid << endl;
  }
  return 0;
}
bool SampleHelpers::branchExists(TTree* tree, TString strname){
  if (!tree) return false;
  bool found=false;
  const TList* blist = nullptr;
  // First search all branches
  blist = (const TList*) tree->GetListOfBranches();
  for (int ib=0; ib<blist->GetSize(); ib++){
    TObject* branch = blist->At(ib);
    if (!branch) continue;
    TString bname = branch->GetName();
    if (strname==bname){ found=true; break; }
  }
  // It is possible that the branch is more composite, so search every leaf in that case.
  if (!found){
    blist = (const TList*) tree->GetListOfLeaves();
    for (int ib=0; ib<blist->GetSize(); ib++){
      TObject* branch = blist->At(ib);
      if (!branch) continue;
      TString bname = branch->GetName();
      if (strname==bname){ found=true; break; }
    }
  }
  return found;
}
bool SampleHelpers::aliasExists(TTree* tree, TString strname){
  if (!tree) return false;
  bool found=false;
  const TList* blist = (const TList*) tree->GetListOfAliases();
  for (int ib=0; ib<blist->GetSize(); ib++){
    TObject* branch = blist->At(ib);
    if (!branch) continue;
    TString bname = branch->GetName();
    if (strname==bname){ found=true; break; }
  }
  return found;
}
void SampleHelpers::getEntry(std::vector<TTree*>& treeList, int evid){
  int ev = evid;
  for (auto const& tree:treeList){
    int nevts = tree->GetEntries();
    if (ev<nevts){ tree->GetEntry(ev); break; }
    else ev -= nevts;
    if (ev<0) cerr << "getEntry::ERROR: Could not find the event " << evid << endl;
  }
}
float SampleHelpers::getEntry(std::vector<std::pair<TTree*, TH1F*>>& treeList, int evid){
  int ev = evid;
  for (auto const& pp:treeList){
    TTree* tree = pp.first;
    int nevts = tree->GetEntries();
    if (ev<nevts){ tree->GetEntry(ev); return float(1./pp.second->GetBinContent(40)); }
    else ev -= nevts;
    if (ev<0) cerr << "getEntry::ERROR: Could not find the event " << evid << endl;
  }
  return 0;
}
