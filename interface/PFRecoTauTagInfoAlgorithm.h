#ifndef RecoTauTag_RecoTau_PFRecoTauTagInfoAlgorithm_H
#define RecoTauTag_RecoTau_PFRecoTauTagInfoAlgorithm_H

#include "DataFormats/TauReco/interface/PFTauTagInfo.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/ParticleFlowReco/interface/PFBlock.h" 
#include "DataFormats/ParticleFlowReco/interface/PFBlockElement.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "Math/GenVector/VectorUtil.h" 

#include "RecoTauTag/TauTagTools/interface/TauTagTools.h"

using namespace std;
using namespace reco;
using namespace edm;

class  PFRecoTauTagInfoAlgorithm  {
 public:
  PFRecoTauTagInfoAlgorithm(){}
  PFRecoTauTagInfoAlgorithm(const ParameterSet&);
  ~PFRecoTauTagInfoAlgorithm(){}
  PFTauTagInfo buildPFTauTagInfo(const PFJetRef&,const TrackRefVector&,const Vertex&); 
 private: 
  double ChargedHadrCand_tkminPt_;
  int ChargedHadrCand_tkminPixelHitsn_;
  int ChargedHadrCand_tkminTrackerHitsn_;
  double ChargedHadrCand_tkmaxipt_;
  double ChargedHadrCand_tkmaxChi2_;
  double ChargedHadrCand_tkPVmaxDZ_;
  // 
  double NeutrHadrCand_HcalclusminE_;
  // 
  double GammaCand_EcalclusminE_;
  // 
  double tkminPt_;
  int tkminPixelHitsn_;
  int tkminTrackerHitsn_;
  double tkmaxipt_;
  double tkmaxChi2_;
  double tkPVmaxDZ_;
  // 
  bool UsePVconstraint_;
};
#endif 
