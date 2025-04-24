config.addBlock('Jets')
config.setOptions (containerName="AnaJets")
config.setOptions (jetCollection="{{calib.jet_collection}}")
config.setOptions (runJvtUpdate=False)
config.setOptions (runNNJvtUpdate=False)
config.setOptions (recalibratePhyslite=False)

config.addBlock ('Thinning')
config.setOptions (containerName="{{calib.jet_calib_truth_collection}}")
config.setOptions (outputName='OutTruthJets')
config.setOptions (skipOnData=True)

# Output jet_collection = {{calib.jet_collection}}

# TODO: Add ghost muon association: "{{calib.run_jet_ghost_muon_association}}"