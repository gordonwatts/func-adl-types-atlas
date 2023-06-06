# func-adl-types-atlas

Generate the Event Data Model and types for `func_adl` for ATLAS AnalysisBase software.

## Usage

Most of the time you will just want to run this on a new version of an `AnalysisBase` release. This can be done by doing the following steps:

1. Clone this repo
1. On Windows, run the PowerShell script found in the `scripts` directory: `<path-to-script>/build_xaod_edm.ps1 21.2.184 184.yaml`.

The `yaml` file will be written out in your local directory and is ready for input into the `python` `func_adl` builder, `func_adl_servicex_type_generator`.

The output will look very much like what you see below in the Development section, with a C++ `cmake` build preceding it.

## Development

This repo is currently designed to be developed inside a container running on docker on your local machine. Everything was tailored to run under `vscode` and tested on Windows (though it should work fine for anything else).

1. Load the workspace up in `vscode`.
1. Check the `Dockerfile` in the root of this repo that it is loading the `atlas/analysisbase` container you want to work on.
1. Let `vscode` build and start up the dev container (as it likely would have asked the second you opened the workspace).

As long as you have the `CMake` extension installed inside `vscode`, you'll want to:

1. `CMake:Configure` the project
1. `CMake:Build` the project. That last step you will need to do repeatedly. There is likely a way to automate that, but I do not know it.
1. `CMake:Run Tests` to run the tests. The tests tend to cover small unit tests, but you'll have to both run those and look at the output `yaml` to make sure it works as a system (see below).

If you change docker containers, make sure to delete the folders `Testing` and `build` from the source directory structure! Otherwise, `cmake` will get very confused between releases!

Running inside the container is very easy.

1. Start a terminal (will start inside the container automatically)
1. `./scripts/run_on_atlas_containers.sh > test1.yaml`

You'll get output that looks something like this:

```bash
[bash][atlas AnalysisBase-21.2.184]:func-adl-types-atlas > ./scripts/run_on_atlas_containers.sh > test1.txt 
ERROR: Cannot translate class 'xAOD::CompositeParticleContainer': ROOT's type system doesn't have it loaded.
ERROR: Cannot translate class 'xAOD::CompositeParticleAuxContainer': ROOT's type system doesn't have it loaded.
ERROR: Cannot translate class 'xAOD::IParticleLinkContainer': ROOT's type system doesn't have it loaded.
ERROR: Cannot translate class 'xAOD::ParticleAuxContainer': ROOT's type system doesn't have it loaded.
ERROR: Cannot translate class 'xAOD::ParticleContainer': ROOT's type system doesn't have it loaded.
ERROR: Cannot translate class 'xAOD::egammaRecContainer': ROOT's type system doesn't have it loaded.
ERROR: Cannot translate class 'xAOD::CMXJetHitsAuxContainer': ROOT's type system doesn't have it loaded.
...
INFO: Not translating '__shared_ptr<ROOT::Math::Minimizer, __gnu_cxx::_Lock_policy::_S_atomic>' as it is a private internal class (__shared_ptr)
INFO: Not translating '__shared_ptr<ROOT::Math::Minimizer>' as it is a private internal class (__shared_ptr)
INFO: Not translating '__shared_ptr_access<ROOT::Math::Minimizer, __gnu_cxx::_Lock_policy::_S_atomic, false, false>' as it is a private internal class (__shared_ptr_access)
ERROR: Cannot translate class 'Axis_t': ROOT's type system doesn't have it loaded.
ERROR: Cannot translate class 'Bool_t()(const TGraph,Int_t,Int_t)': ROOT's type system doesn't have it loaded.
ERROR: Cannot translate class 'mutex::native_handle_type': ROOT's type system doesn't have it loaded.
```

And you will have a giant `yaml` file containing the complete type system. That `yaml` file can be read by the type generator system.

The above instructions and the Usage instructions should be enough to get you developing on platforms other than windows, with or without `vscode`. PR's welcome to add instructions on how to run on other OS's and development environments!

## Supported Releases

* ATLAS AnalysisBase Containers
  * R21 Series
  * R22 Series
