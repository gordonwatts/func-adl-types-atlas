from AnalysisAlgorithmsConfig.ConfigText import TextConfig

config = TextConfig()

# Switch on systematics
config.addBlock('CommonServices')
config.setOptions (runSystematics=True)

import logging
logging.basicConfig(level=logging.INFO)