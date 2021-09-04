#include <iostream>
#include "IvyPDGHelpers.h"
#include "IvyStreamHelpers.hh"


using IvyStreamHelpers::IVYout;
using IvyStreamHelpers::IVYerr;


bool IvyPDGHelpers::isAKnownJet(int const& id){
  if (IvyPDGHelpers::isAQuark(id) || IvyPDGHelpers::isAGluon(id)) return true;
  else return false;
}
bool IvyPDGHelpers::isAJet(int const& id){
  if (IvyPDGHelpers::isAnUnknownJet(id) || IvyPDGHelpers::isAQuark(id) || IvyPDGHelpers::isAGluon(id)) return true;
  else return false;
}
bool IvyPDGHelpers::isAnUnknownJet(int const& id){
  if (id==0) return true;
  else return false;
}
bool IvyPDGHelpers::isInvalid(int const& id){
  if (id==-9000) return true;
  else return false;
}
bool IvyPDGHelpers::isALightQuark(int const& id){
  int const& abs_id = std::abs(id);
  if (abs_id<=5 && abs_id>0) return true;
  else return false;
}
bool IvyPDGHelpers::isAQuark(int const& id){
  int const& abs_id = std::abs(id);
  if (abs_id<=6 && abs_id>0) return true;
  else return false;
}
bool IvyPDGHelpers::isUpTypeQuark(int const& id){
  int const& abs_id = std::abs(id);
  if (abs_id==2 || abs_id==4 || abs_id==6) return true;
  else return false;
}
bool IvyPDGHelpers::isDownTypeQuark(int const& id){
  int const& abs_id = std::abs(id);
  if (abs_id==1 || abs_id==3 || abs_id==5) return true;
  else return false;
}
bool IvyPDGHelpers::isATopQuark(int const& id){
  if (std::abs(id)==6) return true;
  else return false;
}
bool IvyPDGHelpers::isATauLepton(int const& id){
  if (std::abs(id)==15) return true;
  else return false;
}
bool IvyPDGHelpers::isAGluon(int const& id){
  if (std::abs(id)==21) return true;
  else return false;
}
bool IvyPDGHelpers::isALepton(int const& id){
  int const& abs_id = std::abs(id);
  if (abs_id==11 || abs_id==13 || abs_id==15) return true;
  else return false;
}
bool IvyPDGHelpers::isANeutrino(int const& id){
  int const& abs_id = std::abs(id);
  if (abs_id==12 || abs_id==14 || abs_id==16) return true;
  else return false;
}
bool IvyPDGHelpers::isAPhoton(int const& id){
  if (std::abs(id)==22) return true;
  else return false;
}
bool IvyPDGHelpers::isAZBoson(int const& id){
  if (std::abs(id)==23) return true;
  else return false;
}
bool IvyPDGHelpers::isAWBoson(int const& id){
  if (std::abs(id)==24) return true;
  else return false;
}
bool IvyPDGHelpers::isAHiggs(int const& id){
  if (std::abs(id)==25) return true;
  else return false;
}
void IvyPDGHelpers::orderParticles(
  std::vector<int> const& idlist,
  std::vector<bool(*)(const int)> const& testlist,
  std::vector<int>& ordering,
  bool allowUnknown
  ){
  ordering.clear();
  if (testlist.size() != idlist.size()){ IVYerr << "IvyPDGHelpers::orderParticles: List of ids and list of their tests do not have the same size!" << std::endl; return; }
  for (std::vector<bool(*)(const int)>::const_iterator it_test=testlist.cbegin(); it_test!=testlist.cend(); it_test++){
    bool testFilled=false;
    for (unsigned int ipart=0; ipart<idlist.size(); ipart++){
      bool isAvailable=true;
      for (int const& ord:ordering){
        if (ord==(int)ipart){ isAvailable=false; break; }
      }
      if (!isAvailable) continue;
      int tested = idlist.at(ipart);
      if (IvyPDGHelpers::isAnUnknownJet(tested)) continue;
      if ((*it_test)(tested)){
        ordering.push_back(tested);
        testFilled=true;
      }
    }
    if (!testFilled && allowUnknown){
      for (unsigned int ipart=0; ipart<idlist.size(); ipart++){
        bool isAvailable=true;
        for (int const& ord:ordering){
          if (ord==(int) ipart){ isAvailable=false; break; }
        }
        if (!isAvailable) continue;
        int tested = idlist.at(ipart);
        if (!IvyPDGHelpers::isAnUnknownJet(tested)) continue;
        if ((*it_test)(tested)){
          ordering.push_back(tested);
          testFilled=true;
        }
      }
    }
    if (!testFilled) break; // No need to try to order further, ordering is unsuccessful
  }
  if (idlist.size()!=ordering.size()) ordering.clear(); // This means not all particles were matched
}
void IvyPDGHelpers::groupIdenticalParticles(
  std::vector<int> const& ids,
  std::vector<std::vector<int>>& ordering,
  bool* hasUnknownParticles
  ){
  ordering.clear();
  if (hasUnknownParticles) *hasUnknownParticles=false;

  for (unsigned int ipart=0; ipart<ids.size(); ipart++){
    int const& id_part = ids.at(ipart);
    if (IvyPDGHelpers::isAnUnknownJet(id_part)) continue; // Group them the last
    bool grouped=false;
    for (auto& ord:ordering){
      if (ord.at(0)==id_part){ ord.push_back(ipart); grouped=true; break; }
    }
    if (!grouped){ std::vector<int> newgroup; newgroup.push_back(ipart); ordering.push_back(newgroup); }
  }
  bool firstUnknown=true;
  for (unsigned int ipart=0; ipart<ids.size(); ipart++){
    int const& id_part = ids.at(ipart);
    if (!IvyPDGHelpers::isAnUnknownJet(id_part)) continue; // Unknwon parton group
    if (hasUnknownParticles!=0) *hasUnknownParticles=true;
    if (firstUnknown){
      std::vector<int> newgroup; newgroup.push_back(ipart); ordering.push_back(newgroup);
      firstUnknown=false;
    }
    else ordering.at(ordering.size()-1).push_back(id_part);
  }
}
void IvyPDGHelpers::pairIdenticalParticles(
  std::vector<int> const& ids,
  std::vector<std::pair<int, int>>& ordering,
  bool allowUnknown
  ){
  ordering.clear();

  std::vector<std::vector<int>> grouping;
  bool hasUnknown=false;
  IvyPDGHelpers::groupIdenticalParticles(ids, grouping, &hasUnknown);

  for (auto const& group:grouping){
    unsigned int group_rep = group.at(0);
    if (group_rep>=ids.size()){ IVYerr << "IvyPDGHelpers::pairIdenticalParticles: Group " << group << " has a representative location >=ids.size (" << ids.size() << ")!" << std::endl; continue; }
    int group_flag = ids.at(group_rep);
    if (!IvyPDGHelpers::isAnUnknownJet(group_flag)){ // Known particles first!
      unsigned int npairs = group.size()/2;
      bool isOdd=(group.size() % 2 == 1);
      if (isOdd) npairs++;
      for (unsigned int ip=0; ip<npairs; ip++){
        if (isOdd && ip==npairs-1) ordering.push_back(std::pair<int, int>(group.at(2*ip), -1));
        else ordering.push_back(std::pair<int, int>(group.at(2*ip), group.at(2*ip+1)));
      }
    }
  }
  if (allowUnknown){
    for (auto const& group:grouping){
      unsigned int group_rep = group.at(0);
      if (group_rep>=ids.size()){ IVYerr << "IvyPDGHelpers::pairIdenticalParticles: Group " << group << " has a representative location >=ids.size (" << ids.size() << ")!" << std::endl; continue; }
      int group_flag = ids.at(group_rep);
      if (!IvyPDGHelpers::isAnUnknownJet(group_flag)) continue; // Unknown particles
      // Now that we found the unknown particles group, loop over it
      for (int const& pos:group){
        bool paired=false;
        // Check if there are unpaired particles left
        for (auto& order:ordering){
          if (order.second==-1 && IvyPDGHelpers::isAJet(ids.at(order.first))){
            order.second = pos;
            paired=true;
            break;
          }
        }
        if (paired) continue;
        // All pairs are filled, create a new pair
        ordering.push_back(std::pair<int, int>(pos, -1)); // Since this is a new pair, the second position is yet to be filled.
      }
    }
  }
}
bool IvyPDGHelpers::allEquivalent(std::vector<int> const& ids, bool allowUnknown){
  bool result=true;
  bool hasNonJet=false;
  for (std::vector<int>::const_iterator it_first=ids.cbegin(); it_first!=ids.cend(); it_first++){
    int const& id_first = *it_first;
    if (!IvyPDGHelpers::isAJet(id_first)) hasNonJet=true;
    if (allowUnknown && IvyPDGHelpers::isAnUnknownJet(id_first)){
      if (hasNonJet){ result = false; break; }
      else continue;
    }
    for (std::vector<int>::const_iterator it_second=it_first; it_second!=ids.cend(); it_second++){
      int const& id_second = *it_second;
      if (allowUnknown && IvyPDGHelpers::isAnUnknownJet(id_second)){
        if (hasNonJet){ result = false; break; }
        else continue;
      }
      if (id_first!=id_second){ result = false; break; }
    }
    if (!result) break;
  }
  return result;
}


int IvyPDGHelpers::convertPythiaStatus(int const& pSt){
  if (pSt==0) return 1;
  else if (pSt==1 || pSt==23) return 1;
  else if (pSt==21) return -1;
  else if (pSt==22 || pSt==62) return 2;
  else{
    IVYerr << "Unknown Pythia particle status: " << pSt << std::endl;
    return -99;
  }
}
