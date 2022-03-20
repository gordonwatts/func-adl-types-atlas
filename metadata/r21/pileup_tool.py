from AsgAnalysisAlgorithms.PileupAnalysisSequence import makePileupAnalysisSequence
pileupSequence = makePileupAnalysisSequence( 'mc' )
pileupSequence.configure( inputName = {}, outputName = {} )
print( pileupSequence ) # For debugging
calibrationAlgSeq += pileupSequence