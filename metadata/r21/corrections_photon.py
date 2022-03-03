#TODO: Get photon corrections working. It does not seem possible in R21 and on PHYS_DAOD.
# If you remove the fudge tool, then the photon eff tool has trouble.
# If you remove that, then the sequence has trouble. Needs more work.
# from EgammaAnalysisAlgorithms.PhotonAnalysisSequence import makePhotonAnalysisSequence
# photonSequence = makePhotonAnalysisSequence( 'mc', '{{calib.photon_working_point}}.{{calib.photon_isolation}}', postfix = '{{calib.photon_working_point}}_{{calib.photon_isolation}}')
# photonSequence.configure( inputName = '{{calib.photon_collection}}',
#                             outputName = '{{calib.photon_collection}}_{{calib.photon_working_point}}_{{calib.photon_isolation}}_%SYS%' )
# print( photonSequence ) # For debugging
# attr = getattr(photonSequence, 'PhotonShowerShapeFudgeAlg_Tight_FixedCutTight')
# del attr
# del photonSequence.PhotonShowerShapeFudgeAlg_Tight_FixedCutTight
# del photonSequence.PhotonIsolationCorrectionAlg_Tight_FixedCutTight
# calibrationAlgSeq += photonSequence
# Output photon_collection = {{calib.photon_collection}}
