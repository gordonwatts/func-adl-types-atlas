jetContainer = '{{calib.jet_collection}}'
from JetAnalysisAlgorithms.JetAnalysisSequence import makeJetAnalysisSequence
jetSequence = makeJetAnalysisSequence( 'mc', jetContainer)
jetSequence.configure( inputName = jetContainer, outputName = jetContainer + '_Base_%SYS%' )
jetSequence.JvtEfficiencyAlg.truthJetCollection = '{{calib.jet_calib_truth_collection}}'
jetSequence.ForwardJvtEfficiencyAlg.truthJetCollection = '{{calib.jet_calib_truth_collection}}'
calibrationAlgSeq += jetSequence
print( jetSequence ) # For debugging
# Include, and then set up the jet analysis algorithm sequence:
from JetAnalysisAlgorithms.JetJvtAnalysisSequence import makeJetJvtAnalysisSequence
jvtSequence = makeJetJvtAnalysisSequence( 'mc', jetContainer, enableCutflow=True )
jvtSequence.configure( inputName = {'jets'      : jetContainer + '_Base_%SYS%' },
                       outputName = { 'jets'      : jetContainer + 'Calib_%SYS%' },
                       )
calibrationAlgSeq += jvtSequence
print( jvtSequence ) # For debugging
output_jet_container = "{{calib.jet_collection}}Calib_%SYS%"
# Output jet_collection = {{calib.jet_collection}}Calib_{{ sys_error }}