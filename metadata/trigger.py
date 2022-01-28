# Temp file to develop the code we need to run as the TDT
from typing import Tuple, TypeVar
from func_adl import func_adl_callable, ObjectStream
import ast

T = TypeVar("T")


def tdt_chain_fired_processor(
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
    ).MetaData(
        {
            "metadata_type": "inject_code",
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
            ],
            "link_libraries": ["TrigDecisionToolLib", "TrigConfInterfaces"],
        }
    )
    return new_s, a


@func_adl_callable(tdt_chain_fired_processor)
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
