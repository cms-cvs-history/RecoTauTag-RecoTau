/*
 * =============================================================================
 *       Filename:  RecoTauImpactParameterSignificancePlugin.cc
 *
 *    Description:  Add the IP significance of the lead track w.r.t to the PV.
 *                  to a PFTau.
 *        Created:  10/31/2010 13:32:14
 *
 *         Authors:  Evan K. Friis (UC Davis), evan.klose.friis@cern.ch,
 *                   Simone Gennai, Ludovic Houchu
 *
 * =============================================================================
 */

#include "RecoTauTag/RecoTau/interface/RecoTauBuilderPlugins.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"

#include "FWCore/Framework/interface/ESHandle.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"

#include "TrackingTools/IPTools/interface/IPTools.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"
#include "TrackingTools/TransientTrack/interface/TransientTrack.h"

namespace reco { namespace tau {

class RecoTauImpactParameterSignificancePlugin : public RecoTauModifierPlugin {
  public:
    explicit RecoTauImpactParameterSignificancePlugin(
        const edm::ParameterSet& pset);
    virtual ~RecoTauImpactParameterSignificancePlugin() {}
    void operator()(PFTau& tau) const;
    virtual void beginEvent();
  private:
    edm::InputTag pvSrc_;
    const TransientTrackBuilder *builder_;
    const reco::Vertex* pv_;
};

RecoTauImpactParameterSignificancePlugin
::RecoTauImpactParameterSignificancePlugin(const edm::ParameterSet& pset)
  :RecoTauModifierPlugin(pset) {
  pvSrc_ = pset.getParameter<edm::InputTag>("pvSrc");
}

void RecoTauImpactParameterSignificancePlugin::beginEvent() {
  // Get primary vertex
  edm::Handle<reco::VertexCollection> pvs;
  evt()->getByLabel(pvSrc_, pvs);
  pv_ = &((*pvs)[0]);
  // Get tranisent track builder.
  edm::ESHandle<TransientTrackBuilder> myTransientTrackBuilder;
  evtSetup()->get<TransientTrackRecord>().get("TransientTrackBuilder",
                                              myTransientTrackBuilder);
  builder_= myTransientTrackBuilder.product();
}

void RecoTauImpactParameterSignificancePlugin::operator()(PFTau& tau) const {
  // Get the transient lead track
  if (tau.leadPFChargedHadrCand().isNonnull()) {
    TrackRef leadTrack = tau.leadPFChargedHadrCand()->trackRef();
    if (leadTrack.isNonnull()) {
      const TransientTrack track = builder_->build(leadTrack);
      GlobalVector direction(tau.jetRef()->px(), tau.jetRef()->py(),
                             tau.jetRef()->pz());
      // Compute the significance
      std::pair<bool,Measurement1D> ipsig =
          IPTools::signedTransverseImpactParameter(track, direction, *pv_);
      if (ipsig.first)
        tau.setleadPFChargedHadrCandsignedSipt(ipsig.second.significance());
    }
  }
}

}} // end namespace reco::tau
#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_EDM_PLUGIN(RecoTauModifierPluginFactory,
    reco::tau::RecoTauImpactParameterSignificancePlugin,
    "RecoTauImpactParameterSignificancePlugin");