muon_container = '{{calib.muon_collection}}'
from MuonAnalysisAlgorithms.MuonAnalysisSequence import makeMuonAnalysisSequence
muonSequence = makeMuonAnalysisSequence('mc', workingPoint='{{calib.muon_working_point}}.{{calib.muon_isolation}}', postfix = '{{calib.muon_working_point}}_{{calib.muon_isolation}}')
muonSequence.configure( inputName = muon_container,
                        outputName = muon_container + 'Calib_{{calib.muon_working_point}}{{calib.muon_isolation}}_%SYS%' )
calibrationAlgSeq += muonSequence
print( muonSequence ) # For debugging
# Output muon_collection = {{calib.muon_collection}}Calib_{{calib.muon_working_point}}{{calib.muon_isolation}}_{{ sys_error }}