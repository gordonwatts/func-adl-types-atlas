import ast
import copy
import logging
import re
from pathlib import Path
from typing import Dict, Optional, Tuple, TypeVar

import jinja2
from func_adl import ObjectStream
from func_adl.ast.meta_data import lookup_query_metadata
from .default_calibration_config import (
    default_calibration_config,
    default_calibration_name,
)
from .calibration_event_config import CalibrationEventConfig

from .metadata_for_collections import (
    g_metadata_names_no_overlap,
    g_metadata_names_overlap,
)

T = TypeVar("T")


class calib_tools:
    """Helper functions to work with a query's calibration configuration."""

    _default_calibration: Optional[Dict[str, CalibrationEventConfig]] = None

    _default_sys_error: Optional[str] = "NOSYS"

    @classmethod
    def reset_config(cls):
        """Reset calibration config to the default.

        * This is configured by release default.

        """
        cls._default_calibration = default_calibration_config()

    @classmethod
    def _setup(cls):
        if cls._default_calibration is None:
            cls.reset_config()

    @classmethod
    def set_default_config(
        cls, config: CalibrationEventConfig, config_name: Optional[str] = None
    ):
        """Store a new default config. Will be used by everyone after this.
        It can be named - but if not the default config is over-written.

        Args:
            config (CalibrationEventConfig): The configuration to store
            config_name (Optional[str], optional): The configuration name to
                store this in. If none, the default one is used. Defaults to None.
        """
        if config_name is None:
            config_name = default_calibration_name()

        assert cls._default_calibration is not None
        cls._default_calibration[config_name] = copy.copy(config)

    @classmethod
    def default_config(
        cls, config_name: Optional[str] = None
    ) -> CalibrationEventConfig:
        """Return a copy of the current default calibration configuration.

        If no name is given, then the default data format is returned.

        Args:
            config_name (Optional[str], optional): The calibration config name.
                        Defaults to None.

        Returns:
            CalibrationEventConfig: Config for the requested name (or default one).
        """
        "Return a copy of the current default calibration configuration."
        cls._setup()

        if config_name is None:
            config_name = default_calibration_name()

        assert cls._default_calibration is not None
        return copy.copy(cls._default_calibration[config_name])

    @classmethod
    def query_update(
        cls,
        query: ObjectStream[T],
        calib_config: Optional[CalibrationEventConfig] = None,
        **kwargs,
    ) -> ObjectStream[T]:
        """Add metadata to a query to indicate a change in the calibration
        configuration for the query.

        Args:
            query (ObjectStream[T]): The query to update.

            calib_config (Optional[CalibrationEventConfig]): The new calibration
                configuration to use. If specified will override all calibration
                configuration options in the query.

            jet_collection, ...: Use any property name from the `CalibrationEventConfig`
                class to override that particular options for this query. You may
                specify as many of them as you like.

        Returns:
            ObjectStream[T]: The updated query.

        Notes:

            * This function can be chained - resolution works by looking at the most
              recent `query_update` in the query.
            * This function works by storing a complete `CalibrationEventConfig` object,
              updated as requested, in the query. So even if you just update
              `jet_collection`, changing the `default_config` after calling this will
              have no effect.
        """

        # Get a base calibration config we can modify (e.g. a copy)
        config = calib_config
        if config is None:
            config = calib_tools.query_get(query)

        # Now, modify by any arguments we were given
        for k, v in kwargs.items():
            if hasattr(config, k):
                setattr(config, k, v)
            else:
                raise ValueError(
                    f"Unknown calibration config option: {k} in `query_update`"
                )

        # Place it in the query stream for later use
        return query.QMetaData({"calibration": config})

    @classmethod
    def query_get(cls, query: ObjectStream[T]) -> CalibrationEventConfig:
        """Return a copy of the calibration if the query were issued at this point.

        Args:
            query (ObjectStream[T]): The query to inspect.

        Returns:
            CalibrationEventConfig: The calibration configuration for the query.
        """
        assert query is not None, "Call to `query_get`: `query` argument is null."
        r = lookup_query_metadata(query, "calibration")
        if r is None:
            # Really, a user needs to be more careful!
            logging.warning(
                "Fetched the default calibration configuration for a query. It should "
                "have been intentionally configured - using configuration for data "
                f"format {default_calibration_name()}"
            )
            return calib_tools.default_config()
        else:
            return copy.copy(r)

    @classmethod
    @property
    def default_sys_error(cls) -> str:
        """Return the default systematic error"""
        if cls._default_sys_error is None:
            return "NOSYS"
        return cls._default_sys_error

    @classmethod
    def set_default_sys_error(cls, value: str):
        """Set the default systematic error"""
        cls._default_sys_error = value

    @classmethod
    def reset_sys_error(cls):
        """Reset to 'NOSYS' the default systematic error"""
        cls._default_sys_error = "NOSYS"

    @classmethod
    def query_sys_error(cls, query: ObjectStream[T], sys_error: str) -> ObjectStream[T]:
        """Add metadata to a query to indicate a change in the systematic error for the
        events.

        Args:
            query (ObjectStream[T]): The query to update.

            sys_error (str): The systematic error to fetch. Only a single one is
                possible at any time. The sys error names are the same as used
                by the common CP algorithms.

        Returns:
            ObjectStream[T]: The updated query.

        Notes:

            * This function can be chained - resolution works by looking at the most
              recent `query_sys_error` in the query.
        """
        return query.QMetaData({"calibration_sys_error": sys_error})


_g_jinja2_env: Optional[jinja2.Environment] = None


def template_configure() -> jinja2.Environment:
    """Configure the jinja2 template"""
    global _g_jinja2_env
    if _g_jinja2_env is None:
        template_path = Path(__file__).parent / "templates"
        loader = jinja2.FileSystemLoader(str(template_path))
        _g_jinja2_env = jinja2.Environment(loader=loader)
    return _g_jinja2_env


def fixup_collection_call(
    s: ObjectStream[T], a: ast.Call, collection_attr_name: str
) -> Tuple[ObjectStream[T], ast.Call]:
    "Apply all the fixes to the collection call"

    # Find the two arguments
    bank_name = None
    calibrate = None

    if len(a.args) >= 1:
        bank_name = ast.literal_eval(a.args[0])

    if len(a.args) >= 2:
        calibrate = ast.literal_eval(a.args[1])

    for arg in a.keywords:
        if arg.arg == "collection":
            bank_name = ast.literal_eval(arg.value)
        elif arg.arg == "calibrate":
            calibrate = ast.literal_eval(arg.value)
        else:
            raise TypeError(f'Unknown argument "{arg.arg}" to collection call.')

    new_s = s
    if bank_name is not None:
        new_s = calib_tools.query_update(new_s, **{collection_attr_name: bank_name})

    # See if there is a systematic error we need to fetch
    sys_error = lookup_query_metadata(new_s, "calibration_sys_error")
    if sys_error is None:
        sys_error = calib_tools.default_sys_error

    # Make sure the bank name is set properly (or defaulted)
    calibration_info = calib_tools.query_get(new_s)
    if bank_name is None:
        bank_name = getattr(calibration_info, collection_attr_name)

    # Default behavior for running calibrations
    if calibrate is None:
        # Force calibration code to run if we are looking at systematic errors unless
        # user has requested...
        if sys_error != "NOSYS":
            calibrate = True
        else:
            calibrate = calibration_info.calibrate
    else:
        if (not calibrate) and (not calibration_info.uncalibrated_possible):
            raise NotImplementedError(
                f"Requested uncalibrated {bank_name}, but that "
                "is not possible on this dataset type"
            )
    if sys_error != "NOSYS" and not calibrate:
        raise NotImplementedError(
            "Cannot request a systematic error and not have calibration run "
            f"for {bank_name}"
        )

    # Uncalibrated collection is pretty easy - nothing to do here!
    if not calibrate:
        output_collection_name = bank_name
    else:
        # Going to have to run calibrations, so load up the meta-data
        j_env = template_configure()
        dependent_md_name = None
        output_collection_name = None
        md_to_transmit = (
            g_metadata_names_overlap[collection_attr_name]
            if calibration_info.perform_overlap_removal
            else g_metadata_names_no_overlap[collection_attr_name]
        )
        for md_name in md_to_transmit:
            md_template = j_env.get_template(f"{md_name}.py")
            text = md_template.render(calib=calibration_info, sys_error=sys_error)
            md_text = {
                "metadata_type": "add_job_script",
                "name": md_name,
                "script": text.splitlines(),
            }
            if dependent_md_name is not None:
                md_text["depends_on"] = [dependent_md_name]

            new_s = new_s.MetaData(md_text)

            dependent_md_name = md_name

            # Have we found the output collection name?
            found = re.search(f"# Output {collection_attr_name} = (.+)(\\s|$)", text)
            if found is not None:
                output_collection_name = found.group(1)

    if output_collection_name is None:
        raise RuntimeError(
            "Could not find output collection name in templates for collection"
            f" '{collection_attr_name}' - xAOD job options templates are malformed."
        )

    # Finally, rewrite the call to fetch the collection with the actual collection name
    # we want to fetch.
    new_call = copy.copy(a)
    new_call.args = [
        ast.parse(f"'{output_collection_name}'").body[0].value
    ]  # type: ignore

    return new_s, new_call
