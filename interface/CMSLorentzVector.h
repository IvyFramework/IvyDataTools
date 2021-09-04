#ifndef IVYLORENTZVECTOR_H
#define IVYLORENTZVECTOR_H

#ifdef _COMPILE_STANDALONE_

#include "Math/PtEtaPhiE4D.h"
#include "Math/PtEtaPhiM4D.h"
#include "Math/PxPyPzE4D.h"
#include "Math/PxPyPzM4D.h"
#include "Math/LorentzVector.h"
#include "Math/Vector3D.h"

typedef ROOT::Math::LorentzVector< ROOT::Math::PxPyPzE4D<float> > IvyLorentzVector_f;
typedef ROOT::Math::LorentzVector< ROOT::Math::PxPyPzE4D<double> > IvyLorentzVector_d;
typedef ROOT::Math::LorentzVector< ROOT::Math::PtEtaPhiM4D<float> > IvyPolarLorentzVector_f;
typedef ROOT::Math::LorentzVector< ROOT::Math::PtEtaPhiM4D<double> > IvyPolarLorentzVector_d;
typedef ROOT::Math::DisplacementVector3D< ROOT::Math::Cartesian3D<float> > IvyVector3D_f;
typedef ROOT::Math::DisplacementVector3D< ROOT::Math::Cartesian3D<double> > IvyVector3D_d;

#else

#include <DataFormats/Math/interface/LorentzVector.h>
#include <DataFormats/Math/interface/Vector3D.h>

typedef math::XYZTLorentzVectorF IvyLorentzVector_f;
typedef math::XYZTLorentzVectorD IvyLorentzVector_d;
typedef math::PtEtaPhiMLorentzVectorF IvyPolarLorentzVector_f;
typedef math::PtEtaPhiMLorentzVectorD IvyPolarLorentzVector_d;
typedef math::XYZVectorF IvyVector3D_f;
typedef math::XYZVectorD IvyVector3D_d;

#endif

#include "Math/GenVector/DisplacementVector2D.h"

typedef ROOT::Math::DisplacementVector2D< ROOT::Math::Cartesian2D<float> > IvyVector2D_f;
typedef ROOT::Math::DisplacementVector2D< ROOT::Math::Cartesian2D<double> > IvyVector2D_d;


typedef IvyLorentzVector_f CMSLorentzVector_f;
typedef IvyLorentzVector_d CMSLorentzVector_d;
typedef IvyLorentzVector_f IvyLorentzVector;
typedef CMSLorentzVector_f CMSLorentzVector;

typedef IvyPolarLorentzVector_f CMSPolarLorentzVector_f;
typedef IvyPolarLorentzVector_d CMSPolarLorentzVector_d;
typedef IvyPolarLorentzVector_f IvyPolarLorentzVector;
typedef CMSPolarLorentzVector_f CMSPolarLorentzVector;

typedef IvyVector3D_f CMSVector3D_f;
typedef IvyVector3D_d CMSVector3D_d;
typedef IvyVector3D_f IvyVector3D;
typedef CMSVector3D_f CMSVector3D;

typedef IvyVector2D_f CMSVector2D_f;
typedef IvyVector2D_d CMSVector2D_d;
typedef IvyVector2D_f IvyVector2D;
typedef CMSVector2D_f CMSVector2D;

#endif
