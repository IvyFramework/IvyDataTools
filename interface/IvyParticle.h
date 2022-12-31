#ifndef IVYPARTICLE_H
#define IVYPARTICLE_H

#include "IvyDataTypes.h"
#include "TLorentzVector.h"


class IvyParticle{
public:
  typedef IvyLorentzVector_d LorentzVector_t;
  typedef IvyPolarLorentzVector_d PolarLorentzVector_t;
  typedef IvyVector3D_d Vector3D_t;
  typedef IvyVector2D_d Vector2D_t;
  typedef ivy_listIndex_short_t UniqueId_t;
  typedef unsigned long long SelectionBitsType_t;

protected:
  ivy_id_t id;
  UniqueId_t uniqueIdentifier;
  SelectionBitsType_t selectionBits;
  LorentzVector_t momentum;

  std::vector<IvyParticle*> mothers;
  std::vector<IvyParticle*> daughters;

public:
  IvyParticle();
  IvyParticle(ivy_id_t id_);
  IvyParticle(ivy_id_t id_, LorentzVector_t const& mom_);
  IvyParticle(IvyParticle const& other);
  IvyParticle(IvyParticle&& other);
  virtual ~IvyParticle(){}

  // Swap and assignment operators are not virtual; they bring more complication than necessary, so they are implemented in the derived classes.
  void swap(IvyParticle& other);

  void setPdgId(ivy_id_t const& id_){ id=id_; }
  void setP4(LorentzVector_t const& momentum_){ momentum=momentum_; }

  void setUniqueIdentifier(UniqueId_t uid_){ uniqueIdentifier = uid_; }

  void resetSelectionBits(){ selectionBits=0; }
  void setSelectionBit(unsigned int ibit, bool val);
  bool testSelectionBit(unsigned int ibit) const;
  bool testSelection(unsigned int ibit) const{ return this->testSelectionBit(ibit); }

  ivy_id_t const& pdgId() const{ return id; }
  ivy_id_t& pdgId(){ return id; }

  LorentzVector_t const& p4() const{ return momentum; }
  LorentzVector_t& p4(){ return momentum; }

  UniqueId_t const& getUniqueIdentifier() const{ return uniqueIdentifier; }
  UniqueId_t& getUniqueIdentifier(){ return uniqueIdentifier; }

  SelectionBitsType_t const& getSelectionBits() const{ return selectionBits; }
  SelectionBitsType_t& getSelectionBits(){ return selectionBits; }

  float charge() const;
  LorentzVector_t::Scalar m() const{ return momentum.M(); }
  LorentzVector_t::Scalar px() const{ return momentum.X(); }
  LorentzVector_t::Scalar py() const{ return momentum.Y(); }
  LorentzVector_t::Scalar pz() const{ return momentum.Z(); }
  LorentzVector_t::Scalar E() const{ return momentum.T(); }
  LorentzVector_t::Scalar p() const{ return momentum.P(); }
  LorentzVector_t::Scalar pt() const{ return momentum.Pt(); }
  LorentzVector_t::Scalar eta() const{ return momentum.Eta(); }
  LorentzVector_t::Scalar phi() const{ return momentum.Phi(); }
  LorentzVector_t::Scalar rapidity() const{ return momentum.Rapidity(); }
  virtual LorentzVector_t::Scalar uncorrected_pt() const{ return pt(); }
  virtual LorentzVector_t uncorrected_p4() const{ return p4(); }
  LorentzVector_t::Scalar energy() const{ return this->E(); }
  LorentzVector_t::Scalar mass() const{ return this->m(); }
  LorentzVector_t::Scalar dot(const TLorentzVector& v) const{ return (momentum.T()*v.T()-(momentum.X()*v.X()+momentum.Y()*v.Y()+momentum.Z()*v.Z())); }
  LorentzVector_t::Scalar dot(const LorentzVector_t& v) const{ return (momentum.T()*v.T()-(momentum.X()*v.X()+momentum.Y()*v.Y()+momentum.Z()*v.Z())); }
  LorentzVector_t::Scalar dot(const IvyParticle& part) const{ return dot(part.p4()); }
  LorentzVector_t::Scalar dot(const IvyParticle* part) const{ return (part ? dot(*part) : LorentzVector_t::Scalar(0)); }
  LorentzVector_t::Scalar euclidean_dot(const TLorentzVector& v) const{ return (momentum.T()*v.T()+momentum.X()*v.X()+momentum.Y()*v.Y()+momentum.Z()*v.Z()); }
  LorentzVector_t::Scalar euclidean_dot(const LorentzVector_t& v) const{ return (momentum.T()*v.T()+momentum.X()*v.X()+momentum.Y()*v.Y()+momentum.Z()*v.Z()); }
  LorentzVector_t::Scalar euclidean_dot(const IvyParticle& part) const{ return euclidean_dot(part.p4()); }
  LorentzVector_t::Scalar euclidean_dot(const IvyParticle* part) const{ return (part ? euclidean_dot(*part) : LorentzVector_t::Scalar(0)); }
  LorentzVector_t::Scalar deltaR(LorentzVector_t::Scalar eta_, LorentzVector_t::Scalar phi_) const;
  LorentzVector_t::Scalar deltaR(const TLorentzVector& v) const{ return deltaR(v.Eta(), v.Phi()); }
  LorentzVector_t::Scalar deltaR(const LorentzVector_t& v) const{ return deltaR(v.Eta(), v.Phi()); }
  LorentzVector_t::Scalar deltaR(const IvyParticle& part) const{ return deltaR(part.p4()); }
  LorentzVector_t::Scalar deltaR(const IvyParticle* part) const{ return (part ? deltaR(*part) : LorentzVector_t::Scalar(-1)); }
  LorentzVector_t::Scalar deltaEta(LorentzVector_t::Scalar eta_) const;
  LorentzVector_t::Scalar deltaEta(const TLorentzVector& v) const{ return deltaEta(v.Eta()); }
  LorentzVector_t::Scalar deltaEta(const LorentzVector_t& v) const{ return deltaEta(v.Eta()); }
  LorentzVector_t::Scalar deltaEta(const IvyParticle& part) const{ return deltaEta(part.eta()); }
  LorentzVector_t::Scalar deltaEta(const IvyParticle* part) const{ return (part ? deltaEta(*part) : LorentzVector_t::Scalar(0)); }
  LorentzVector_t::Scalar deltaPhi(LorentzVector_t::Scalar phi_) const;
  LorentzVector_t::Scalar deltaPhi(const TLorentzVector& v) const{ return deltaPhi(v.Phi()); }
  LorentzVector_t::Scalar deltaPhi(const LorentzVector_t& v) const{ return deltaPhi(v.Phi()); }
  LorentzVector_t::Scalar deltaPhi(const IvyParticle& part) const{ return deltaPhi(part.phi()); }
  LorentzVector_t::Scalar deltaPhi(const IvyParticle* part) const{ return (part ? deltaPhi(*part) : LorentzVector_t::Scalar(0)); }

  TLorentzVector p4_TLV() const{ return TLorentzVector(this->px(), this->py(), this->pz(), this->E()); }

  Vector3D_t vect() const{ return Vector3D_t(momentum.X(), momentum.Y(), momentum.Z()); }
  Vector2D_t vect_trans() const{ return Vector2D_t(momentum.X(), momentum.Y()); }

  void addMother(IvyParticle* part);
  void addDaughter(IvyParticle* part);
  int getNMothers() const{ return mothers.size(); };
  int getNDaughters() const{ return daughters.size(); };
  IvyParticle* mother(size_t index) const{ return (index<mothers.size() ? mothers.at(index) : nullptr); }
  IvyParticle* daughter(size_t index) const{ return (index<daughters.size() ? daughters.at(index) : nullptr); }
  IvyParticle* getMother(size_t index) const{ return this->mother(index); }
  IvyParticle* getDaughter(size_t index) const{ return this->daughter(index); }
  std::vector<IvyParticle*>& getMothers(){ return mothers; }
  std::vector<IvyParticle*>& getDaughters(){ return daughters; }
  std::vector<IvyParticle*> const& getMothers()const{ return mothers; }
  std::vector<IvyParticle*> const& getDaughters()const{ return daughters; }
  bool hasMother(IvyParticle* part) const;
  bool hasDaughter(IvyParticle* part) const;
  void resetMothers(){ mothers.clear(); }
  void resetDaughters(){ daughters.clear(); }

  void getDeepDaughters(std::vector<IvyParticle*>& deepdaus, bool addSelf);
  void getDeepDaughters(std::vector<IvyParticle const*>& deepdaus, bool addSelf) const;

  static bool checkParticleExists(IvyParticle*, const std::vector<IvyParticle*>&);
  static bool checkDeepDaughtership(IvyParticle const* part1, IvyParticle const* part2);

};

#endif
