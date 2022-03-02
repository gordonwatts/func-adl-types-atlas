# pulled from:https://gitlab.cern.ch/atlas/athena/-/blob/21.2/PhysicsAnalysis/Algorithms/JetAnalysisAlgorithms/python/JetAnalysisAlgorithmsTest.py
# Set up the systematics loader/handler service:
from AnaAlgorithm.DualUseConfig import createService
from AnaAlgorithm.AlgSequence import AlgSequence
calibrationAlgSeq = AlgSequence()
sysService = createService( 'CP::SystematicsSvc', 'SystematicsSvc', sequence = calibrationAlgSeq )
sysService.systematicsList = ['{{ sys_error }}']
# Add sequence to job