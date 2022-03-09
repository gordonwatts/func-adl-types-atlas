from MetAnalysisAlgorithms.MetAnalysisSequence import makeMetAnalysisSequence
met_jetContainer = '{{calib.jet_collection}}'
metSequence = makeMetAnalysisSequence('mc', metSuffix = met_jetContainer[:-4] )
metSequence.configure( inputName = { 'jets'      : output_jet_container,
                                        'muons'     : output_muon_container,
                                        'electrons' : output_electron_container },
                        outputName = 'AnalysisMET_%SYS%' )
print(metSequence)  # For debugging
calibrationAlgSeq += metSequence
# Output met_collection = AnalysisMET_{{ sys_error }}