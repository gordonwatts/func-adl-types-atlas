from AnaAlgorithm.AlgSequence import AlgSequence
from AnalysisAlgorithmsConfig.ConfigAccumulator import ConfigAccumulator
from AnalysisAlgorithmsConfig.ConfigText import TextConfig
from Campaigns.Utils import Campaign
from AthenaConfiguration.AllConfigFlags import initConfigFlags

dataType='mc'
isPhyslite=False
geometry='RUN2'
campaign=Campaign.MC20e

# Needed to configure the AlgSequence
from AthenaConfiguration.AllConfigFlags import initConfigFlags
flags = initConfigFlags()
flags.Input.Files = [sh.at(0).fileName(0)]
flags.lock()

autoconfigFromFlags=flags

config = TextConfig()

# Switch on systematics
config.addBlock('CommonServices')
config.setOptions(systematicsHistogram='systematicsList')