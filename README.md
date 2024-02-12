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
[bash][atlas AnalysisBase-21.2.247]:func-adl-types-atlas > 
[bash][atlas AnalysisBase-21.2.247]:func-adl-types-atlas > ./scripts/run_on_atlas_containers.sh > test1.yaml
ERROR: Cannot translate class 'xAOD::CaloRingsAuxContainer': ROOT's type system doesn't have it loaded as a class.
ERROR: Cannot translate class 'xAOD::RingSetAuxContainer': ROOT's type system doesn't have it loaded as a class.
ERROR: Cannot translate class 'xAOD::CaloRingsContainer': ROOT's type system doesn't have it loaded as a class.
ERROR: Cannot translate class 'xAOD::RingSetConfAuxContainer': ROOT's type system doesn't have it loaded as a class.
ERROR: Cannot translate class 'xAOD::RingSetConfContainer': ROOT's type system doesn't have it loaded as a class.
...
ERROR: Class DataModel_detail::DVLIteratorBase not translated: no methods to emit.
ERROR: Class ROOT::Experimental::Internal::TBulkBranchRead not translated: no methods to emit.
ERROR: Class ROOT::TIOFeatures not translated: no methods to emit.
ERROR: Class TPoint not translated: no methods to emit.
ERROR: Class TRefCnt not translated: no methods to emit.
ERROR: Class TTree::TClusterIterator not translated: no methods to emit.
ERROR: Class TVirtualIsAProxy not translated: no methods to emit.
ERROR: Class condition_variable not translated: no methods to emit.
ERROR: Class vector<TStreamerInfoActions::TIDNode> not translated: template arguments were bad.
ERROR: Class vector<const ROOT::TSchemaRule *> not translated: template arguments were bad.
ERROR: Class vector<const double *> not translated: template arguments were bad.
ERROR: Class vector<const xAOD::CaloCluster_v1 *> not translated: template arguments were bad.
ERROR: Class vector<const xAOD::IParticle *> not translated: template arguments were bad.
...
ERROR: Cannot emit method ROOT::Detail::TSchemaRuleSet::GetClassName - some types not emitted: TString, 
ERROR: Cannot emit method ROOT::Detail::TSchemaRuleSet::GetRules - some types not emitted: TObjArray, 
ERROR: Cannot emit method ROOT::Detail::TSchemaRuleSet::GetPersistentRules - some types not emitted: TObjArray, 
ERROR: Cannot emit method ROOT::Detail::TSchemaRuleSet::Class_Version - some types not emitted: Version_t, 
ERROR: Cannot emit method ROOT::Detail::TSchemaRuleSet::IsA - some types not emitted: TClass, 
ERROR: Cannot emit method ROOT::Detail::TSchemaRuleSet::DeclFileName - some types not emitted: char, 
ERROR: Cannot emit method ROOT::Detail::TSchemaRuleSet::ImplFileName - some types not emitted: char, 
ERROR: Cannot emit method ROOT::Detail::TSchemaRuleSet::Class_Name - some types not emitted: char, 
ERROR: Cannot emit method ROOT::Detail::TSchemaRuleSet::Dictionary - some types not emitted: TClass, 
ERROR: Cannot emit method ROOT::Detail::TSchemaRuleSet::Class - some types not emitted: TClass, 
ERROR: Cannot emit method ROOT::Detail::TSchemaRuleSet::Hash - some types not emitted: ULong_t, 
ERROR: Cannot emit method ROOT::Detail::TSchemaRuleSet::ClassName - some types not emitted: char, 
ERROR: Cannot emit method ROOT::Detail::TSchemaRuleSet::CheckedHash - some types not emitted: ULong_t, 
ERROR: Cannot emit method ROOT::Detail::TSchemaRuleSet::Clone - some types not emitted: TObject, char, 
...
```

And you will have a giant `yaml` file containing the complete type system. That `yaml` file can be read by the type generator system.

The above instructions and the Usage instructions should be enough to get you developing on platforms other than windows, with or without `vscode`. PR's welcome to add instructions on how to run on other OS's and development environments!

## Limitations

There are lots! However, the log file from running this script will tell you everything that wasn't translated and a top-level reason as to why not.

That said, some classes of things are missing:

* template classes require a lot of extra work, and often custom work. The basics are implemented, but if you find something missing that is required, then please open an issue!
* class `typedef`'s are not supported and must be hand-coded. This is due to a limitation in ROOT: they do not store this information in a place easily accessible.

## Supported Releases

* ATLAS AnalysisBase Containers
  * R21 Series
  * R22 Series
