electron_container_name = "{{calib.electron_collection}}_{{calib.electron_working_point}}_{{calib.electron_isolation}}_Calib"

# Electrons
config.addBlock ('Electrons')
config.setOptions (containerName=electron_container_name)
config.setOptions (recalibratePhyslite=False)

# Electrons.WorkingPoint
config.addBlock ('Electrons.WorkingPoint')
config.setOptions (containerName=electron_container_name)
config.setOptions (selectionName='loose_ele')
config.setOptions (forceFullSimConfig=True)
config.setOptions (noEffSF=True)
config.setOptions (identificationWP="{{calib.electron_working_point}}")
config.setOptions (isolationWP="{{calib.electron_isolation}}")

# Thinning
config.addBlock ('Thinning')
config.setOptions (containerName=electron_container_name)
config.setOptions (selectionName='loose_ele')
config.setOptions (outputName='OutElectrons')


# Output electron_collection = OutElectrons_{{ sys_error }}