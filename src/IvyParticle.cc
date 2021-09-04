#include <algorithm>
#include <utility>
#include "IvyParticle.h"
#include "HelperFunctionsCore.h"
#include "IvyPDGHelpers.h"


using namespace IvyPDGHelpers;


IvyParticle::IvyParticle() :
  id(-9000),
  uniqueIdentifier(0),
  selectionBits(0),
  momentum(0, 0, 0, 0)
{}
IvyParticle::IvyParticle(ivy_id_t id_) :
  id(id_),
  uniqueIdentifier(0),
  selectionBits(0),
  momentum(0, 0, 0, 0)
{}
IvyParticle::IvyParticle(ivy_id_t id_, LorentzVector_t const& momentum_) :
  id(id_),
  uniqueIdentifier(0),
  selectionBits(0),
  momentum(momentum_)
{}
IvyParticle::IvyParticle(const IvyParticle& other) :
  id(other.id),
  uniqueIdentifier(other.uniqueIdentifier),
  selectionBits(other.selectionBits),
  momentum(other.momentum),
  mothers(other.mothers),
  daughters(other.daughters)
{}

void IvyParticle::swap(IvyParticle& other){
  std::swap(id, other.id);
  std::swap(uniqueIdentifier, other.uniqueIdentifier);
  std::swap(selectionBits, other.selectionBits);
  std::swap(momentum, other.momentum);
  std::swap(mothers, other.mothers);
  std::swap(daughters, other.daughters);
}

void IvyParticle::setSelectionBit(unsigned int ibit, bool val){ HelperFunctions::set_bit(this->selectionBits, ibit, val); }
bool IvyParticle::testSelectionBit(unsigned int ibit) const{ return HelperFunctions::test_bit(this->selectionBits, ibit); }

float IvyParticle::charge() const{
  float cpos=0;
  if (isAWBoson(id) || abs(id)==37 || abs(id)==2212 || abs(id)==211 || abs(id)==321 || abs(id)==411 || abs(id)==521) cpos = 1.;
  else if (isALepton(id)) cpos = -1.;
  else if (isUpTypeQuark(id)) cpos = 2./3.;
  else if (isDownTypeQuark(id)) cpos = -1./3.;
  if (id<0) cpos *= -1.;
  return cpos;
}
IvyParticle::LorentzVector_t::Scalar IvyParticle::deltaR(LorentzVector_t::Scalar eta_, LorentzVector_t::Scalar phi_) const{
  IvyParticle::LorentzVector_t::Scalar res;
  HelperFunctions::deltaR(eta(), phi(), eta_, phi_, res);
  return res;
}
IvyParticle::LorentzVector_t::Scalar IvyParticle::deltaEta(LorentzVector_t::Scalar eta_) const{
  IvyParticle::LorentzVector_t::Scalar res;
  HelperFunctions::deltaEta(eta(), eta_, res);
  return res;
}
IvyParticle::LorentzVector_t::Scalar IvyParticle::deltaPhi(LorentzVector_t::Scalar phi_) const{
  IvyParticle::LorentzVector_t::Scalar res;
  HelperFunctions::deltaPhi(phi(), phi_, res);
  return res;
}

bool IvyParticle::checkParticleExists(IvyParticle* myParticle, std::vector<IvyParticle*> const& particleArray){ return HelperFunctions::checkListVariable(particleArray, myParticle); }
bool IvyParticle::checkDeepDaughtership(IvyParticle const* part1, IvyParticle const* part2){
  if (!part1 || !part2) return false;
  if (part1 == part2) return true;
  std::vector<IvyParticle*> const& daughters1 = part1->getDaughters();
  std::vector<IvyParticle*> const& daughters2 = part2->getDaughters();
  return HelperFunctions::hasCommonElements(daughters1, daughters2);
}

void IvyParticle::addMother(IvyParticle* myParticle){ if (!checkParticleExists(myParticle, mothers)) mothers.push_back(myParticle); }
void IvyParticle::addDaughter(IvyParticle* myParticle){ if (!checkParticleExists(myParticle, daughters)) daughters.push_back(myParticle); }

bool IvyParticle::hasMother(IvyParticle* part) const{ return checkParticleExists(part, mothers); }
bool IvyParticle::hasDaughter(IvyParticle* part) const{ return checkParticleExists(part, daughters); }

void IvyParticle::getDeepDaughters(std::vector<IvyParticle*>& deepdaus, bool addSelf){
  if (this->getNDaughters()==0){
    if (addSelf && !HelperFunctions::checkListVariable(deepdaus, (IvyParticle*) this)) deepdaus.push_back(this);
  }
  else{
    for (auto const& dau:daughters) dau->getDeepDaughters(deepdaus, true);
  }
}
void IvyParticle::getDeepDaughters(std::vector<IvyParticle const*>& deepdaus, bool addSelf) const{
  if (this->getNDaughters()==0){
    if (addSelf && !HelperFunctions::checkListVariable(deepdaus, (IvyParticle const*) this)) deepdaus.push_back(this);
  }
  else{
    for (auto const& dau:daughters) dau->getDeepDaughters(deepdaus, true);
  }
}
