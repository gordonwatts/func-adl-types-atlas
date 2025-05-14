from AnalysisAlgorithmsConfig.ConfigText import TextConfig

config = TextConfig()

# Switch on systematics
config.addBlock('CommonServices')
config.setOptions(runSystematics=True)
config.setOptions(filterSystematics="^(?=.*{{sys_error}}|$).*")

import logging
logging.basicConfig(level=logging.INFO)