jetContainer = "{{calib.jet_collection}}"
output_jet_container = "{{calib.jet_collection}}Calib_%SYS%"

config.addBlock('Jets')
config.setOptions (containerName=output_jet_container)
config.setOptions (jetCollection=jetContainer)
config.setOptions (runJvtUpdate=False)
config.setOptions (runNNJvtUpdate=True)
config.setOptions (recalibratePhyslite=False)


config.addBlock('Jets.JVT', containerName=output_jet_container)

config.addBlock ('SystObjectLink')
config.setOptions (containerName='AnaJets')

config.addBlock ('ObjectCutFlow')
config.setOptions (containerName='AnaJets')
config.setOptions (selectionName='jvt')


# TODO: Figure out why this needs to be here:
# Output jet_collection = {{calib.jet_collection}}Calib_{{ sys_error }}

# TODO: Figure out if these need to stay or not:
#test2 = "{{calib.run_jet_ghost_muon_association}}"
#test3 = "{{calib.jet_calib_truth_collection}}"
