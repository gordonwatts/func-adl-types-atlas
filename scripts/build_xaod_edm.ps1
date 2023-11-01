<#
.Description
Build the yaml file that describes the event-data-model for a particular release of the ATLAS experiment's xAOD.

The output is a single yaml file that describes all the collections, types, methods, and job option scripts to run
corrections, etc., required for the release. The script uses ROOT's type system to extract the information.

The output yaml file can be converted into python typeshed files and code necessary for injecting type information
into a `func_adl` query using the func_adl_servicex_type_generator package.

.Parameter release
This is the AnalysisBase release (for example, `21.2.184`). It must match exactly the tag for a docker image
called `atlas/analysisbase` up dockerhub.

.Parameter outputfile
The yaml output will be written here.

.Example
PS> build_xaod_edm.ps1 21.2.184

.Synopsis
Build yaml file containing container and type infromation for an ATLAS anlysisbase release.

.Notes
Docker must be installed.

.Link
https://github.com/gordonwatts/func-adl-types-atlas
#> 
Param (
    [Parameter(Mandatory = $true)][string]$release,
    [Parameter(Mandatory = $true)][string]$outputfile
)
Write-Host "Script is getting started"
$ErrorActionPreference = "Stop"

# Get location of script so we can find the github repo clone.
$scriptPath = Split-Path -Parent $MyInvocation.MyCommand.Definition
$repoPath = Split-Path -Parent $scriptPath

# And output file should be written to the output directory
$resolvedOutputFile = $ExecutionContext.SessionState.Path.GetUnresolvedProviderPathFromPSPath($outputfile)
$resolvedOutputFileDir = Split-Path -Parent $resolvedOutputFile
$resolvedOutputFileName = Split-Path -Leaf $resolvedOutputFile

# Do the work inside the container
Write-Host "Building yaml file for release ${release}"
docker run --rm -it --mount "type=bind,source=${repoPath},target=/func_adl_xaod_types" --mount "type=bind,source=${resolvedOutputFileDir},target=/output" gitlab-registry.cern.ch/atlas/athena/analysisbase:$release bash -c "/func_adl_xaod_types/scripts/build_run_incontainer.sh  ${resolvedOutputFileName}"
Write-Host "$?"
Write-Host "Done building yaml file for release ${release}"
