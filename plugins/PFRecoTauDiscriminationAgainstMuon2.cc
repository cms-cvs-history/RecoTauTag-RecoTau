
/** \class PFRecoTauDiscriminationAgainstMuon2
 *
 * Compute tau Id. discriminator against muons.
 * 
 * \author Christian Veelken, LLR
 *
 * \version $Revision: 1.6 $
 *
 * $Id: PFRecoTauDiscriminationAgainstMuon2.cc,v 1.6 2013/03/12 18:17:20 jez Exp $
 *
 */

#include "RecoTauTag/RecoTau/interface/TauDiscriminationProducerBase.h"

#include "FWCore/Utilities/interface/Exception.h"

#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "DataFormats/MuonReco/interface/MuonSelectors.h"
#include "DataFormats/TrackReco/interface/HitPattern.h"
#include "DataFormats/TrackReco/interface/Track.h"

#include <string>

class PFRecoTauDiscriminationAgainstMuon2 : public PFTauDiscriminationProducerBase 
{
  enum { kLoose, kMedium, kTight, kCustom };
 public:
  explicit PFRecoTauDiscriminationAgainstMuon2(const edm::ParameterSet& cfg)
    : PFTauDiscriminationProducerBase(cfg) 
  {   
    std::string discriminatorOption_string  = cfg.getParameter<std::string>("discriminatorOption");  
    if      ( discriminatorOption_string == "loose"  ) discriminatorOption_ = kLoose;
    else if ( discriminatorOption_string == "medium" ) discriminatorOption_ = kMedium;
    else if ( discriminatorOption_string == "tight"  ) discriminatorOption_ = kTight;
    else if ( discriminatorOption_string == "custom" ) discriminatorOption_ = kCustom;
    else throw edm::Exception(edm::errors::UnimplementedFeature) 
      << " Invalid Configuration parameter 'discriminatorOption' = " << discriminatorOption_string << " !!\n";
    hop_ = cfg.getParameter<double>("HoPMin"); 
    maxNumberOfMatches_ = cfg.exists("maxNumberOfMatches") ? cfg.getParameter<int>("maxNumberOfMatches"): 0;
    doCaloMuonVeto_ = cfg.exists("doCaloMuonVeto") ? cfg.getParameter<bool>("doCaloMuonVeto"): false;
    maxNumberOfHitsLast2Stations_ = cfg.exists("maxNumberOfHitsLast2Stations") ? cfg.getParameter<int>("maxNumberOfHitsLast2Stations"): 0;
   }
  ~PFRecoTauDiscriminationAgainstMuon2() {} 

  double discriminate(const reco::PFTauRef&);

 private:  
  int discriminatorOption_;
  double hop_;
  int maxNumberOfMatches_;
  bool doCaloMuonVeto_;
  int maxNumberOfHitsLast2Stations_;
};

double PFRecoTauDiscriminationAgainstMuon2::discriminate(const reco::PFTauRef& pfTau)
{
  int numMatches = 0;
 
  std::vector<int> numHitsDT(4);
  std::vector<int> numHitsCSC(4);
  std::vector<int> numHitsRPC(4);
  for ( int iStation = 0; iStation < 4; ++iStation ) {
    numHitsDT[iStation]  = 0;
    numHitsCSC[iStation] = 0;
    numHitsRPC[iStation] = 0;
  }

  if ( pfTau->leadPFChargedHadrCand().isNonnull() ) {
    reco::MuonRef muonRef = pfTau->leadPFChargedHadrCand()->muonRef();      
    if ( muonRef.isNonnull() ) {
      numMatches = muonRef->numberOfMatches(reco::Muon::NoArbitration);
      if ( muonRef->outerTrack().isNonnull() ) {
	const reco::HitPattern & muonHitPattern = muonRef->outerTrack()->hitPattern();
	for ( int iHit = 0; iHit < muonHitPattern.numberOfHits(); ++iHit ) {
	  uint32_t hit = muonHitPattern.getHitPattern(iHit);
	  if ( hit == 0 ) break;	    
	  if ( muonHitPattern.muonHitFilter(hit) && muonHitPattern.getHitType(hit) == TrackingRecHit::valid ) {
	    int muonStation = muonHitPattern.getMuonStation(hit) - 1; // CV: map into range 0..3
	    if ( muonStation >= 0 && muonStation < 4 ) {
	      if      ( muonHitPattern.muonDTHitFilter(hit)  ) ++numHitsDT[muonStation];
	      else if ( muonHitPattern.muonCSCHitFilter(hit) ) ++numHitsCSC[muonStation];
	      else if ( muonHitPattern.muonRPCHitFilter(hit) ) ++numHitsRPC[muonStation];
	    }
	  }
	}
      }
    }
  }
  
  int numLast2StationsWithHits = 0;
  for ( int iStation = 2; iStation < 4; ++iStation ) {
    if ( numHitsDT[iStation]  > 0 ) ++numLast2StationsWithHits;
    if ( numHitsCSC[iStation] > 0 ) ++numLast2StationsWithHits;
    if ( numHitsRPC[iStation] > 0 ) ++numLast2StationsWithHits;
  }

  bool passesCaloMuonVeto = true;
  if ( pfTau->leadPFChargedHadrCand().isNonnull() ) {
    double energyECALplusHCAL = pfTau->leadPFChargedHadrCand()->ecalEnergy() + pfTau->leadPFChargedHadrCand()->hcalEnergy();
    if ( pfTau->decayMode() == 0 && energyECALplusHCAL < (hop_*pfTau->leadPFChargedHadrCand()->p()) ) passesCaloMuonVeto = false;
  }
  
  double discriminatorValue = 0.;
  if      ( discriminatorOption_ == kLoose  && numMatches <= maxNumberOfMatches_                                                                                    ) discriminatorValue = 1.;
  else if ( discriminatorOption_ == kMedium && numMatches <= maxNumberOfMatches_ && numLast2StationsWithHits <= maxNumberOfHitsLast2Stations_                       ) discriminatorValue = 1.;
  else if ( discriminatorOption_ == kTight  && numMatches <= maxNumberOfMatches_ && numLast2StationsWithHits <= maxNumberOfHitsLast2Stations_ && passesCaloMuonVeto ) discriminatorValue = 1.;
  else if ( discriminatorOption_ == kCustom){
    bool pass = true;
    if(maxNumberOfMatches_>=0 && numMatches > maxNumberOfMatches_) pass=false;
    if(maxNumberOfHitsLast2Stations_>=0 && numLast2StationsWithHits > maxNumberOfHitsLast2Stations_) pass=false;
    if(doCaloMuonVeto_ && !passesCaloMuonVeto) pass=false;
    discriminatorValue = pass ? 1.: 0.;
    }

  return discriminatorValue;
} 

DEFINE_FWK_MODULE(PFRecoTauDiscriminationAgainstMuon2);
