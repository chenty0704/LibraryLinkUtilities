BeginPackage["LibraryLinkUtilities`", "Utilities`"];

$BuildType::usage = UsageString@"$BuildType specifies whether debug or release builds of libraries will be used.";

$SystemLibrariesDirectory::usage = UsageString@"$SystemLibrariesDirectory is the directory that contains libraries installed on the system.";

SystemLibraryLoad::usage = UsageString@"SystemLibraryLoad[`library`] loads the dynamic system library `library` into the runtime.";

Begin["`Private`"];

Get@FileNameJoin@{$SystemLibrariesDirectory, "share", "LLU", "LibraryLinkUtilities.wl"};

SystemLibraryLoad[library_String] := LibraryLoad@First@FileNames[
    library ~~ __ ~~ ".dll",
    FileNameJoin@{$SystemLibrariesDirectory, If[$BuildType == "Release", Nothing, "debug"], "bin"}
];

SystemLibraryLoad["boost_json"];

ObjectConvert[object_Association] := ExportString[object, "JSON"];
`LLU`MArgumentType["Object", String, ObjectConvert];

OptionsConvert[options_List] := ExportString[<|options|>, "JSON"];
`LLU`MArgumentType["Options", String, OptionsConvert];

TypedOptionsConvert[options_List] := Sequence[First[options] ~~ "Options", ExportString[<|Rest[options]|>, "JSON"]];
`LLU`MArgumentType["TypedOptions", {String, String}, TypedOptionsConvert];

TimeSeriesConvert[series_TimeSeries] := Sequence[MinimumTimeIncrement[series], series["Values"]];
Do[`LLU`MArgumentType[LibraryDataType[TimeSeries, type],
    {Real, {type, _, "Constant"}}, TimeSeriesConvert
], {type, {Real, Integer}}];

TemporalDataConvert[data_TemporalData] := Sequence[First@MinimumTimeIncrement[data], data["ValueList"]];
Do[`LLU`MArgumentType[LibraryDataType[TemporalData, type],
    {Real, {type, _, "Constant"}}, TemporalDataConvert
], {type, {Real, Integer}}];

End[];

EndPackage[];
