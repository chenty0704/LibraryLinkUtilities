BeginPackage["LibraryLinkUtilities`", "Utilities`"];

$SystemLibrariesDirectory::usage = UsageString@"$SystemLibrariesDirectory is the directory that contains libraries installed on the system.";

SystemLibraryLoad::usage = UsageString@"SystemLibraryLoad[`lib`] loads the dynamic system library `lib` into the runtime.";

Begin["`Private`"];

Get@FileNameJoin@{$SystemLibrariesDirectory, "share", "LLU", "LibraryLinkUtilities.wl"};

SystemLibraryLoad[lib_String] := LibraryLoad@First@FileNames[
    lib ~~ __ ~~ ".dll",
    FileNameJoin@{$SystemLibrariesDirectory, "bin"}
];

SystemLibraryLoad["boost_json"];

OptionsConvert[opts_List] := ExportString[<|opts|>, "JSON"];
`LLU`MArgumentType["Options", String, OptionsConvert];

TypedOptionsConvert[opts_List] := Sequence[First[opts] ~~ "Options", ExportString[<|Rest[opts]|>, "JSON"]];
`LLU`MArgumentType["TypedOptions", {String, String}, TypedOptionsConvert];

TimeSeriesConvert[ts_TimeSeries] := Sequence[MinimumTimeIncrement[ts], ts["Values"]];
Do[`LLU`MArgumentType[LibraryDataType[TimeSeries, type],
    {Real, {type, _, "Constant"}}, TimeSeriesConvert
], {type, {Real, Integer}}];

TemporalDataConvert[td_TemporalData] := Sequence[First@MinimumTimeIncrement[td], td["ValueList"]];
Do[`LLU`MArgumentType[LibraryDataType[TemporalData, type],
    {Real, {type, _, "Constant"}}, TemporalDataConvert
], {type, {Real, Integer}}];

End[];

EndPackage[];
