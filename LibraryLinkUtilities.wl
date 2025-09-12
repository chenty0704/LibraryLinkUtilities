BeginPackage["LibraryLinkUtilities`", "Utilities`"];

Begin["`Private`"];

Get["LLU`"];

ObjectConvert[object_Association] := ExportString[object, "JSON"];
`LLU`MArgumentType["Object", String, ObjectConvert];

OptionsConvert[options_List] := ExportString[<|options|>, "JSON"];
`LLU`MArgumentType["Options", String, OptionsConvert];

TypedOptionsConvert[type_String] := Sequence[type ~~ "Options", "{}"];
TypedOptionsConvert[options_List] := Sequence[First[options] ~~ "Options", ExportString[<|Rest[options]|>, "JSON"]];
`LLU`MArgumentType["TypedOptions", {String, String}, TypedOptionsConvert];

TimeSeriesConvert[series_TimeSeries] := Sequence[MinimumTimeIncrement[series], series["Values"]];
Do[`LLU`MArgumentType[LibraryDataType[TimeSeries, type],
    {Real, {type, _, "Constant"}}, TimeSeriesConvert
], {type, {Real, Integer}}];

TemporalDataConvert[data_TemporalData] :=
        Sequence[If[data["PathCount"] > 1, First, Identity]@MinimumTimeIncrement[data], data["ValueList"]];
Do[`LLU`MArgumentType[LibraryDataType[TemporalData, type],
    {Real, {type, _, "Constant"}}, TemporalDataConvert
], {type, {Real, Integer}}];

End[];

EndPackage[];
