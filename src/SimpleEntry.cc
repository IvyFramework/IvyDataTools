#include <iostream>
#include "SimpleEntry.h"
#include "SimpleEntry.hpp"
#include "SampleHelpersCore.h"


using namespace std;


// Simple scalar entry
SimpleScalarEntry::SimpleScalarEntry() : BaseIOMessenger() {}
SimpleScalarEntry::SimpleScalarEntry(SimpleScalarEntry const& other) :
  BaseIOMessenger(other)
#define SIMPLE_DATA_OUTPUT_DIRECTIVE(name_t, type) , named##name_t##s(other.named##name_t##s)
  SIMPLE_DATA_OUTPUT_DIRECTIVES
#undef SIMPLE_DATA_OUTPUT_DIRECTIVE
{}
SimpleScalarEntry::SimpleScalarEntry(SimpleScalarEntry&& other) : BaseIOMessenger(other)
{ this->swap(other); }

void SimpleScalarEntry::swap(SimpleScalarEntry& other){
#define SIMPLE_DATA_OUTPUT_DIRECTIVE(name_t, type) std::swap(named##name_t##s, other.named##name_t##s);
  SIMPLE_DATA_OUTPUT_DIRECTIVES;
#undef SIMPLE_DATA_OUTPUT_DIRECTIVE
}
SimpleScalarEntry& SimpleScalarEntry::operator=(SimpleScalarEntry const& other){
  SimpleScalarEntry tmp(other);
  this->swap(tmp);
  return *this;
}


// Simple vector entry
SimpleVectorEntry::SimpleVectorEntry() : BaseIOMessenger() {}
SimpleVectorEntry::SimpleVectorEntry(SimpleVectorEntry const& other) :
  BaseIOMessenger(other)
#define VECTOR_DATA_OUTPUT_DIRECTIVE(name_t, type) , namedV##name_t##s(other.namedV##name_t##s)
  VECTOR_DATA_OUTPUT_DIRECTIVES
#undef VECTOR_DATA_OUTPUT_DIRECTIVE
{}
SimpleVectorEntry::SimpleVectorEntry(SimpleVectorEntry&& other) : BaseIOMessenger(other)
{ this->swap(other); }

void SimpleVectorEntry::swap(SimpleVectorEntry& other){
#define VECTOR_DATA_OUTPUT_DIRECTIVE(name_t, type) std::swap(namedV##name_t##s, other.namedV##name_t##s);
  VECTOR_DATA_OUTPUT_DIRECTIVES;
#undef VECTOR_DATA_OUTPUT_DIRECTIVE
}
SimpleVectorEntry& SimpleVectorEntry::operator=(SimpleVectorEntry const& other){
  SimpleVectorEntry tmp(other);
  this->swap(tmp);
  return *this;
}


// Simple tensor entry
SimpleTensorEntry::SimpleTensorEntry() : BaseIOMessenger() {}
SimpleTensorEntry::SimpleTensorEntry(SimpleTensorEntry const& other) :
  BaseIOMessenger(other)
#define DOUBLEVECTOR_DATA_OUTPUT_DIRECTIVE(name_t, type) , namedVV##name_t##s(other.namedVV##name_t##s)
  DOUBLEVECTOR_DATA_OUTPUT_DIRECTIVES
#undef DOUBLEVECTOR_DATA_OUTPUT_DIRECTIVE
{}
SimpleTensorEntry::SimpleTensorEntry(SimpleTensorEntry&& other) : BaseIOMessenger(other)
{ this->swap(other); }

void SimpleTensorEntry::swap(SimpleTensorEntry& other){
#define DOUBLEVECTOR_DATA_OUTPUT_DIRECTIVE(name_t, type) std::swap(namedVV##name_t##s, other.namedVV##name_t##s);
  DOUBLEVECTOR_DATA_OUTPUT_DIRECTIVES;
#undef DOUBLEVECTOR_DATA_OUTPUT_DIRECTIVE
}
SimpleTensorEntry& SimpleTensorEntry::operator=(SimpleTensorEntry const& other){
  SimpleTensorEntry tmp(other);
  this->swap(tmp);
  return *this;
}


// Collection of scalar, vector and tensor entry types
SimpleEntry::SimpleEntry() :
  BaseIOMessenger(), SimpleScalarEntry(), SimpleVectorEntry(), SimpleTensorEntry(),
  id(0), trackingval(0), weight(0)
{}
SimpleEntry::SimpleEntry(int const& id_, float const& trackingval_, float weight_) :
  BaseIOMessenger(), SimpleScalarEntry(), SimpleVectorEntry(), SimpleTensorEntry(),
  id(id_), trackingval(trackingval_), weight(weight_)
{}
SimpleEntry::SimpleEntry(int const& id_, float const& trackingval_, std::vector<float> const& recoval_, float weight_) :
  BaseIOMessenger(), SimpleScalarEntry(), SimpleVectorEntry(), SimpleTensorEntry(),
  id(id_), trackingval(trackingval_), weight(weight_), recoval(recoval_)
{}
SimpleEntry::SimpleEntry(SimpleEntry const& other) :
  BaseIOMessenger(other), SimpleScalarEntry(other), SimpleVectorEntry(other), SimpleTensorEntry(other),
  id(other.id),
  trackingval(other.trackingval),
  weight(other.weight),
  recoval(other.recoval)
{}
SimpleEntry::SimpleEntry(SimpleEntry&& other) :
  BaseIOMessenger(other), SimpleScalarEntry(other), SimpleVectorEntry(other), SimpleTensorEntry(other),
  id(0), trackingval(0), weight(0)
{ this->swap(other); }

void SimpleEntry::swap(SimpleEntry& other){
  SimpleScalarEntry::swap(other);
  SimpleVectorEntry::swap(other);
  SimpleTensorEntry::swap(other);

  std::swap(id, other.id);
  std::swap(trackingval, other.trackingval);
  std::swap(weight, other.weight);
  std::swap(recoval, other.recoval);
}
SimpleEntry& SimpleEntry::operator=(SimpleEntry const& other){
  SimpleEntry tmp(other);
  this->swap(tmp);
  return *this;
}

bool SimpleEntry::operator != (SimpleEntry const& other) const{ return trackingval!=other.trackingval; }
bool SimpleEntry::operator == (SimpleEntry const& other) const{ return trackingval==other.trackingval; }
bool SimpleEntry::operator > (SimpleEntry const& other) const{ return trackingval>other.trackingval; }
bool SimpleEntry::operator >= (SimpleEntry const& other) const{ return trackingval>=other.trackingval; }
bool SimpleEntry::operator < (SimpleEntry const& other) const{ return trackingval<other.trackingval; }
bool SimpleEntry::operator <= (SimpleEntry const& other) const{ return trackingval<=other.trackingval; }

void SimpleEntry::cropByTrackingVal(std::vector<SimpleEntry>& vec, float minval, float maxval){
  std::vector<unsigned int> erasepos;
  unsigned int pos=0;
  for (std::vector<SimpleEntry>::const_iterator it=vec.cbegin(); it!=vec.cend(); it++){
    if (it->trackingval<minval || it->trackingval>maxval) erasepos.push_back(pos);
    pos++;
  }
  for (std::vector<unsigned int>::reverse_iterator ipos=erasepos.rbegin(); ipos!=erasepos.rend(); ipos++) vec.erase(vec.begin()+(*ipos));
}
void SimpleEntry::writeToTree(std::vector<SimpleEntry>::const_iterator const& vecBegin, std::vector<SimpleEntry>::const_iterator const& vecEnd, TTree* const& tree, bool forceRelink){
  if (!tree) return;
  SimpleEntry commonEntry;
  for (std::vector<SimpleEntry>::const_iterator it=vecBegin; it!=vecEnd; it++){
    SimpleEntry const& entry = *it;
#define SIMPLE_DATA_OUTPUT_DIRECTIVE(name_t, type) for (auto itb=entry.named##name_t##s.begin(); itb!=entry.named##name_t##s.end(); itb++) commonEntry.setNamedVal(itb->first, itb->second);
#define VECTOR_DATA_OUTPUT_DIRECTIVE(name_t, type) for (auto itb=entry.namedV##name_t##s.begin(); itb!=entry.namedV##name_t##s.end(); itb++) commonEntry.setNamedVal(itb->first, itb->second);
#define DOUBLEVECTOR_DATA_OUTPUT_DIRECTIVE(name_t, type) for (auto itb=entry.namedVV##name_t##s.begin(); itb!=entry.namedVV##name_t##s.end(); itb++) commonEntry.setNamedVal(itb->first, itb->second);
    SIMPLE_DATA_OUTPUT_DIRECTIVES
    VECTOR_DATA_OUTPUT_DIRECTIVES
    DOUBLEVECTOR_DATA_OUTPUT_DIRECTIVES
#undef SIMPLE_DATA_OUTPUT_DIRECTIVE
#undef VECTOR_DATA_OUTPUT_DIRECTIVE
#undef DOUBLEVECTOR_DATA_OUTPUT_DIRECTIVE
    if (it==vecBegin){
#define SIMPLE_DATA_OUTPUT_DIRECTIVE(name_t, type) for (auto itb=commonEntry.named##name_t##s.begin(); itb!=commonEntry.named##name_t##s.end(); itb++) SampleHelpers::putBranch(tree, itb->first, itb->second, forceRelink);
#define VECTOR_DATA_OUTPUT_DIRECTIVE(name_t, type) for (auto itb=commonEntry.namedV##name_t##s.begin(); itb!=commonEntry.namedV##name_t##s.end(); itb++) SampleHelpers::putBranch(tree, itb->first, itb->second, forceRelink);
#define DOUBLEVECTOR_DATA_OUTPUT_DIRECTIVE(name_t, type) for (auto itb=commonEntry.namedVV##name_t##s.begin(); itb!=commonEntry.namedVV##name_t##s.end(); itb++) SampleHelpers::putBranch(tree, itb->first, itb->second, forceRelink);
      SIMPLE_DATA_OUTPUT_DIRECTIVES
      VECTOR_DATA_OUTPUT_DIRECTIVES
      DOUBLEVECTOR_DATA_OUTPUT_DIRECTIVES
#undef SIMPLE_DATA_OUTPUT_DIRECTIVE
#undef VECTOR_DATA_OUTPUT_DIRECTIVE
#undef DOUBLEVECTOR_DATA_OUTPUT_DIRECTIVE
    }
    tree->Fill();
  }
}

void SimpleEntry::print(){
  cout << "Simple entry:" << endl;
  cout << " - Id = " << id << endl;
  cout << " - Weight = " << weight << endl;
  cout << " - Trueval: " << trackingval << endl;
  cout << " - Recoval: ";
  for (auto& v : recoval) cout << v << " ";
  cout << endl;
}

void ExtBin::addEvent(SimpleEntry const& evt){ collection.push_back(evt); }

