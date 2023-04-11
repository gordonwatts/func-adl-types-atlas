from AsgAnalysisAlgorithms.PileupAnalysisSequence import makePileupAnalysisSequence

# Use the sh object (sample Handler) to get the first tile and extract the filename
# from it, which can then be used to fetch the MC campaign. `calib.datatype`
# should contain `data` or `mc`
pileupSequence = makePileupAnalysisSequence(
    "{{calib.datatype}}", files=sh.at(0).fileName(0)
)
pileupSequence.configure(inputName={}, outputName={})
print(pileupSequence)  # For debugging

calibrationAlgSeq += pileupSequence
