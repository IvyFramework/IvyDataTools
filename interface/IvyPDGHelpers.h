#ifndef IVYPDGHELPERS_H
#define IVYPDGHELPERS_H

#include <iostream>
#include <cmath>
#include <vector>

namespace IvyPDGHelpers{
  constexpr double Wmass = 80.399;
  constexpr double Zmass = 91.1876;
  constexpr double Topmass = 173.2;
  constexpr double Bottommass = 4.75;
  constexpr double Zeromass = 0;
  constexpr double m_el = 0.00051100;
  constexpr double m_mu = 0.10566;
  constexpr double m_tau = 1.7768;
  constexpr double Wwidth = 2.085;
  constexpr double Zwidth = 2.4952;
  constexpr double Topwidth = 2.;

  bool isALepton(int const& id); // Charged leptons to be more specific
  bool isANeutrino(int const& id); // Neutral leptons
  bool isAnUnknownJet(int const& id); // id==0 particles
  bool isAKnownJet(int const& id); // q or g
  bool isAJet(int const& id); // q, g or unknown
  bool isInvalid(int const& id); // id=-9000
  bool isAQuark(int const& id); // |id|<=6
  bool isALightQuark(int const& id); // |id|<=5, so includes b quarks
  bool isUpTypeQuark(int const& id); // |id|==2, 4, 6
  bool isDownTypeQuark(int const& id); // |id|==1, 3, 5
  bool isATopQuark(int const& id); // |id|==6
  bool isATauLepton(int const& id); // |id|==15
  bool isAGluon(int const& id); // id==21
  bool isAPhoton(int const& id); // id==22
  bool isAZBoson(int const& id); // id==23
  bool isAWBoson(int const& id); // |id|==24
  bool isAHiggs(int const& id); // id==25

  void orderParticles(
    std::vector<int> const& idlist,
    std::vector<bool(*)(const int)> const& testlist,
    std::vector<int>& ordering,
    bool allowUnknown=false
    );
  void groupIdenticalParticles(
    std::vector<int> const& ids,
    std::vector<std::vector<int>>& ordering,
    bool* hasUnknownParticles=0
    );
  void pairIdenticalParticles(
    std::vector<int> const& ids,
    std::vector<std::pair<int, int>>& ordering,
    bool allowUnknown=false
    );

  bool allEquivalent(std::vector<int> const& ids, bool allowUnknown=false);

  int convertPythiaStatus(int const& pSt);
}

#endif
