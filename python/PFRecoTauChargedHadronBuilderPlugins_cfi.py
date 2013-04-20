import FWCore.ParameterSet.Config as cms

'''

Configuration for ChargedHadron producer plugins.

Author: Christian Veelken, LLR


'''

from RecoTauTag.RecoTau.PFRecoTauQualityCuts_cfi import PFTauQualityCuts

# Produce a ChargedHadron candidate for each charged PFCandidate
chargedPFCandidates = cms.PSet(
    name = cms.string("chargedPFCandidates"),
    plugin = cms.string("PFRecoTauChargedHadronFromPFCandidatePlugin"),
    # process PFChargedHadrons and PFElectrons
    # (numbering scheme defined in DataFormats/ParticleFlowCandidate/interface/PFCandidate.h)
    chargedHadronCandidatesParticleIds = cms.vint32(1, 2),    
    qualityCuts = PFTauQualityCuts,
    dRmergeNeutralHadron = cms.double(0.005),
    dRmergePhoton = cms.double(0.005)
)

# Produce a ChargedHadron candidate for each reco::Track
# (overlap with charged PFCandidate is removed by PFRecoTauChargedHadronProducer module)
tracks = cms.PSet(
    name = cms.string("tracks"),
    plugin = cms.string("PFRecoTauChargedHadronFromTrackPlugin"),
    srcTracks = cms.InputTag('generalTracks'),
    dRcone = cms.double(0.5),
    qualityCuts = PFTauQualityCuts,
    dRmergeNeutralHadron = cms.double(0.10),
    dRmergePhoton = cms.double(0.05)
)

# Produce a ChargedHadron candidate for high Pt PFNeutralHadrons
PFNeutralHadrons = cms.PSet(
    name = cms.string("PFNeutralHadrons"),
    plugin = cms.string("PFRecoTauChargedHadronFromPFCandidatePlugin"),
    # process PFNeutralHadrons
    # (numbering scheme defined in DataFormats/ParticleFlowCandidate/interface/PFCandidate.h)
    chargedHadronCandidatesParticleIds = cms.vint32(5),
    qualityCuts = PFTauQualityCuts,
    dRmergeNeutralHadron = cms.double(0.01),
    dRmergePhoton = cms.double(0.01)
)