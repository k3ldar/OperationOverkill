// This file is used by Code Analysis to maintain SuppressMessage
// attributes that are applied to this project.
// Project-level suppressions either have no target or are given
// a specific target and scoped to a namespace, type, member, etc.

using System.Diagnostics.CodeAnalysis;

[assembly: SuppressMessage("Minor Code Smell", "S101:Types should be named in PascalCase", Justification = "<Pending>", Scope = "type", Target = "~T:OpOverkillShared.Classes.Hourly_Units")]
[assembly: SuppressMessage("Critical Code Smell", "S2696:Instance members should not write to \"static\" fields", Justification = "<Pending>", Scope = "member", Target = "~M:OpOverkillShared.Classes.WeatherUpdateThread.Run(System.Object)~System.Boolean")]
[assembly: SuppressMessage("Style", "IDE0063:Use simple 'using' statement", Justification = "<Pending>", Scope = "member", Target = "~P:OpOverkillShared.Classes.WeatherUpdateThread.Json")]
[assembly: SuppressMessage("Style", "IDE0063:Use simple 'using' statement", Justification = "<Pending>", Scope = "member", Target = "~M:OpOverkillShared.Classes.WeatherUpdateThread.Run(System.Object)~System.Boolean")]
[assembly: SuppressMessage("Major Code Smell", "S6580:Use a format provider when parsing date and time", Justification = "<Pending>", Scope = "member", Target = "~M:OpOverkillShared.Classes.OpenMeteoApiJson.CurrentData~OpOverkillShared.Classes.CurrentWeatherData")]
