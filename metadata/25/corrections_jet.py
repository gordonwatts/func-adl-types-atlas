jet_container = '{{calib.jet_collection}}_Calib'

config.addBlock('Jets')
config.setOptions (containerName=jet_container)
config.setOptions (jetCollection='{{calib.jet_collection}}')
config.setOptions (runJvtUpdate=False)
config.setOptions (runNNJvtUpdate=False)
config.setOptions (recalibratePhyslite=False)
config.setOptions (runGhostMuonAssociation={{calib.run_jet_ghost_muon_association}})

# Output jet_collection = {{calib.jet_collection}}_Calib_{{ sys_error }}