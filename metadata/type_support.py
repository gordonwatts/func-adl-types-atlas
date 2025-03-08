from typing import Callable, Generic, Iterable, Optional, Tuple, Type, TypeVar
from func_adl import ObjectStream
import ast

T = TypeVar("T")


class cpp_type(Generic[T]):
    "Used to indicate C++ types in template arguments"

    def __init__(
        self,
        cpp_base_type: str,
        python_base_type: Type,
        cpp_collection_type: Optional[str] = None,
    ):
        """For a particular type"""
        self._cpp_type = cpp_base_type
        self._python_type = python_base_type
        self._cpp_collection_type = cpp_collection_type

    @property
    def cpp_type(self) -> str:
        return self._cpp_type

    @property
    def python_type(self) -> Type:
        return self._python_type

    @property
    def is_collection(self) -> bool:
        return self._cpp_collection_type is not None

    @property
    def actual_cpp_type(self) -> str:
        if self._cpp_collection_type is None:
            return self.cpp_type
        else:
            return self._cpp_collection_type

    @property
    def actual_cpp_type_norm(self) -> str:
        return (
            self.actual_cpp_type.replace("::", "_").replace("<", "_").replace(">", "_")
        )


cpp_float = cpp_type[float]("float", float)
cpp_int = cpp_type[float]("int", int)
cpp_double = cpp_type[float]("double", float)
cpp_string = cpp_type[float]("std::string", float)
cpp_vfloat = cpp_type[Iterable[float]]("float", float, "std::vector<float>")
cpp_vint = cpp_type[Iterable[int]]("int", int, "std::vector<int>")
cpp_vdouble = cpp_type[Iterable[float]]("double", float, "std::vector<double>")


# TODO: 3.10 and this should be a ParamSpec, not a TypeVar.
ParamValue = TypeVar("ParamValue")


class index_type_forwarder(Generic[ParamValue]):
    def __getitem__(self, typ: cpp_type[T]) -> Callable[[ParamValue], T]: ...

    def __call__(self, typ: cpp_type[T]) -> Callable[[ParamValue], T]: ...


def cpp_generic_1arg_callback(
    method_name: str, s: ObjectStream[T], a: ast.Call, param_1: cpp_type
) -> Tuple[ObjectStream[T], ast.Call, Type]:
    "We deal with generic function return types"

    new_s = s.MetaData(
        {
            "metadata_type": "add_cpp_function",
            "name": f"{method_name}_{param_1.actual_cpp_type_norm}",
            "arguments": ["moment_name"],
            "code": [
                f"auto result = obj_j->{method_name}<{param_1.actual_cpp_type}>(moment_name);"
            ],
            "instance_object": "xAOD::Jet_v1",
            "method_object": "obj_j",
            "return_type": param_1.cpp_type,
            "return_is_collection": param_1.is_collection,
            "include_files": [],
        }
    )

    import copy

    new_a = copy.copy(a)
    new_a.func = ast.Attribute(
        a.func.value, f"{method_name}_{param_1.actual_cpp_type_norm}", a.func.ctx
    )

    return new_s, new_a, param_1.python_type
