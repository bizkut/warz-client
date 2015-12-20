@echo off
echo Running asset exporter with sample configuration and network list...

REM Export and process a list of networks using the SampleConfig file.

"../MorphemeAssetExporter/CommandLine/bin/Release/AssetExporterCLI.exe" export "SampleConfig.cfg" "SampleNetworkList.lst"

REM Only export without processing.

REM "../MorphemeAssetExporter/bin/Release/AssetExporterCLI.exe" exportOnly "SampleConfig.cfg" "SampleNetworkList.lst"

if ERRORLEVEL 0 (echo Done!) else (echo An error occurred!)
