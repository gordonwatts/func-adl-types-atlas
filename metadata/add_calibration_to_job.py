configSeq = config.configure()

# compile
algSeq = AlgSequence()
configAccumulator = ConfigAccumulator(algSeq, dataType=dataType,
        isPhyslite=isPhyslite,
        geometry=geometry,
        campaign=campaign,
        autoconfigFromFlags=autoconfigFromFlags)
configSeq.fullConfigure(configAccumulator)

algSeq.addSelfToJob( job )
print(job) # for debugging