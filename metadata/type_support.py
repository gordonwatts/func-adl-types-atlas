
class cpp_type(Generic[T]):
    'Used to indicate C++ types in template arguments'
    def __init__ (self, cpp_base_type: Optional[str]):
        '''For a particular type
        '''
        self._cpp_type = cpp_base_type


cpp_float = cpp_type[float]("float")
cpp_double = cpp_type[float]("double")


class fetcher:
    def __getitem__(self, typ: cpp_type[T]) -> Callable[[str], T]:
        ...
    
    def __call__(self, typ: cpp_type[T]) -> Callable[[str], T]:
        ...
