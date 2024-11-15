#include <cassert>
#include <cstring>
#include "HelperFunctions.h"
#include "StatisticsHelpers.h"
#include "IvyNCSplineFactory_1D.h"
#include "IvyNCSplineFactory_2D.h"
#include "IvyNCSplineFactory_3D.h"


using namespace std;
using namespace IvyStreamHelpers;


template<> void HelperFunctions::castStringToValue(std::string const& name, bool& val){
  std::string namelower=name;
  std::transform(namelower.begin(), namelower.end(), namelower.begin(), ::tolower);
  if (namelower=="true" || namelower=="t") val=1;
  else if (namelower=="false" || namelower=="f") val=0;
  else{ std::stringstream ss(name); ss >> val; }
}

TString HelperFunctions::todaysdate(){
  TString result;
  time_t now = time(0);
  tm* ltm = localtime(&now);
  int year = 1900 + ltm->tm_year-2000;
  int month = 1 + ltm->tm_mon;
  int day = ltm->tm_mday;
  TString strday = (day>=10 ? Form("%i", day) : Form("%i%i", 0, day));
  TString strmonth = (month>=10 ? Form("%i", month) : Form("%i%i", 0, month));
  TString stryear = Form("%i", year);
  result = stryear + strmonth + strday;
  return result;
}

void HelperFunctions::progressbar(unsigned int val, unsigned int tot){
  unsigned int percent=std::ceil(0.01*tot);
  tot=(tot==0 ? tot : tot-1);
  if (val%percent==0 || val==tot){
    unsigned int percent_done = val/percent;
    if (val==tot) percent_done=100;
    cout << "[ " << setw(3) << percent_done << "% | ";
    for (unsigned int k=0; k<percent_done; k++) cout << '=';
    if (percent_done<100) cout << '>';
    else cout << ' ';
    for (unsigned int k=percent_done; k<100; k++) cout << ' ';
    cout << "| ]";
    cout << flush;
    if (val==tot) cout << endl;
    else cout << '\r';
  }
}

bool HelperFunctions::splitOption(const std::string& rawoption, std::string& wish, std::string& value, char delimiter){
  size_t posEq = rawoption.find(delimiter);
  if (posEq!=string::npos){
    wish=rawoption;
    value=rawoption.substr(posEq+1);
    wish.erase(wish.begin()+posEq, wish.end());
    return true;
  }
  else{
    wish="";
    value=rawoption;
    return false;
  }
}
void HelperFunctions::splitOptionRecursive(const std::string& rawoption, std::vector<std::string>& splitoptions, char delimiter, bool uniqueResults, bool allowEmpty){
  string suboption = rawoption, result = rawoption;
  string remnant;
  while (!result.empty()){
    bool const is_last = !splitOption(suboption, result, remnant, delimiter);
    if (((allowEmpty && !is_last) || !result.empty()) && (!uniqueResults || (uniqueResults && !checkListVariable(splitoptions, result)))) splitoptions.push_back(result);
    suboption = remnant;
    if (result.empty() && suboption.find(delimiter)!=std::string::npos) result = suboption; // This can happen if the string starts with the delimiter.
  }
  if ((allowEmpty || !remnant.empty()) && (!uniqueResults || (uniqueResults && !checkListVariable(splitoptions, remnant)))) splitoptions.push_back(remnant);
}

bool HelperFunctions::splitOption(const TString& rawoption, TString& wish, TString& value, char delimiter){
  std::string const s_rawoption = rawoption.Data();
  std::string s_wish, s_value;
  bool const res = splitOption(s_rawoption, s_wish, s_value, delimiter);
  wish = s_wish.data();
  value = s_value.data();
  return res;
}
void HelperFunctions::splitOptionRecursive(const TString& rawoption, std::vector<TString>& splitoptions, char delimiter, bool uniqueResults, bool allowEmpty){
  std::string const s_rawoption = rawoption.Data();
  std::vector<std::string> s_splitoptions;
  splitOptionRecursive(s_rawoption, s_splitoptions, delimiter, uniqueResults, allowEmpty);
  for (std::string const& s:s_splitoptions) splitoptions.push_back(s.data());
}

void HelperFunctions::getExtendedBinning(TAxis const* theAxis, ExtendedBinning& res){
  if (theAxis){
    res.resetBinning();
    int nbins = theAxis->GetNbins();
    for (int ix=1; ix<=nbins+1; ix++) res.addBinBoundary(theAxis->GetBinLowEdge(ix));
    res.setLabel(theAxis->GetTitle());
  }
}
void HelperFunctions::getExtendedBinning(TH1 const* histo, unsigned int iaxis, ExtendedBinning& res){
  TAxis const* theAxis = nullptr;
  switch (iaxis){
  case 0:
    theAxis = histo->GetXaxis();
    break;
  case 1:
    theAxis = histo->GetYaxis();
    break;
  case 2:
    theAxis = histo->GetZaxis();
    break;
  default:
    IVYerr << "HelperFunctions::getExtendedBinning: Axis index can only be <3" << endl;
    assert(0);
    break;
  }
  getExtendedBinning(theAxis, res);
}

/* SPECIFIC COMMENT: Get a1 and a2 as well as a TF1 object for the formula a0+a1*exp(x) */
TF1* HelperFunctions::getFcn_a0plusa1expX(TSpline3* sp, double xmin, double xmax, bool useLowBound){
  double x, y, s;
  if (useLowBound) x = sp->GetXmin();
  else x = sp->GetXmax();
  y = sp->Eval(x);
  s = sp->Derivative(x);

  double a0, a1;
  a0 = y-s;
  a1 = s*exp(-x);

  TString fcnName;
  if (useLowBound) fcnName = Form("lowFcn_%s", sp->GetName());
  else fcnName = Form("highFcn_%s", sp->GetName());
  TF1* fcn = new TF1(fcnName, "[0]+[1]*exp(x)", xmin, xmax);
  fcn->SetParameter(0, a0);
  fcn->SetParameter(1, a1);

  return fcn;
}

/* SPECIFIC COMMENT: Get a1 and a2 as well as a TF1 object for the formula a0*exp(a1*x) */
TF1* HelperFunctions::getFcn_a0timesexpa1X(TSpline3* sp, double xmin, double xmax, bool useLowBound){
  double x, y, s;
  if (useLowBound) x = sp->GetXmin();
  else x = sp->GetXmax();
  y = sp->Eval(x);
  s = sp->Derivative(x);

  double a0, a1;
  // a0*a1*exp(a1*x)=s a0*exp(a1*x)=y
  a1=s/y;
  a0=y*exp(-a1*x);

  TString fcnName;
  if (useLowBound) fcnName = Form("lowFcn_%s", sp->GetName());
  else fcnName = Form("highFcn_%s", sp->GetName());
  TF1* fcn = new TF1(fcnName, "[0]*exp([1]*x)", xmin, xmax);
  fcn->SetParameter(0, a0);
  fcn->SetParameter(1, a1);

  return fcn;
}

/* SPECIFIC COMMENT: Get a1 and a2 as well as a TF1 object for the formula a0+a1*exp(x/a2) */
TF1* HelperFunctions::getFcn_a0plusa1timesexpXovera2(TSpline3* sp, double xmin, double xmax, bool useLowBound){
  double x, xp, dx, y, b, c, d;
  double s, ss;
  if (useLowBound){
    x = sp->GetXmin();
    sp->GetCoeff(0, xp, y, b, c, d); // Hopefully xp=x; we don't care about y.
  }
  else{
    x = sp->GetXmax();
    sp->GetCoeff(sp->GetNp()-2, xp, y, b, c, d); // We don't care about y.
  }
  dx=x-xp;
  y = sp->Eval(x);
  s = sp->Derivative(x); // == b + 2.*c*dx + 3.*d*dx*dx
  ss = 2.*c + 6.*d*dx;

  double a0, a1, a2;
  // y=a0+a1*exp(x/a2) => s=a1/a2*exp(x/a2), ss=a1/pow(a2, 2)*exp(x/a2)
  TF1* fcn=nullptr;
  if (ss!=0.){
    a2=s/ss;
    a1=s*a2*exp(-x/a2);
    a0=y-a1*exp(-x/a2);

    TString fcnName;
    if (useLowBound) fcnName = Form("lowFcn_%s", sp->GetName());
    else fcnName = Form("highFcn_%s", sp->GetName());
    fcn = new TF1(fcnName, "[0]+[1]*exp(x/[2])", xmin, xmax);
    fcn->SetParameter(0, a0);
    fcn->SetParameter(1, a1);
    fcn->SetParameter(2, a2);
  }
  else{
    IVYout << "HelperFunctions::getFcn_a0plusa1timesexpXovera2: Second derivative was 0! Forcing constant interpolation." << endl;
    a0=y;

    TString fcnName;
    if (useLowBound) fcnName = Form("lowFcn_%s", sp->GetName());
    else fcnName = Form("highFcn_%s", sp->GetName());
    fcn = new TF1(fcnName, "[0]", xmin, xmax);
    fcn->SetParameter(0, a0);
  }
  return fcn;
}

/* SPECIFIC COMMENT: Get a TF1 object for the formula a0+a1*atan(a2*(x-a3)) */
TF1* HelperFunctions::getFcn_a0plusa1timesatana2timesXminusa3(TSpline3* sp, double xmin, double xmax, bool useLowBound){
  double x, xp, dx, y, b, c, d;
  double s, ss, sss;
  if (useLowBound){
    x = sp->GetXmin();
    sp->GetCoeff(0, xp, y, b, c, d); // Hopefully xp=x; we don't care about y.
  }
  else{
    x = sp->GetXmax();
    sp->GetCoeff(sp->GetNp()-2, xp, y, b, c, d); // We don't care about y.
  }
  dx=x-xp;
  y = sp->Eval(x);
  s = sp->Derivative(x); // == b + 2.*c*dx + 3.*d*dx*dx
  ss = 2.*c + 6.*d*dx;
  sss = 6.*d;

  double a0, a1, a2, a3;
  TF1* fcn=nullptr;
  if (ss!=0.){
    double a1a2 = (8.*pow(s, 3)/sss - 4.*pow(s*s/ss, 2))/(6.*pow(s, 2)/sss-4.*pow(s, 3)/pow(ss, 2));
    double a1sq = 4.*pow(s, 3)/pow(ss, 2)*a1a2-4.*pow(s*s/ss, 2);
    if (a1sq<0.){
      IVYerr << "HelperFunctions::getFcn_a0plusa1timesatana2timesXminusa3: a1**2<0!" << endl;
      assert(0);
    }
    a1=sqrt(a1sq)*(a1a2<0. ? -1. : 1.);
    a2=a1a2/a1;
    a3=x + ss/pow(s, 2) * a1/a2/2.;
    a0=y-a1*atan(a2*(x-a3));

    TString fcnName;
    if (useLowBound) fcnName = Form("lowFcn_%s", sp->GetName());
    else fcnName = Form("highFcn_%s", sp->GetName());
    fcn = new TF1(fcnName, "[0]+[1]*atan([2]*(x-[3]))", xmin, xmax);
    fcn->SetParameter(0, a0);
    fcn->SetParameter(1, a1);
    fcn->SetParameter(2, a2);
    fcn->SetParameter(3, a3);
  }
  else{
    IVYerr << "HelperFunctions::getFcn_a0plusa1timesatana2timesXminusa3: Second derivative was 0!" << endl;
    assert(0);
  }
  return fcn;
}

/* SPECIFIC COMMENT: Get a TF1 object for the formula ( Pi/2 + atan(a0*(x-a1)) )/Pi */
TF1* HelperFunctions::getFcn_EfficiencyAtan(TSpline3* sp, double xmin, double xmax, bool useLowBound){
  constexpr double pi = TMath::Pi();
  constexpr double pi_over_two = TMath::Pi()/2.;
  constexpr double two_pi = TMath::Pi()*2.;
  double x, xp, dx, y, b, c, d;
  double s, ss;
  if (useLowBound){
    x = sp->GetXmin();
    sp->GetCoeff(0, xp, y, b, c, d); // Hopefully xp=x; we don't care about y.
  }
  else{
    x = sp->GetXmax();
    sp->GetCoeff(sp->GetNp()-2, xp, y, b, c, d); // We don't care about y.
  }
  dx=x-xp;
  y = sp->Eval(x);
  s = sp->Derivative(x); // == b + 2.*c*dx + 3.*d*dx*dx
  ss = 2.*c + 6.*d*dx;

  double a0=0, a1=0;
  TF1* fcn=nullptr;
  if (s!=0. && std::abs(ss)>1e-10){
    double kappa = ss/(two_pi*pow(s, 2));
    a1 = x + kappa;
    a0 = tan(pi*y - pi_over_two) / (-kappa);
    //IVYout << "tan fac: " << tan(pi*y - pi_over_two) << endl;
    //IVYout << "kappa: " << kappa << endl;
    //IVYout << "a1, a0: " << a1 << ", " << a0 << endl;

    TString fcnName;
    if (useLowBound) fcnName = Form("lowFcn_%s", sp->GetName());
    else fcnName = Form("highFcn_%s", sp->GetName());
    fcn = new TF1(fcnName, "[0]+[1]*atan([2]*(x-[3]))", xmin, xmax);
    fcn->SetParameter(0, 0.5);
    fcn->SetParameter(1, 1./pi);
    fcn->SetParameter(2, a0);
    fcn->SetParameter(3, a1);

    //IVYout << "HelperFunctions::getFcn_EfficiencyAtan: Initial input (y, s, ss) = ( " << y << ", " << s << ", " << ss << " )" << endl;
    //IVYout << "HelperFunctions::getFcn_EfficiencyAtan: Final output (y, s, ss) = ( " << fcn->Eval(x) << ", " << fcn->Derivative(x) << ", " << fcn->Derivative2(x) << " )" << endl;
  }
  else if (s!=0.){
    a0=y/pi_over_two;
    if (y>0.5){
      if ((!useLowBound && s>0.) || (useLowBound && s<0.)) a0 = (1.-y)/pi_over_two;
    }
    a1=s/a0;
    //IVYout << "a1, a0: " << a1 << ", " << a0 << endl;

    TString fcnName;
    if (useLowBound) fcnName = Form("lowFcn_%s", sp->GetName());
    else fcnName = Form("highFcn_%s", sp->GetName());
    fcn = new TF1(fcnName, "[0]+[1]*atan([2]*(x-[3]))", xmin, xmax);
    fcn->SetParameter(0, y);
    fcn->SetParameter(1, a0);
    fcn->SetParameter(2, a1);
    fcn->SetParameter(3, x);
  }
  else{
    IVYerr << "HelperFunctions::getFcn_EfficiencyAtan: First derivative was 0!" << endl;
    assert(0);
  }
  return fcn;
}


TSpline3* HelperFunctions::convertGraphToSpline3(TGraph const* tg, bool faithfulFirst, bool faithfulSecond, double* dfirst, double* dlast){
  unsigned int nbins = tg->GetN();
  double* xy[2]={
    tg->GetX(),
    tg->GetY()
  };
  double derivative_first=0;
  double derivative_last=0;
  TString spopt="";
  if (nbins==1) spopt="b1e1";
  else{
    if (faithfulFirst){
      spopt += "b1";
      derivative_first = (xy[1][1]-xy[1][0])/(xy[0][1]-xy[0][0]);
    }
    else spopt += "b2";
    if (faithfulSecond){
      spopt += "e1";
      derivative_last = (xy[1][nbins-1]-xy[1][nbins-2])/(xy[0][nbins-1]-xy[0][nbins-2]);
    }
    else spopt += "e2";
  }
  TSpline3* spline = new TSpline3("spline", tg, spopt, derivative_first, derivative_last);
  spline->SetName(Form("sp_%s", tg->GetName()));
  if (dfirst!=0) *dfirst = spline->Derivative(xy[0][0]);
  if (dlast!=0) *dlast = spline->Derivative(xy[0][nbins-1]);
  return spline;
}

void HelperFunctions::convertTGraphErrorsToTH1F(TGraphErrors const* tg, TH1F*& histo){
  if (tg==0){
    IVYerr << "convertTGraphErrorsToTH1F: TGraph is 0!" << endl;
    histo=0;
    return;
  }

  double* xx = tg->GetX();
  double* yy = tg->GetY();
  double* ex = tg->GetEX();
  double* ey = tg->GetEY();
  // This is just stupid.
  /*
  double* ex_up = tg->GetEXhigh();
  double* ex_dn = tg->GetEXlow();
  double* ey_up = tg->GetEYhigh();
  double* ey_dn = tg->GetEYlow();
  */
  const int nbins = tg->GetN();
  double xarray[nbins+1];
  /*
  for (int ix=0; ix<nbins; ix++) xarray[ix] = xx[ix]-ex_dn[ix];
  xarray[nbins] = xx[nbins-1]+ex_up[nbins-1];
  */
  for (int ix=0; ix<nbins; ix++) xarray[ix] = xx[ix]-ex[ix];
  xarray[nbins] = xx[nbins-1]+ex[nbins-1];
  gROOT->cd();
  histo = new TH1F(Form("h_%s", tg->GetName()), "", nbins, xarray);
  for (int ix=1; ix<=nbins; ix++){
    //IVYout << "x, y = " << xarray[ix-1] << " " << yy[ix-1] << endl;
    histo->SetBinContent(ix, yy[ix-1]);
    //histo->SetBinError(ix, sqrt((pow(ey_dn[ix-1], 2)+pow(ey_up[ix-1], 2))*0.5));
    histo->SetBinError(ix, ey[ix-1]);
  }
}
void HelperFunctions::convertTGraphAsymmErrorsToTH1F(TGraphAsymmErrors const* tg, TH1F*& histo){
  if (tg==0){
    IVYerr << "convertTGraphAsymmErrorsToTH1F: TGraph is 0!" << endl;
    histo=0;
    return;
  }

  double* xx = tg->GetX();
  double* yy = tg->GetY();
  //double* ex = tg->GetEX();
  //double* ey = tg->GetEY();
  double* ex_up = tg->GetEXhigh();
  double* ex_dn = tg->GetEXlow();
  double* ey_up = tg->GetEYhigh();
  double* ey_dn = tg->GetEYlow();
  const int nbins = tg->GetN();
  double xarray[nbins+1];

  for (int ix=0; ix<nbins; ix++) xarray[ix] = xx[ix]-ex_dn[ix];
  xarray[nbins] = xx[nbins-1]+ex_up[nbins-1];

  //for (int ix=0; ix<nbins; ix++) xarray[ix] = xx[ix]-ex[ix];
  //xarray[nbins] = xx[nbins-1]+ex[nbins-1];
  gROOT->cd();
  histo = new TH1F(Form("h_%s", tg->GetName()), "", nbins, xarray);
  for (int ix=1; ix<=nbins; ix++){
    //IVYout << "x, y = " << xarray[ix-1] << " " << yy[ix-1] << endl;
    histo->SetBinContent(ix, yy[ix-1]);
    histo->SetBinError(ix, sqrt((pow(ey_dn[ix-1], 2)+pow(ey_up[ix-1], 2))*0.5));
    //histo->SetBinError(ix, ey[ix-1]);
  }
}
void HelperFunctions::convertTH1FToTGraphAsymmErrors(TH1F const* histo, TGraphAsymmErrors*& tg, bool errorsOnZero, bool useAsymError, bool addXErrors){
  if (!histo){
    IVYerr << "convertTH1FToTGraphAsymmErrors: Histogram is null!" << endl;
    tg=nullptr;
    return;
  }

  const int nbins = histo->GetNbinsX();
  std::vector<double> xx; xx.reserve(nbins);
  std::vector<double> exl; exl.reserve(nbins);
  std::vector<double> exh; exh.reserve(nbins);
  std::vector<double> yy; yy.reserve(nbins);
  std::vector<double> eyl; eyl.reserve(nbins);
  std::vector<double> eyh; eyh.reserve(nbins);
  for (int bin=1; bin<=nbins; bin++){
    double bincontent = histo->GetBinContent(bin);
    double binerrorlow, binerrorhigh, binerror;
    binerrorlow = binerrorhigh = binerror = histo->GetBinError(bin);
    if (useAsymError && (errorsOnZero || bincontent!=0.)){
      if (!errorsOnZero && bincontent==0.){
        binerrorhigh=0;
        binerrorlow=0;
      }
      else{
        StatisticsHelpers::getPoissonCountingConfidenceInterval_Frequentist(bincontent, std::pow(binerror, 2), StatisticsHelpers::VAL_CL_1SIGMA, binerrorlow, binerrorhigh);
        binerrorhigh = binerrorhigh - bincontent;
        binerrorlow = bincontent - binerrorlow;
      }
    }

    if (!(bincontent==0. && binerrorlow==0. && binerrorhigh==0.)){
      TAxis const* xaxis = histo->GetXaxis();
      xx.push_back(xaxis->GetBinCenter(bin));
      yy.push_back(bincontent);
      eyl.push_back(binerrorlow);
      eyh.push_back(binerrorhigh);
      if (!addXErrors){
        exl.push_back(0);
        exh.push_back(0);
      }
      else{
        exl.push_back(xx.back() - xaxis->GetBinLowEdge(bin));
        exh.push_back(xaxis->GetBinUpEdge(bin) - xx.back());
      }
    }
  }

  tg = new TGraphAsymmErrors(xx.size(), xx.data(), yy.data(), exl.data(), exh.data(), eyl.data(), eyh.data());
  tg->SetName(Form("tg_%s", histo->GetName()));
  tg->SetTitle(histo->GetTitle());
  histo->GetXaxis()->Copy(*(tg->GetXaxis()));
  histo->GetYaxis()->Copy(*(tg->GetYaxis()));
  tg->SetLineStyle(histo->GetLineStyle());
  tg->SetLineColor(histo->GetLineColor());
  tg->SetLineWidth(histo->GetLineWidth());
  tg->SetFillStyle(histo->GetFillStyle());
  tg->SetFillColor(histo->GetFillColor());
  tg->SetMarkerStyle(histo->GetMarkerStyle());
  tg->SetMarkerSize(histo->GetMarkerSize());
  tg->SetMarkerColor(histo->GetMarkerColor());
}


TGraph* HelperFunctions::createROCFromDistributions(TH1 const* hA, TH1 const* hB, TString name){
  if (!hA || !hB) return nullptr;
  assert(hA->GetNbinsX()==hB->GetNbinsX());
  const int nbins=hA->GetNbinsX();
  double integral[2]={ hA->Integral(0, nbins), hB->Integral(0, nbins) };
  vector<pair<float, float>> sumWgtsPerBin; sumWgtsPerBin.assign(nbins, pair<float, float>(0, 0));
  for (int ix=1; ix<=nbins; ix++){
    for (int jx=ix; jx<=nbins; jx++){
      sumWgtsPerBin.at(ix-1).second += hA->GetBinContent(jx)/integral[0];
      sumWgtsPerBin.at(ix-1).first += hB->GetBinContent(jx)/integral[1];
    }
  }
  TGraph* tg=HelperFunctions::makeGraphFromPair(sumWgtsPerBin, name);
  HelperFunctions::addPoint(tg, 0, 0);
  tg->GetYaxis()->SetRangeUser(0, 1);
  tg->GetYaxis()->SetTitle(TString(hA->GetTitle())+" eff.");
  tg->GetXaxis()->SetRangeUser(0, 1);
  tg->GetXaxis()->SetTitle(TString(hB->GetTitle())+" eff.");
  return tg;
}

TGraphErrors* HelperFunctions::makeGraphFromTH1(TH1 const* hx, TH1 const* hy, TString name){
  if (!hy) return nullptr;
  if (hx && hx->GetNbinsX()!=hy->GetNbinsX()){
    IVYerr << "Number of bins for x coordinate != those for y" << endl;
    assert(0);
  }
  unsigned int nbins = hy->GetNbinsX();
  double* xexyey[4];
  for (unsigned int ix=0; ix<4; ix++) xexyey[ix] = new double[nbins];
  for (unsigned int bin=0; bin<nbins; bin++){
    if (hx){
      xexyey[0][bin] = hx->GetBinContent(bin+1);
      xexyey[1][bin] = hx->GetBinError(bin+1);
    }
    else{
      xexyey[0][bin] = hy->GetBinCenter(bin+1);
      xexyey[1][bin] = 0;
    }

    //IVYout << "Bin " << bin << " x-center: " << xexyey[0][bin] << " +- " << xexyey[1][bin] << endl;
    xexyey[2][bin] = hy->GetBinContent(bin+1);
    xexyey[3][bin] = hy->GetBinError(bin+1);
  }
  TGraphErrors* tg = new TGraphErrors(nbins, xexyey[0], xexyey[2], xexyey[1], xexyey[3]);
  tg->SetName(name);
  for (unsigned int ix=0; ix<4; ix++) delete[] xexyey[ix];
  return tg;
}

TGraphErrors* HelperFunctions::makeGraphFromCumulantHistogram(TH1 const* histo, TString name){
  if (!histo) return nullptr;
  if (name=="") name = Form("tg_%s", histo->GetName());
  unsigned int npoints = histo->GetNbinsX()+1;
  double* xexyey[4];
  for (unsigned int ix=0; ix<4; ix++) xexyey[ix] = new double[npoints];
  for (unsigned int bin=0; bin<npoints; bin++){
    xexyey[0][bin] = histo->GetXaxis()->GetBinUpEdge(bin);
    xexyey[1][bin] = 0;

    xexyey[2][bin] = histo->GetBinContent(bin);
    xexyey[3][bin] = histo->GetBinError(bin);
  }
  TGraphErrors* tg = new TGraphErrors(npoints, xexyey[0], xexyey[2], xexyey[1], xexyey[3]);
  tg->SetName(name);
  for (unsigned int ix=0; ix<4; ix++) delete[] xexyey[ix];
  return tg;
}

TGraph* HelperFunctions::addTGraphs(TGraph* tgfirst, TGraph* tgsecond){
  TSpline3* spfirst = convertGraphToSpline3(tgfirst);
  TSpline3* spsecond = convertGraphToSpline3(tgsecond);

  vector<pair<double, double>> xy;
  double* xx = tgfirst->GetX();
  for (int ip=0; ip<tgfirst->GetN(); ip++) addByLowest<double, double>(xy, xx[ip], 0);
  xx = tgsecond->GetX();
  for (int ip=0; ip<tgsecond->GetN(); ip++) addByLowest<double, double>(xy, xx[ip], 0);

  vector<pair<double, double>> xynew;
  for (unsigned int ip=0; ip<xy.size(); ip++){
    double xval = xy.at(ip).first;
    double yval = spfirst->Eval(xval);
    yval += spsecond->Eval(xval);
    xynew.push_back(pair<double, double>(xval, yval));
  }

  delete spfirst;
  delete spsecond;

  return makeGraphFromPair(xynew, Form("%s_plus_%s", tgfirst->GetName(), tgsecond->GetName()));
}
void HelperFunctions::multiplyTGraph(TGraph* tg, const double scale){ for (int ip=0; ip<tg->GetN(); ip++) tg->GetY()[ip] *= scale; }
TGraph* HelperFunctions::multiplyTGraph(TGraph* tgfirst, TSpline3* spsecond){
  TSpline3* spfirst = convertGraphToSpline3(tgfirst);

  vector<pair<double, double>> xy;
  double* xx = tgfirst->GetX();
  for (int ip=0; ip<tgfirst->GetN(); ip++) addByLowest<double, double>(xy, xx[ip], 0);

  vector<pair<double, double>> xynew;
  for (unsigned int ip=0; ip<xy.size(); ip++){
    double xval = xy.at(ip).first;
    double yval = spfirst->Eval(xval);
    yval *= spsecond->Eval(xval);
    xynew.push_back(pair<double, double>(xval, yval));
  }

  return makeGraphFromPair(xynew, Form("%s_x_%s", tgfirst->GetName(), spsecond->GetName()));
}
TGraph* HelperFunctions::multiplyTGraphs(TGraph* tgfirst, TGraph* tgsecond){
  TSpline3* spfirst = convertGraphToSpline3(tgfirst);
  TSpline3* spsecond = convertGraphToSpline3(tgsecond);

  vector<pair<double, double>> xy;
  double* xx = tgfirst->GetX();
  for (int ip=0; ip<tgfirst->GetN(); ip++) addByLowest<double, double>(xy, xx[ip], 0);
  xx = tgsecond->GetX();
  for (int ip=0; ip<tgsecond->GetN(); ip++) addByLowest<double, double>(xy, xx[ip], 0);

  vector<pair<double, double>> xynew;
  for (unsigned int ip=0; ip<xy.size(); ip++){
    double xval = xy.at(ip).first;
    double yval = spfirst->Eval(xval);
    yval *= spsecond->Eval(xval);
    xynew.push_back(pair<double, double>(xval, yval));
  }

  delete spfirst;
  delete spsecond;

  return makeGraphFromPair(xynew, Form("%s_x_%s", tgfirst->GetName(), tgsecond->GetName()));
}
TGraph* HelperFunctions::divideTGraphs(TGraph* tgnum, TGraph* tgdenom, double powernum, double powerdenom){
  TSpline3* spnum = convertGraphToSpline3(tgnum);
  TSpline3* spdenom = convertGraphToSpline3(tgdenom);

  vector<pair<double, double>> xy;
  double* xxnum = tgnum->GetX();
  for (int ip=0; ip<tgnum->GetN(); ip++) addByLowest<double, double>(xy, xxnum[ip], 0);
  double* xxdenom = tgdenom->GetX();
  for (int ip=0; ip<tgdenom->GetN(); ip++) addByLowest<double, double>(xy, xxdenom[ip], 0);

  vector<pair<double, double>> xynew;
  for (unsigned int ip=0; ip<xy.size(); ip++){
    double xval = xy.at(ip).first;
    double numerator = spnum->Eval(xval);
    double denominator = spdenom->Eval(xval);
    if (denominator!=0.){
      double result = pow(numerator, powernum)/pow(denominator, powerdenom);
      if (std::isnan(result) || std::isinf(result)){
        IVYerr << "Division gave NaN! ";
        IVYerr << "Numerator = " << numerator << ", denominator = " << denominator << " at " << xval << endl;
        IVYerr << " - TGraph numerator = " << tgnum->Eval(xval) << ", denominator = " << tgdenom->Eval(xval) << " at " << xval << endl;
      }
      else xynew.push_back(pair<double, double>(xval, result));
    }
  }

  delete spnum;
  delete spdenom;

  return makeGraphFromPair(xynew, Form("%s_over_%s", tgnum->GetName(), tgdenom->GetName()));
}

void HelperFunctions::removePoint(TGraph*& tg, unsigned int idx){
  unsigned int npoints_before = tg->GetN();
  if (npoints_before==0 || idx>=npoints_before) return;
  std::vector<double> xxlist; xxlist.reserve(npoints_before-1);
  std::vector<double> exllist; exllist.reserve(npoints_before-1);
  std::vector<double> exhlist; exhlist.reserve(npoints_before-1);
  std::vector<double> yylist; yylist.reserve(npoints_before-1);
  std::vector<double> eyllist; eyllist.reserve(npoints_before-1);
  std::vector<double> eyhlist; eyhlist.reserve(npoints_before-1);
  TGraphAsymmErrors* tgasymerrs = dynamic_cast<TGraphAsymmErrors*>(tg);
  TGraphErrors* tgerrs = dynamic_cast<TGraphErrors*>(tg);
  for (unsigned int ix=0; ix<npoints_before; ix++){
    if (ix==idx) continue;
    if (tgasymerrs){
      exllist.push_back(tgasymerrs->GetEXlow()[ix]);
      exhlist.push_back(tgasymerrs->GetEXhigh()[ix]);
      eyllist.push_back(tgasymerrs->GetEYlow()[ix]);
      eyhlist.push_back(tgasymerrs->GetEYhigh()[ix]);
    }
    else if (tgerrs){
      exhlist.push_back(tgasymerrs->GetEX()[ix]);
      eyhlist.push_back(tgasymerrs->GetEY()[ix]);
    }
    xxlist.push_back(tgasymerrs->GetX()[ix]);
    yylist.push_back(tgasymerrs->GetY()[ix]);
  }

  TString strname = tg->GetName();
  TString strtitle = tg->GetTitle();
  TString strxtitle = tg->GetXaxis()->GetTitle();
  TString strytitle = tg->GetYaxis()->GetTitle();
  TAttMarker atts_marker; tg->TAttMarker::Copy(atts_marker);
  TAttLine atts_line; tg->TAttLine::Copy(atts_line);
  TAttFill atts_fill; tg->TAttFill::Copy(atts_fill);

  delete tg;

  if (tgasymerrs) tg = new TGraphAsymmErrors(npoints_before-1, xxlist.data(), yylist.data(), exllist.data(), exhlist.data(), eyllist.data(), eyhlist.data());
  else tg = new TGraphErrors(npoints_before-1, xxlist.data(), yylist.data(), exhlist.data(), eyhlist.data());
  tg->SetName(strname);
  tg->SetTitle(strtitle);
  tg->GetXaxis()->SetTitle(strxtitle);
  tg->GetYaxis()->SetTitle(strytitle);
  tg->SetMarkerColor(atts_marker.GetMarkerColor());
  tg->SetMarkerStyle(atts_marker.GetMarkerStyle());
  tg->SetMarkerSize(atts_marker.GetMarkerSize());
  tg->SetLineColor(atts_line.GetLineColor());
  tg->SetLineStyle(atts_line.GetLineStyle());
  tg->SetLineWidth(atts_line.GetLineWidth());
  tg->SetFillColor(atts_fill.GetFillColor());
  tg->SetFillStyle(atts_fill.GetFillStyle());
}

TGraphErrors* HelperFunctions::addPoint(TGraphErrors* tgSlice, double x){
  const unsigned int nbins_slice = tgSlice->GetN();
  double* xexyey_slice[4]={
    tgSlice->GetX(),
    tgSlice->GetEX(),
    tgSlice->GetY(),
    tgSlice->GetEY()
  };

  double xexyey[4][nbins_slice+1];
  unsigned int lowbin=0, highbin=0;
  for (unsigned int iy=0; iy<nbins_slice; iy++){
    if (xexyey_slice[0][iy]<x) lowbin=iy;
    if (xexyey_slice[0][iy]>x){ highbin=iy; break; }
  }
  //IVYout << "Low bin " << lowbin << " at " << xexyey_slice[0][lowbin] << endl;
  //IVYout << "High bin " << highbin << " at " << xexyey_slice[0][highbin] << endl;

  int ctr=0;
  for (unsigned int iy=0; iy<nbins_slice; iy++){
    for (unsigned int ix=0; ix<4; ix++) xexyey[ix][ctr] = xexyey_slice[ix][iy];
    ctr++;
    if (iy==lowbin){
      for (unsigned int ix=0; ix<4; ix++){
        double ylow = xexyey_slice[ix][lowbin];
        double yhigh = xexyey_slice[ix][highbin];
        double val = (yhigh+ylow)*0.5;
        xexyey[ix][ctr] = val;
      }
      ctr++;
    }
  }
  TGraphErrors* tgSlice_new = new TGraphErrors(ctr, xexyey[0], xexyey[2], xexyey[1], xexyey[3]);
  tgSlice_new->SetName(tgSlice->GetName());
  tgSlice_new->SetTitle(tgSlice->GetTitle());
  tgSlice_new->GetXaxis()->SetTitle(tgSlice->GetXaxis()->GetTitle());
  tgSlice_new->GetYaxis()->SetTitle(tgSlice->GetYaxis()->GetTitle());
  tgSlice_new->SetMarkerColor(tgSlice->GetMarkerColor());
  tgSlice_new->SetMarkerStyle(tgSlice->GetMarkerStyle());
  tgSlice_new->SetMarkerSize(tgSlice->GetMarkerSize());
  tgSlice_new->SetLineColor(tgSlice->GetLineColor());
  tgSlice_new->SetLineStyle(tgSlice->GetLineStyle());
  tgSlice_new->SetLineWidth(tgSlice->GetLineWidth());
  tgSlice_new->SetFillColor(tgSlice->GetFillColor());
  tgSlice_new->SetFillStyle(tgSlice->GetFillStyle());
  return tgSlice_new;
}
void HelperFunctions::addPoint(TGraph*& tg, double x, double y){
  TString strname = tg->GetName();
  TString strtitle = tg->GetTitle();
  TString strxtitle = tg->GetXaxis()->GetTitle();
  TString strytitle = tg->GetYaxis()->GetTitle();
  TAttMarker atts_marker; tg->TAttMarker::Copy(atts_marker);
  TAttLine atts_line; tg->TAttLine::Copy(atts_line);
  TAttFill atts_fill; tg->TAttFill::Copy(atts_fill);

  vector<double> xarray;
  vector<double> yarray;
  xarray.push_back(x);
  yarray.push_back(y);
  for (int ip=0; ip<tg->GetN(); ip++){
    if (tg->GetX()[ip]!=x){
      xarray.push_back(tg->GetX()[ip]);
      yarray.push_back(tg->GetY()[ip]);
    }
  }
  vector<pair<double, int>> xorder;
  for (unsigned int ip=0; ip<xarray.size(); ip++) addByLowest<double, int>(xorder, xarray.at(ip), ip);

  double* xynew[2];
  for (unsigned int i=0; i<2; i++) xynew[i] = new double[xorder.size()];
  for (unsigned int ip=0; ip<xarray.size(); ip++){
    unsigned int pos = xorder[ip].second;
    xynew[0][ip] = xarray[pos];
    xynew[1][ip] = yarray[pos];
  }

  delete tg;

  tg = new TGraph(xorder.size(), xynew[0], xynew[1]);
  tg->SetName(strname);
  tg->SetTitle(strtitle);
  tg->GetXaxis()->SetTitle(strxtitle);
  tg->GetYaxis()->SetTitle(strytitle);
  tg->SetMarkerColor(atts_marker.GetMarkerColor());
  tg->SetMarkerStyle(atts_marker.GetMarkerStyle());
  tg->SetMarkerSize(atts_marker.GetMarkerSize());
  tg->SetLineColor(atts_line.GetLineColor());
  tg->SetLineStyle(atts_line.GetLineStyle());
  tg->SetLineWidth(atts_line.GetLineWidth());
  tg->SetFillColor(atts_fill.GetFillColor());
  tg->SetFillStyle(atts_fill.GetFillStyle());

  for (unsigned int i=0; i<2; i++) delete[] xynew[i];
}
void HelperFunctions::addPoint(TGraphErrors*& tg, double x, double y, double ex, double ey){
  TString strname = tg->GetName();
  TString strtitle = tg->GetTitle();
  TString strxtitle = tg->GetXaxis()->GetTitle();
  TString strytitle = tg->GetYaxis()->GetTitle();
  TAttMarker atts_marker; tg->TAttMarker::Copy(atts_marker);
  TAttLine atts_line; tg->TAttLine::Copy(atts_line);
  TAttFill atts_fill; tg->TAttFill::Copy(atts_fill);

  vector<double> xarray;
  vector<double> yarray;
  vector<double> exarray;
  vector<double> eyarray;
  xarray.push_back(x);
  yarray.push_back(y);
  exarray.push_back(ex);
  eyarray.push_back(ey);
  for (int ip=0; ip<tg->GetN(); ip++){
    if (tg->GetX()[ip]!=x){
      xarray.push_back(tg->GetX()[ip]);
      yarray.push_back(tg->GetY()[ip]);
      exarray.push_back(tg->GetEX()[ip]);
      eyarray.push_back(tg->GetEY()[ip]);
    }
  }
  vector<pair<double, int>> xorder;
  for (unsigned int ip=0; ip<xarray.size(); ip++) addByLowest<double, int>(xorder, xarray.at(ip), ip);

  double* xynew[4];
  for (unsigned int i=0; i<4; i++) xynew[i] = new double[xorder.size()];
  for (unsigned int ip=0; ip<xarray.size(); ip++){
    unsigned int pos = xorder[ip].second;
    xynew[0][ip] = xarray[pos];
    xynew[1][ip] = yarray[pos];
    xynew[2][ip] = exarray[pos];
    xynew[3][ip] = eyarray[pos];
  }

  delete tg;

  tg = new TGraphErrors(xorder.size(), xynew[0], xynew[1], xynew[2], xynew[3]);
  tg->SetName(strname);
  tg->SetTitle(strtitle);
  tg->GetXaxis()->SetTitle(strxtitle);
  tg->GetYaxis()->SetTitle(strytitle);
  tg->SetMarkerColor(atts_marker.GetMarkerColor());
  tg->SetMarkerStyle(atts_marker.GetMarkerStyle());
  tg->SetMarkerSize(atts_marker.GetMarkerSize());
  tg->SetLineColor(atts_line.GetLineColor());
  tg->SetLineStyle(atts_line.GetLineStyle());
  tg->SetLineWidth(atts_line.GetLineWidth());
  tg->SetFillColor(atts_fill.GetFillColor());
  tg->SetFillStyle(atts_fill.GetFillStyle());

  for (unsigned int i=0; i<4; i++) delete[] xynew[i];
}

TGraph* HelperFunctions::genericPatcher(
  TGraph* tg, const TString newname,
  double const xmin, double const xmax,
  TF1* (*lowf)(TSpline3*, double, double, bool),
  TF1* (*highf)(TSpline3*, double, double, bool),
  bool useFaithfulSlopeFirst, bool useFaithfulSlopeSecond,
  std::vector<std::pair<std::pair<double, double>, unsigned int>>* addpoints
){
  vector<pair<double, double>> points;
  if (tg->GetN()==1){
    addByLowest<double, double>(points, xmin, tg->GetY()[0]);
    addByLowest<double, double>(points, tg->GetX()[0], tg->GetY()[0]);
    addByLowest<double, double>(points, xmax, tg->GetY()[0]);
  }
  else{
    if (addpoints!=0){ for (auto& prange : *addpoints) addPointsBetween(tg, prange.first.first, prange.first.second, prange.second); }

    int n = tg->GetN();
    double* xx = tg->GetX();
    double* yy = tg->GetY();

    TSpline3* sp = convertGraphToSpline3(tg, useFaithfulSlopeFirst, useFaithfulSlopeSecond);
    double tglow = xx[0];
    double tghigh = xx[tg->GetN()-1];
    TF1* lowFcn = lowf(sp, xmin, tglow, true);
    TF1* highFcn = highf(sp, tghigh, xmax, false);
    lowFcn->SetNpx((int) (tglow-xmin)*5);
    highFcn->SetNpx((int) (xmax-tghigh)*5);
    delete sp;

    for (double xval=xmin; xval<tglow; xval+=1){
      double yval = lowFcn->Eval(xval);
      addByLowest<double, double>(points, xval, yval);
    }
    delete lowFcn;

    for (int ix=0; ix<n; ix++) addByLowest<double, double>(points, xx[ix], yy[ix]);

    int tghigh_int = ((int) ((tghigh+1.)/100.+0.5))*100;
    if (tghigh>=(double) tghigh_int) tghigh_int+=100;
    for (double xval=tghigh_int; xval<=xmax; xval+=100){
      double yval = highFcn->Eval(xval);
      addByLowest<double, double>(points, xval, yval);
    }
    delete highFcn;
  }
  TGraph* res = makeGraphFromPair(points, newname);
  return res;
}


void HelperFunctions::regularizeSlice(
  TGraph* tgSlice,
  std::vector<double>* fixedX, double omitbelow, double omitabove,
  int nIter_, double threshold_,
  signed char forceUseFaithfulSlopeFirst, signed char forceUseFaithfulSlopeSecond
  ){
  unsigned int nbins_slice = tgSlice->GetN();
  double* xy_slice[2]={
    tgSlice->GetX(),
    tgSlice->GetY()
  };

  double* xy_mod[2];
  for (unsigned int ix=0; ix<2; ix++){
    xy_mod[ix] = new double[nbins_slice];
    for (unsigned int iy=0; iy<nbins_slice; iy++) xy_mod[ix][iy] = xy_slice[ix][iy];
  }
  unsigned int bin_first = 2, bin_last = nbins_slice-1;

  std::vector<int> fixedBins;
  if (fixedX!=0){
    for (unsigned int ifx=0; ifx<fixedX->size(); ifx++){
      double requestedVal = fixedX->at(ifx);
      double distance=1e15;
      int bin_to_fix=-1;
      for (unsigned int bin=0; bin<nbins_slice; bin++){ if (distance>fabs(xy_mod[0][bin]-requestedVal)){ bin_to_fix = bin; distance = fabs(xy_mod[0][bin]-requestedVal); } }
      if (bin_to_fix>=0) fixedBins.push_back(bin_to_fix);
      //IVYout << "Requested to fix bin " << bin_to_fix << endl;
    }
  }
  for (unsigned int bin=0; bin<nbins_slice; bin++){
    if (xy_mod[0][bin]<omitbelow) fixedBins.push_back(bin);
    //IVYout << "Requested to fix bin " << bin << endl;
  }
  for (unsigned int bin=0; bin<nbins_slice; bin++){
    if (xy_mod[0][bin]>omitabove) fixedBins.push_back(bin);
    //IVYout << "Requested to fix bin " << bin << endl;
  }

  double* xx_second;
  double* yy_second;

  int nIter = (nIter_<0 ? 1000 : nIter_);
  for (int it=0; it<nIter; it++){
    double threshold = (threshold_<0. ? 0.01 : threshold_);
    for (unsigned int binIt = bin_first; binIt<=bin_last; binIt++){
      bool doFix=false;
      for (unsigned int ifx=0; ifx<fixedBins.size(); ifx++){
        if ((int) (binIt-1)==fixedBins.at(ifx)){ doFix=true; /*IVYout << "Iteration " << it << " is fixing bin " << (binIt-1) << endl; */break; }
      }
      if (doFix) continue;

      int ctr = 0;
      int nbins_second = nbins_slice-1;
      xx_second = new double[nbins_second];
      yy_second = new double[nbins_second];
      for (unsigned int bin = 1; bin<=nbins_slice; bin++){
        if (bin==binIt) continue;
        xx_second[ctr] = xy_mod[0][bin-1];
        yy_second[ctr] = xy_mod[1][bin-1];
        ctr++;
      }

      TGraph* interpolator = new TGraph(nbins_second, xx_second, yy_second);
      //double derivative_first = (yy_second[1]-yy_second[0])/(xx_second[1]-xx_second[0]);
      //double derivative_last = (yy_second[nbins_second-1]-yy_second[nbins_second-2])/(xx_second[nbins_second-1]-xx_second[nbins_second-2]);
      //TSpline3* spline = new TSpline3("spline", interpolator, "b1e1", derivative_first, derivative_last);
      TSpline3* spline = convertGraphToSpline3(
        interpolator,
        forceUseFaithfulSlopeFirst==1,
        forceUseFaithfulSlopeSecond==1
      );

      double center = xy_mod[0][binIt-1];
      double val = spline->Eval(center);
      if (val!=0. && fabs(xy_mod[1][binIt-1]/val-1.)>threshold) xy_mod[1][binIt-1]=val;

      delete spline;
      delete interpolator;

      delete[] yy_second;
      delete[] xx_second;
    }
  }

  for (unsigned int iy=0; iy<nbins_slice; iy++) xy_slice[1][iy] = xy_mod[1][iy];
  for (unsigned int ix=0; ix<2; ix++) delete[] xy_mod[ix];
}
void HelperFunctions::regularizeSlice(
  TGraphErrors* tgSlice,
  std::vector<double>* fixedX, double omitbelow, double omitabove,
  int nIter_, double threshold_, double acceleration_,
  signed char forceUseFaithfulSlopeFirst, signed char forceUseFaithfulSlopeSecond
){
  const unsigned int ndim=4;
  unsigned int nbins_slice = tgSlice->GetN();
  double* xy_slice[ndim]={
    tgSlice->GetX(),
    tgSlice->GetY(),
    tgSlice->GetEX(),
    tgSlice->GetEY()
  };

  double* xy_mod[ndim];
  for (unsigned int ix=0; ix<ndim; ix++){
    xy_mod[ix] = new double[nbins_slice];
    for (unsigned int iy=0; iy<nbins_slice; iy++) xy_mod[ix][iy] = xy_slice[ix][iy];
  }
  unsigned int bin_first = 0, bin_last = nbins_slice-1;

  std::vector<int> fixedBins;
  if (fixedX){
    for (unsigned int ifx=0; ifx<fixedX->size(); ifx++){
      double requestedVal = fixedX->at(ifx);
      double distance=1e15;
      int bin_to_fix=-1;
      for (unsigned int bin=0; bin<nbins_slice; bin++){ if (distance>fabs(xy_mod[0][bin]-requestedVal)){ bin_to_fix = bin; distance = fabs(xy_mod[0][bin]-requestedVal); } }
      if (bin_to_fix>=0) fixedBins.push_back(bin_to_fix);
    }
  }
  for (unsigned int bin=0; bin<nbins_slice; bin++){ if (xy_mod[0][bin]<omitbelow) fixedBins.push_back(bin); }
  for (unsigned int bin=0; bin<nbins_slice; bin++){ if (xy_mod[0][bin]>omitabove) fixedBins.push_back(bin); }

  double* xx_second;
  double* yy_second;

  int nIter = (nIter_<0 ? 1000 : nIter_);
  double threshold = (threshold_<0. ? 0.5 : threshold_);
  double acceleration = (acceleration_<0. ? 0.5 : acceleration_);
  for (int it=0; it<nIter; it++){
    vector<double> residuals;

    // First loop: Collect unbiased residuals
    unsigned int binIt = bin_first;
    while (binIt<=bin_last){
      double residual=0;
      bool doFix=false;
      for (unsigned int ifx=0; ifx<fixedBins.size(); ifx++){
        if ((int) binIt==fixedBins.at(ifx)){ doFix=true; break; }
      }
      if (!doFix){
        int ctr = 0;
        int nbins_second = nbins_slice-1;
        xx_second = new double[nbins_second];
        yy_second = new double[nbins_second];
        for (unsigned int bin = 0; bin<nbins_slice; bin++){
          if (bin==binIt) continue;
          xx_second[ctr] = xy_mod[0][bin];
          yy_second[ctr] = xy_mod[1][bin];
          ctr++;
        }

        TGraph* interpolator = new TGraph(nbins_second, xx_second, yy_second);
        TSpline3* spline = convertGraphToSpline3(
          interpolator,
          (binIt!=bin_first && forceUseFaithfulSlopeFirst==-1) || forceUseFaithfulSlopeFirst==1,
          (binIt!=bin_last && forceUseFaithfulSlopeSecond==-1) || forceUseFaithfulSlopeSecond==1
        );
        spline->SetName("tmpspline");

        double center = xy_mod[0][binIt];
        double val;
        if (binIt!=bin_first && binIt!=bin_last) val = spline->Eval(center);
        else if (binIt==bin_first){
          double deriv = spline->Derivative(xx_second[0]);
          val = spline->Eval(xx_second[0]) + deriv*(center-xx_second[0]);
        }
        else{
          double deriv = spline->Derivative(xx_second[nbins_second-1]);
          val = spline->Eval(xx_second[nbins_second-1]) + deriv*(center-xx_second[nbins_second-1]);
        }
        residual = xy_mod[1][binIt]-val;

        delete spline;
        delete interpolator;
        delete[] yy_second;
        delete[] xx_second;
      }
      residuals.push_back(residual);
      binIt++;
    }

    // Second loop: Shift values according to the unbiased residuals
    binIt = bin_first;
    for (double const& residual:residuals){
      if (fabs(residual)>threshold*xy_mod[3][binIt]){
        double valMove = -residual*acceleration;
        if (xy_mod[3][binIt]>0. && fabs(valMove)>xy_mod[3][binIt]) valMove = xy_mod[3][binIt] * (valMove<0. ? double(-1) : double(1)); // Slow down the movement to allow better convergence
        double newval = xy_mod[1][binIt] + valMove;
        if (newval<=0.) newval = xy_mod[1][binIt];
        xy_mod[3][binIt] *= newval / xy_mod[1][binIt];
        xy_mod[1][binIt] = newval;
      }
      binIt++;
    }
  }

  for (unsigned int iy=0; iy<nbins_slice; iy++){
    xy_slice[1][iy] = xy_mod[1][iy];
    xy_slice[3][iy] = xy_mod[3][iy];
  }
  for (unsigned int ix=0; ix<ndim; ix++) delete[] xy_mod[ix];
}


float HelperFunctions::calculateSimpleProductError(
  float const v1, float const e1, float const p1,
  float const v2, float const e2, float const p2
){
  assert(e1>=0. && e2>=0.);
  float d=0;
  float val=0;
  if (
    ((p1<0. && v1!=0.) || p1>=0.)
    &&
    ((p2<0. && v2!=0.) || p2>=0.)
    ){
    val=pow(v1, p1)*pow(v2, p2);
    if (v1!=0.) d += pow(p1*e1/v1, 2);
    if (v2!=0.) d += pow(p2*e2/v2, 2);
  }
  d=std::abs(val)*sqrt(d);
  return d;
}
float HelperFunctions::calculateEfficiencyError(
  float const sumW, float const sumWAll,
  float const sumWsq, float const sumWsqAll
){
  float const& sumWp=sumW;
  float const& sumWsqp=sumWsq;
  float const sumWm = sumWAll-sumWp;
  float const sumWsqm = sumWsqAll-sumWsqp;
  float numerator, denominator;
  float ratio=0;
  if (sumWAll!=0.){
    numerator = sqrt(std::max(float(0), float(sumWsqp*pow(sumWm, 2) + sumWsqm*pow(sumWp, 2))));
    denominator = pow(sumWAll, 2);
    ratio = numerator/denominator;
  }
  return ratio;
}
float HelperFunctions::translateEfficiencyErrorToNumeratorError(
  float const eff, float const sumWAll,
  float const effErr, float const sumWsqAll
){
  float numerator = pow(effErr*pow(sumWAll, 2), 2);
  float sumWp = eff*sumWAll;
  float sumWm = sumWAll-sumWp;
  float res = std::max(float(0), float((numerator - sumWsqAll*pow(sumWp, 2))/(pow(sumWm, 2)-pow(sumWp, 2))));
  return sqrt(res);
}

bool HelperFunctions::isASCIICharacter(signed char ch){ unsigned char uch = static_cast<unsigned char>(ch); return uch<128; }
bool HelperFunctions::isNonASCIICharacter(signed char ch){ return !HelperFunctions::isASCIICharacter(ch); }

template<> unsigned long long int HelperFunctions::strlength<std::string>(std::string str){ return str.length(); }
template<> unsigned long long int HelperFunctions::strlength<TString>(TString str){ return str.Length(); }
template<> unsigned long long int HelperFunctions::strlength<char const*>(char const* str){ return strlen(str); }

template<> void HelperFunctions::removeNonASCIIChars<std::string>(std::string& str){
  str.erase(
    std::remove_if(
      str.begin(), str.end(), HelperFunctions::isNonASCIICharacter
    ),
    str.end()
  );
}
template<> void HelperFunctions::removeNonASCIIChars<TString>(TString& str){
  std::string strtmp = str.Data();
  HelperFunctions::removeNonASCIIChars<std::string>(strtmp);
  str=strtmp.c_str();
}

template<> void HelperFunctions::lstrip<std::string>(std::string& str, const char* chars){
  str.erase(
    str.begin(),
    std::find_if(
      str.begin(), str.end(),
      [&chars] (int ch){
        if (!chars) return !std::isspace(ch);
        else{
          bool found = false;
          for (size_t ic=0; ic<strlen(chars); ic++) found |= (ch == static_cast<int>(chars[ic]));
          return !found;
        }
      }
    )
  );
}
template<> void HelperFunctions::rstrip<std::string>(std::string& str, const char* chars){
  str.erase(
    std::find_if(
      str.rbegin(), str.rend(),
      [&chars] (int ch){
        if (!chars) return !std::isspace(ch);
        else{
          bool found = false;
          for (size_t ic=0; ic<strlen(chars); ic++) found |= (ch == static_cast<int>(chars[ic]));
          return !found;
        }
      }
    ).base(), str.end()
        );
}
template<> void HelperFunctions::lstrip<TString>(TString& str, const char* chars){
  std::string strtmp = str.Data();
  HelperFunctions::lstrip<std::string>(strtmp, chars);
  str=strtmp.c_str();
}
template<> void HelperFunctions::rstrip<TString>(TString& str, const char* chars){
  std::string strtmp = str.Data();
  HelperFunctions::rstrip<std::string>(strtmp, chars);
  str=strtmp.c_str();
}

template<> void HelperFunctions::lowercase(std::string const& name, std::string& val, int ibegin, int iend){
  val = name;
  auto it_begin = std::begin(val);
  auto it_end = std::end(val);
  auto len = std::distance(it_begin, it_end);
  if (ibegin>=0 && ibegin<len){
    for (int i=0; i<ibegin; i++) it_begin++;
  }
  if (iend>=0 && iend<len){
    it_end = it_begin;
    for (int i=0; i<iend; i++) it_end++;
  }
  std::transform(it_begin, it_end, it_begin, [] (unsigned char c){ return std::tolower(c); });
}
template<> void HelperFunctions::lowercase(TString const& name, TString& val, int ibegin, int iend){
  std::string strname = name.Data();
  std::string strval;
  HelperFunctions::lowercase(strname, strval, ibegin, iend);
  val = strval.data();
}
template<> void HelperFunctions::lowercase(const char* const& name, const char*& val, int ibegin, int iend){
  std::string strname = name;
  std::string strval;
  HelperFunctions::lowercase(strname, strval, ibegin, iend);
  val = strval.data();
}

template<> void HelperFunctions::uppercase(std::string const& name, std::string& val, int ibegin, int iend){
  val = name;
  auto it_begin = std::begin(val);
  auto it_end = std::end(val);
  auto len = std::distance(it_begin, it_end);
  if (ibegin>=0 && ibegin<len){
    for (int i=0; i<ibegin; i++) it_begin++;
  }
  if (iend>=0 && iend<len){
    it_end = it_begin;
    for (int i=0; i<iend; i++) it_end++;
  }
  std::transform(it_begin, it_end, it_begin, [] (unsigned char c){ return std::toupper(c); });
}
template<> void HelperFunctions::uppercase(TString const& name, TString& val, int ibegin, int iend){
  std::string strname = name.Data();
  std::string strval;
  HelperFunctions::uppercase(strname, strval, ibegin, iend);
  val = strval.data();
}
template<> void HelperFunctions::uppercase(const char* const& name, const char*& val, int ibegin, int iend){
  std::string strname = name;
  std::string strval;
  HelperFunctions::uppercase(strname, strval, ibegin, iend);
  val = strval.data();
}

template<> void HelperFunctions::addPointsBetween<TGraph>(TGraph*& tgOriginal, double xmin, double xmax, unsigned int nadd){
  const unsigned int np = tgOriginal->GetN();
  double* xy[2]={
    tgOriginal->GetX(),
    tgOriginal->GetY()
  };

  unsigned int pfirst=0, plast=np-1;
  std::vector<std::pair<double, double>> tmpPoints;
  for (unsigned int ip=0; ip<np; ip++){
    if (xy[0][ip]<xmin) pfirst=ip;
    if (xy[0][ip]>xmax){ plast=ip; break; }
  }
  for (unsigned int ip=pfirst; ip<=plast; ip++) tmpPoints.push_back(std::pair<double, double>(xy[0][ip], xy[1][ip]));

  TGraph* tgtmp = makeGraphFromPair(tmpPoints, "tgtmp");
  TSpline3* sptmp = convertGraphToSpline3(tgtmp);

  IVYout << "HelperFunctions::addPointsBetween: Adding " << nadd << " points between " << xy[0][pfirst] << " and " << xy[0][plast] << endl;

  double addWidth = (xmax-xmin)/double(nadd+1);
  for (unsigned int it=1; it<=nadd; it++){
    double xnew = xmin + addWidth*double(it);
    double ynew = sptmp->Eval(xnew);
    addPoint(tgOriginal, xnew, ynew);
  }

  delete sptmp;
  delete tgtmp;
}
template<> void HelperFunctions::addPointsBetween<TGraphErrors>(TGraphErrors*& tgOriginal, double xmin, double xmax, unsigned int nadd){
  const unsigned int np = tgOriginal->GetN();
  double* xy[2]={
    tgOriginal->GetX(),
    tgOriginal->GetY()
  };

  unsigned int pfirst=0, plast=np-1;
  std::vector<std::pair<double, double>> tmpPoints;
  for (unsigned int ip=0; ip<np; ip++){
    if (xy[0][ip]<xmin) pfirst=ip;
    if (xy[0][ip]>xmax){ plast=ip; break; }
  }
  for (unsigned int ip=pfirst; ip<=plast; ip++) tmpPoints.push_back(std::pair<double, double>(xy[0][ip], xy[1][ip]));

  TGraph* tgtmp = makeGraphFromPair(tmpPoints, "tgtmp");
  TSpline3* sptmp = convertGraphToSpline3(tgtmp);

  IVYout << "HelperFunctions::addPointsBetween: Adding " << nadd << " points between " << xy[0][pfirst] << " and " << xy[0][plast] << endl;

  double addWidth = (xmax-xmin)/double(nadd+1);
  for (unsigned int it=1; it<=nadd; it++){
    double xnew = xmin + addWidth*double(it);
    double ynew = sptmp->Eval(xnew);
    addPoint(tgOriginal, xnew, ynew, 0, 0);
  }

  delete sptmp;
  delete tgtmp;
}

template<> double HelperFunctions::evaluateTObject<TH1F>(TH1F* obj, float val){
  int bin = obj->GetXaxis()->FindBin(val);
  if (bin>obj->GetXaxis()->GetNbins()) bin=obj->GetXaxis()->GetNbins();
  else if (bin<1) bin=1;
  return obj->GetBinContent(bin);
}
template<> double HelperFunctions::evaluateTObject<TGraph>(TGraph* obj, float val){
  double* xx = obj->GetX();
  int n = obj->GetN();
  if (val>xx[n-1]) val=xx[n-1];
  else if (val<xx[0]) val=xx[0];
  return obj->Eval(val);
}
template<> double HelperFunctions::evaluateTObject<TSpline3>(TSpline3* obj, float val){
  float xmin=obj->GetXmin();
  float xmax=obj->GetXmax();
  if (val>xmax) val=xmax;
  else if (val<xmin) val=xmin;
  return obj->Eval(val);
}

template<> bool HelperFunctions::checkVarNonNegative<TH1F>(TH1F const& val){
  bool res=true;
  int nx=val.GetNbinsX();
  for (int ix=0; ix<=nx+1; ix++){
    res &= checkVarNonNegative(val.GetBinContent(ix));
    res &= checkVarNonNegative(val.GetBinError(ix));
  }
  return res;
}
template<> bool HelperFunctions::checkVarNonNegative<TH2F>(TH2F const& val){
  bool res=true;
  int nx=val.GetNbinsX();
  int ny=val.GetNbinsY();
  for (int ix=0; ix<=nx+1; ix++){
    for (int iy=0; iy<=ny+1; iy++){
      res &= checkVarNonNegative(val.GetBinContent(ix, iy));
      res &= checkVarNonNegative(val.GetBinError(ix, iy));
    }
  }
  return res;
}
template<> bool HelperFunctions::checkVarNonNegative<TH3F>(TH3F const& val){
  bool res=true;
  int nx=val.GetNbinsX();
  int ny=val.GetNbinsY();
  int nz=val.GetNbinsZ();
  for (int ix=0; ix<=nx+1; ix++){
    for (int iy=0; iy<=ny+1; iy++){
      for (int iz=0; iz<=nz+1; iz++){
        res &= checkVarNonNegative(val.GetBinContent(ix, iy, iz));
        res &= checkVarNonNegative(val.GetBinError(ix, iy, iz));
      }
    }
  }
  return res;
}

template<> bool HelperFunctions::checkHistogramIntegrity<TH1>(TH1 const* histo){
  if (!histo) return false;
  TH2 const* h2d = dynamic_cast<TH2 const*>(histo);
  if (h2d) return HelperFunctions::checkHistogramIntegrity<TH2>(h2d);
  TH3 const* h3d = dynamic_cast<TH3 const*>(histo);
  if (h3d) return HelperFunctions::checkHistogramIntegrity<TH3>(h3d);
  for (int ix=0; ix<=histo->GetNbinsX()+1; ix++){
    double val=histo->GetBinContent(ix);
    double err=histo->GetBinError(ix);
    if (!checkVarNanInf(val) || !checkVarNanInf(err) || !checkVarNonNegative(err)){
      IVYerr
        << "HelperFunctions::checkHistogramIntegrity[" << histo->GetName() << "]: "
        << "Bin " << ix << " failed integrity check. Value / error = " << val << " / " << err
        << endl;
      return false;
    }
  }
  return true;
}
template<> bool HelperFunctions::checkHistogramIntegrity<TH2>(TH2 const* histo){
  if (!histo) return false;
  for (int ix=0; ix<=histo->GetNbinsX()+1; ix++){
    for (int iy=0; iy<=histo->GetNbinsY()+1; iy++){
      double val=histo->GetBinContent(ix, iy);
      double err=histo->GetBinError(ix, iy);
      if (!checkVarNanInf(val) || !checkVarNanInf(err) || !checkVarNonNegative(err)){
        IVYerr
          << "HelperFunctions::checkHistogramIntegrity[" << histo->GetName() << "]: "
          << "Bin ( " << ix << "," << iy
          << " ) failed integrity check. Value / error = " << val << " / " << err
          << endl;
        return false;
      }
    }
  }
  return true;
}
template<> bool HelperFunctions::checkHistogramIntegrity<TH3>(TH3 const* histo){
  if (!histo) return false;
  for (int ix=0; ix<=histo->GetNbinsX()+1; ix++){
    for (int iy=0; iy<=histo->GetNbinsY()+1; iy++){
      for (int iz=0; iz<=histo->GetNbinsZ()+1; iz++){
        double val=histo->GetBinContent(ix, iy, iz);
        double err=histo->GetBinError(ix, iy, iz);
        if (!checkVarNanInf(val) || !checkVarNanInf(err) || !checkVarNonNegative(err)){
          IVYerr
            << "HelperFunctions::checkHistogramIntegrity[" << histo->GetName() << "]: "
            << "Bin ( " << ix << "," << iy << "," << iz
            << " ) failed integrity check. Value / error = " << val << " / " << err
            << endl;
          return false;
        }
      }
    }
  }
  return true;
}
template<> bool HelperFunctions::checkHistogramIntegrity<TH1F>(TH1F const* histo){ return checkHistogramIntegrity<TH1>(histo); }
template<> bool HelperFunctions::checkHistogramIntegrity<TH2F>(TH2F const* histo){ return checkHistogramIntegrity<TH2>(histo); }
template<> bool HelperFunctions::checkHistogramIntegrity<TH3F>(TH3F const* histo){ return checkHistogramIntegrity<TH3>(histo); }
template<> bool HelperFunctions::checkHistogramIntegrity<TH1D>(TH1D const* histo){ return checkHistogramIntegrity<TH1>(histo); }
template<> bool HelperFunctions::checkHistogramIntegrity<TH2D>(TH2D const* histo){ return checkHistogramIntegrity<TH2>(histo); }
template<> bool HelperFunctions::checkHistogramIntegrity<TH3D>(TH3D const* histo){ return checkHistogramIntegrity<TH3>(histo); }


template<> void HelperFunctions::regularizeHistogram<TH1F>(TH1F*& histo, int nIter_, double threshold_, double acceleration_){
  if (!histo) return;
  const TString histoname=histo->GetName();

  TH1F* cumulant=nullptr;
  getCumulantHistogram(histo, cumulant);
  TGraphErrors* grCumulant=makeGraphFromCumulantHistogram(cumulant, "");

  if (grCumulant->GetN()>1){
    regularizeSlice(
      grCumulant,
      nullptr,
      (grCumulant->GetX()[0]+grCumulant->GetX()[1])/2., (grCumulant->GetX()[grCumulant->GetN()-1]+grCumulant->GetX()[grCumulant->GetN()-2])/2.,
      nIter_, threshold_, acceleration_,
      false, false
    );
  }
  for (int ix=0; ix<grCumulant->GetN()+1; ix++){
    if (ix<grCumulant->GetN()){
      cumulant->SetBinContent(ix, grCumulant->GetY()[ix]);
      cumulant->SetBinError(ix, grCumulant->GetEY()[ix]);
    }
    else{
      cumulant->SetBinContent(ix, cumulant->GetBinContent(ix-1));
      cumulant->SetBinError(ix, cumulant->GetBinError(ix-1));
    }
  }

  translateCumulantToHistogram(cumulant, histo, histoname);

  delete grCumulant;
  delete cumulant;
}
template<> void HelperFunctions::regularizeHistogram<TH2F>(TH2F*& histo, int nIter_, double threshold_, double acceleration_){
  if (!histo) return;
  const TString histoname=histo->GetName();

  TH2F* cumulant=nullptr;
  getCumulantHistogram(histo, cumulant);
  vector<TGraphErrors*> grCumulantX;
  vector<TGraphErrors*> grCumulantY;
  for (int ix=0; ix<=cumulant->GetNbinsX(); ix++){
    TH1F* cumulantSlice=new TH1F("cumulantSlice", "", cumulant->GetNbinsY(), cumulant->GetYaxis()->GetBinLowEdge(1), cumulant->GetYaxis()->GetBinLowEdge(cumulant->GetNbinsY()+1));
    for (int iy=0; iy<=cumulant->GetNbinsY()+1; iy++){
      cumulantSlice->SetBinContent(iy, cumulant->GetBinContent(ix, iy));
      cumulantSlice->SetBinError(iy, cumulant->GetBinError(ix, iy));
    }
    TGraphErrors* grCumulant = makeGraphFromCumulantHistogram(cumulantSlice, Form("grCumulant_xbin_%i", ix));
    grCumulantX.push_back(grCumulant);
    delete cumulantSlice;
  }
  for (int iy=0; iy<=cumulant->GetNbinsY(); iy++){
    TH1F* cumulantSlice=new TH1F("cumulantSlice", "", cumulant->GetNbinsX(), cumulant->GetXaxis()->GetBinLowEdge(1), cumulant->GetXaxis()->GetBinLowEdge(cumulant->GetNbinsX()+1));
    for (int ix=0; ix<=cumulant->GetNbinsX()+1; ix++){
      cumulantSlice->SetBinContent(ix, cumulant->GetBinContent(ix, iy));
      cumulantSlice->SetBinError(ix, cumulant->GetBinError(ix, iy));
    }
    TGraphErrors* grCumulant = makeGraphFromCumulantHistogram(cumulantSlice, Form("grCumulant_ybin_%i", iy));
    grCumulantY.push_back(grCumulant);
    delete cumulantSlice;
  }

  for (int iter=0; iter<nIter_; iter++){
    for (auto& grCumulant:grCumulantX){
      if (grCumulant->GetN()>1){
        regularizeSlice(
          grCumulant,
          nullptr,
          (grCumulant->GetX()[0]+grCumulant->GetX()[1])/2., (grCumulant->GetX()[grCumulant->GetN()-1]+grCumulant->GetX()[grCumulant->GetN()-2])/2.,
          1, threshold_, acceleration_,
          false, false
        );
      }
    }
    for (auto& grCumulant:grCumulantY){
      if (grCumulant->GetN()>1){
        regularizeSlice(
          grCumulant,
          nullptr,
          (grCumulant->GetX()[0]+grCumulant->GetX()[1])/2., (grCumulant->GetX()[grCumulant->GetN()-1]+grCumulant->GetX()[grCumulant->GetN()-2])/2.,
          1, threshold_, acceleration_,
          false, false
        );
      }
    }
    for (int ix=0; ix<=cumulant->GetNbinsX(); ix++){
      for (int iy=0; iy<=cumulant->GetNbinsY(); iy++){
        double valXiter=grCumulantX.at(ix)->GetY()[iy];
        double errXiter=grCumulantX.at(ix)->GetEY()[iy];
        
        double valYiter=grCumulantY.at(iy)->GetY()[ix];
        double errYiter=grCumulantY.at(iy)->GetEY()[ix];

        double val=(valXiter+valYiter)/2.;
        double err=sqrt((pow(errXiter, 2)+pow(errYiter, 2))/4.);

        grCumulantX.at(ix)->GetY()[iy]=val;
        grCumulantY.at(iy)->GetY()[ix]=val;
        grCumulantX.at(ix)->GetEY()[iy]=err;
        grCumulantY.at(iy)->GetEY()[ix]=err;
      }
    }
  }

  for (int ix=0; ix<=cumulant->GetNbinsX()+1; ix++){
    for (int iy=0; iy<=cumulant->GetNbinsY()+1; iy++){
      if (ix<cumulant->GetNbinsX()+1 && iy<cumulant->GetNbinsY()+1){
        cumulant->SetBinContent(ix, iy, grCumulantX.at(ix)->GetY()[iy]);
        cumulant->SetBinError(ix, iy, grCumulantX.at(ix)->GetEY()[iy]);
      }
      else if (ix<cumulant->GetNbinsX()+1){
        cumulant->SetBinContent(ix, iy, cumulant->GetBinContent(ix, iy-1));
        cumulant->SetBinError(ix, iy, cumulant->GetBinError(ix, iy-1));
      }
      else if (iy<cumulant->GetNbinsY()+1){
        cumulant->SetBinContent(ix, iy, cumulant->GetBinContent(ix-1, iy));
        cumulant->SetBinError(ix, iy, cumulant->GetBinError(ix-1, iy));
      }
      else{
        cumulant->SetBinContent(ix, iy, cumulant->GetBinContent(ix-1, iy-1));
        cumulant->SetBinError(ix, iy, cumulant->GetBinError(ix-1, iy-1));
      }
    }
  }

  translateCumulantToHistogram(cumulant, histo, histoname);

  for (auto& grCumulant:grCumulantY) delete grCumulant;
  for (auto& grCumulant:grCumulantX) delete grCumulant;
  delete cumulant;
}

template<> void HelperFunctions::conditionalizeHistogram<TH2F>(TH2F* histo, unsigned int iaxis, std::vector<std::pair<TH2F*, float>> const* conditionalsReference, bool useWidth, bool useEffErr){
  const bool forceUseSimpleErr = (conditionalsReference || !useEffErr);
  TAxis* axis[2]={ nullptr };
  switch (iaxis){
  case 0:
    axis[0]=histo->GetXaxis();
    axis[1]=histo->GetYaxis();
    break;
  case 1:
    axis[0]=histo->GetYaxis();
    axis[1]=histo->GetXaxis();
    break;
  default:
    return;
  }
  int nbins[2]; for (unsigned int i=0; i<2; i++) nbins[i]=axis[i]->GetNbins();

  for (int i=0; i<=nbins[0]+1; i++){
    double integral=1;
    double integralerror=0;

    int int_xb[2]={ 0 }, int_yb[2]={ 0 };
    switch (iaxis){
    case 0:
      int_xb[0]=i;
      int_xb[1]=i;
      int_yb[0]=0;
      int_yb[1]=nbins[1]+1;
      break;
    case 1:
      int_yb[0]=i;
      int_yb[1]=i;
      int_xb[0]=0;
      int_xb[1]=nbins[1]+1;
      break;
    }

    if (!conditionalsReference) integral = getHistogramIntegralAndError<TH2F>(histo, int_xb[0], int_xb[1], int_yb[0], int_yb[1], useWidth, &integralerror);
    else{
      for (std::pair<TH2F*, float> const& hh:(*conditionalsReference)){
        double extraintegralerror=0;
        double extraintegral = getHistogramIntegralAndError<TH2F>(hh.first, int_xb[0], int_xb[1], int_yb[0], int_yb[1], useWidth, &extraintegralerror);
        integralerror = calculateSimpleProductError(extraintegral, extraintegralerror, hh.second, integral, integralerror, 1);
        integral *= pow(extraintegral, hh.second);
      }
    }
    for (int j=0; j<=nbins[1]+1; j++){
      int ix=0, iy=0;
      switch (iaxis){
      case 0:
        ix=i;
        iy=j;
        break;
      case 1:
        iy=i;
        ix=j;
        break;
      }

      double width = 1;
      double binerror;
      double bincontent = getHistogramIntegralAndError<TH2F>(histo, ix, ix, iy, iy, useWidth, &binerror);

      if (useWidth && j>=1 && j<=nbins[1]) width *= axis[1]->GetBinWidth(j);

      double hval=0;
      double herr=0;
      if (integral!=0.){
        hval = bincontent/integral;
        if (!forceUseSimpleErr) herr = calculateEfficiencyError(bincontent, integral, pow(binerror, 2), pow(integralerror, 2));
        else herr = calculateSimpleProductError(bincontent, binerror, 1, integral, integralerror, -1);
        hval /= width; herr /= width;
      }

      histo->SetBinContent(ix, iy, hval);
      histo->SetBinError(ix, iy, herr);
    }
  }
}
template<> void HelperFunctions::conditionalizeHistogram<TH3F>(TH3F* histo, unsigned int iaxis, std::vector<std::pair<TH3F*, float>> const* conditionalsReference, bool useWidth, bool useEffErr){
  const bool forceUseSimpleErr = (conditionalsReference || !useEffErr);
  TAxis* axis[3]={ nullptr };
  switch (iaxis){
  case 0:
    axis[0]=histo->GetXaxis();
    axis[1]=histo->GetYaxis();
    axis[2]=histo->GetZaxis();
    break;
  case 1:
    axis[0]=histo->GetYaxis();
    axis[1]=histo->GetZaxis();
    axis[2]=histo->GetXaxis();
    break;
  case 2:
    axis[0]=histo->GetZaxis();
    axis[1]=histo->GetXaxis();
    axis[2]=histo->GetYaxis();
    break;
  default:
    return;
  }
  int nbins[3]; for (unsigned int i=0; i<3; i++) nbins[i]=axis[i]->GetNbins();

  for (int i=0; i<=nbins[0]+1; i++){
    double integral=1;
    double integralerror=0;

    int int_xb[2]={ 0 }, int_yb[2]={ 0 }, int_zb[2]={ 0 };
    switch (iaxis){
    case 0:
      int_xb[0]=i;
      int_xb[1]=i;
      int_yb[0]=0;
      int_yb[1]=nbins[1]+1;
      int_zb[0]=0;
      int_zb[1]=nbins[2]+1;
      break;
    case 1:
      int_yb[0]=i;
      int_yb[1]=i;
      int_zb[0]=0;
      int_zb[1]=nbins[1]+1;
      int_xb[0]=0;
      int_xb[1]=nbins[2]+1;
      break;
    case 2:
      int_zb[0]=i;
      int_zb[1]=i;
      int_xb[0]=0;
      int_xb[1]=nbins[1]+1;
      int_yb[0]=0;
      int_yb[1]=nbins[2]+1;
      break;
    }

    if (!conditionalsReference) integral = getHistogramIntegralAndError<TH3F>(histo, int_xb[0], int_xb[1], int_yb[0], int_yb[1], int_zb[0], int_zb[1], useWidth, &integralerror);
    else{
      for (std::pair<TH3F*, float> const& hh:(*conditionalsReference)){
        double extraintegralerror=0;
        double extraintegral = getHistogramIntegralAndError<TH3F>(hh.first, int_xb[0], int_xb[1], int_yb[0], int_yb[1], int_zb[0], int_zb[1], useWidth, &extraintegralerror);
        integralerror = calculateSimpleProductError(extraintegral, extraintegralerror, hh.second, integral, integralerror, 1);
        integral *= pow(extraintegral, hh.second);
      }
    }
    for (int j=0; j<=nbins[1]+1; j++){
      for (int k=0; k<=nbins[2]+1; k++){
        int ix=0, iy=0, iz=0;
        switch (iaxis){
        case 0:
          ix=i;
          iy=j;
          iz=k;
          break;
        case 1:
          ix=k;
          iy=i;
          iz=j;
          break;
        case 2:
          ix=j;
          iy=k;
          iz=i;
          break;
        }

        double width = 1;
        double binerror;
        double bincontent = getHistogramIntegralAndError<TH3F>(histo, ix, ix, iy, iy, iz, iz, useWidth, &binerror);

        if (useWidth && j>=1 && j<=nbins[1]) width *= axis[1]->GetBinWidth(j);
        if (useWidth && k>=1 && k<=nbins[2]) width *= axis[2]->GetBinWidth(k);

        double hval=0;
        double herr=0;
        if (integral!=0.){
          hval = bincontent/integral;
          if (!forceUseSimpleErr) herr = calculateEfficiencyError(bincontent, integral, pow(binerror, 2), pow(integralerror, 2));
          else herr = calculateSimpleProductError(bincontent, binerror, 1, integral, integralerror, -1);
          hval /= width; herr /= width;
        }

        histo->SetBinContent(ix, iy, iz, hval);
        histo->SetBinError(ix, iy, iz, herr);
      }
    }
  }
}

template<> void HelperFunctions::conditionalizeHistogram<TH3F>(TH3F* histo, unsigned int iaxis, unsigned int jaxis, std::vector<std::pair<TH3F*, float>> const* conditionalsReference, bool useWidth, bool useEffErr){
  const bool forceUseSimpleErr = (conditionalsReference || !useEffErr);
  TAxis* axis[3]={ nullptr };
  assert(iaxis<3 && jaxis<3);
  if ((jaxis==0 && iaxis==1) || (jaxis==1 && iaxis==2) || (jaxis==2 && iaxis==0)) std::swap(iaxis, jaxis);
  unsigned int kaxis = 3-iaxis-jaxis;
  switch (kaxis){
  case 0:
    axis[0]=histo->GetYaxis();
    axis[1]=histo->GetZaxis();
    axis[2]=histo->GetXaxis();
    break;
  case 1:
    axis[0]=histo->GetZaxis();
    axis[1]=histo->GetXaxis();
    axis[2]=histo->GetYaxis();
    break;
  case 2:
    axis[0]=histo->GetXaxis();
    axis[1]=histo->GetYaxis();
    axis[2]=histo->GetZaxis();
    break;
  default:
    return;
  }
  int nbins[3]; for (unsigned int i=0; i<3; i++) nbins[i]=axis[i]->GetNbins();

  for (int i=0; i<=nbins[0]+1; i++){
    for (int j=0; j<=nbins[1]+1; j++){
      double integral=1;
      double integralerror=0;

      int int_xb[2]={ 0 }, int_yb[2]={ 0 }, int_zb[2]={ 0 };
      switch (kaxis){
      case 0:
        int_yb[0]=i;
        int_yb[1]=i;
        int_zb[0]=j;
        int_zb[1]=j;
        int_xb[0]=0;
        int_xb[1]=nbins[2]+1;
        break;
      case 1:
        int_zb[0]=i;
        int_zb[1]=i;
        int_xb[0]=j;
        int_xb[1]=j;
        int_yb[0]=0;
        int_yb[1]=nbins[2]+1;
        break;
      case 2:
        int_xb[0]=i;
        int_xb[1]=i;
        int_yb[0]=j;
        int_yb[1]=j;
        int_zb[0]=0;
        int_zb[1]=nbins[2]+1;
        break;
      }

      if (!conditionalsReference) integral = getHistogramIntegralAndError<TH3F>(histo, int_xb[0], int_xb[1], int_yb[0], int_yb[1], int_zb[0], int_zb[1], useWidth, &integralerror);
      else{
        for (std::pair<TH3F*, float> const& hh:(*conditionalsReference)){
          double extraintegralerror=0;
          double extraintegral = getHistogramIntegralAndError<TH3F>(hh.first, int_xb[0], int_xb[1], int_yb[0], int_yb[1], int_zb[0], int_zb[1], useWidth, &extraintegralerror);
          integralerror = calculateSimpleProductError(extraintegral, extraintegralerror, hh.second, integral, integralerror, 1);
          integral *= pow(extraintegral, hh.second);
        }
      }
      for (int k=0; k<=nbins[2]+1; k++){
        int ix=0, iy=0, iz=0;
        switch (kaxis){
        case 0:
          ix=j;
          iy=k;
          iz=i;
          break;
        case 1:
          ix=k;
          iy=i;
          iz=j;
          break;
        case 2:
          ix=i;
          iy=j;
          iz=k;
          break;
        }

        double width = 1;
        double binerror;
        double bincontent = getHistogramIntegralAndError<TH3F>(histo, ix, ix, iy, iy, iz, iz, useWidth, &binerror);

        if (useWidth && j>=1 && j<=nbins[1]) width *= axis[1]->GetBinWidth(j);
        if (useWidth && k>=1 && k<=nbins[2]) width *= axis[2]->GetBinWidth(k);

        double hval=0;
        double herr=0;
        if (integral!=0.){
          hval = bincontent/integral;
          if (!forceUseSimpleErr) herr = calculateEfficiencyError(bincontent, integral, pow(binerror, 2), pow(integralerror, 2));
          else herr = calculateSimpleProductError(bincontent, binerror, 1, integral, integralerror, -1);
          hval /= width; herr /= width;
        }

        histo->SetBinContent(ix, iy, iz, hval);
        histo->SetBinError(ix, iy, iz, herr);
      }
    }
  }
}

template<> void HelperFunctions::wipeOverUnderFlows<TH1>(TH1* hwipe, bool rescale, bool addToLastBin){
  int const nx = hwipe->GetNbinsX();
  double integral = hwipe->Integral(0, nx+1);
  for (int binx=0; binx<=nx+1; binx++){
    if (binx>=1 && binx<=nx) continue;
    if (addToLastBin){
      int binx_last = std::min(std::max(binx, 1), nx);
      if (binx!=binx_last){
        double bincontent = hwipe->GetBinContent(binx);
        double binerror = hwipe->GetBinError(binx);
        double bincontent_last = hwipe->GetBinContent(binx_last);
        double binerror_last = hwipe->GetBinError(binx_last);
        bincontent += bincontent_last;
        binerror = sqrt(pow(binerror, 2) + pow(binerror_last, 2));
        hwipe->SetBinContent(binx_last, bincontent);
        hwipe->SetBinError(binx_last, binerror);
      }
    }
    hwipe->SetBinContent(binx, 0);
    hwipe->SetBinError(binx, 0);
  }
  if (rescale){
    double wipeScale = hwipe->Integral();
    wipeScale = integral / wipeScale;
    hwipe->Scale(wipeScale);
  }
}
template<> void HelperFunctions::wipeOverUnderFlows<TH2>(TH2* hwipe, bool rescale, bool addToLastBin){
  int const nx = hwipe->GetNbinsX();
  int const ny = hwipe->GetNbinsY();
  double integral = hwipe->Integral(0, nx+1, 0, ny+1);
  for (int binx=0; binx<=nx+1; binx++){
    for (int biny=0; biny<=ny+1; biny++){
      if (
        (binx>=1 && binx<=nx)
        &&
        (biny>=1 && biny<=ny)
        ) continue;
      if (addToLastBin){
        int binx_last = std::min(std::max(binx, 1), nx);
        int biny_last = std::min(std::max(biny, 1), ny);
        if (binx!=binx_last || biny!=biny_last){
          double bincontent = hwipe->GetBinContent(binx, biny);
          double binerror = hwipe->GetBinError(binx, biny);
          double bincontent_last = hwipe->GetBinContent(binx_last, biny_last);
          double binerror_last = hwipe->GetBinError(binx_last, biny_last);
          bincontent += bincontent_last;
          binerror = sqrt(pow(binerror, 2) + pow(binerror_last, 2));
          hwipe->SetBinContent(binx_last, biny_last, bincontent);
          hwipe->SetBinError(binx_last, biny_last, binerror);
        }
      }
      hwipe->SetBinContent(binx, biny, 0);
      hwipe->SetBinError(binx, biny, 0);
    }
  }
  if (rescale){
    double wipeScale = hwipe->Integral();
    wipeScale = integral / wipeScale;
    hwipe->Scale(wipeScale);
  }
}
template<> void HelperFunctions::wipeOverUnderFlows<TH3>(TH3* hwipe, bool rescale, bool addToLastBin){
  int const nx = hwipe->GetNbinsX();
  int const ny = hwipe->GetNbinsY();
  int const nz = hwipe->GetNbinsZ();
  double integral = hwipe->Integral(0, nx+1, 0, ny+1, 0, nz+1);
  for (int binx=0; binx<=nx+1; binx++){
    for (int biny=0; biny<=ny+1; biny++){
      for (int binz=0; binz<=nz+1; binz++){
        if (
          (binx>=1 && binx<=nx)
          &&
          (biny>=1 && biny<=ny)
          &&
          (binz>=1 && binz<=nz)
          ) continue;
        if (addToLastBin){
          int binx_last = std::min(std::max(binx, 1), nx);
          int biny_last = std::min(std::max(biny, 1), ny);
          int binz_last = std::min(std::max(binz, 1), nz);
          if (binx!=binx_last || biny!=biny_last || binz!=binz_last){
            double bincontent = hwipe->GetBinContent(binx, biny, binz);
            double binerror = hwipe->GetBinError(binx, biny, binz);
            double bincontent_last = hwipe->GetBinContent(binx_last, biny_last, binz_last);
            double binerror_last = hwipe->GetBinError(binx_last, biny_last, binz_last);
            bincontent += bincontent_last;
            binerror = sqrt(pow(binerror, 2) + pow(binerror_last, 2));
            hwipe->SetBinContent(binx_last, biny_last, binz_last, bincontent);
            hwipe->SetBinError(binx_last, biny_last, binz_last, binerror);
          }
        }
        hwipe->SetBinContent(binx, biny, binz, 0);
        hwipe->SetBinError(binx, biny, binz, 0);
      }
    }
  }
  if (rescale){
    double wipeScale = hwipe->Integral();
    wipeScale = integral / wipeScale;
    hwipe->Scale(wipeScale);
  }
}
template<> void HelperFunctions::wipeOverUnderFlows<TH1F>(TH1F* hwipe, bool rescale, bool addToLastBin){ HelperFunctions::wipeOverUnderFlows<TH1>(hwipe, rescale, addToLastBin); }
template<> void HelperFunctions::wipeOverUnderFlows<TH2F>(TH2F* hwipe, bool rescale, bool addToLastBin){ HelperFunctions::wipeOverUnderFlows<TH2>(hwipe, rescale, addToLastBin); }
template<> void HelperFunctions::wipeOverUnderFlows<TH3F>(TH3F* hwipe, bool rescale, bool addToLastBin){ HelperFunctions::wipeOverUnderFlows<TH3>(hwipe, rescale, addToLastBin); }
template<> void HelperFunctions::wipeOverUnderFlows<TH1D>(TH1D* hwipe, bool rescale, bool addToLastBin){ HelperFunctions::wipeOverUnderFlows<TH1>(hwipe, rescale, addToLastBin); }
template<> void HelperFunctions::wipeOverUnderFlows<TH2D>(TH2D* hwipe, bool rescale, bool addToLastBin){ HelperFunctions::wipeOverUnderFlows<TH2>(hwipe, rescale, addToLastBin); }
template<> void HelperFunctions::wipeOverUnderFlows<TH3D>(TH3D* hwipe, bool rescale, bool addToLastBin){ HelperFunctions::wipeOverUnderFlows<TH3>(hwipe, rescale, addToLastBin); }

template<> TH1F* HelperFunctions::flattenHistogram<TH1>(TH1 const* histo, bool useWidth, TString newname){
  TString strnewname = newname;
  if (newname=="") strnewname = Form("%s_flat", histo->GetName());
  TAxis const* xaxis = histo->GetXaxis();
  int const nbins = histo->GetNbinsX();
  TH1F* res = new TH1F(strnewname, histo->GetTitle(), nbins, 0, nbins);
  for (int ix=1; ix<=nbins; ix++){
    double bw = 1;
    if (useWidth) bw = xaxis->GetBinWidth(ix);
    res->SetBinContent(ix, histo->GetBinContent(ix)*bw);
    res->SetBinError(ix, histo->GetBinError(ix)*bw);
  }
  return res;
}
template<> TH1F* HelperFunctions::flattenHistogram<TH2>(TH2 const* histo, bool useWidth, TString newname){
  TString strnewname = newname;
  if (newname=="") strnewname = Form("%s_flat", histo->GetName());
  TAxis const* xaxis = histo->GetXaxis();
  TAxis const* yaxis = histo->GetYaxis();
  int const nbins_x = histo->GetNbinsX();
  int const nbins_y = histo->GetNbinsY();
  int const nbins = nbins_x*nbins_y;
  TH1F* res = new TH1F(strnewname, histo->GetTitle(), nbins, 0, nbins);
  for (int ix=1; ix<=nbins_x; ix++){
    int jx = (ix-1);
    double bwx = 1;
    if (useWidth) bwx = xaxis->GetBinWidth(ix);
    for (int iy=1; iy<=nbins_y; iy++){
      int jy = (iy-1);
      double bwy = 1;
      if (useWidth) bwy = yaxis->GetBinWidth(iy);

      int ibin = jy + nbins_y*jx + 1;
      res->SetBinContent(ibin, histo->GetBinContent(ix, iy)*bwx*bwy);
      res->SetBinError(ibin, histo->GetBinError(ix, iy)*bwx*bwy);
    }
  }
  return res;
}
template<> TH1F* HelperFunctions::flattenHistogram<TH3>(TH3 const* histo, bool useWidth, TString newname){
  TString strnewname = newname;
  if (newname=="") strnewname = Form("%s_flat", histo->GetName());
  TAxis const* xaxis = histo->GetXaxis();
  TAxis const* yaxis = histo->GetYaxis();
  TAxis const* zaxis = histo->GetZaxis();
  int const nbins_x = histo->GetNbinsX();
  int const nbins_y = histo->GetNbinsY();
  int const nbins_z = histo->GetNbinsZ();
  int const nbins = nbins_x*nbins_y*nbins_z;
  TH1F* res = new TH1F(strnewname, histo->GetTitle(), nbins, 0, nbins);
  for (int ix=1; ix<=nbins_x; ix++){
    int jx = (ix-1);
    double bwx = 1;
    if (useWidth) bwx = xaxis->GetBinWidth(ix);
    for (int iy=1; iy<=nbins_y; iy++){
      int jy = (iy-1);
      double bwy = 1;
      if (useWidth) bwy = yaxis->GetBinWidth(iy);
      for (int iz=1; iz<=nbins_z; iz++){
        int jz = (iz-1);
        double bwz = 1;
        if (useWidth) bwz = zaxis->GetBinWidth(iz);

        int ibin = jz + nbins_z*(jy + nbins_y*jx) + 1;
        res->SetBinContent(ibin, histo->GetBinContent(ix, iy, iz)*bwx*bwy*bwz);
        res->SetBinError(ibin, histo->GetBinError(ix, iy, iz)*bwx*bwy*bwz);
      }
    }
  }
  return res;
}
template<> TH1F* HelperFunctions::flattenHistogram<TH1F>(TH1F const* histo, bool useWidth, TString newname){ return HelperFunctions::flattenHistogram<TH1>(histo, useWidth, newname); }
template<> TH1F* HelperFunctions::flattenHistogram<TH2F>(TH2F const* histo, bool useWidth, TString newname){ return HelperFunctions::flattenHistogram<TH2>(histo, useWidth, newname); }
template<> TH1F* HelperFunctions::flattenHistogram<TH3F>(TH3F const* histo, bool useWidth, TString newname){ return HelperFunctions::flattenHistogram<TH3>(histo, useWidth, newname); }
template<> TH1F* HelperFunctions::flattenHistogram<TH1D>(TH1D const* histo, bool useWidth, TString newname){ return HelperFunctions::flattenHistogram<TH1>(histo, useWidth, newname); }
template<> TH1F* HelperFunctions::flattenHistogram<TH2D>(TH2D const* histo, bool useWidth, TString newname){ return HelperFunctions::flattenHistogram<TH2>(histo, useWidth, newname); }
template<> TH1F* HelperFunctions::flattenHistogram<TH3D>(TH3D const* histo, bool useWidth, TString newname){ return HelperFunctions::flattenHistogram<TH3>(histo, useWidth, newname); }

template<> void HelperFunctions::divideBinWidth<TH1F>(TH1F* histo){
  TAxis const* xaxis = histo->GetXaxis();
  for (int binx=1; binx<=histo->GetNbinsX(); binx++){
    float binwidthX = xaxis->GetBinWidth(binx);
    histo->SetBinContent(binx, histo->GetBinContent(binx)/binwidthX);
    histo->SetBinError(binx, histo->GetBinError(binx)/binwidthX);
  }
}
template<> void HelperFunctions::divideBinWidth<TH2F>(TH2F* histo){
  TAxis const* xaxis = histo->GetXaxis();
  TAxis const* yaxis = histo->GetYaxis();
  for (int binx=1; binx<=histo->GetNbinsX(); binx++){
    float binwidthX = xaxis->GetBinWidth(binx);
    for (int biny=1; biny<=histo->GetNbinsY(); biny++){
      float binwidthY = yaxis->GetBinWidth(biny);
      float binwidth=binwidthX*binwidthY;
      histo->SetBinContent(binx, biny, histo->GetBinContent(binx, biny)/binwidth);
      histo->SetBinError(binx, biny, histo->GetBinError(binx, biny)/binwidth);
    }
  }
}
template<> void HelperFunctions::divideBinWidth<TH3F>(TH3F* histo){
  TAxis const* xaxis = histo->GetXaxis();
  TAxis const* yaxis = histo->GetYaxis();
  TAxis const* zaxis = histo->GetZaxis();
  for (int binx=1; binx<=histo->GetNbinsX(); binx++){
    float binwidthX = xaxis->GetBinWidth(binx);
    for (int biny=1; biny<=histo->GetNbinsY(); biny++){
      float binwidthY = yaxis->GetBinWidth(biny);
      for (int binz=1; binz<=histo->GetNbinsZ(); binz++){
        float binwidthZ = zaxis->GetBinWidth(binz);
        float binwidth=binwidthX*binwidthY*binwidthZ;
        histo->SetBinContent(binx, biny, binz, histo->GetBinContent(binx, biny, binz)/binwidth);
        histo->SetBinError(binx, biny, binz, histo->GetBinError(binx, biny, binz)/binwidth);
      }
    }
  }
}

template<> void HelperFunctions::multiplyBinWidth<TH1F>(TH1F* histo){
  TAxis const* xaxis = histo->GetXaxis();
  for (int binx=1; binx<=histo->GetNbinsX(); binx++){
    float binwidthX = xaxis->GetBinWidth(binx);
    histo->SetBinContent(binx, histo->GetBinContent(binx)*binwidthX);
    histo->SetBinError(binx, histo->GetBinError(binx)*binwidthX);
  }
}
template<> void HelperFunctions::multiplyBinWidth<TH2F>(TH2F* histo){
  TAxis const* xaxis = histo->GetXaxis();
  TAxis const* yaxis = histo->GetYaxis();
  for (int binx=1; binx<=histo->GetNbinsX(); binx++){
    float binwidthX = xaxis->GetBinWidth(binx);
    for (int biny=1; biny<=histo->GetNbinsY(); biny++){
      float binwidthY = yaxis->GetBinWidth(biny);
      float binwidth=binwidthX*binwidthY;
      histo->SetBinContent(binx, biny, histo->GetBinContent(binx, biny)*binwidth);
      histo->SetBinError(binx, biny, histo->GetBinError(binx, biny)*binwidth);
    }
  }
}
template<> void HelperFunctions::multiplyBinWidth<TH3F>(TH3F* histo){
  TAxis const* xaxis = histo->GetXaxis();
  TAxis const* yaxis = histo->GetYaxis();
  TAxis const* zaxis = histo->GetZaxis();
  for (int binx=1; binx<=histo->GetNbinsX(); binx++){
    float binwidthX = xaxis->GetBinWidth(binx);
    for (int biny=1; biny<=histo->GetNbinsY(); biny++){
      float binwidthY = yaxis->GetBinWidth(biny);
      for (int binz=1; binz<=histo->GetNbinsZ(); binz++){
        float binwidthZ = zaxis->GetBinWidth(binz);
        float binwidth=binwidthX*binwidthY*binwidthZ;
        histo->SetBinContent(binx, biny, binz, histo->GetBinContent(binx, biny, binz)*binwidth);
        histo->SetBinError(binx, biny, binz, histo->GetBinError(binx, biny, binz)*binwidth);
      }
    }
  }
}

template<> float HelperFunctions::computeIntegral<TH1F>(TH1F* histo, bool useWidth){
  TAxis const* xaxis = histo->GetXaxis();
  float sum=0;
  for (int binx=1; binx<=histo->GetNbinsX(); binx++){
    float binwidthX = xaxis->GetBinWidth(binx);
    float bincontent = histo->GetBinContent(binx);
    if (useWidth) bincontent *= binwidthX;
    sum += bincontent;
  }
  return sum;
}
template<> float HelperFunctions::computeIntegral<TH2F>(TH2F* histo, bool useWidth){
  TAxis const* xaxis = histo->GetXaxis();
  TAxis const* yaxis = histo->GetYaxis();
  float sum=0;
  for (int binx=1; binx<=histo->GetNbinsX(); binx++){
    float binwidthX = xaxis->GetBinWidth(binx);
    for (int biny=1; biny<=histo->GetNbinsY(); biny++){
      float binwidthY = yaxis->GetBinWidth(biny);
      float binwidth=binwidthX*binwidthY;
      float bincontent = histo->GetBinContent(binx, biny);
      if (useWidth) bincontent *= binwidth;
      sum += bincontent;
    }
  }
  return sum;
}
template<> float HelperFunctions::computeIntegral<TH3F>(TH3F* histo, bool useWidth){
  TAxis const* xaxis = histo->GetXaxis();
  TAxis const* yaxis = histo->GetYaxis();
  TAxis const* zaxis = histo->GetZaxis();
  float sum=0;
  for (int binx=1; binx<=histo->GetNbinsX(); binx++){
    float binwidthX = xaxis->GetBinWidth(binx);
    for (int biny=1; biny<=histo->GetNbinsY(); biny++){
      float binwidthY = yaxis->GetBinWidth(biny);
      for (int binz=1; binz<=histo->GetNbinsZ(); binz++){
        float binwidthZ = zaxis->GetBinWidth(binz);
        float binwidth=binwidthX*binwidthY*binwidthZ;
        float bincontent = histo->GetBinContent(binx, biny, binz);
        if (useWidth) bincontent *= binwidth;
        sum += bincontent;
      }
    }
  }
  return sum;
}

template<> double HelperFunctions::computeChiSq<TH1F>(TH1F const* h1, TH1F const* h2){
  double res=0; double norm=0;
  if (h1->GetNbinsX()!=h2->GetNbinsX()) return res;
  const int nbinsx = h1->GetNbinsX();
  for (int binx=0; binx<=nbinsx+1; binx++){
    double sumW[2] ={
      h1->GetBinContent(binx),
      h2->GetBinContent(binx)
    };
    double sumWsq[2] ={
      pow(h1->GetBinError(binx), 2),
      pow(h2->GetBinError(binx), 2)
    };
    double totalWsq=(sumWsq[0]+sumWsq[1]);
    if (sumWsq[0]>0. && sumWsq[1]>0.){ res += pow(sumW[1]-sumW[0], 2)/totalWsq; norm+=1; }
  }
  if (norm!=0.) res /= norm;
  return res;
}
template<> double HelperFunctions::computeChiSq<TH2F>(TH2F const* h1, TH2F const* h2){
  double res=0, norm=0;
  if (h1->GetNbinsX()!=h2->GetNbinsX()) return res;
  if (h1->GetNbinsY()!=h2->GetNbinsY()) return res;
  const int nbinsx = h1->GetNbinsX();
  const int nbinsy = h1->GetNbinsY();
  for (int binx=0; binx<=nbinsx+1; binx++){
    for (int biny=0; biny<=nbinsy+1; biny++){
      double sumW[2] ={
        h1->GetBinContent(binx, biny),
        h2->GetBinContent(binx, biny)
      };
      double sumWsq[2] ={
        pow(h1->GetBinError(binx, biny), 2),
        pow(h2->GetBinError(binx, biny), 2)
      };
      double totalWsq=(sumWsq[0]+sumWsq[1]);
      if (sumWsq[0]>0. && sumWsq[1]>0.){ res += pow(sumW[1]-sumW[0], 2)/totalWsq; norm+=1; }
    }
  }
  if (norm!=0.) res /= norm;
  return res;
}
template<> double HelperFunctions::computeChiSq<TH3F>(TH3F const* h1, TH3F const* h2){
  double res=0, norm=0;
  if (h1->GetNbinsX()!=h2->GetNbinsX()) return res;
  if (h1->GetNbinsY()!=h2->GetNbinsY()) return res;
  if (h1->GetNbinsZ()!=h2->GetNbinsZ()) return res;
  const int nbinsx = h1->GetNbinsX();
  const int nbinsy = h1->GetNbinsY();
  const int nbinsz = h1->GetNbinsZ();
  for (int binx=0; binx<=nbinsx+1; binx++){
    for (int biny=0; biny<=nbinsy+1; biny++){
      for (int binz=0; binz<=nbinsz+1; binz++){
        double sumW[2] ={
          h1->GetBinContent(binx, biny, binz),
          h2->GetBinContent(binx, biny, binz)
        };
        double sumWsq[2] ={
          pow(h1->GetBinError(binx, biny, binz), 2),
          pow(h2->GetBinError(binx, biny, binz), 2)
        };
        double totalWsq=(sumWsq[0]+sumWsq[1]);
        if (sumWsq[0]>0. && sumWsq[1]>0.){ res += pow(sumW[1]-sumW[0], 2)/totalWsq; norm+=1; }
      }
    }
  }
  if (norm!=0.) res /= norm;
  return res;
}

template<> void HelperFunctions::divideHistograms<TH1F>(TH1F const* hnum, TH1F const* hden, TH1F*& hAssign, bool useEffErr, TH1F** hAssign_dn, TH1F** hAssign_up){
  if (hnum->GetNbinsX()!=hden->GetNbinsX()) return;
  const int nbinsx = hnum->GetNbinsX();
  for (int binx=0; binx<=nbinsx+1; binx++){
    double sumW = hnum->GetBinContent(binx);
    double sumWAll = hden->GetBinContent(binx);
    double sumWsq = pow(hnum->GetBinError(binx), 2);
    double sumWsqAll = pow(hden->GetBinError(binx), 2);
    double bincontent=0;
    double binerror=0;
    double binerror_dn=0;
    double binerror_up=0;
    if (sumWAll!=0.) bincontent = sumW/sumWAll;
    if (useEffErr){
      binerror = calculateEfficiencyError(sumW, sumWAll, sumWsq, sumWsqAll);
      if (hAssign_dn || hAssign_up){
        StatisticsHelpers::getPoissonEfficiencyConfidenceInterval_Frequentist(sumWAll, sumW, sumWsqAll, StatisticsHelpers::VAL_CL_1SIGMA, binerror_dn, binerror_up);
        binerror_dn = bincontent - binerror_dn;
        binerror_up = binerror_up - bincontent;
      }
    }
    else{
      binerror = calculateSimpleProductError(sumW, sqrt(sumWsq), 1, sumWAll, sqrt(sumWsqAll), -1);
      if (hAssign_dn || hAssign_up){
        double bc_denom_dn, bc_denom_up;
        StatisticsHelpers::getPoissonCountingConfidenceInterval_Frequentist(sumWAll, sumWsqAll, StatisticsHelpers::VAL_CL_1SIGMA, bc_denom_dn, bc_denom_up);

        double bc_num_dn, bc_num_up;
        StatisticsHelpers::getPoissonCountingConfidenceInterval_Frequentist(sumW, sumWsq, StatisticsHelpers::VAL_CL_1SIGMA, bc_num_dn, bc_num_up);

        binerror_dn = sqrt((sumWAll!=0. ? pow(bc_num_dn/sumWAll - bincontent, 2) : 0.) + (bc_denom_up!=0. ? pow(sumW/bc_denom_up - bincontent, 2) : 0.));
        binerror_up = sqrt((sumWAll!=0. ? pow(bc_num_up/sumWAll - bincontent, 2) : 0.) + (bc_denom_dn!=0. ? pow(sumW/bc_denom_dn - bincontent, 2) : 0.));
      }
    }
    if (!checkVarNanInf(bincontent) || !checkVarNanInf(binerror) || !checkVarNanInf(binerror_dn) || !checkVarNanInf(binerror_up)){
      bincontent = binerror = binerror_dn = binerror_up = 0;
    }
    hAssign->SetBinContent(binx, bincontent);
    hAssign->SetBinError(binx, binerror);
    if (hAssign_dn) (*hAssign_dn)->SetBinContent(binx, bincontent-binerror_dn);
    if (hAssign_up) (*hAssign_up)->SetBinContent(binx, bincontent+binerror_up);
  }
}
template<> void HelperFunctions::divideHistograms<TH2F>(TH2F const* hnum, TH2F const* hden, TH2F*& hAssign, bool useEffErr, TH2F** hAssign_dn, TH2F** hAssign_up){
  if (hnum->GetNbinsX()!=hden->GetNbinsX()) return;
  const int nbinsx = hnum->GetNbinsX();
  if (hnum->GetNbinsY()!=hden->GetNbinsY()) return;
  const int nbinsy = hnum->GetNbinsY();
  for (int binx=0; binx<=nbinsx+1; binx++){
    for (int biny=0; biny<=nbinsy+1; biny++){
      double sumW = hnum->GetBinContent(binx, biny);
      double sumWAll = hden->GetBinContent(binx, biny);
      double sumWsq = pow(hnum->GetBinError(binx, biny), 2);
      double sumWsqAll = pow(hden->GetBinError(binx, biny), 2);
      double bincontent=0;
      double binerror=0;
      double binerror_dn=0;
      double binerror_up=0;
      if (sumWAll!=0.) bincontent = sumW/sumWAll;
      if (useEffErr){
        binerror = calculateEfficiencyError(sumW, sumWAll, sumWsq, sumWsqAll);
        if (hAssign_dn || hAssign_up){
          StatisticsHelpers::getPoissonEfficiencyConfidenceInterval_Frequentist(sumWAll, sumW, sumWsqAll, StatisticsHelpers::VAL_CL_1SIGMA, binerror_dn, binerror_up);
          binerror_dn = bincontent - binerror_dn;
          binerror_up = binerror_up - bincontent;
        }
      }
      else{
        binerror = calculateSimpleProductError(sumW, sqrt(sumWsq), 1, sumWAll, sqrt(sumWsqAll), -1);
        if (hAssign_dn || hAssign_up){
          double bc_denom_dn, bc_denom_up;
          StatisticsHelpers::getPoissonCountingConfidenceInterval_Frequentist(sumWAll, sumWsqAll, StatisticsHelpers::VAL_CL_1SIGMA, bc_denom_dn, bc_denom_up);

          double bc_num_dn, bc_num_up;
          StatisticsHelpers::getPoissonCountingConfidenceInterval_Frequentist(sumW, sumWsq, StatisticsHelpers::VAL_CL_1SIGMA, bc_num_dn, bc_num_up);

          binerror_dn = sqrt((sumWAll!=0. ? pow(bc_num_dn/sumWAll - bincontent, 2) : 0.) + (bc_denom_up!=0. ? pow(sumW/bc_denom_up - bincontent, 2) : 0.));
          binerror_up = sqrt((sumWAll!=0. ? pow(bc_num_up/sumWAll - bincontent, 2) : 0.) + (bc_denom_dn!=0. ? pow(sumW/bc_denom_dn - bincontent, 2) : 0.));
        }
      }
      if (!checkVarNanInf(bincontent) || !checkVarNanInf(binerror) || !checkVarNanInf(binerror_dn) || !checkVarNanInf(binerror_up)){
        bincontent = binerror = binerror_dn = binerror_up = 0;
      }
      hAssign->SetBinContent(binx, biny, bincontent);
      hAssign->SetBinError(binx, biny, binerror);
      if (hAssign_dn) (*hAssign_dn)->SetBinContent(binx, biny, bincontent-binerror_dn);
      if (hAssign_up) (*hAssign_up)->SetBinContent(binx, biny, bincontent+binerror_up);
    }
  }
}
template<> void HelperFunctions::divideHistograms<TH3F>(TH3F const* hnum, TH3F const* hden, TH3F*& hAssign, bool useEffErr, TH3F** hAssign_dn, TH3F** hAssign_up){
  if (hnum->GetNbinsX()!=hden->GetNbinsX()) return;
  const int nbinsx = hnum->GetNbinsX();
  if (hnum->GetNbinsY()!=hden->GetNbinsY()) return;
  const int nbinsy = hnum->GetNbinsY();
  if (hnum->GetNbinsZ()!=hden->GetNbinsZ()) return;
  const int nbinsz = hnum->GetNbinsZ();
  for (int binx=0; binx<=nbinsx+1; binx++){
    for (int biny=0; biny<=nbinsy+1; biny++){
      for (int binz=0; binz<=nbinsz+1; binz++){
        double sumW = hnum->GetBinContent(binx, biny, binz);
        double sumWAll = hden->GetBinContent(binx, biny, binz);
        double sumWsq = pow(hnum->GetBinError(binx, biny, binz), 2);
        double sumWsqAll = pow(hden->GetBinError(binx, biny, binz), 2);
        double bincontent=0;
        double binerror=0;
        double binerror_dn=0;
        double binerror_up=0;
        if (sumWAll!=0.) bincontent = sumW/sumWAll;
        if (useEffErr){
          binerror = calculateEfficiencyError(sumW, sumWAll, sumWsq, sumWsqAll);
          if (hAssign_dn || hAssign_up){
            StatisticsHelpers::getPoissonEfficiencyConfidenceInterval_Frequentist(sumWAll, sumW, sumWsqAll, StatisticsHelpers::VAL_CL_1SIGMA, binerror_dn, binerror_up);
            binerror_dn = bincontent - binerror_dn;
            binerror_up = binerror_up - bincontent;
          }
        }
        else{
          binerror = calculateSimpleProductError(sumW, sqrt(sumWsq), 1, sumWAll, sqrt(sumWsqAll), -1);
          if (hAssign_dn || hAssign_up){
            double bc_denom_dn, bc_denom_up;
            StatisticsHelpers::getPoissonCountingConfidenceInterval_Frequentist(sumWAll, sumWsqAll, StatisticsHelpers::VAL_CL_1SIGMA, bc_denom_dn, bc_denom_up);

            double bc_num_dn, bc_num_up;
            StatisticsHelpers::getPoissonCountingConfidenceInterval_Frequentist(sumW, sumWsq, StatisticsHelpers::VAL_CL_1SIGMA, bc_num_dn, bc_num_up);

            binerror_dn = sqrt((sumWAll!=0. ? pow(bc_num_dn/sumWAll - bincontent, 2) : 0.) + (bc_denom_up!=0. ? pow(sumW/bc_denom_up - bincontent, 2) : 0.));
            binerror_up = sqrt((sumWAll!=0. ? pow(bc_num_up/sumWAll - bincontent, 2) : 0.) + (bc_denom_dn!=0. ? pow(sumW/bc_denom_dn - bincontent, 2) : 0.));
          }
        }
        if (!checkVarNanInf(bincontent) || !checkVarNanInf(binerror) || !checkVarNanInf(binerror_dn) || !checkVarNanInf(binerror_up)){
          bincontent = binerror = binerror_dn = binerror_up = 0;
        }
        hAssign->SetBinContent(binx, biny, binz, bincontent);
        hAssign->SetBinError(binx, biny, binz, binerror);
        if (hAssign_dn) (*hAssign_dn)->SetBinContent(binx, biny, binz, bincontent-binerror_dn);
        if (hAssign_up) (*hAssign_up)->SetBinContent(binx, biny, binz, bincontent+binerror_up);
      }
    }
  }
}

template<> void HelperFunctions::multiplyHistograms<TH1F>(TH1F const* h1, TH1F const* h2, TH1F*& hAssign, bool useEffErr){
  if (h1->GetNbinsX()!=h2->GetNbinsX()) return;
  const int nbinsx = h1->GetNbinsX();
  for (int binx=0; binx<=nbinsx+1; binx++){
    float sumW = h1->GetBinContent(binx);
    float sumWAll = h2->GetBinContent(binx);
    float sumWsq = pow(h1->GetBinError(binx), 2);
    float sumWsqAll = pow(h2->GetBinError(binx), 2);
    float bincontent=sumW*sumWAll;
    float binerror=0;
    if (useEffErr) binerror = translateEfficiencyErrorToNumeratorError(sumW, sumWAll, sumWsq, sumWsqAll);
    else binerror = calculateSimpleProductError(sumW, sqrt(sumWsq), 1, sumWAll, sqrt(sumWsqAll), 1);
    if (!checkVarNanInf(bincontent) || !checkVarNanInf(binerror)){
      bincontent=0;
      binerror=0;
    }
    hAssign->SetBinContent(binx, bincontent);
    hAssign->SetBinError(binx, binerror);
  }
}
template<> void HelperFunctions::multiplyHistograms<TH2F>(TH2F const* h1, TH2F const* h2, TH2F*& hAssign, bool useEffErr){
  if (h1->GetNbinsX()!=h2->GetNbinsX()) return;
  const int nbinsx = h1->GetNbinsX();
  if (h1->GetNbinsY()!=h2->GetNbinsY()) return;
  const int nbinsy = h1->GetNbinsY();
  for (int binx=0; binx<=nbinsx+1; binx++){
    for (int biny=0; biny<=nbinsy+1; biny++){
      float sumW = h1->GetBinContent(binx, biny);
      float sumWAll = h2->GetBinContent(binx, biny);
      float sumWsq = pow(h1->GetBinError(binx, biny), 2);
      float sumWsqAll = pow(h2->GetBinError(binx, biny), 2);
      float bincontent=sumW*sumWAll;
      float binerror=0;
      if (useEffErr) binerror = translateEfficiencyErrorToNumeratorError(sumW, sumWAll, sumWsq, sumWsqAll);
      else binerror = calculateSimpleProductError(sumW, sqrt(sumWsq), 1, sumWAll, sqrt(sumWsqAll), 1);
      if (!checkVarNanInf(bincontent) || !checkVarNanInf(binerror)){
        bincontent=0;
        binerror=0;
      }
      hAssign->SetBinContent(binx, biny, bincontent);
      hAssign->SetBinError(binx, biny, binerror);
    }
  }
}
template<> void HelperFunctions::multiplyHistograms<TH3F>(TH3F const* h1, TH3F const* h2, TH3F*& hAssign, bool useEffErr){
  if (h1->GetNbinsX()!=h2->GetNbinsX()) return;
  const int nbinsx = h1->GetNbinsX();
  if (h1->GetNbinsY()!=h2->GetNbinsY()) return;
  const int nbinsy = h1->GetNbinsY();
  if (h1->GetNbinsZ()!=h2->GetNbinsZ()) return;
  const int nbinsz = h1->GetNbinsZ();
  for (int binx=0; binx<=nbinsx+1; binx++){
    for (int biny=0; biny<=nbinsy+1; biny++){
      for (int binz=0; binz<=nbinsz+1; binz++){
        float sumW = h1->GetBinContent(binx, biny, binz);
        float sumWAll = h2->GetBinContent(binx, biny, binz);
        float sumWsq = pow(h1->GetBinError(binx, biny, binz), 2);
        float sumWsqAll = pow(h2->GetBinError(binx, biny, binz), 2);
        float bincontent=sumW*sumWAll;
        float binerror=0;
        if (useEffErr) binerror = translateEfficiencyErrorToNumeratorError(sumW, sumWAll, sumWsq, sumWsqAll);
        else binerror = calculateSimpleProductError(sumW, sqrt(sumWsq), 1, sumWAll, sqrt(sumWsqAll), 1);
        if (!checkVarNanInf(bincontent) || !checkVarNanInf(binerror)){
          bincontent=0;
          binerror=0;
        }
        hAssign->SetBinContent(binx, biny, binz, bincontent);
        hAssign->SetBinError(binx, biny, binz, binerror);
      }
    }
  }
}

template<> void HelperFunctions::multiplyHistograms<TH2F>(TH2F const* h1, TH1F const* h2, unsigned int matchDimension, TH2F*& hAssign, bool useEffErr){
  if (matchDimension>1) assert(0);
  if (matchDimension==0 && h1->GetNbinsX()!=h2->GetNbinsX()) return;
  const int nbinsx = h1->GetNbinsX();
  if (matchDimension==1 && h1->GetNbinsY()!=h2->GetNbinsX()) return;
  const int nbinsy = h1->GetNbinsY();
  for (int binx=0; binx<=nbinsx+1; binx++){
    for (int biny=0; biny<=nbinsy+1; biny++){
      float sumW = h1->GetBinContent(binx, biny);
      float sumWsq = pow(h1->GetBinError(binx, biny), 2);
      float sumWAll=0, sumWsqAll=0;
      switch (matchDimension){
      case 0:
        sumWAll = h2->GetBinContent(binx);
        sumWsqAll = pow(h2->GetBinError(binx), 2);
        break;
      case 1:
        sumWAll = h2->GetBinContent(biny);
        sumWsqAll = pow(h2->GetBinError(biny), 2);
        break;
      }
      float bincontent=sumW*sumWAll;
      float binerror=0;
      if (useEffErr) binerror = translateEfficiencyErrorToNumeratorError(sumW, sumWAll, sumWsq, sumWsqAll);
      else binerror = calculateSimpleProductError(sumW, sqrt(sumWsq), 1, sumWAll, sqrt(sumWsqAll), 1);
      if (!checkVarNanInf(bincontent) || !checkVarNanInf(binerror)){
        bincontent=0;
        binerror=0;
      }
      hAssign->SetBinContent(binx, biny, bincontent);
      hAssign->SetBinError(binx, biny, binerror);
    }
  }
}
template<> void HelperFunctions::multiplyHistograms<TH3F>(TH3F const* h1, TH1F const* h2, unsigned int matchDimension, TH3F*& hAssign, bool useEffErr){
  if (matchDimension>2) assert(0);
  if (matchDimension==0 && h1->GetNbinsX()!=h2->GetNbinsX()) return;
  const int nbinsx = h1->GetNbinsX();
  if (matchDimension==1 && h1->GetNbinsY()!=h2->GetNbinsX()) return;
  const int nbinsy = h1->GetNbinsY();
  if (matchDimension==2 && h1->GetNbinsZ()!=h2->GetNbinsX()) return;
  const int nbinsz = h1->GetNbinsZ();
  for (int binx=0; binx<=nbinsx+1; binx++){
    for (int biny=0; biny<=nbinsy+1; biny++){
      for (int binz=0; binz<=nbinsz+1; binz++){
        float sumW = h1->GetBinContent(binx, biny, binz);
        float sumWsq = pow(h1->GetBinError(binx, biny, binz), 2);
        float sumWAll=0, sumWsqAll=0;
        switch (matchDimension){
        case 0:
          sumWAll = h2->GetBinContent(binx);
          sumWsqAll = pow(h2->GetBinError(binx), 2);
          break;
        case 1:
          sumWAll = h2->GetBinContent(biny);
          sumWsqAll = pow(h2->GetBinError(biny), 2);
          break;
        case 2:
          sumWAll = h2->GetBinContent(binz);
          sumWsqAll = pow(h2->GetBinError(binz), 2);
          break;
        }
        float bincontent=sumW*sumWAll;
        float binerror=0;
        if (useEffErr) binerror = translateEfficiencyErrorToNumeratorError(sumW, sumWAll, sumWsq, sumWsqAll);
        else binerror = calculateSimpleProductError(sumW, sqrt(sumWsq), 1, sumWAll, sqrt(sumWsqAll), 1);
        if (!checkVarNanInf(bincontent) || !checkVarNanInf(binerror)){
          bincontent=0;
          binerror=0;
        }
        hAssign->SetBinContent(binx, biny, binz, bincontent);
        hAssign->SetBinError(binx, biny, binz, binerror);
      }
    }
  }
}

template <> void HelperFunctions::symmetrizeHistogram<TH1F>(TH1F* histo, unsigned int const /*axis*/){
  if (!histo) return;
  const int nbinsx = histo->GetNbinsX();
  const int ixlast = nbinsx/2;
  for (int ix=1; ix<=ixlast; ix++){
    int jx = nbinsx-ix+1;
    float cfirst = histo->GetBinContent(ix);
    float csecond = histo->GetBinContent(jx);
    float c = (cfirst+csecond)/2.;
    float efirst = histo->GetBinError(ix);
    float esecond = histo->GetBinError(jx);
    float e = sqrt(pow(efirst, 2)+pow(esecond, 2))/2.;
    histo->SetBinContent(ix, c);
    histo->SetBinContent(jx, c);
    histo->SetBinError(ix, e);
    histo->SetBinError(jx, e);
  }
}
template <> void HelperFunctions::symmetrizeHistogram<TH2F>(TH2F* histo, unsigned int const axis){
  if (!histo || axis>1) return;
  const int nbinsx = histo->GetNbinsX();
  const int ixlast = (axis==0 ? nbinsx/2 : nbinsx);
  const int nbinsy = histo->GetNbinsY();
  const int iylast = (axis==1 ? nbinsy/2 : nbinsy);
  for (int ix=1; ix<=ixlast; ix++){
    int jx = (axis==0 ? nbinsx-ix+1 : ix);
    for (int iy=1; iy<=iylast; iy++){
      int jy = (axis==1 ? nbinsy-iy+1 : iy);
      float cfirst = histo->GetBinContent(ix, iy);
      float csecond = histo->GetBinContent(jx, jy);
      float c = (cfirst+csecond)/2.;
      float efirst = histo->GetBinError(ix, iy);
      float esecond = histo->GetBinError(jx, jy);
      float e = sqrt(pow(efirst, 2)+pow(esecond, 2))/2.;
      histo->SetBinContent(ix, iy, c);
      histo->SetBinContent(jx, jy, c);
      histo->SetBinError(ix, iy, e);
      histo->SetBinError(jx, jy, e);
    }
  }
}
template <> void HelperFunctions::symmetrizeHistogram<TH3F>(TH3F* histo, unsigned int const axis){
  if (!histo || axis>2) return;
  const int nbinsx = histo->GetNbinsX();
  const int ixlast = (axis==0 ? nbinsx/2 : nbinsx);
  const int nbinsy = histo->GetNbinsY();
  const int iylast = (axis==1 ? nbinsy/2 : nbinsy);
  const int nbinsz = histo->GetNbinsZ();
  const int izlast = (axis==2 ? nbinsz/2 : nbinsz);
  for (int ix=1; ix<=ixlast; ix++){
    int jx = (axis==0 ? nbinsx-ix+1 : ix);
    for (int iy=1; iy<=iylast; iy++){
      int jy = (axis==1 ? nbinsy-iy+1 : iy);
      for (int iz=1; iz<=izlast; iz++){
        int jz = (axis==2 ? nbinsz-iz+1 : iz);
        float cfirst = histo->GetBinContent(ix, iy, iz);
        float csecond = histo->GetBinContent(jx, jy, jz);
        float c = (cfirst+csecond)/2.;
        float efirst = histo->GetBinError(ix, iy, iz);
        float esecond = histo->GetBinError(jx, jy, jz);
        float e = sqrt(pow(efirst, 2)+pow(esecond, 2))/2.;
        histo->SetBinContent(ix, iy, iz, c);
        histo->SetBinContent(jx, jy, jz, c);
        histo->SetBinError(ix, iy, iz, e);
        histo->SetBinError(jx, jy, jz, e);
      }
    }
  }
}

template <> void HelperFunctions::antisymmetrizeHistogram<TH1F>(TH1F* histo, unsigned int const /*axis*/){
  if (!histo) return;
  const int nbinsx = histo->GetNbinsX();
  const int ixlast = nbinsx/2;
  for (int ix=1; ix<=ixlast; ix++){
    int jx = nbinsx-ix+1;
    float cfirst = histo->GetBinContent(ix);
    float csecond = histo->GetBinContent(jx);
    float c = (cfirst-csecond)/2.;
    float efirst = histo->GetBinError(ix);
    float esecond = histo->GetBinError(jx);
    float e = sqrt(pow(efirst, 2)+pow(esecond, 2))/2.;
    histo->SetBinContent(ix, c);
    histo->SetBinContent(jx, -c);
    histo->SetBinError(ix, e);
    histo->SetBinError(jx, e);
  }
}
template <> void HelperFunctions::antisymmetrizeHistogram<TH2F>(TH2F* histo, unsigned int const axis){
  if (!histo || axis>1) return;
  const int nbinsx = histo->GetNbinsX();
  const int ixlast = (axis==0 ? nbinsx/2 : nbinsx);
  const int nbinsy = histo->GetNbinsY();
  const int iylast = (axis==1 ? nbinsy/2 : nbinsy);
  for (int ix=1; ix<=ixlast; ix++){
    int jx = (axis==0 ? nbinsx-ix+1 : ix);
    for (int iy=1; iy<=iylast; iy++){
      int jy = (axis==1 ? nbinsy-iy+1 : iy);
      float cfirst = histo->GetBinContent(ix, iy);
      float csecond = histo->GetBinContent(jx, jy);
      float c = (cfirst-csecond)/2.;
      float efirst = histo->GetBinError(ix, iy);
      float esecond = histo->GetBinError(jx, jy);
      float e = sqrt(pow(efirst, 2)+pow(esecond, 2))/2.;
      histo->SetBinContent(ix, iy, c);
      histo->SetBinContent(jx, jy, -c);
      histo->SetBinError(ix, iy, e);
      histo->SetBinError(jx, jy, e);
    }
  }
}
template <> void HelperFunctions::antisymmetrizeHistogram<TH3F>(TH3F* histo, unsigned int const axis){
  if (!histo || axis>2) return;
  const int nbinsx = histo->GetNbinsX();
  const int ixlast = (axis==0 ? nbinsx/2 : nbinsx);
  const int nbinsy = histo->GetNbinsY();
  const int iylast = (axis==1 ? nbinsy/2 : nbinsy);
  const int nbinsz = histo->GetNbinsZ();
  const int izlast = (axis==2 ? nbinsz/2 : nbinsz);
  for (int ix=1; ix<=ixlast; ix++){
    int jx = (axis==0 ? nbinsx-ix+1 : ix);
    for (int iy=1; iy<=iylast; iy++){
      int jy = (axis==1 ? nbinsy-iy+1 : iy);
      for (int iz=1; iz<=izlast; iz++){
        int jz = (axis==2 ? nbinsz-iz+1 : iz);
        float cfirst = histo->GetBinContent(ix, iy, iz);
        float csecond = histo->GetBinContent(jx, jy, jz);
        float c = (cfirst-csecond)/2.;
        float efirst = histo->GetBinError(ix, iy, iz);
        float esecond = histo->GetBinError(jx, jy, jz);
        float e = sqrt(pow(efirst, 2)+pow(esecond, 2))/2.;
        histo->SetBinContent(ix, iy, iz, c);
        histo->SetBinContent(jx, jy, jz, -c);
        histo->SetBinError(ix, iy, iz, e);
        histo->SetBinError(jx, jy, jz, e);
      }
    }
  }
}

template <> void HelperFunctions::getCumulantHistogram<TH1F>(TH1F const* histo, TH1F*& res, TString newname, std::vector<unsigned int>* /*condDims*/){
  if (!histo) return;
  if (newname=="") newname=Form("Cumulant_%s", histo->GetName());
  delete res;
  res = new TH1F(*histo);
  res->SetName(newname);
  res->Reset("ICES");
  for (int ix=0; ix<=res->GetNbinsX()+1; ix++){
    double sumw=histo->GetBinContent(ix);
    double sumw2=pow(histo->GetBinError(ix), 2);
    if (ix>0){
      sumw += res->GetBinContent(ix-1);
      sumw2 += pow(res->GetBinError(ix-1), 2);
    }
    res->SetBinContent(ix, sumw);
    res->SetBinError(ix, sqrt(sumw2));
  }
}
template <> void HelperFunctions::getCumulantHistogram<TH2F>(TH2F const* histo, TH2F*& res, TString newname, std::vector<unsigned int>* condDims){
  if (!histo) return;
  if (newname=="") newname=Form("Cumulant_%s", histo->GetName());
  delete res;
  res = new TH2F(*histo);
  res->SetName(newname);

  bool isXcond=false;
  bool isYcond=false;
  if (condDims){
    for (unsigned int const& idim:*condDims){
      isXcond |= (idim==0);
      isYcond |= (idim==1);
    }
  }
  if (!(isXcond && isYcond)){
    res->Reset("ICES");
    for (int ix=0; ix<=res->GetNbinsX()+1; ix++){
      for (int iy=0; iy<=res->GetNbinsY()+1; iy++){
        double sumw=histo->GetBinContent(ix, iy);
        double sumw2=pow(histo->GetBinError(ix, iy), 2);
        if (ix>0 && iy>0){
          if (!isXcond){
            sumw += res->GetBinContent(ix-1, iy);
            sumw2 += pow(res->GetBinError(ix-1, iy), 2);
          }
          if (!isYcond){
            sumw += res->GetBinContent(ix, iy-1);
            sumw2 += pow(res->GetBinError(ix, iy-1), 2);
          }
          if (!isXcond && !isYcond){
            sumw -= res->GetBinContent(ix-1, iy-1);
            sumw2 -= pow(res->GetBinError(ix-1, iy-1), 2);
          }
        }
        else if (ix>0){
          if (!isXcond){
            sumw += res->GetBinContent(ix-1, iy);
            sumw2 += pow(res->GetBinError(ix-1, iy), 2);
          }
        }
        else if (iy>0){
          if (!isYcond){
            sumw += res->GetBinContent(ix, iy-1);
            sumw2 += pow(res->GetBinError(ix, iy-1), 2);
          }
        }
        res->SetBinContent(ix, iy, sumw);
        res->SetBinError(ix, iy, sqrt(sumw2));
      }
    }
  }
}
template <> void HelperFunctions::getCumulantHistogram<TH3F>(TH3F const* histo, TH3F*& res, TString newname, std::vector<unsigned int>* condDims){
  if (!histo) return;
  if (newname=="") newname=Form("Cumulant_%s", histo->GetName());
  delete res;
  res = new TH3F(*histo);
  res->SetName(newname);

  bool isXcond=false;
  bool isYcond=false;
  bool isZcond=false;
  if (condDims){
    for (unsigned int const& idim:*condDims){
      isXcond |= (idim==0);
      isYcond |= (idim==1);
      isZcond |= (idim==2);
    }
  }
  if (!(isXcond && isYcond && isZcond)){
    res->Reset("ICES");
    for (int ix=0; ix<=res->GetNbinsX()+1; ix++){
      for (int iy=0; iy<=res->GetNbinsY()+1; iy++){
        for (int iz=0; iz<=res->GetNbinsZ()+1; iz++){
          double sumw=histo->GetBinContent(ix, iy, iz);
          double sumw2=pow(histo->GetBinError(ix, iy, iz), 2);
          if (ix>0 && iy>0 && iz>0){
            if (!isXcond){
              sumw += res->GetBinContent(ix-1, iy, iz);
              sumw2 += pow(res->GetBinError(ix-1, iy, iz), 2);
            }
            if (!isYcond){
              sumw += res->GetBinContent(ix, iy-1, iz);
              sumw2 += pow(res->GetBinError(ix, iy-1, iz), 2);
            }
            if (!isZcond){
              sumw += res->GetBinContent(ix, iy, iz-1);
              sumw2 += pow(res->GetBinError(ix, iy, iz-1), 2);
            }

            if (!isXcond && !isYcond){
              sumw -= res->GetBinContent(ix-1, iy-1, iz);
              sumw2 -= pow(res->GetBinError(ix-1, iy-1, iz), 2);
            }
            if (!isXcond && !isZcond){
              sumw -= res->GetBinContent(ix-1, iy, iz-1);
              sumw2 -= pow(res->GetBinError(ix-1, iy, iz-1), 2);
            }
            if (!isYcond && !isZcond){
              sumw -= res->GetBinContent(ix, iy-1, iz-1);
              sumw2 -= pow(res->GetBinError(ix, iy-1, iz-1), 2);
            }

            if (!isXcond && !isYcond && !isZcond){
              sumw += res->GetBinContent(ix-1, iy-1, iz-1);
              sumw2 += pow(res->GetBinError(ix-1, iy-1, iz-1), 2);
            }
          }
          else if (ix>0 && iy>0){
            if (!isXcond){
              sumw += res->GetBinContent(ix-1, iy, iz);
              sumw2 += pow(res->GetBinError(ix-1, iy, iz), 2);
            }
            if (!isYcond){
              sumw += res->GetBinContent(ix, iy-1, iz);
              sumw2 += pow(res->GetBinError(ix, iy-1, iz), 2);
            }
            if (!isXcond && !isYcond){
              sumw -= res->GetBinContent(ix-1, iy-1, iz);
              sumw2 -= pow(res->GetBinError(ix-1, iy-1, iz), 2);
            }
          }
          else if (ix>0 && iz>0){
            if (!isXcond){
              sumw += res->GetBinContent(ix-1, iy, iz);
              sumw2 += pow(res->GetBinError(ix-1, iy, iz), 2);
            }
            if (!isZcond){
              sumw += res->GetBinContent(ix, iy, iz-1);
              sumw2 += pow(res->GetBinError(ix, iy, iz-1), 2);
            }
            if (!isXcond && !isZcond){
              sumw -= res->GetBinContent(ix-1, iy, iz-1);
              sumw2 -= pow(res->GetBinError(ix-1, iy, iz-1), 2);
            }
          }
          else if (iy>0 && iz>0){
            if (!isYcond){
              sumw += res->GetBinContent(ix, iy-1, iz);
              sumw2 += pow(res->GetBinError(ix, iy-1, iz), 2);
            }
            if (!isZcond){
              sumw += res->GetBinContent(ix, iy, iz-1);
              sumw2 += pow(res->GetBinError(ix, iy, iz-1), 2);
            }
            if (!isYcond && !isZcond){
              sumw -= res->GetBinContent(ix, iy-1, iz-1);
              sumw2 -= pow(res->GetBinError(ix, iy-1, iz-1), 2);
            }
          }
          else if (ix>0){
            if (!isXcond){
              sumw += res->GetBinContent(ix-1, iy, iz);
              sumw2 += pow(res->GetBinError(ix-1, iy, iz), 2);
            }
          }
          else if (iy>0){
            if (!isYcond){
              sumw += res->GetBinContent(ix, iy-1, iz);
              sumw2 += pow(res->GetBinError(ix, iy-1, iz), 2);
            }
          }
          else if (iz>0){
            if (!isZcond){
              sumw += res->GetBinContent(ix, iy, iz-1);
              sumw2 += pow(res->GetBinError(ix, iy, iz-1), 2);
            }
          }
          res->SetBinContent(ix, iy, iz, sumw);
          res->SetBinError(ix, iy, iz, sqrt(sumw2));
        }
      }
    }
  }
}

template <> void HelperFunctions::translateCumulantToHistogram<TH1F>(TH1F const* histo, TH1F*& res, TString newname, std::vector<unsigned int>* /*condDims*/){
  if (!histo) return;
  if (newname=="") newname=Form("h_%s", histo->GetName());
  delete res;
  res = new TH1F(*histo);
  res->SetName(newname);
  res->Reset("ICES");
  for (int ix=0; ix<=res->GetNbinsX()+1; ix++){
    double sumw=histo->GetBinContent(ix);
    double sumw2=pow(histo->GetBinError(ix), 2);
    if (ix>0){
      sumw -= histo->GetBinContent(ix-1);
      sumw2 -= pow(histo->GetBinError(ix-1), 2);
    }
    if (sumw2<0.) sumw2=0;
    res->SetBinContent(ix, sumw);
    res->SetBinError(ix, sqrt(sumw2));
  }
}
template <> void HelperFunctions::translateCumulantToHistogram<TH2F>(TH2F const* histo, TH2F*& res, TString newname, std::vector<unsigned int>* condDims){
  if (!histo) return;
  if (newname=="") newname=Form("h_%s", histo->GetName());
  delete res;
  res = new TH2F(*histo);
  res->SetName(newname);

  bool isXcond=false;
  bool isYcond=false;
  if (condDims){
    for (unsigned int const& idim:*condDims){
      isXcond |= (idim==0);
      isYcond |= (idim==1);
    }
  }
  if (!(isXcond && isYcond)){
    res->Reset("ICES");
    for (int ix=0; ix<=res->GetNbinsX()+1; ix++){
      for (int iy=0; iy<=res->GetNbinsY()+1; iy++){
        double sumw=histo->GetBinContent(ix, iy);
        double sumw2=pow(histo->GetBinError(ix, iy), 2);
        if (ix>0 && iy>0){
          if (!isXcond){
            sumw -= histo->GetBinContent(ix-1, iy);
            sumw2 -= pow(histo->GetBinError(ix-1, iy), 2);
          }
          if (!isYcond){
            sumw -= histo->GetBinContent(ix, iy-1);
            sumw2 -= pow(histo->GetBinError(ix, iy-1), 2);
          }
          if (!isXcond && !isYcond){
            sumw += histo->GetBinContent(ix-1, iy-1);
            sumw2 += pow(histo->GetBinError(ix-1, iy-1), 2);
          }
        }
        else if (ix>0){
          if (!isXcond){
            sumw -= histo->GetBinContent(ix-1, iy);
            sumw2 -= pow(histo->GetBinError(ix-1, iy), 2);
          }
        }
        else if (iy>0){
          if (!isYcond){
            sumw -= histo->GetBinContent(ix, iy-1);
            sumw2 -= pow(histo->GetBinError(ix, iy-1), 2);
          }
        }
        if (sumw2<0.) sumw2=0;
        res->SetBinContent(ix, iy, sumw);
        res->SetBinError(ix, iy, sqrt(sumw2));
      }
    }
  }
}
template <> void HelperFunctions::translateCumulantToHistogram<TH3F>(TH3F const* histo, TH3F*& res, TString newname, std::vector<unsigned int>* condDims){
  if (!histo) return;
  if (newname=="") newname=Form("h_%s", histo->GetName());
  delete res;
  res = new TH3F(*histo);
  res->SetName(newname);

  bool isXcond=false;
  bool isYcond=false;
  bool isZcond=false;
  if (condDims){
    for (unsigned int const& idim:*condDims){
      isXcond |= (idim==0);
      isYcond |= (idim==1);
      isZcond |= (idim==2);
    }
  }
  if (!(isXcond && isYcond && isZcond)){
    res->Reset("ICES");
    for (int ix=0; ix<=res->GetNbinsX()+1; ix++){
      for (int iy=0; iy<=res->GetNbinsY()+1; iy++){
        for (int iz=0; iz<=histo->GetNbinsZ()+1; iz++){
          double sumw=histo->GetBinContent(ix, iy, iz);
          double sumw2=pow(histo->GetBinError(ix, iy, iz), 2);
          if (ix>0 && iy>0 && iz>0){
            if (!isXcond){
              sumw -= histo->GetBinContent(ix-1, iy, iz);
              sumw2 -= pow(histo->GetBinError(ix-1, iy, iz), 2);
            }
            if (!isYcond){
              sumw -= histo->GetBinContent(ix, iy-1, iz);
              sumw2 -= pow(histo->GetBinError(ix, iy-1, iz), 2);
            }
            if (!isZcond){
              sumw -= histo->GetBinContent(ix, iy, iz-1);
              sumw2 -= pow(histo->GetBinError(ix, iy, iz-1), 2);
            }

            if (!isXcond && !isYcond){
              sumw += histo->GetBinContent(ix-1, iy-1, iz);
              sumw2 += pow(histo->GetBinError(ix-1, iy-1, iz), 2);
            }
            if (!isXcond && !isZcond){
              sumw += histo->GetBinContent(ix-1, iy, iz-1);
              sumw2 += pow(histo->GetBinError(ix-1, iy, iz-1), 2);
            }
            if (!isYcond && !isZcond){
              sumw += histo->GetBinContent(ix, iy-1, iz-1);
              sumw2 += pow(histo->GetBinError(ix, iy-1, iz-1), 2);
            }

            if (!isXcond && !isYcond && !isZcond){
              sumw -= histo->GetBinContent(ix-1, iy-1, iz-1);
              sumw2 -= pow(histo->GetBinError(ix-1, iy-1, iz-1), 2);
            }
          }
          else if (ix>0 && iy>0){
            if (!isXcond){
              sumw -= histo->GetBinContent(ix-1, iy, iz);
              sumw2 -= pow(histo->GetBinError(ix-1, iy, iz), 2);
            }
            if (!isYcond){
              sumw -= histo->GetBinContent(ix, iy-1, iz);
              sumw2 -= pow(histo->GetBinError(ix, iy-1, iz), 2);
            }
            if (!isXcond && !isYcond){
              sumw += histo->GetBinContent(ix-1, iy-1, iz);
              sumw2 += pow(histo->GetBinError(ix-1, iy-1, iz), 2);
            }
          }
          else if (ix>0 && iz>0){
            if (!isXcond){
              sumw -= histo->GetBinContent(ix-1, iy, iz);
              sumw2 -= pow(histo->GetBinError(ix-1, iy, iz), 2);
            }
            if (!isZcond){
              sumw -= histo->GetBinContent(ix, iy, iz-1);
              sumw2 -= pow(histo->GetBinError(ix, iy, iz-1), 2);
            }
            if (!isXcond && !isZcond){
              sumw += histo->GetBinContent(ix-1, iy, iz-1);
              sumw2 += pow(histo->GetBinError(ix-1, iy, iz-1), 2);
            }
          }
          else if (iy>0 && iz>0){
            if (!isYcond){
              sumw -= histo->GetBinContent(ix, iy-1, iz);
              sumw2 -= pow(histo->GetBinError(ix, iy-1, iz), 2);
            }
            if (!isZcond){
              sumw -= histo->GetBinContent(ix, iy, iz-1);
              sumw2 -= pow(histo->GetBinError(ix, iy, iz-1), 2);
            }
            if (!isYcond && !isZcond){
              sumw += histo->GetBinContent(ix, iy-1, iz-1);
              sumw2 += pow(histo->GetBinError(ix, iy-1, iz-1), 2);
            }
          }
          else if (ix>0){
            if (!isXcond){
              sumw -= histo->GetBinContent(ix-1, iy, iz);
              sumw2 -= pow(histo->GetBinError(ix-1, iy, iz), 2);
            }
          }
          else if (iy>0){
            if (!isYcond){
              sumw -= histo->GetBinContent(ix, iy-1, iz);
              sumw2 -= pow(histo->GetBinError(ix, iy-1, iz), 2);
            }
          }
          else if (iz>0){
            if (!isZcond){
              sumw -= histo->GetBinContent(ix, iy, iz-1);
              sumw2 -= pow(histo->GetBinError(ix, iy, iz-1), 2);
            }
          }
          if (sumw2<0.) sumw2=0;
          res->SetBinContent(ix, iy, iz, sumw);
          res->SetBinError(ix, iy, iz, sqrt(sumw2));
        }
      }
    }
  }
}

template <> void HelperFunctions::combineHistogramListByWeightedAverage<TProfile>(std::vector<TProfile const*> const& hList, TProfile*& hAssign, bool /*useNeff*/){
  const int nbinsx = hList.at(0)->GetNbinsX();
  for (auto const& h:hList){
    if (h->GetNbinsX()!=nbinsx) return;
  }
  for (int binx=0; binx<=nbinsx+1; binx++){
    float bincontent=0;
    float binerror=0;
    for (auto const& h:hList){
      double bc=h->GetBinContent(binx);
      double be=pow(h->GetBinError(binx), 2);
      if (be>0.){
        bincontent += bc/be;
        binerror += 1./be;
      }
    }
    if (binerror>0.){
      bincontent /= binerror;
      binerror = sqrt(1./binerror);
    }
    if (!checkVarNanInf(bincontent) || !checkVarNanInf(binerror)){
      bincontent=0;
      binerror=0;
    }
    hAssign->SetBinEntries(binx, (binerror>0. ? std::min(1., double(bincontent/binerror)) : 1.));
    hAssign->SetBinContent(binx, bincontent);
    hAssign->SetBinError(binx, binerror);
  }
}
template <> void HelperFunctions::combineHistogramListByWeightedAverage<TH1F>(std::vector<TH1F const*> const& hList, TH1F*& hAssign, bool useNeff){
  const int nbinsx = hList.at(0)->GetNbinsX();
  for (auto const& h:hList){
    if (h->GetNbinsX()!=nbinsx) return;
  }
  for (int binx=0; binx<=nbinsx+1; binx++){
    float bincontent=0;
    float binerror=0;
    float norm=0;
    for (auto const& h:hList){
      double bc=h->GetBinContent(binx);
      double be=pow(h->GetBinError(binx), 2);
      if (be>0.){
        if (!useNeff){
          bincontent += bc/be;
          binerror += 1./be;
        }
        else{
          double neff = pow(bc, 2)/be;
          bincontent += bc*neff;
          binerror += be*pow(neff, 2);
          norm += neff;
        }
      }
    }
    if (useNeff){
      if (norm>0.){
        bincontent /= norm;
        binerror = sqrt(binerror)/norm;
      }
    }
    else if (binerror>0.){
      bincontent /= binerror;
      binerror = sqrt(1./binerror);
    }
    if (!checkVarNanInf(bincontent) || !checkVarNanInf(binerror)){
      bincontent=0;
      binerror=0;
    }
    hAssign->SetBinContent(binx, bincontent);
    hAssign->SetBinError(binx, binerror);
  }
}
template <> void HelperFunctions::combineHistogramListByWeightedAverage<TH2F>(std::vector<TH2F const*> const& hList, TH2F*& hAssign, bool useNeff){
  const int nbinsx = hList.at(0)->GetNbinsX();
  const int nbinsy = hList.at(0)->GetNbinsY();
  for (auto const& h:hList){
    if (h->GetNbinsX()!=nbinsx) return;
    if (h->GetNbinsY()!=nbinsy) return;
  }
  for (int binx=0; binx<=nbinsx+1; binx++){
    for (int biny=0; biny<=nbinsy+1; biny++){
      float bincontent=0;
      float binerror=0;
      float norm=0;
      for (auto const& h:hList){
        double bc=h->GetBinContent(binx, biny);
        double be=pow(h->GetBinError(binx, biny), 2);
        if (be>0.){
          if (!useNeff){
            bincontent += bc/be;
            binerror += 1./be;
          }
          else{
            double neff = pow(bc, 2)/be;
            bincontent += bc*neff;
            binerror += be*pow(neff, 2);
            norm += neff;
          }
        }
      }
      if (useNeff){
        if (norm>0.){
          bincontent /= norm;
          binerror = sqrt(binerror)/norm;
        }
      }
      else if (binerror>0.){
        bincontent /= binerror;
        binerror = sqrt(1./binerror);
      }
      if (!checkVarNanInf(bincontent) || !checkVarNanInf(binerror)){
        bincontent=0;
        binerror=0;
      }
      hAssign->SetBinContent(binx, biny, bincontent);
      hAssign->SetBinError(binx, biny, binerror);
    }
  }
}
template <> void HelperFunctions::combineHistogramListByWeightedAverage<TH3F>(std::vector<TH3F const*> const& hList, TH3F*& hAssign, bool useNeff){
  const int nbinsx = hList.at(0)->GetNbinsX();
  const int nbinsy = hList.at(0)->GetNbinsY();
  const int nbinsz = hList.at(0)->GetNbinsZ();
  for (auto const& h:hList){
    if (h->GetNbinsX()!=nbinsx) return;
    if (h->GetNbinsY()!=nbinsy) return;
    if (h->GetNbinsZ()!=nbinsz) return;
  }
  for (int binx=0; binx<=nbinsx+1; binx++){
    for (int biny=0; biny<=nbinsy+1; biny++){
      for (int binz=0; binz<=nbinsz+1; binz++){
        float bincontent=0;
        float binerror=0;
        float norm=0;
        for (auto const& h:hList){
          double bc=h->GetBinContent(binx, biny, binz);
          double be=pow(h->GetBinError(binx, biny, binz), 2);
          if (be>0.){
            if (!useNeff){
              bincontent += bc/be;
              binerror += 1./be;
            }
            else{
              double neff = pow(bc, 2)/be;
              bincontent += bc*neff;
              binerror += be*pow(neff, 2);
              norm += neff;
            }
          }
        }
        if (useNeff){
          if (norm>0.){
            bincontent /= norm;
            binerror = sqrt(binerror)/norm;
          }
        }
        else if (binerror>0.){
          bincontent /= binerror;
          binerror = sqrt(1./binerror);
        }
        if (!checkVarNanInf(bincontent) || !checkVarNanInf(binerror)){
          bincontent=0;
          binerror=0;
        }
        hAssign->SetBinContent(binx, biny, binz, bincontent);
        hAssign->SetBinError(binx, biny, binz, binerror);
      }
    }
  }
}

template <> void HelperFunctions::findBinContentRange<TProfile>(TProfile const* h, float& bcmin, float& bcmax, bool includeBinErrors, bool includeOverUnderflows, bool onlyPositiveBins){
  const int nbinsx = h->GetNbinsX();
  bool firstBin = true;
  for (int binx=(includeOverUnderflows ? 0 : 1); binx<=nbinsx+(includeOverUnderflows ? 1 : 0); binx++){
    float bc = h->GetBinContent(binx);
    float be = h->GetBinError(binx);
    if (onlyPositiveBins && bc<=0.f) continue;
    float tmpbcmin = (includeBinErrors && !(onlyPositiveBins && bc<=be) ? bc-be : bc);
    float tmpbcmax = (includeBinErrors ? bc+be : bc);
    if (firstBin){
      bcmin=tmpbcmin;
      bcmax=tmpbcmax;
      firstBin=false;
    }
    else{
      bcmin=std::min(bcmin, tmpbcmin);
      bcmax=std::max(bcmax, tmpbcmax);
    }
  }
}
template <> void HelperFunctions::findBinContentRange<TH1F>(TH1F const* h, float& bcmin, float& bcmax, bool includeBinErrors, bool includeOverUnderflows, bool onlyPositiveBins){
  const int nbinsx = h->GetNbinsX();
  bool firstBin = true;
  for (int binx=(includeOverUnderflows ? 0 : 1); binx<=nbinsx+(includeOverUnderflows ? 1 : 0); binx++){
    float bc = h->GetBinContent(binx);
    float be = h->GetBinError(binx);
    if (onlyPositiveBins && bc<=0.f) continue;
    float tmpbcmin = (includeBinErrors && !(onlyPositiveBins && bc<=be) ? bc-be : bc);
    float tmpbcmax = (includeBinErrors ? bc+be : bc);
    if (firstBin){
      bcmin=tmpbcmin;
      bcmax=tmpbcmax;
      firstBin=false;
    }
    else{
      bcmin=std::min(bcmin, tmpbcmin);
      bcmax=std::max(bcmax, tmpbcmax);
    }
  }
}
template <> void HelperFunctions::findBinContentRange<TH2F>(TH2F const* h, float& bcmin, float& bcmax, bool includeBinErrors, bool includeOverUnderflows, bool onlyPositiveBins){
  const int nbinsx = h->GetNbinsX();
  const int nbinsy = h->GetNbinsY();
  bool firstBin = true;
  for (int binx=(includeOverUnderflows ? 0 : 1); binx<=nbinsx+(includeOverUnderflows ? 1 : 0); binx++){
    for (int biny=(includeOverUnderflows ? 0 : 1); biny<=nbinsy+(includeOverUnderflows ? 1 : 0); biny++){
      float bc = h->GetBinContent(binx, biny);
      float be = h->GetBinError(binx, biny);
      if (onlyPositiveBins && bc<=0.f) continue;
      float tmpbcmin = (includeBinErrors && !(onlyPositiveBins && bc<=be) ? bc-be : bc);
      float tmpbcmax = (includeBinErrors ? bc+be : bc);
      if (firstBin){
        bcmin=tmpbcmin;
        bcmax=tmpbcmax;
        firstBin=false;
      }
      else{
        bcmin=std::min(bcmin, tmpbcmin);
        bcmax=std::max(bcmax, tmpbcmax);
      }
    }
  }
}
template <> void HelperFunctions::findBinContentRange<TH3F>(TH3F const* h, float& bcmin, float& bcmax, bool includeBinErrors, bool includeOverUnderflows, bool onlyPositiveBins){
  const int nbinsx = h->GetNbinsX();
  const int nbinsy = h->GetNbinsY();
  const int nbinsz = h->GetNbinsZ();
  bool firstBin = true;
  for (int binx=(includeOverUnderflows ? 0 : 1); binx<=nbinsx+(includeOverUnderflows ? 1 : 0); binx++){
    for (int biny=(includeOverUnderflows ? 0 : 1); biny<=nbinsy+(includeOverUnderflows ? 1 : 0); biny++){
      for (int binz=(includeOverUnderflows ? 0 : 1); binz<=nbinsz+(includeOverUnderflows ? 1 : 0); binz++){
        float bc = h->GetBinContent(binx, biny, binz);
        float be = h->GetBinError(binx, biny, binz);
        if (onlyPositiveBins && bc<=0.f) continue;
        float tmpbcmin = (includeBinErrors && !(onlyPositiveBins && bc<=be) ? bc-be : bc);
        float tmpbcmax = (includeBinErrors ? bc+be : bc);
        if (firstBin){
          bcmin=tmpbcmin;
          bcmax=tmpbcmax;
          firstBin=false;
        }
        else{
          bcmin=std::min(bcmin, tmpbcmin);
          bcmax=std::max(bcmax, tmpbcmax);
        }
      }
    }
  }
}


void HelperFunctions::rebinProfile(TProfile*& prof, const ExtendedBinning& binningX){
  if (!prof) return;
  if (!binningX.isValid()) IVYerr << "HelperFunctions::rebinProfile: New binning is not valid!" << endl;

  const TString hname=prof->GetName();
  const TString htitle=prof->GetTitle();

  double boundaries[2]={ prof->GetXaxis()->GetBinLowEdge(1), prof->GetXaxis()->GetBinLowEdge(prof->GetNbinsX()+1) };
  double val_under_over[2]={ prof->GetBinContent(0), prof->GetBinContent(prof->GetNbinsX()+1) };
  double err_under_over[2]={ prof->GetBinError(0), prof->GetBinError(prof->GetNbinsX()+1) };
  RooRealVar xvar("xvar", "", boundaries[0], boundaries[1]);

  IvyNCSplineFactory_1D spFac(xvar, "tmpSpline");
  IvyNCSplineFactory_1D spErrFac(xvar, "tmpSplineErr");
  vector<pair<IvyNCSplineCore::T, IvyNCSplineCore::T>> pList, pErrList;
  for (int ix=1; ix<=prof->GetNbinsX(); ix++){
    if (prof->GetBinError(ix)==0.){ IVYout << "HelperFunctions::rebinProfile: Omitting bin " << ix << endl; continue; }
    pList.emplace_back(prof->GetXaxis()->GetBinCenter(ix), prof->GetBinContent(ix));
    pErrList.emplace_back(prof->GetXaxis()->GetBinCenter(ix), pow(prof->GetBinError(ix), 2));
  }
  spFac.setPoints(pList);
  spErrFac.setPoints(pErrList);
  const IvyNCSpline_1D_fast* sp = spFac.getFunc();
  const IvyNCSpline_1D_fast* spErr = spErrFac.getFunc();

  delete prof;
  prof = new TProfile(hname, htitle, binningX.getNbins(), binningX.getBinning());
  for (unsigned int ix=0; ix<binningX.getNbins(); ix++){
    double xval=binningX.getBinCenter(ix);
    double cval=0;
    double errval=0;
    if (xval>=xvar.getMin() && xval<=xvar.getMax()){
      xvar.setVal(xval);
      cval = sp->getVal();
      errval = spErr->getVal();
      if (errval<0.) errval=0.;
    }
    else IVYerr << "HelperFunctions::rebinProfile: X val " << xval << " outside of range " << xvar.getMin() << " , " << xvar.getMax() << endl;
    prof->SetBinEntries(ix+1, 1);
    prof->SetBinContent(ix+1, cval);
    prof->SetBinError(ix+1, sqrt(std::max(0., errval)));
  }
  if (boundaries[0]<=binningX.getBinLowEdge(0)){ prof->SetBinEntries(0, 1); prof->SetBinContent(0, val_under_over[0]); prof->SetBinError(0, err_under_over[0]); }
  if (boundaries[1]>=binningX.getBinLowEdge(binningX.getNbins())){ prof->SetBinEntries(prof->GetNbinsX()+1, 1); prof->SetBinContent(prof->GetNbinsX()+1, val_under_over[1]); prof->SetBinError(prof->GetNbinsX()+1, err_under_over[1]); }
}

void HelperFunctions::rebinCumulant(TH1F*& histo, const ExtendedBinning& binningX){
  if (!histo) return;
  if (!binningX.isValid()){ IVYerr << "HelperFunctions::rebinCumulant: Cannot rebin a 1D cumulant with an invalid binning" << endl; return; }

  const TString hname=histo->GetName();
  const TString htitle=histo->GetTitle();
  RooRealVar xvar(
    "xvar", "",
    std::min(binningX.getBinLowEdge(0), histo->GetXaxis()->GetBinLowEdge(1)),
    std::max(binningX.getBinLowEdge(binningX.getNbins()), histo->GetXaxis()->GetBinLowEdge(histo->GetNbinsX()+1))
  );
  int nx=0;
  IvyNCSplineCore::BoundaryCondition bcBeginX = IvyNCSplineCore::bcQuadraticWithNullSlope;
  IvyNCSplineCore::BoundaryCondition bcEndX = IvyNCSplineCore::bcQuadraticWithNullSlope;
  vector<pair<IvyNCSplineCore::T, IvyNCSplineCore::T>> pList, pErrList;
  for (int ix=0; ix<=histo->GetNbinsX(); ix++){
    pList.emplace_back(histo->GetXaxis()->GetBinUpEdge(ix), histo->GetBinContent(ix));
    pErrList.emplace_back(histo->GetXaxis()->GetBinUpEdge(ix), pow(histo->GetBinError(ix), 2));
    nx++;
  }
  if (nx<=3){ bcBeginX = IvyNCSplineCore::bcNaturalSpline; bcEndX = IvyNCSplineCore::bcNaturalSpline; }
  IvyNCSplineFactory_1D spFac(xvar, "tmpSpline", bcBeginX, bcEndX);
  IvyNCSplineFactory_1D spErrFac(xvar, "tmpSplineErr", bcBeginX, bcEndX);
  spFac.setPoints(pList);
  spErrFac.setPoints(pErrList);
  const IvyNCSpline_1D_fast* sp = spFac.getFunc();
  const IvyNCSpline_1D_fast* spErr = spErrFac.getFunc();

  delete histo;
  histo = new TH1F(hname, htitle, binningX.getNbins(), binningX.getBinning());
  for (unsigned int ix=0; ix<=binningX.getNbins()+1; ix++){
    double xval;
    if (ix==binningX.getNbins()+1) xval=binningX.getBinLowEdge(ix-1);
    else xval=binningX.getBinLowEdge(ix);
    double cval=0;
    double errval=0;
    xvar.setVal(xval);
    cval = sp->getVal();
    errval = spErr->getVal();
    histo->SetBinContent(ix, cval);
    histo->SetBinError(ix, sqrt(std::max(0., errval)));
  }
}
void HelperFunctions::rebinCumulant(TH2F*& histo, const ExtendedBinning& binningX, const ExtendedBinning& binningY, std::vector<std::pair<TProfile const*, unsigned int>>* condProfs){
  if (!histo) return;
  if (!binningX.isValid() || !binningY.isValid()){ IVYerr << "HelperFunctions::rebinCumulant: Cannot rebin a 2D cumulant with an invalid binning" << endl; return; }

  const TString hname=histo->GetName();
  const TString htitle=histo->GetTitle();
  RooRealVar xvar(
    "xvar", "",
    std::min(binningX.getBinLowEdge(0), histo->GetXaxis()->GetBinLowEdge(1)),
    std::max(binningX.getBinLowEdge(binningX.getNbins()), histo->GetXaxis()->GetBinLowEdge(histo->GetNbinsX()+1))
  );
  RooRealVar yvar(
    "yvar", "",
    std::min(binningY.getBinLowEdge(0), histo->GetYaxis()->GetBinLowEdge(1)),
    std::max(binningY.getBinLowEdge(binningY.getNbins()), histo->GetYaxis()->GetBinLowEdge(histo->GetNbinsY()+1))
  );

  unsigned int nbinsXoriginal=histo->GetNbinsX(); unsigned int nbinsXrebin=binningX.getNbins();
  unsigned int nbinsYoriginal=histo->GetNbinsY(); unsigned int nbinsYrebin=binningY.getNbins();
  const TProfile* prof_x=nullptr; TProfile* prof_x_rebin=nullptr;
  const TProfile* prof_y=nullptr; TProfile* prof_y_rebin=nullptr;
  if (condProfs){
    for (std::pair<TProfile const*, unsigned int> prof_dim:*condProfs){
      if (prof_dim.second==0) prof_x=prof_dim.first;
      if (prof_dim.second==1) prof_y=prof_dim.first;
    }
    if (prof_x){
      prof_x_rebin = new TProfile(*prof_x); prof_x_rebin->SetName(TString(prof_x->GetName())+"_copy");
      rebinProfile(prof_x_rebin, binningX);
    }
    if (prof_y){
      prof_y_rebin = new TProfile(*prof_y); prof_y_rebin->SetName(TString(prof_y->GetName())+"_copy");
      rebinProfile(prof_y_rebin, binningY);
    }
  }

  int nx=0, ny=0;
  IvyNCSplineCore::BoundaryCondition bcBeginX = IvyNCSplineCore::bcQuadraticWithNullSlope;
  IvyNCSplineCore::BoundaryCondition bcEndX = IvyNCSplineCore::bcQuadraticWithNullSlope;
  IvyNCSplineCore::BoundaryCondition bcBeginY = IvyNCSplineCore::bcQuadraticWithNullSlope;
  IvyNCSplineCore::BoundaryCondition bcEndY = IvyNCSplineCore::bcQuadraticWithNullSlope;
  vector<splineTriplet_t> pList, pErrList;
  for (unsigned int ix=0; ix<=nbinsXoriginal+1; ix++){
    double xval;
    if (prof_x){
      if (prof_x->GetBinError(ix)==0.) continue;
      xval = prof_x->GetBinContent(ix);
    }
    else{
      if (ix==nbinsXoriginal+1) continue;
      xval = histo->GetXaxis()->GetBinLowEdge(ix+1);
    }
    for (unsigned int iy=0; iy<=nbinsYoriginal+1; iy++){
      double yval;
      if (prof_y){
        if (prof_y->GetBinError(iy)==0.) continue;
        yval = prof_y->GetBinContent(iy);
      }
      else{
        if (iy==nbinsYoriginal+1) continue;
        yval = histo->GetYaxis()->GetBinLowEdge(iy+1);
      }

      double wval = histo->GetBinContent(ix, iy);
      double errsqval = pow(histo->GetBinError(ix, iy), 2);
      pList.emplace_back(xval, yval, wval);
      pErrList.emplace_back(xval, yval, errsqval);

      if (nx==0) ny++;
    }
    nx++;
  }
  if (nx<=3){ bcBeginX = IvyNCSplineCore::bcNaturalSpline; bcEndX = IvyNCSplineCore::bcNaturalSpline; }
  if (ny<=3){ bcBeginY = IvyNCSplineCore::bcNaturalSpline; bcEndY = IvyNCSplineCore::bcNaturalSpline; }
  IvyNCSplineFactory_2D spFac(xvar, yvar, "tmpSpline", bcBeginX, bcEndX, bcBeginY, bcEndY);
  IvyNCSplineFactory_2D spErrFac(xvar, yvar, "tmpSplineErr", bcBeginX, bcEndX, bcBeginY, bcEndY);
  spFac.setPoints(pList);
  spErrFac.setPoints(pErrList);
  const IvyNCSpline_2D_fast* sp = spFac.getFunc();
  const IvyNCSpline_2D_fast* spErr = spErrFac.getFunc();

  delete histo;
  histo = new TH2F(hname, htitle, binningX.getNbins(), binningX.getBinning(), binningY.getNbins(), binningY.getBinning());
  for (unsigned int ix=0; ix<=nbinsXrebin+1; ix++){
    double xval;
    if (prof_x){
      if (prof_x_rebin->GetBinError(ix)==0.) continue;
      xval=prof_x_rebin->GetBinContent(ix);
    }
    else{
      if (ix==nbinsXrebin+1) xval=binningX.getBinLowEdge(ix-1);
      else xval=binningX.getBinLowEdge(ix);
    }
    for (unsigned int iy=0; iy<=nbinsYrebin+1; iy++){
      double yval;
      if (prof_y){
        if (prof_y_rebin->GetBinError(iy)==0.) continue;
        yval=prof_y_rebin->GetBinContent(iy);
      }
      else{
        if (iy==nbinsYrebin+1) yval=binningY.getBinLowEdge(iy-1);
        else yval=binningY.getBinLowEdge(iy);
      }

      double cval=0;
      double errval=0;
      xvar.setVal(xval);
      yvar.setVal(yval);
      cval = sp->getVal();
      errval = spErr->getVal();
      histo->SetBinContent(ix, iy, cval);
      histo->SetBinError(ix, iy, sqrt(std::max(0., errval)));
    }
  }

  delete prof_y_rebin;
  delete prof_x_rebin;
}
void HelperFunctions::rebinCumulant(TH3F*& histo, const ExtendedBinning& binningX, const ExtendedBinning& binningY, const ExtendedBinning& binningZ, std::vector<std::pair<TProfile const*, unsigned int>>* condProfs){
  if (!histo) return;
  if (!binningX.isValid() || !binningY.isValid() || !binningZ.isValid()){ IVYerr << "HelperFunctions::rebinCumulant: Cannot rebin a 3D cumulant with an invalid binning" << endl; return; }

  const TString hname=histo->GetName();
  const TString htitle=histo->GetTitle();
  RooRealVar xvar(
    "xvar", "",
    std::min(binningX.getBinLowEdge(0), histo->GetXaxis()->GetBinLowEdge(1)),
    std::max(binningX.getBinLowEdge(binningX.getNbins()), histo->GetXaxis()->GetBinLowEdge(histo->GetNbinsX()+1))
  );
  RooRealVar yvar(
    "yvar", "",
    std::min(binningY.getBinLowEdge(0), histo->GetYaxis()->GetBinLowEdge(1)),
    std::max(binningY.getBinLowEdge(binningY.getNbins()), histo->GetYaxis()->GetBinLowEdge(histo->GetNbinsY()+1))
  );
  RooRealVar zvar(
    "zvar", "",
    std::min(binningZ.getBinLowEdge(0), histo->GetZaxis()->GetBinLowEdge(1)),
    std::max(binningZ.getBinLowEdge(binningZ.getNbins()), histo->GetZaxis()->GetBinLowEdge(histo->GetNbinsZ()+1))
  );

  unsigned int nbinsXoriginal=histo->GetNbinsX(); unsigned int nbinsXrebin=binningX.getNbins();
  unsigned int nbinsYoriginal=histo->GetNbinsY(); unsigned int nbinsYrebin=binningY.getNbins();
  unsigned int nbinsZoriginal=histo->GetNbinsZ(); unsigned int nbinsZrebin=binningZ.getNbins();
  const TProfile* prof_x=nullptr; TProfile* prof_x_rebin=nullptr;
  const TProfile* prof_y=nullptr; TProfile* prof_y_rebin=nullptr;
  const TProfile* prof_z=nullptr; TProfile* prof_z_rebin=nullptr;
  if (condProfs){
    for (std::pair<TProfile const*, unsigned int> prof_dim:*condProfs){
      if (prof_dim.second==0) prof_x=prof_dim.first;
      if (prof_dim.second==1) prof_y=prof_dim.first;
      if (prof_dim.second==2) prof_z=prof_dim.first;
    }
    if (prof_x){
      prof_x_rebin = new TProfile(*prof_x); prof_x_rebin->SetName(TString(prof_x->GetName())+"_copy");
      rebinProfile(prof_x_rebin, binningX);
    }
    if (prof_y){
      prof_y_rebin = new TProfile(*prof_y); prof_y_rebin->SetName(TString(prof_y->GetName())+"_copy");
      rebinProfile(prof_y_rebin, binningY);
    }
    if (prof_z){
      prof_z_rebin = new TProfile(*prof_z); prof_z_rebin->SetName(TString(prof_z->GetName())+"_copy");
      rebinProfile(prof_z_rebin, binningZ);
    }
  }

  int nx=0, ny=0, nz=0;
  IvyNCSplineCore::BoundaryCondition bcBeginX = IvyNCSplineCore::bcQuadraticWithNullSlope;
  IvyNCSplineCore::BoundaryCondition bcEndX = IvyNCSplineCore::bcQuadraticWithNullSlope;
  IvyNCSplineCore::BoundaryCondition bcBeginY = IvyNCSplineCore::bcQuadraticWithNullSlope;
  IvyNCSplineCore::BoundaryCondition bcEndY = IvyNCSplineCore::bcQuadraticWithNullSlope;
  IvyNCSplineCore::BoundaryCondition bcBeginZ = IvyNCSplineCore::bcQuadraticWithNullSlope;
  IvyNCSplineCore::BoundaryCondition bcEndZ = IvyNCSplineCore::bcQuadraticWithNullSlope;
  vector<splineQuadruplet_t> pList, pErrList;
  for (unsigned int ix=0; ix<=nbinsXoriginal+1; ix++){
    double xval;
    if (prof_x){
      if (prof_x->GetBinError(ix)==0.) continue;
      xval = prof_x->GetBinContent(ix);
    }
    else{
      if (ix==nbinsXrebin+1) xval=binningX.getBinLowEdge(ix-1);
      else xval=binningX.getBinLowEdge(ix);
    }
    for (unsigned int iy=0; iy<=nbinsYoriginal+1; iy++){
      double yval;
      if (prof_y){
        if (prof_y->GetBinError(iy)==0.) continue;
        yval = prof_y->GetBinContent(iy);
      }
      else{
        if (iy==nbinsYrebin+1) yval=binningY.getBinLowEdge(iy-1);
        else yval=binningY.getBinLowEdge(iy);
      }
      for (unsigned int iz=0; iz<=nbinsZoriginal+1; iz++){
        double zval;
        if (prof_z){
          if (prof_z->GetBinError(iz)==0.) continue;
          zval = prof_z->GetBinContent(iz);
        }
        else{
          if (iz==nbinsZrebin+1) zval=binningZ.getBinLowEdge(iz-1);
          else zval=binningZ.getBinLowEdge(iz);
        }

        double wval = histo->GetBinContent(ix, iy, iz);
        double errsqval = pow(histo->GetBinError(ix, iy, iz), 2);
        pList.emplace_back(xval, yval, zval, wval);
        pErrList.emplace_back(xval, yval, zval, errsqval);
        if (nx==0 && ny==0) nz++;
      }
      if (nx==0) ny++;
    }
    nx++;
  }
  if (nx<=3){ bcBeginX = IvyNCSplineCore::bcNaturalSpline; bcEndX = IvyNCSplineCore::bcNaturalSpline; }
  if (ny<=3){ bcBeginY = IvyNCSplineCore::bcNaturalSpline; bcEndY = IvyNCSplineCore::bcNaturalSpline; }
  if (nz<=3){ bcBeginZ = IvyNCSplineCore::bcNaturalSpline; bcEndZ = IvyNCSplineCore::bcNaturalSpline; }
  IvyNCSplineFactory_3D spFac(xvar, yvar, zvar, "tmpSpline", bcBeginX, bcEndX, bcBeginY, bcEndY, bcBeginZ, bcEndZ);
  IvyNCSplineFactory_3D spErrFac(xvar, yvar, zvar, "tmpSplineErr", bcBeginX, bcEndX, bcBeginY, bcEndY, bcBeginZ, bcEndZ);
  spFac.setPoints(pList);
  spErrFac.setPoints(pErrList);
  const IvyNCSpline_3D_fast* sp = spFac.getFunc();
  const IvyNCSpline_3D_fast* spErr = spErrFac.getFunc();

  delete histo;
  histo = new TH3F(hname, htitle, binningX.getNbins(), binningX.getBinning(), binningY.getNbins(), binningY.getBinning(), binningZ.getNbins(), binningZ.getBinning());
  for (unsigned int ix=0; ix<=nbinsXrebin+1; ix++){
    double xval;
    if (prof_x){
      if (prof_x_rebin->GetBinError(ix)==0.) continue;
      xval=prof_x_rebin->GetBinContent(ix);
    }
    else{
      if (ix==nbinsXrebin+1) continue;
      xval=binningX.getBinLowEdge(ix);
    }
    for (unsigned int iy=0; iy<=nbinsYrebin+1; iy++){
      double yval;
      if (prof_y){
        if (prof_y_rebin->GetBinError(iy)==0.) continue;
        yval=prof_y_rebin->GetBinContent(iy);
      }
      else{
        if (iy==nbinsYrebin+1) continue;
        yval=binningY.getBinLowEdge(iy);
      }
      for (unsigned int iz=0; iz<=nbinsZrebin+1; iz++){
        double zval;
        if (prof_z){
          if (prof_z_rebin->GetBinError(iz)==0.) continue;
          zval=prof_z_rebin->GetBinContent(iz);
        }
        else{
          if (iz==binningZ.getNbins()+1) continue;
          zval=binningZ.getBinLowEdge(iz);
        }

        double cval=0;
        double errval=0;
        xvar.setVal(xval);
        yvar.setVal(yval);
        zvar.setVal(zval);
        cval = sp->getVal();
        errval = spErr->getVal();
        histo->SetBinContent(ix, iy, iz, cval);
        histo->SetBinError(ix, iy, iz, sqrt(std::max(0., errval)));
      }
    }
  }

  delete prof_z_rebin;
  delete prof_y_rebin;
  delete prof_x_rebin;
}

void HelperFunctions::rebinHistogram(TH1F*& histo, const ExtendedBinning& binningX){
  if (!histo) return;
  const TString hname=histo->GetName();
  const TString htitle=histo->GetTitle();

  TH1F* htmp_cumulant=nullptr;
  getCumulantHistogram(histo, htmp_cumulant);
  rebinCumulant(htmp_cumulant, binningX);
  delete histo; histo=nullptr;
  translateCumulantToHistogram(htmp_cumulant, histo, hname);
  histo->SetTitle(htitle);
  delete htmp_cumulant;
}
void HelperFunctions::rebinHistogram(TH2F*& histo, const ExtendedBinning& binningX, const ExtendedBinning& binningY, std::vector<std::pair<TProfile const*, unsigned int>>* condProfs){
  if (!histo) return;
  const TString hname=histo->GetName();
  const TString htitle=histo->GetTitle();

  std::vector<unsigned int>* condDims=nullptr;
  if (condProfs){
    condDims = new std::vector<unsigned int>;
    for (pair<TProfile const*, unsigned int>& condProf:*condProfs) condDims->push_back(condProf.second);
  }

  TH2F* htmp_cumulant=nullptr;
  getCumulantHistogram(histo, htmp_cumulant, "", condDims);
  rebinCumulant(htmp_cumulant, binningX, binningY, condProfs);
  delete histo; histo=nullptr;
  translateCumulantToHistogram(htmp_cumulant, histo, hname, condDims);
  histo->SetTitle(htitle);
  delete htmp_cumulant;
  delete condDims;
}
void HelperFunctions::rebinHistogram(TH3F*& histo, const ExtendedBinning& binningX, const ExtendedBinning& binningY, const ExtendedBinning& binningZ, std::vector<std::pair<TProfile const*, unsigned int>>* condProfs){
  if (!histo) return;
  const TString hname=histo->GetName();
  const TString htitle=histo->GetTitle();

  std::vector<unsigned int>* condDims=nullptr;
  if (condProfs){
    condDims = new std::vector<unsigned int>;
    for (pair<TProfile const*, unsigned int>& condProf:*condProfs) condDims->push_back(condProf.second);
  }

  TH3F* htmp_cumulant=nullptr;
  getCumulantHistogram(histo, htmp_cumulant, "", condDims);
  rebinCumulant(htmp_cumulant, binningX, binningY, binningZ, condProfs);
  delete histo; histo=nullptr;
  translateCumulantToHistogram(htmp_cumulant, histo, hname, condDims);
  histo->SetTitle(htitle);
  delete htmp_cumulant;
  delete condDims;
}

void HelperFunctions::rebinHistogram_NoCumulant(TH1F*& histo, const ExtendedBinning& binningX, const TProfile* prof_x){
  if (!histo || !prof_x || !binningX.isValid()) return;

  TProfile* prof_final_x = new TProfile(*prof_x); prof_final_x->SetName(TString(prof_final_x->GetName())+"_copy");
  rebinProfile(prof_final_x, binningX);

  const TString hname=histo->GetName();
  const TString htitle=histo->GetTitle();
  RooRealVar xvar(
    "xvar", "",
    std::min(binningX.getBinLowEdge(0), histo->GetXaxis()->GetBinLowEdge(1)),
    std::max(binningX.getBinLowEdge(binningX.getNbins()), histo->GetXaxis()->GetBinLowEdge(histo->GetNbinsX()+1))
  );

  IvyNCSplineFactory_1D spFac(xvar, "tmpSpline");
  IvyNCSplineFactory_1D spErrFac(xvar, "tmpSplineErr");
  vector<pair<IvyNCSplineCore::T, IvyNCSplineCore::T>> pList, pErrList;
  for (int ix=0; ix<=histo->GetNbinsX()+1; ix++){
    if (prof_x->GetBinError(ix)!=0.){
      pList.emplace_back(prof_x->GetBinContent(ix), histo->GetBinContent(ix));
      pErrList.emplace_back(prof_x->GetBinContent(ix), pow(histo->GetBinError(ix), 2));
    }
  }
  spFac.setPoints(pList);
  spErrFac.setPoints(pErrList);
  const IvyNCSpline_1D_fast* sp = spFac.getFunc();
  const IvyNCSpline_1D_fast* spErr = spErrFac.getFunc();

  // Once cumulant is rebinned, errors are lost
  delete histo;
  histo = new TH1F(hname, htitle, binningX.getNbins(), binningX.getBinning());
  for (unsigned int ix=0; ix<=binningX.getNbins()+1; ix++){
    double xval=prof_final_x->GetBinContent(ix);
    double cval=0;
    double errval=0;
    xvar.setVal(xval);
    cval = sp->getVal();
    errval = spErr->getVal();
    if (errval<0.) errval=0.;
    histo->SetBinContent(ix, cval);
    histo->SetBinError(ix, sqrt(std::max(0., errval)));
  }

  delete prof_final_x;
}
void HelperFunctions::rebinHistogram_NoCumulant(TH2F*& histo, const ExtendedBinning& binningX, const TProfile* prof_x, const ExtendedBinning& binningY, const TProfile* prof_y){
  if (!histo || !prof_x || !binningX.isValid() || !prof_y || !binningY.isValid()) return;

  TProfile* prof_final_x = new TProfile(*prof_x); prof_final_x->SetName(TString(prof_final_x->GetName())+"_copy");
  rebinProfile(prof_final_x, binningX);
  TProfile* prof_final_y = new TProfile(*prof_y); prof_final_y->SetName(TString(prof_final_y->GetName())+"_copy");
  rebinProfile(prof_final_y, binningY);

  const TString hname=histo->GetName();
  const TString htitle=histo->GetTitle();
  RooRealVar xvar(
    "xvar", "",
    std::min(binningX.getBinLowEdge(0), histo->GetXaxis()->GetBinLowEdge(1)),
    std::max(binningX.getBinLowEdge(binningX.getNbins()), histo->GetXaxis()->GetBinLowEdge(histo->GetNbinsX()+1))
  );
  RooRealVar yvar(
    "yvar", "",
    std::min(binningY.getBinLowEdge(0), histo->GetYaxis()->GetBinLowEdge(1)),
    std::max(binningY.getBinLowEdge(binningY.getNbins()), histo->GetYaxis()->GetBinLowEdge(histo->GetNbinsY()+1))
  );

  IvyNCSplineFactory_2D spFac(xvar, yvar, "tmpSpline");
  IvyNCSplineFactory_2D spErrFac(xvar, yvar, "tmpSplineErr");
  vector<splineTriplet_t> pList, pErrList;
  for (int ix=0; ix<=histo->GetNbinsX()+1; ix++){
    if (prof_x->GetBinError(ix)==0.) continue;
    for (int iy=0; iy<=histo->GetNbinsY()+1; iy++){
      if (prof_y->GetBinError(iy)==0.) continue;

      pList.emplace_back(prof_x->GetBinContent(ix), prof_y->GetBinContent(iy), histo->GetBinContent(ix, iy));
      pErrList.emplace_back(prof_x->GetBinContent(ix), prof_y->GetBinContent(iy), pow(histo->GetBinError(ix, iy), 2));
    }
  }
  spFac.setPoints(pList);
  spErrFac.setPoints(pErrList);
  const IvyNCSpline_2D_fast* sp = spFac.getFunc();
  const IvyNCSpline_2D_fast* spErr = spErrFac.getFunc();

  // Once cumulant is rebinned, errors are lost
  delete histo;
  histo = new TH2F(hname, htitle, binningX.getNbins(), binningX.getBinning(), binningY.getNbins(), binningY.getBinning());
  for (unsigned int ix=0; ix<=binningX.getNbins()+1; ix++){
    double xval=prof_final_x->GetBinContent(ix);
    for (unsigned int iy=0; iy<=binningY.getNbins()+1; iy++){
      double yval=prof_final_y->GetBinContent(iy);
      double cval=0;
      double errval=0;
      xvar.setVal(xval);
      yvar.setVal(yval);
      cval = sp->getVal();
      errval = spErr->getVal();
      if (errval<0.) errval=0.;
      histo->SetBinContent(ix, iy, cval);
      histo->SetBinError(ix, iy, sqrt(std::max(0., errval)));
    }
  }

  delete prof_final_x;
  delete prof_final_y;
}
void HelperFunctions::rebinHistogram_NoCumulant(TH3F*& histo, const ExtendedBinning& binningX, const TProfile* prof_x, const ExtendedBinning& binningY, const TProfile* prof_y, const ExtendedBinning& binningZ, const TProfile* prof_z){
  if (!histo || !prof_x || !binningX.isValid() || !prof_y || !binningY.isValid() || !prof_z || !binningZ.isValid()) return;

  TProfile* prof_final_x = new TProfile(*prof_x); prof_final_x->SetName(TString(prof_final_x->GetName())+"_copy");
  rebinProfile(prof_final_x, binningX);
  TProfile* prof_final_y = new TProfile(*prof_y); prof_final_y->SetName(TString(prof_final_y->GetName())+"_copy");
  rebinProfile(prof_final_y, binningY);
  TProfile* prof_final_z = new TProfile(*prof_z); prof_final_z->SetName(TString(prof_final_z->GetName())+"_copy");
  rebinProfile(prof_final_z, binningZ);

  const TString hname=histo->GetName();
  const TString htitle=histo->GetTitle();
  RooRealVar xvar(
    "xvar", "",
    std::min(binningX.getBinLowEdge(0), histo->GetXaxis()->GetBinLowEdge(1)),
    std::max(binningX.getBinLowEdge(binningX.getNbins()), histo->GetXaxis()->GetBinLowEdge(histo->GetNbinsX()+1))
  );
  RooRealVar yvar(
    "yvar", "",
    std::min(binningY.getBinLowEdge(0), histo->GetYaxis()->GetBinLowEdge(1)),
    std::max(binningY.getBinLowEdge(binningY.getNbins()), histo->GetYaxis()->GetBinLowEdge(histo->GetNbinsY()+1))
  );
  RooRealVar zvar(
    "zvar", "",
    std::min(binningZ.getBinLowEdge(0), histo->GetZaxis()->GetBinLowEdge(1)),
    std::max(binningZ.getBinLowEdge(binningZ.getNbins()), histo->GetZaxis()->GetBinLowEdge(histo->GetNbinsZ()+1))
  );

  IvyNCSplineFactory_3D spFac(xvar, yvar, zvar, "tmpSpline");
  IvyNCSplineFactory_3D spErrFac(xvar, yvar, zvar, "tmpSplineErr");
  vector<splineQuadruplet_t> pList, pErrList;
  for (int ix=0; ix<=histo->GetNbinsX()+1; ix++){
    if (prof_x->GetBinError(ix)==0.) continue;
    for (int iy=0; iy<=histo->GetNbinsY()+1; iy++){
      if (prof_y->GetBinError(iy)==0.) continue;
      for (int iz=0; iz<=histo->GetNbinsZ()+1; iz++){
        if (prof_z->GetBinError(iz)==0.) continue;

        pList.emplace_back(prof_x->GetBinContent(ix), prof_y->GetBinContent(iy), prof_z->GetBinContent(iz), histo->GetBinContent(ix, iy, iz));
        pErrList.emplace_back(prof_x->GetBinContent(ix), prof_y->GetBinContent(iy), prof_z->GetBinContent(iz), pow(histo->GetBinError(ix, iy, iz), 2));
      }
    }
  }
  spFac.setPoints(pList);
  spErrFac.setPoints(pErrList);
  const IvyNCSpline_3D_fast* sp = spFac.getFunc();
  const IvyNCSpline_3D_fast* spErr = spErrFac.getFunc();

  // Once cumulant is rebinned, errors are lost
  delete histo;
  histo = new TH3F(hname, htitle, binningX.getNbins(), binningX.getBinning(), binningY.getNbins(), binningY.getBinning(), binningZ.getNbins(), binningZ.getBinning());
  for (unsigned int ix=0; ix<=binningX.getNbins()+1; ix++){
    double xval=prof_final_x->GetBinContent(ix);
    for (unsigned int iy=0; iy<=binningY.getNbins()+1; iy++){
      double yval=prof_final_y->GetBinContent(iy);
      for (unsigned int iz=0; iz<=binningZ.getNbins()+1; iz++){
        double zval=prof_final_z->GetBinContent(iz);
        double cval=0;
        double errval=0;
        xvar.setVal(xval);
        yvar.setVal(yval);
        zvar.setVal(zval);
        cval = sp->getVal();
        errval = spErr->getVal();
        if (errval<0.) errval=0.;
        histo->SetBinContent(ix, iy, iz, cval);
        histo->SetBinError(ix, iy, iz, sqrt(std::max(0., errval)));
      }
    }
  }

  delete prof_final_x;
  delete prof_final_y;
  delete prof_final_z;
}

TH1F* HelperFunctions::getHistogramSlice(TH1F const* histo, TString newname){
  TH1F* res = nullptr;
  getGenericHistogramSlice(res, histo, newname);
  return res;
}
TH1F* HelperFunctions::getHistogramSlice(TH2F const* histo, unsigned char XDirection, int iy, int jy, TString newname){
  TH1F* res = nullptr;
  getGenericHistogramSlice(res, histo, XDirection, iy, jy, newname);
  return res;
}
TH1F* HelperFunctions::getHistogramSlice(TH3F const* histo, unsigned char XDirection, int iy, int jy, int iz, int jz, TString newname){
  TH1F* res = nullptr;
  getGenericHistogramSlice(res, histo, XDirection, iy, jy, iz, jz, newname);
  return res;
}
TH2F* HelperFunctions::getHistogramSlice(TH3F const* histo, unsigned char XDirection, unsigned char YDirection, int iz, int jz, TString newname){
  TH2F* res = nullptr;
  getGenericHistogramSlice(res, histo, XDirection, YDirection, iz, jz, newname);
  return res;
}


void HelperFunctions::CopyFile(TString fname, TTree*(*fcnTree)(TTree*), TDirectory*(*fcnDirectory)(TDirectory*)){
  // Copy all objects and subdirs of file fname as a subdir of the current directory
  TDirectory* target = gDirectory;
  TFile* f = TFile::Open(fname, "read");
  if (!f || f->IsZombie()){
    IVYerr << "HelperFunctions::CopyFile: Cannot copy file " << fname << endl;
    target->cd();
    if (f && f->IsOpen()){ f->Close(); f=nullptr; }
    delete f;
    return;
  }
  target->cd();
  CopyDirectory(f, fcnTree, fcnDirectory);
  f->Close();
  target->cd();
}
void HelperFunctions::CopyDirectory(TDirectory* source, TTree*(*fcnTree)(TTree*), TDirectory*(*fcnDirectory)(TDirectory*)){
  // Copy all objects and subdirs of directory source as a subdir of the current directory
  source->ls();
  TDirectory* savdir = gDirectory;
  TDirectory* adir;
  if (dynamic_cast<TFile*>(source)==nullptr) adir = savdir->mkdir(source->GetName());
  else adir=savdir;
  adir->cd();
  // Loop on all entries of this directory
  TKey* key;
  TIter nextkey(source->GetListOfKeys());
  vector<TString> copiedTrees;
  while ((key = (TKey*)nextkey())){
    IVYout << "HelperFunctions::CopyDirectory: Copying key " << key->GetName() << endl;
    const char* classname = key->GetClassName();
    TClass* cl = gROOT->GetClass(classname);
    if (!cl) continue;
    if (cl->InheritsFrom(TDirectory::Class())){
      IVYout << "- HelperFunctions::CopyDirectory: Key " << key->GetName() << " is a directory." << endl;
      source->cd(key->GetName());
      TDirectory* subdir = gDirectory;
      if (fcnDirectory) subdir = fcnDirectory(subdir);
      adir->cd();
      CopyDirectory(subdir, fcnTree, fcnDirectory);
      adir->cd();
    }
    else if (cl->InheritsFrom(TTree::Class())){
      IVYout << "- HelperFunctions::CopyDirectory: Key " << key->GetName() << " is a tree." << endl;
      TTree* T = (TTree*)source->Get(key->GetName());
      bool alreadyCopied=false;
      for (auto& k:copiedTrees){
        if (k==key->GetName()){
          alreadyCopied=true;
          break;
        }
      }
      adir->cd();
      if (!alreadyCopied){
        TTree* newT=nullptr;
        if (fcnTree) newT = fcnTree(T);
        if (!newT) newT = T->CloneTree(-1, "fast");
        if (newT){
          IVYout << "- HelperFunctions::CopyDirectory: A new tree " << newT->GetName() << " is created." << endl;
          newT->Write(nullptr, TObject::kWriteDelete);
          copiedTrees.push_back(key->GetName());
        }
        //delete newT;
      }
      else IVYout << "- HelperFunctions::CopyDirectory: A copy of " << key->GetName() << " already exists." << endl;
    }
    else{
      source->cd();
      TObject* obj = key->ReadObj();
      adir->cd();
      obj->Write();
      delete obj;
    }
  }
  adir->SaveSelf(kTRUE);
  savdir->cd();
}

void HelperFunctions::distributeObjects(TDirectory* inputdir, std::vector<TDirectory*> const& outputdirs, MiscUtils::VerbosityLevel verbosity){
  unsigned int const nchunks = outputdirs.size();

  TDirectory* curdir = gDirectory;

  inputdir->cd();
  if (verbosity>=MiscUtils::INFO) cout << "HelperFunctions:distributeObjects: Now inside " << inputdir->GetName() << ":" << endl;

  TKey* key;
  TIter nextkey(inputdir->GetListOfKeys());
  std::vector<TString> copiedKeys;
  while ((key = (TKey*) nextkey())){
    const char* classname = key->GetClassName();
    TClass* cl = gROOT->GetClass(classname);
    if (!cl) continue;
    if (cl->InheritsFrom(TDirectory::Class())){
      inputdir->cd(key->GetName());
      TDirectory* subdir = gDirectory;
      inputdir->cd();

      if (verbosity>=MiscUtils::INFO) cout << "\t- Found subdirectory " << key->GetName() << endl;

      std::vector<TDirectory*> outputsubdirs; outputsubdirs.reserve(outputdirs.size());
      for (auto& outputdir:outputdirs){
        TDirectory* outputsubdir = outputdir->mkdir(key->GetName());
        outputsubdirs.push_back(outputsubdir);
        inputdir->cd();
      }
      distributeObjects(subdir, outputsubdirs);
      inputdir->cd();
    }
    else if (cl->InheritsFrom(TTree::Class())){
      TTree* obj = (TTree*) inputdir->Get(key->GetName());
      if (!obj) continue;
      TString objname = obj->GetName();
      TString keyname = key->GetName();
      if (!HelperFunctions::checkListVariable(copiedKeys, keyname)){
        copiedKeys.push_back(keyname);
        int nEntries = obj->GetEntries();
        int nEntries_step = static_cast<int>(nEntries / nchunks) + 1;
        int nEntries_first = 0;
        if (verbosity>=MiscUtils::INFO) cout << "\t- Found tree " << key->GetName() << " with " << nEntries << " entries:" << endl;
        for (unsigned int ichunk=0; ichunk<nchunks; ichunk++){
          TDirectory* outputdir = outputdirs.at(ichunk);
          outputdir->cd();

          TTree* newtree = obj->CloneTree(0);
          int nEntries_last = std::min(nEntries_first+nEntries_step, nEntries);
          if (ichunk==nchunks-1) nEntries_last = nEntries;
          if (verbosity>=MiscUtils::INFO) cout << "\t\t- Tree[ " << nEntries_first << ", " << nEntries_last << ")" << endl;
          for (int ev=nEntries_first; ev<nEntries_last; ev++){
            obj->GetEntry(ev);
            newtree->Fill();
          }
          outputdir->WriteTObject(newtree);
          delete newtree;
          nEntries_first = nEntries_last;

          inputdir->cd();
        }
      }
    }
    else if (cl->InheritsFrom(TObject::Class())){
      TObject* obj = (TObject*) inputdir->Get(key->GetName());
      if (!obj) continue;
      TString objname = obj->GetName();
      TString keyname = key->GetName();
      if ((objname=="Graph" && keyname!="Graph") || objname=="") objname = keyname; // Holy jumping monkeys for fake rates
      if (dynamic_cast<TNamed*>(obj)) dynamic_cast<TNamed*>(obj)->SetName(objname);
      if (!HelperFunctions::checkListVariable(copiedKeys, keyname)){
        copiedKeys.push_back(keyname);
        if (verbosity>=MiscUtils::INFO) cout << "\t- Found key " << keyname << endl;
        for (auto& outputdir:outputdirs) outputdir->WriteTObject(obj);
        inputdir->cd();
      }
    }
  }

  curdir->cd();
}

void HelperFunctions::extractTreesFromDirectory(TDirectory* source, std::vector<TTree*>& res, bool doClone){
  TDirectory* target = gDirectory;

  source->cd();
  // Loop on all entries of this directory
  TKey* key;
  TIter nextkey(source->GetListOfKeys());
  vector<TString> copiedTrees;
  while ((key = (TKey*) nextkey())){
    IVYout << "HelperFunctions::GetTreesInDirectory: Acquiring key " << key->GetName() << endl;
    const char* classname = key->GetClassName();
    TClass* cl = gROOT->GetClass(classname);
    if (!cl) continue;
    if (cl->InheritsFrom(TDirectory::Class())){
      IVYout << "- HelperFunctions::GetTreesInDirectory: Key " << key->GetName() << " is a directory." << endl;
      source->cd(key->GetName());
      TDirectory* subdir = gDirectory;
      source->cd();
      extractTreesFromDirectory(subdir, res, doClone);
    }
    else if (cl->InheritsFrom(TTree::Class())){
      IVYout << "- HelperFunctions::GetTreesInDirectory: Key " << key->GetName() << " is a tree." << endl;
      TTree* T = (TTree*) source->Get(key->GetName());
      bool alreadyCopied=false;
      for (auto& k:copiedTrees){
        if (k==key->GetName()){
          IVYout << "- HelperFunctions::GetTreesInDirectory: Tree " << key->GetName() << " already copied." << endl;
          alreadyCopied=true;
          break;
        }
      }
      if (!alreadyCopied){
        TTree* newT;
        if (doClone) newT = T->CloneTree(-1, "fast");
        else newT = T;
        if (newT){
          copiedTrees.push_back(key->GetName());
          res.push_back(newT);
        }
      }
    }
  }

  target->cd();
}

