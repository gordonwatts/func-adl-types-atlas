jetContainer = "{{calib.jet_collection}}"
from JetAnalysisAlgorithms.JetAnalysisSequence import makeJetAnalysisSequence

# Do not run ghost muon association if you have already
# corrected objects.
jetSequence = makeJetAnalysisSequence(
    "{{calib.datatype}}",
    jetContainer,
    runGhostMuonAssociation={{calib.run_jet_ghost_muon_association}},
)
jetSequence.configure(inputName=jetContainer, outputName=jetContainer + "_Base_%SYS%")
jetSequence.JvtEfficiencyAlg.truthJetCollection = "{{calib.jet_calib_truth_collection}}"
try:
    jetSequence.ForwardJvtEfficiencyAlg.truthJetCollection = (
        "{{calib.jet_calib_truth_collection}}"
    )
except AttributeError:
    pass

calibrationAlgSeq += jetSequence
print(jetSequence)  # For debugging

# Include, and then set up the jet analysis algorithm sequence:
from JetAnalysisAlgorithms.JetJvtAnalysisSequence import makeJetJvtAnalysisSequence

jvtSequence = makeJetJvtAnalysisSequence("mc", jetContainer, enableCutflow=True)
jvtSequence.configure(
    inputName={"jets": jetContainer + "_Base_%SYS%"},
    outputName={"jets": jetContainer + "Calib_%SYS%"},
)
calibrationAlgSeq += jvtSequence
print(jvtSequence)  # For debugging
output_jet_container = "{{calib.jet_collection}}Calib_%SYS%"
# Output jet_collection = {{calib.jet_collection}}Calib_{{ sys_error }}
