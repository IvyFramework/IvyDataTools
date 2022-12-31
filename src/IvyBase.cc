#include <algorithm>
#include <utility>
#include <iterator>
#include <cassert>
#include "IvyBase.h"
#include "IvyBase.hpp"


using namespace std;


IvyBase::IvyBase() :
  BaseEmptyClass(),
  verbosity(MiscUtils::ERROR),
  currentTree(nullptr),
  eventCache_currentTTree(nullptr),
  eventCache_currentEvent(-1)
{}
IvyBase::~IvyBase(){}


void IvyBase::defineConsumedSloppy(TString name){
  if (std::find(this->sloppyConsumes.begin(), this->sloppyConsumes.end(), name)==this->sloppyConsumes.end()){
    this->sloppyConsumes.push_back(name);
    if (this->verbosity>=MiscUtils::INFO) IVYout << "IvyBase::defineConsumedSloppy: Consumed " << name << " will be treated sloppily." << endl;
  }
}

bool IvyBase::linkConsumes(BaseTree* tree){
  bool process = tree->isValid();
  if (!process && verbosity>=MiscUtils::ERROR) IVYerr << "IvyBase::linkConsumes: Tree " << tree->sampleIdentifier << " is already invalid." << endl;
  if (process){
#define SIMPLE_DATA_INPUT_DIRECTIVE(name, type, default_value) process &= this->linkConsumed<type>(tree);
    FUNDAMENTAL_DATA_INPUT_DIRECTIVES
#undef SIMPLE_DATA_INPUT_DIRECTIVE
#define SIMPLE_DATA_INPUT_DIRECTIVE(name, type, default_value) process &= this->linkConsumed<type*>(tree);
    CLASS_DATA_INPUT_DIRECTIVES
#undef SIMPLE_DATA_INPUT_DIRECTIVE
#define VECTOR_DATA_INPUT_DIRECTIVE(name, type) process &= this->linkConsumed<type*>(tree);
    VECTOR_DATA_INPUT_DIRECTIVES
#undef VECTOR_DATA_INPUT_DIRECTIVE
#define DOUBLEVECTOR_DATA_INPUT_DIRECTIVE(name, type) process &= this->linkConsumed<type*>(tree);
    DOUBLEVECTOR_DATA_INPUT_DIRECTIVES
#undef DOUBLEVECTOR_DATA_INPUT_DIRECTIVE
#define ARRAY_DATA_INPUT_DIRECTIVE(name, type, default_value) process &= this->linkConsumed<type* const>(tree);
    ARRAY_DATA_INPUT_DIRECTIVES
#undef ARRAY_DATA_INPUT_DIRECTIVE
    // Silence unused branches
    tree->silenceUnused();
  }
  if (!process && verbosity>=MiscUtils::ERROR) IVYerr << "IvyBase::linkConsumes: Linking failed for some reason for tree " << tree->sampleIdentifier << endl;
  return process;
}

bool IvyBase::cacheEvent(){
  return (
    this->currentTree
    &&
    this->currentTree->getCurrentEventInfo(this->eventCache_currentTTree, this->eventCache_currentEvent)
    );
}
bool IvyBase::isAlreadyCached() const{
  return (
    this->currentTree
    &&
    this->currentTree->isSameEvent(this->eventCache_currentTTree, this->eventCache_currentEvent)
    );
}
void IvyBase::resetCache(){
  this->eventCache_currentTTree = nullptr;
  this->eventCache_currentEvent = -1;
}

bool IvyBase::wrapTree(BaseTree* tree){
  this->currentTree = tree;
  this->eventCache_currentTTree = nullptr;
  this->eventCache_currentEvent = -1;
  if (!(this->currentTree)){
    if (this->verbosity>=MiscUtils::ERROR) IVYerr << "IvyBase::wrapTree: The input tree is null!" << endl;
    return false;
  }
  return this->linkConsumes(this->currentTree);
}
