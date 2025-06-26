tau_container = '{{calib.tau_collection}}_{{calib.tau_working_point}}_Calib'

config.addBlock ('TauJets', containerName=tau_container)
config.setOptions (decorateTruth=True)

config.addBlock ('TauJets.WorkingPoint')
config.setOptions (containerName=tau_container)
config.setOptions (selectionName='TauSelection')
config.setOptions (quality='{{calib.tau_working_point}}')

config.addBlock ('Thinning')
config.setOptions (containerName=tau_container)
config.setOptions (selectionName='TauSelection')
config.setOptions (outputName='OutTauJets')

# Output tau_collection = OutTauJets_{{ sys_error }}
