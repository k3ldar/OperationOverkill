using System.Text.Json;

namespace OpOverkillShared
{
    public static class Constants
    {
        public const int MaxSubCommandRecursionDepth = 15;

        public const char CharNull = '\0';
        public const char CharCarriageReturn = '\r';
        public const char CharLineFeed = '\n';
        public const char CharTab = '\t';
        public const char CharSpace = ' ';
        public const char CharOpeningBracket = '(';
        public const char CharClosingBracket = ')';
        public const char CharSemiColon = ';';
        public const char CharColon = ':';
        public const char CharA = 'A';
        public const char CharB = 'B';
        public const char CharC = 'C';
        public const char CharD = 'D';
        public const char CharE = 'E';
        public const char CharF = 'F';
        public const char CharG = 'G';
        public const char CharH = 'H';
        public const char CharI = 'I';
        public const char CharJ = 'J';
        public const char CharK = 'K';
        public const char CharL = 'L';
        public const char CharM = 'M';
        public const char CharN = 'N';
        public const char CharO = 'O';
        public const char CharP = 'P';
        public const char CharQ = 'Q';
        public const char CharR = 'R';
        public const char CharS = 'S';
        public const char CharT = 'T';
        public const char CharU = 'U';
        public const char CharV = 'V';
        public const char CharW = 'W';
        public const char CharX = 'X';
        public const char CharY = 'Y';
        public const char CharZ = 'Z';
        public const char CharPercent = '%';
        public const char CharVariable = '#';
        public const char CharDollar = '$';
        public const char CharEquals = '=';
        public const char CharVariableBlockStart = '[';
        public const char CharVariableBlockEnd = ']';

        public const int MCode600Pause = 600;
        public const int MCode601Timeout = 601;
        public const int MCode602JobName = 602;
        public const int MCode605 = 605;
        public const int MCode620 = 620;
        public const int MCode621 = 621;
        public const int MCode622 = 622;
        public const int MCode623 = 623;
        public const int MCode630RunProgram = 630;
        public const decimal MCode630RunProgramParams = 630.1M;
        public const int MCode631RunProgram = 631;
        public const decimal MCode631RunProgramParams = 631.1M;
        public const decimal MCode631RunProgramResult = 631.2M;


        public const int SystemVariableCount = 1;
        public const ushort SystemVariableTimeout = 1;
        public const ushort SystemVariableMax = 99;


        public const int SocketKeepAliveMinutes = 5;

        public const int ReceiveBufferSize = 1024 * 8;

        public const string AppSettings = "appsettings.json";

        public static readonly JsonSerializerOptions DefaultJsonSerializerOptions = new()
        {
            AllowTrailingCommas = true,
            WriteIndented = true,
        };
    }
}
