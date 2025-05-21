config.addBlock('Jets')
config.setOptions (containerName="{{calib.jet_collection}}_Calib")
config.setOptions (jetCollection="{{calib.jet_collection}}")
config.setOptions (runJvtUpdate=False)
config.setOptions (runNNJvtUpdate=False)
config.setOptions (recalibratePhyslite=False)
config.setOptions (runGhostMuonAssociation={{calib.run_jet_ghost_muon_association}})

# Output jet_collection = {{calib.jet_collection}}_Calib_{{ sys_error }}

# TODO: The jets calibrations are running even when we are not running on jets.