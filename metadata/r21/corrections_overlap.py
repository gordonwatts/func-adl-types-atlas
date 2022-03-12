from AsgAnalysisAlgorithms.OverlapAnalysisSequence import makeOverlapAnalysisSequence
overlapSequence = makeOverlapAnalysisSequence('mc')
overlapSequence.configure(
    inputName = {
        'electrons' : output_electron_container,
        'photons'   : '{{calib.photon_collection}}',
        'muons'     : output_muon_container,
        'jets'      : output_jet_container,
        'taus'      : output_tau_container,
    },
    outputName = {
        'electrons' : '{{calib.electron_collection}}_{{calib.electron_working_point}}_{{calib.electron_isolation}}_OR_%SYS%',
        'photons'   : '{{calib.photon_collection}}_OR_%SYS%',
        'muons'     : '{{calib.muon_collection}}Calib_{{calib.muon_working_point}}{{calib.muon_isolation}}_OR_%SYS%',
        'jets'      : '{{calib.jet_collection}}Calib_OR_%SYS%',
        'taus'      : '{{calib.tau_collection}}_{{calib.tau_working_point}}_OR_%SYS%'
    })
calibrationAlgSeq += overlapSequence
# Output electron_collection = {{calib.electron_collection}}_{{calib.electron_working_point}}_{{calib.electron_isolation}}_OR_{{ sys_error }}
# Output photon_collection = {{calib.photon_collection}}_OR_{{ ssy_error }}
# Output muon_collection = {{calib.muon_collection}}Calib_{{calib.muon_working_point}}{{calib.muon_isolation}}_OR_{{ sys_error }}
# Output jet_collection = {{calib.jet_collection}}Calib_OR_{{ sys_error }}
# Output tau_collection = {{calib.tau_collection}}_{{calib.tau_working_point}}_OR_{{ sys_error }}