import FWCore.ParameterSet.Config as cms

process = cms.Process("Demo")

process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(1)
)
process.source = cms.Source("EmptySource")

from Configuration.AlCa.GlobalTag import GlobalTag

process.GlobalTag.toGet = cms.VPSet(cms.PSet(record=cms.string("GEMChMapRcd"),
                                             tag=cms.string("GEMChMap2022"),
                                             connect=cms.string("sqlite_fip:conditions/GEMChMap/test/GEMChMap_2022.db")))

process.test = cms.EDAnalyzer("GEMMaskedStripsMapper",
                              inFile = cms.FileInPath('conditions/GEMMaskedStrips/data/masked-channels.dat'),
                              outName = cms.string('../data/masked-strips.dat'))

process.p = cms.Path(process.test)

### TO ACTIVATE LogTrace NEED TO COMPILE IT WITH:
### -----------------------------------------------------------
### --> scram b -j8 USER_CXXFLAGS="-DEDM_ML_DEBUG"
### Make sure that you first cleaned your CMSSW version:
### --> scram b clean
### before issuing the scram command above
###############################################################
process.load("FWCore.MessageLogger.MessageLogger_cfi")
# 
# 
process.MessageLogger.debugModules = cms.untracked.vstring("*")
process.MessageLogger.cerr.enable = False
process.MessageLogger.files.junk = dict()
process.MessageLogger.cout = cms.untracked.PSet(
    enable = cms.untracked.bool(True),
    threshold = cms.untracked.string("DEBUG"),
    default = cms.untracked.PSet( limit = cms.untracked.int32(0) ),
    FwkReport = cms.untracked.PSet( limit = cms.untracked.int32(-1) ),
    # GEMGeometryBuilderFromDDD   = cms.untracked.PSet( limit = cms.untracked.int32(-1) ),
    # GEMNumberingScheme            = cms.untracked.PSet( limit = cms.untracked.int32(-1) ),
)
