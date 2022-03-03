from EgammaAnalysisAlgorithms.ElectronAnalysisSequence import makeElectronAnalysisSequence
electronSequence = makeElectronAnalysisSequence( 'mc', '{{calib.electron_working_point}}.{{calib.electron_isolation}}', postfix = '{{calib.electron_working_point}}_{{calib.electron_isolation}}')
electronSequence.configure( inputName = '{{calib.electron_collection}}',
                            outputName = '{{calib.electron_collection}}_{{calib.working_point}}_{{calib.isolation}}_%SYS%' )
calibrationAlgSeq += electronSequence
print( electronSequence ) # For debugging
# Output electron_collection = {{calib.electron_collection}}_{{working_point}}_{{isolation}}_OR_{{ sys_error }}