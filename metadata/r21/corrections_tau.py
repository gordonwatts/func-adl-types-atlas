from TauAnalysisAlgorithms.TauAnalysisSequence import makeTauAnalysisSequence
tauSequence = makeTauAnalysisSequence( 'mc', '{{calib.tau_working_point}}', postfix = '{{calib.tau_working_point}}', rerunTruthMatching=False)
tauSequence.configure( inputName = '{{calib.tau_collection}}',
                       outputName = '{{calib.tau_collection}}_{{calib.tau_working_point}}_%SYS%' )
calibrationAlgSeq += tauSequence
print( tauSequence ) # For debugging
# Output tau_collection = {{calib.tau_collection}}_{{calib.tau_working_point}}_{{ sys_error }}