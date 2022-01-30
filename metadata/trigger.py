# Temp file to develop the code we need to run as the TDT
from typing import Tuple, TypeVar
from func_adl import func_adl_callable, ObjectStream
import ast

T = TypeVar("T")


def _add_decision_tool(s: ObjectStream[T]) -> ObjectStream[T]:
    'Add code block for a decision tool initialization'
    return s.MetaData(
        {
            "metadata_type": "inject_code",
            "name": "trigger_decision_tool",
            "header_includes": [
                "TrigConfInterfaces/ITrigConfigTool.h",
                "TrigDecisionTool/TrigDecisionTool.h",
                "AsgTools/AnaToolHandle.h",
            ],
            "private_members": [
                "asg::AnaToolHandle<TrigConf::ITrigConfigTool> m_trigConf;",
                "asg::AnaToolHandle<Trig::TrigDecisionTool> m_trigDec;",
            ],
            "instance_initialization": [
                'm_trigConf("TrigConf::xAODConfigTool/xAODConfigTool")',
                'm_trigDec("Trig::TrigDecisionTool/TrigDecisionTool")',
            ],
            "initialize_lines": [
                "ANA_CHECK (m_trigConf.initialize());",
                'ANA_CHECK (m_trigDec.setProperty("ConfigTool", m_trigConf.getHandle()));',
                'ANA_CHECK (m_trigDec.setProperty("TrigDecisionKey", "xTrigDecision"));',
                "ANA_CHECK (m_trigDec.initialize());",
            ],
            "link_libraries": ["TrigDecisionToolLib", "TrigConfInterfaces"],
        }
    )


def _add_match_tool(s: ObjectStream[T]) -> ObjectStream[T]:
    new_s = s.MetaData(
            {
                "metadata_type": "inject_code",
                "name": "trigger_match_tool",
                "private_members": [
                    "asg::AnaToolHandle<Trig::IMatchingTool> m_tmt;",
                ],
                "header_includes": [
                    "TriggerMatchingTool/MatchFromCompositeTool.h",
                ],
                "instance_initialization": [
                    'm_tmt("Trig::MatchFromCompositeTool")',
                ],
                "initialize_lines": [
                    'ANA_CHECK(m_tmt.initialize());',
                ],
                "link_libraries": ["TriggerMatchingToolLib"],
            }
    )
    return _add_decision_tool(new_s)


def _tdt_chain_fired_processor(
    s: ObjectStream[T], a: ast.Call
) -> Tuple[ObjectStream[T], ast.Call]:
    """Configure the backend to run the Trigger Decision Tool!

    Args:
        s (ObjectStream[T]): The stream func_adl is working on - and we can add meta data to.Tuple
        a (ast.Call): The callsite in case we need to modify it

    Returns:
        Tuple[ObjectStream[T], ast.Call]: Update stream and call site.
    """
    # Make sure the TDT is declared and send the code
    # for this particular function
    new_s = s.MetaData(
        {
            "metadata_type": "add_cpp_function",
            "name": "tdt_chain_fired",
            "include_files": [],
            "arguments": ["triggers"],
            "code": ["auto result = m_trigDec->isPassed(triggers,TrigDefs::Physics);"],
            "result_name": "result",
            "return_type": "bool",
        }
    )

    return _add_decision_tool(new_s), a


def _tmt_match_object_processor(s: ObjectStream[T], a: ast.Call) -> Tuple[ObjectStream[T], ast.Call]:
    new_s = s.MetaData(
        {
            "metadata_type": "add_cpp_function",
            "name": "tmt_match_object",
            "include_files": [],
            "arguments": ["trigger", "offline_object", "dr"],
            "code": [
                "std::vector<const xAOD::IParticle*> myParticles",
                "myParticles.push_back(offline_object);",
                "auto result = m_tmt->match(myParticles, trigger , dr);",
                ],
            "result_name": "result",
            "return_type": "bool",
        })

    return (_add_match_tool(new_s), a)


@func_adl_callable(_tdt_chain_fired_processor)
def tdt_chain_fired(triggers: str) -> bool:
    """Returns true if the event has any of the trigger chain names that have
    fired. Uses the ATLAS Trigger Decision Tool to query the event.

    Args:
        triggers (str): String specifying the triggers to check for. This is passed directly to the
        ATLAS TriggerDecisionTool, so can include any valid wildcards.

    Returns:
        bool: True if the TDT says this chain has fired on this event, false other wise.
    """
    ...

@func_adl_callable(_tmt_match_object_processor)
def tmt_match_object(trigger: str, offline_object, dr: float = 0.7) -> bool:
    '''Returns true if the `offline_object` is a close match to the trigger
    object. Close match is done as a function of $\Delta R < `dr`$.

    Args:
        trigger (str): Trigger name
        offline_object ([type]): The offline object (like an electron)

    Returns:
        bool: True if the match is good.
    '''
    ...
