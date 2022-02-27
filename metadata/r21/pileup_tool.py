from AsgAnalysisAlgorithms.PileupAnalysisSequence import makePileupAnalysisSequence
from AsgAnalysisAlgorithms.AsgAnalysisAlgorithmsTest import pileupConfigFiles
prwfiles, lumicalcfiles = pileupConfigFiles('mc')
# Can't use pwrfiles, lumicalcfiles b.c. they are on cvms and docker does not have those.
# pileupSequence = makePileupAnalysisSequence( 'mc', userPileupConfigs=prwfiles, userLumicalcFiles=lumicalcfiles)
pileupSequence = makePileupAnalysisSequence( 'mc' )
pileupSequence.configure( inputName = {}, outputName = {} )
# print( pileupSequence ) # For debugging
calibrationAlgSeq += pileupSequence
