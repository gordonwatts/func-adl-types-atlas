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


T = TypeVar('T')


class calib_tools:
    # Define the default config for DAOD_PHYS and PHYSLITE
    _default_calibration: Optional[CalibrationEventConfig] = None
    _default_sys_error: Optional[str] = 'NOSYS'

    @classmethod
    def reset_config(cls):
        'Reset calibration config to base state'
        cls._default_calibration = CalibrationEventConfig(
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

    @classmethod
    def _setup(cls):
        if cls._default_calibration is None:
            cls.reset_config()

    @classmethod
    def set_default_config(cls, config: CalibrationEventConfig):
        'Store a copy of a new default config for future use'
        cls._default_calibration = copy.copy(config)

    @classmethod
    @property
    def default_config(cls) -> CalibrationEventConfig:
        'Return a copy of the current default calibration configuration'
        cls._setup()
        assert cls._default_calibration is not None
        return copy.copy(cls._default_calibration)

    @classmethod
    def query_update(cls, query: ObjectStream[T], calib_config: Optional[CalibrationEventConfig] = None, **kwargs) -> ObjectStream[T]:
        # Get a base calibration config we can modify (e.g. a copy)
        config = calib_config
        if config is None:
            config = copy.copy(lookup_query_metadata(query, 'calibration'))
        if config is None:
            config = cls.default_config

        # Now, modify by any arguments we were given
        for k, v in kwargs.items():
            if hasattr(config, k):
                setattr(config, k, v)
            else:
                raise ValueError(f'Unknown calibration config option: {k} in `query_update`')

        # Place it in the query stream for later use
        return query.QMetaData({
            'calibration': config
        })

    @classmethod
    @property
    def default_sys_error(cls) -> str:
        if cls._default_sys_error is None:
            return 'NOSYS'
        return cls._default_sys_error

    @classmethod
    def set_default_sys_error(cls, value: str):
        cls._default_sys_error = value

    @classmethod
    def reset_sys_error(cls):
        cls._default_sys_error = 'NOSYS'

    @classmethod
    def query_sys_error(cls, query: ObjectStream[T], sys_error: str) -> ObjectStream[T]:
        return query.QMetaData({
            'calibration_sys_error': sys_error
        })


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

    # Find the two arguments
    uncalibrated_bank_name = None
    calibrated_bank_name = None

    if len(a.args) >= 1:
        calibrated_bank_name = ast.literal_eval(a.args[0])

    if len(a.args) >= 2:
        uncalibrated_bank_name = ast.literal_eval(a.args[1])

    for arg in a.keywords:
        if arg.arg == 'calibrated_collection':
            calibrated_bank_name = ast.literal_eval(arg.value)
        if arg.arg == 'uncalibrated_collection':
            uncalibrated_bank_name = ast.literal_eval(arg.value)

    if uncalibrated_bank_name is not None and calibrated_bank_name is not None:
        raise ValueError(f"Illegal to specify both `calibrated_collection` and `uncalibrated_collection` when accessing `collection_attr_name`.")

    new_s = s
    if calibrated_bank_name is not None:
        new_s = calib_tools.query_update(new_s, **{collection_attr_name: calibrated_bank_name})

    # See if there is a systematic error we need to fetch
    sys_error = lookup_query_metadata(new_s, 'calibration_sys_error')
    if sys_error is None:
        sys_error = calib_tools.default_sys_error

    # Uncalibrated collection is pretty easy - nothing to do here!
    if uncalibrated_bank_name is not None:
        output_collection_name = uncalibrated_bank_name
    else:

        # Get the most up to date configuration for this run.
        calibration_info = lookup_query_metadata(new_s, "calibration")
        if calibration_info is None:
            calibration_info = calib_tools.default_config

        # Add an argument for the proper bank name.
        collection_name = getattr(calibration_info, collection_attr_name)

        # Next, load up all the data for this collection.
        j_env = template_configure()
        dependent_md_name = None
        output_collection_name = None
        for md_name in metadata_names:
            md_template = j_env.get_template(f"{md_name}.py")
            text = md_template.render(calib=calibration_info, sys_error=sys_error)
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
    new_call.args = [ast.parse(f"'{output_collection_name}'").body[0].value]  # type: ignore

    return new_s, new_call
