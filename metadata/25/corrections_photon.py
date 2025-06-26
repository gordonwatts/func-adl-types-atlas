photon_container = '{{calib.photon_collection}}_{{calib.photon_working_point}}_{{calib.photon_isolation}}_Calib'

config.addBlock ('Photons', containerName=photon_container)
config.setOptions (recomputeIsEM=False)
config.setOptions (recalibratePhyslite=False)
config.setOptions (decorateTruth=True)

config.addBlock ('Photons.WorkingPoint')
config.setOptions (containerName=photon_container)
config.setOptions (selectionName='PhotonSelection')
config.setOptions (qualityWP='{{calib.photon_working_point}}')
config.setOptions (isolationWP='{{calib.photon_isolation}}')
config.setOptions (recomputeIsEM=False)

config.addBlock ('Thinning')
config.setOptions (containerName=photon_container)
config.setOptions (selectionName='PhotonSelection')
config.setOptions (outputName='OutPhotons')

# Output photon_collection = OutPhotons_{{ sys_error }}
