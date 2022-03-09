# run like
# cmsRun xsec_cfg.py datasetName=/WWZ_TuneCUETP8M1_13TeV-amcatnlo-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/MINIAODSIM maxEvents=100000

import FWCore.ParameterSet.Config as cms
from FWCore.ParameterSet.VarParsing import VarParsing

from IvyFramework.IvyDataTools.cmseostools import listFiles


def get_max_files(DAS_name, max_files, dbase) :
   result = []
   file_names = listFiles(DAS_name, dbase)
   nfiles = len(file_names)
   if max_files>0:
      nfiles=min(len(file_names), max_files)
   for i in range(nfiles):
      if file_names[i]:
         result.append(file_names[i])
   return result


options = VarParsing ('analysis')

options.register('datasetName',
		'',
		VarParsing.multiplicity.singleton,
		VarParsing.varType.string,
		"DAS-style name of a primary dataset, e.g. /ZZTo4L_13TeV-sherpa/RunIISummer16MiniAODv2-PUMoriond17_v2_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/MINIAODSIM")
options.register('localdir', False, mytype=VarParsing.varType.bool)
options.register('maxfiles', 50, mytype=VarParsing.varType.int)
options.register('disableDuplicateCheck', False, mytype=VarParsing.varType.bool)

options.parseArguments()

dbase="dbs"
if options.localdir:
   dbase="local"

process = cms.Process('XSec')

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(options.maxEvents),
)

process.load('FWCore.MessageService.MessageLogger_cfi')
# How often do you want to see the "Begin processing the .."
process.MessageLogger.cerr.FwkReport.reportEvery = 1000

process.source = cms.Source ("PoolSource",
   fileNames = cms.untracked.vstring(*get_max_files(options.datasetName, options.maxfiles, dbase)),
)
if options.disableDuplicateCheck:
   process.source.duplicateCheckMode = cms.untracked.string("noDuplicateCheck")

process.xsec = cms.EDAnalyzer("GenXSecAnalyzer")

process.ana = cms.Path(process.xsec)
process.schedule = cms.Schedule(process.ana)
