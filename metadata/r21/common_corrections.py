jetContainer = '{{calib.jet_collection}}'
from JetAnalysisAlgorithms.JetAnalysisSequence import makeJetAnalysisSequence
jetSequence = makeJetAnalysisSequence( 'mc', jetContainer)
jetSequence.configure( inputName = jetContainer, outputName = jetContainer + '_Base_%SYS%' )
jetSequence.JvtEfficiencyAlg.truthJetCollection = '{{calib.jet_calib_truth_collection}}'
jetSequence.ForwardJvtEfficiencyAlg.truthJetCollection = '{{calib.jet_calib_truth_collection}}'
calibrationAlgSeq += jetSequence
print( jetSequence ) # For debugging
# Include, and then set up the jet analysis algorithm sequence:
from JetAnalysisAlgorithms.JetJvtAnalysisSequence import makeJetJvtAnalysisSequence
jvtSequence = makeJetJvtAnalysisSequence( 'mc', jetContainer, enableCutflow=True )
jvtSequence.configure( inputName = {'jets'      : jetContainer + '_Base_%SYS%' },
                       outputName = { 'jets'      : jetContainer + 'Calib_%SYS%' },
                       )
calibrationAlgSeq += jvtSequence
print( jvtSequence ) # For debugging
#
muon_container = '{{calib.muon_collection}}'
from MuonAnalysisAlgorithms.MuonAnalysisSequence import makeMuonAnalysisSequence
muonSequence = makeMuonAnalysisSequence('mc', workingPoint='{{calib.muon_working_point}}.{{calib.muon_isolation}}', postfix = '{{calib.muon_working_point}}_{{calib.muon_isolation}}')
muonSequence.configure( inputName = muon_container,
                        outputName = muon_container + 'Calib_{{calib.muon_working_point}}{{calib.muon_isolation}}_%SYS%' )
calibrationAlgSeq += muonSequence
print( muonSequence ) # For debugging
#
from EgammaAnalysisAlgorithms.ElectronAnalysisSequence import makeElectronAnalysisSequence
electronSequence = makeElectronAnalysisSequence( 'mc', '{{calib.electron_working_point}}.{{calib.electron_isolation}}', postfix = '{{calib.electron_working_point}}_{{calib.electron_isolation}}')
electronSequence.configure( inputName = '{{calib.electron_collection}}',
                            outputName = '{{calib.electron_collection}}_{{calib.working_point}}_{{calib.isolation}}_%SYS%' )
calibrationAlgSeq += electronSequence
print( electronSequence ) # For debugging
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
# TODO: THis needs to be fixed as well!!
from TauAnalysisAlgorithms.TauAnalysisSequence import makeTauAnalysisSequence
tauSequence = makeTauAnalysisSequence( 'mc', '{{calib.tau_working_point}}', postfix = '{{calib.tau_working_point}}', rerunTruthMatching=False)
tauSequence.configure( inputName = '{{calib.tau_collection}}',
                       outputName = '{{calib.tau_collection}}_{{calib.tau_working_point}}_%SYS%' )
calibrationAlgSeq += tauSequence
print( tauSequence ) # For debugging

# Overlap removal
from AsgAnalysisAlgorithms.OverlapAnalysisSequence import makeOverlapAnalysisSequence
overlapSequence = makeOverlapAnalysisSequence('mc')
overlapSequence.configure(
    inputName = {
        'electrons' : '{{calib.electron_collection}}_{{calib.working_point}}_{{calib.isolation}}_%SYS%',
        'photons'   : '{{calib.photon_collection}}',
        'muons'     : '{{calib.muon_collection}}Calib_{{calib.muon_working_point}}{{calib.muon_isolation}}_%SYS%',
        'jets'      : '{{calib.jet_collection}}Calib_%SYS%',
        'taus'      : '{{calib.tau_collection}}_{{calib.tau_working_point}}_%SYS%'
    },
    outputName = {
        'electrons' : '{{calib.electron_collection}}_{{calib.working_point}}_{{calib.isolation}}_OR_%SYS%',
        'photons'   : '{{calib.photon_collection}}_OR_%SYS%',
        'muons'     : '{{calib.muon_collection}}Calib_{{calib.muon_working_point}}{{calib.muon_isolation}}_OR_%SYS%',
        'jets'      : '{{calib.jet_collection}}Calib_OR_%SYS%',
        'taus'      : '{{calib.tau_collection}}_{{calib.tau_working_point}}_OR_%SYS%'
    })
calibrationAlgSeq += overlapSequence
# Output electron_collection = {{calib.electron_collection}}_{{working_point}}_{{isolation}}_OR_{{ sys_error }}
# Output photon_collection = {{calib.photon_collection}}_OR_{{ ssy_error }}
# Output muon_collection = {{calib.muon_collection}}Calib_{{calib.muon_working_point}}{{calib.muon_isolation}}_OR_{{ sys_error }}
# Output jet_collection = {{calib.jet_collection}}Calib_OR_{{ sys_error }}
# Output tau_collection = {{calib.tau_collection}}_{{calib.tau_working_point}}_OR_{{ sys_error }}
