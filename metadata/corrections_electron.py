from EgammaAnalysisAlgorithms.ElectronAnalysisSequence import makeElectronAnalysisSequence
electronSequence = makeElectronAnalysisSequence( 'mc', '{{calib.electron_working_point}}.{{calib.electron_isolation}}', postfix = '{{calib.electron_working_point}}_{{calib.electron_isolation}}')
electronSequence.configure( inputName = '{{calib.electron_collection}}',
                            outputName = '{{calib.electron_collection}}_{{calib.electron_working_point}}_{{calib.electron_isolation}}_%SYS%' )
calibrationAlgSeq += electronSequence
print( electronSequence ) # For debugging
output_electron_container = "{{calib.electron_collection}}_{{calib.electron_working_point}}_{{calib.electron_isolation}}_%SYS%"
# Output electron_collection = {{calib.electron_collection}}_{{calib.electron_working_point}}_{{calib.electron_isolation}}_{{ sys_error }}