import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing
from CondCore.CondDB.CondDB_cfi import *

sourceConnection = 'oracle://cms_omds_lb/CMS_RPC_CONF'
sourceConnection = 'oracle://cms_omds_adg/CMS_COND_GENERAL_R'

confType = "mc"

options = VarParsing.VarParsing()
options.register( 'runNumber',
                  1, #default value
                  VarParsing.VarParsing.multiplicity.singleton,
                  VarParsing.VarParsing.varType.int,
                  "Run number to be uploaded." )
options.register( 'destinationConnection',
                  'sqlite_file:GEMDeadStrips_{}.db'.format(confType), #default value
                  VarParsing.VarParsing.multiplicity.singleton,
                  VarParsing.VarParsing.varType.string,
                  "Connection string to the DB where payloads will be possibly written." )
options.register( 'targetConnection',
                  '', #default value
                  VarParsing.VarParsing.multiplicity.singleton,
                  VarParsing.VarParsing.varType.string,
                  """Connection string to the target DB:
                     if not empty (default), this provides the latest IOV and payloads to compare;
                     it is the DB where payloads should be finally uploaded.""" )
options.register( 'tag',
                  'GEMDeadStripsRcd',
                  VarParsing.VarParsing.multiplicity.singleton,
                  VarParsing.VarParsing.varType.string,
                  "Tag written in destinationConnection and finally appended in targetConnection." )
options.register( 'messageLevel',
                  0, #default value
                  VarParsing.VarParsing.multiplicity.singleton,
                  VarParsing.VarParsing.varType.int,
                  "Message level; default to 0" )

options.parseArguments()

CondDBConnection = CondDB.clone( connect = cms.string( options.destinationConnection ) )
CondDBConnection.DBParameters.messageLevel = cms.untracked.int32( options.messageLevel )

SourceDBConnection = CondDB.clone( connect = cms.string( sourceConnection ) )
SourceDBConnection.DBParameters.messageLevel = cms.untracked.int32( options.messageLevel )

#SourceDBConnection.DBParameters.authenticationPath = cms.untracked.string('/afs/cern.ch/cms/DB/conddb')

process = cms.Process("Write2DB")

process.load("CondCore.DBCommon.CondDBCommon_cfi")
process.MessageLogger = cms.Service( "MessageLogger",
                                     destinations = cms.untracked.vstring( 'cout' ),
                                     cout = cms.untracked.PSet( #default = cms.untracked.PSet( limit = cms.untracked.int32( 0 ) ),
                                                                threshold = cms.untracked.string( 'INFO' ) ) )

if options.messageLevel == 3:
    #enable LogDebug output: remember the USER_CXXFLAGS="-DEDM_ML_DEBUG" compilation flag!
    process.MessageLogger.cout = cms.untracked.PSet( threshold = cms.untracked.string( 'DEBUG' ) )
    process.MessageLogger.debugModules = cms.untracked.vstring( '*' )

process.source = cms.Source( "EmptyIOVSource",
                             lastValue = cms.uint64( options.runNumber ),
                             timetype = cms.string( 'runnumber' ),
                             firstValue = cms.uint64( options.runNumber ),
                             interval = cms.uint64( 1 ) )

process.PoolDBOutputService = cms.Service( "PoolDBOutputService",
                                           CondDBConnection,
                                           timetype = cms.untracked.string( 'runnumber' ),
                                           toPut = cms.VPSet( cms.PSet( record = cms.string( 'GEMDeadStripsRcd' ),
                                                                        tag = cms.string( options.tag ) ) ) )

process.WriteInDB = cms.EDAnalyzer( "GEMDeadStripsDBWriter",
                                    SinceAppendMode = cms.bool( True ),
                                    record = cms.string( 'GEMDeadStripsRcd' ),
                                    Source = cms.PSet( SourceDBConnection,
                                                       loggingOn = cms.untracked.bool( False ),
                                                       Validate = cms.untracked.int32( 0 ),
                                                       maskedStrip = cms.FileInPath('conditions/GEMDeadStrips/data/dead-strips-dummy.dat'))
)

process.p = cms.Path( process.WriteInDB )
