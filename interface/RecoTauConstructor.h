#ifndef RecoTauTag_RecoTau_RecoTauConstructor_h
#define RecoTauTag_RecoTau_RecoTauConstructor_h

/*
 * RecoTauConstructor
 *
 * A generalized builder of reco::PFTau objects.  Takes a variety of
 * different collections and converts them to the proper Ref format
 * needed for PFTau storage.  Automatically sets the p4, charge, and
 * other properties correctly.  Optionally, it can determine the 
 * lead track information, and copy the gamma candidates owned by the
 * reconstructed pi zeros into the appropriate PiZero collection.
 *
 * Note that the p4 of the tau is *always* set as the sum of objects in 
 * signal cone.
 *
 * Author: Evan K. Friis, UC Davis
 *
 * $Id $
 */

#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"
#include "DataFormats/TauReco/interface/PFTau.h"
#include "DataFormats/JetReco/interface/PFJetCollection.h"
#include "DataFormats/TauReco/interface/RecoTauPiZero.h"

namespace reco { namespace tau {

class RecoTauConstructor {
  public:
    enum Region {
      kSignal,
      kIsolation
    };

    enum ParticleType {
      kChargedHadron,
      kGamma,
      kNeutralHadron,
      kAll
    };

    /// Constructor with PFCandidate Handle
    RecoTauConstructor(const PFJetRef& jetRef,
        const edm::Handle<PFCandidateCollection>& pfCands, 
        bool copyGammasFromPiZeros=false);

    /*
     * Code to set leading candidates.  These are just wrappers about
     * the embedded taus methods, but with the ability to convert Ptrs
     * to Refs.
     */

    /// Set leading PFChargedHadron candidate
    template<typename T> void setleadPFChargedHadrCand(const T& cand) {
      tau_.setleadPFChargedHadrCand(convertToRef(cand));
    }

    /// Set leading PFGamma candidate
    template<typename T> void setleadPFNeutralCand(const T& cand) {
      tau_.setleadPFNeutralCand(convertToRef(cand));
    }

    /// Set leading PF candidate
    template<typename T> void setleadPFCand(const T& cand) {
      tau_.setleadPFCand(convertToRef(cand));
    }

    /// Append a PFCandidateRef to a given collection
    void addPFCand(Region region, ParticleType type, const PFCandidateRef& ref);

    /// Reserve a set amount of space for a given RefVector
    void reserve(Region region, ParticleType type, size_t size);

    // Add a collection of objects to the charged hadrons collection
    template<typename InputIterator>
      void addPFCands(Region region, ParticleType type, InputIterator begin, 
          InputIterator end) {
        for(InputIterator iter = begin; iter != end; ++iter) {
          addPFCand(region, type, convertToRef(*iter));
        }
      }

    /// Reserve a set amount of space for the PiZeros
    void reservePiZero(Region region, size_t size);

    /// Add a PiZero to the given collection
    void addPiZero(Region region, const RecoTauPiZero& piZero);

    /// Add a list of pi zeros to the input collection
    template<typename InputIterator>
      void addPiZeros(Region region, InputIterator begin, InputIterator end)
      {
        for(InputIterator iter = begin; iter != end; ++iter)
        {
          addPiZero(region, *iter);
        }
      }

    // Build and return the associated tau
    const PFTau& get(bool setupLeadingCandidates=true);

  private:
    bool copyGammas_;
    // Retreive collection associated to signal/iso and type
    PFCandidateRefVector * getCollection(Region region, ParticleType type);
    PFCandidateRef convertToRef(const PFCandidatePtr& pfPtr) const;
    PFCandidateRef convertToRef(const CandidatePtr& candPtr) const;
    PFCandidateRef convertToRef(const PFCandidateRef& pfRef) const;

    const edm::Handle<PFCandidateCollection>& pfCands_;
    PFTau tau_;
    std::map<std::pair<Region, ParticleType>, PFCandidateRefVector *> collections_;
};
} } // end reco::tau namespace
#endif
