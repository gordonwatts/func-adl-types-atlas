import ast
import copy
import re
from dataclasses import dataclass
from pathlib import Path
from typing import Optional, Tuple, TypeVar

import jinja2
from func_adl import ObjectStream
from func_adl.ast.meta_data import lookup_query_metadata


@dataclass
class CalibrationEventConfig:
    # Name of the jet collection to calibrate and use by default
    jet_collection: str

    # Name of the truth jets to be used for the jet calibration
    jet_calib_truth_collection: str

    ########### Electrons
    # Name of the electron collection to calibrate and use by default
    electron_collection: str

    # The working point (e.g. xxx)
    electron_working_point: str

    # The isolation (e.g. xxxx)
    electron_isolation: str

    ########### Photons
    # Name of the photon collection to calibrate and use by default.
    photon_collection: str

    # The working point (e.g. xxx)
    photon_working_point: str

    # The isolation (e.g. xxxx)
    photon_isolation: str

    ########### Muons
    # Name of the muon collection to calibration and use by default.
    muon_collection: str

    # The working point (e.g. xxx)
    muon_working_point: str

    # The isolation (e.g. xxxx)
    muon_isolation: str

    ########### Taus
    # Name of the tau collection to calibrate and use by default.
    tau_collection: str

    # The working point (e.g. xxxx)
    tau_working_point: str


# Define the default config for DAOD_PHYS and PHYSLITE
default_calibration = CalibrationEventConfig(
    jet_collection="AntiKt4EMPFlowJets",
    jet_calib_truth_collection="AntiKt4TruthDressedWZJets",
    electron_collection="Electrons",
    electron_working_point="MediumLHElectron",
    electron_isolation="NonIso",
    photon_collection="Photons",
    photon_working_point="Tight",
    photon_isolation="FixedCutTight",
    muon_collection="Muons",
    muon_working_point="Medium",
    muon_isolation="NonIso",
    tau_collection="TauJets",
    tau_working_point="Tight",
)


T = TypeVar('T')


_g_jinja2_env: Optional[jinja2.Environment] = None


def template_configure() -> jinja2.Environment:
    '''Configure the jinja2 template
    '''
    global _g_jinja2_env
    if _g_jinja2_env is None:
        template_path = Path(__file__).parent / "templates"
        loader = jinja2.FileSystemLoader(str(template_path))
        _g_jinja2_env = jinja2.Environment(loader=loader)
    return _g_jinja2_env


def fixup_collection_call(s: ObjectStream[T], a: ast.Call) -> Tuple[ObjectStream[T], ast.Call]:
    'Apply all the fixes to the collection call'

    collection_attr_name = "jet_collection"
    metadata_names = ["sys_error_tool", "pileup_tool", "common_corrections", "add_calibration_to_job"]

    # Get the most up to date configuration for this run.
    calibration_info = default_calibration

    # Add an argument for the proper bank name.
    collection_name = getattr(calibration_info, collection_attr_name)

    # Next, load up all the data for this collection.
    new_s = s
    j_env = template_configure()
    dependent_md_name = None
    output_collection_name = None
    for md_name in metadata_names:
        md_template = j_env.get_template(f"{md_name}.py")
        text = md_template.render(calib=calibration_info)
        md_text = {
            "metadata_type": "add_job_script",
            "name": md_name,
            "script": text.splitlines()
        }
        if dependent_md_name is not None:
            md_text["depends_on"] = [dependent_md_name]

        new_s = new_s.MetaData(md_text)

        dependent_md_name = md_name

        # Have we found the output collection name?
        found = re.search(f"# Output {collection_attr_name} = (.+)\\s", text)
        if found is not None:
            output_collection_name = found.group(1)

    if output_collection_name is None:
        raise RuntimeError(f"Could not find output collection name in templates for collection '{collection_attr_name} - xAOD job options templates are malformed.")

    # Finally, rewrite the call to fetch the collection with the actual collection name we want
    # to fetch.
    new_call = copy.copy(a)
    new_call.args.append(ast.parse(f"'{output_collection_name}'").body[0].value)  # type: ignore

    return new_s, new_call
