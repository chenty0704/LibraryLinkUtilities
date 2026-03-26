BeginPackage["LibraryLinkUtilities`", "Utilities`"];

FromTypedOptions::usage = UsageString@"FromTypedOptions[`options`] converts `options` to the standard form.";

Begin["`Private`"];

Get["LLU`"];

SetSystemOptions["LibraryLinkOptions" -> "TestFloatingPointExceptions" -> False];

ObjectConvert[object_Association] := ExportString[object, "JSON"];
`LLU`MArgumentType["Object", String, ObjectConvert];

OptionsConvert[options_List] := If[Length[options] > 0, ExportString[options, "JSON"], "{}"];
`LLU`MArgumentType["Options", String, OptionsConvert];

TimeSeriesConvert[series_TimeSeries] := Sequence[MinimumTimeIncrement[series], series["Values"]];
Do[`LLU`MArgumentType[LibraryDataType[TimeSeries, type],
    {Real, {type, _, "Constant"}}, TimeSeriesConvert
], {type, {Real, Integer}}];

TemporalDataConvert[data_TemporalData] :=
        Sequence[If[data["PathCount"] > 1, First, Identity]@MinimumTimeIncrement[data], data["ValueList"]];
Do[`LLU`MArgumentType[LibraryDataType[TemporalData, type],
    {Real, {type, _, "Constant"}}, TemporalDataConvert
], {type, {Real, Integer}}];

FromTypedOptions[type_String] := {"$Type" -> type ~~ "Options"};
FromTypedOptions[options_List] := Prepend[Rest[options], "$Type" -> First[options] ~~ "Options"];

End[];

EndPackage[];
