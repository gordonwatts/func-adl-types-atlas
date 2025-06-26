{%- if calib.correct_pileup %}
from AsgAnalysisAlgorithms.PileupAnalysisSequence import makePileupAnalysisSequence

# Use the sh object (sample Handler) to get the first tile and extract the filename
# from it, which can then be used to fetch the MC campaign. `calib.datatype`
# should contain `data` or `mc`
pileupSequence = makePileupAnalysisSequence("{{calib.datatype}}")
pileupSequence.configure(inputName={}, outputName={})
print(pileupSequence)  # For debugging

calibrationAlgSeq += pileupSequence
{% endif %}