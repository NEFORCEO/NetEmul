#ifndef AppVersion
  #define AppVersion "0.0.0"
#endif

#define AppName "NetEmul"
#define AppPublisher "NetEmul"
#define AppExeName "netemul.exe"
#ifndef ReleaseDir
  #define ReleaseDir AddBackslash(SourcePath) + "..\release"
#endif

[Setup]
AppId={{BB2CF0A1-710E-4C3F-A2DE-6AD12EEDC97A}
AppName={#AppName}
AppVersion={#AppVersion}
AppPublisher={#AppPublisher}
DefaultDirName={autopf}\NetEmul
DefaultGroupName=NetEmul
UninstallDisplayIcon={app}\{#AppExeName}
OutputDir=.
OutputBaseFilename=NetEmul-Setup
Compression=lzma2
SolidCompression=yes
WizardStyle=modern
ArchitecturesInstallIn64BitMode=x64
PrivilegesRequired=lowest

[Files]
Source: "{#ReleaseDir}\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: "{group}\NetEmul"; Filename: "{app}\{#AppExeName}"
Name: "{autodesktop}\NetEmul"; Filename: "{app}\{#AppExeName}"; Tasks: desktopicon

[Tasks]
Name: "desktopicon"; Description: "Create a &desktop shortcut"; GroupDescription: "Additional icons:"

[Run]
Filename: "{app}\{#AppExeName}"; Description: "Launch NetEmul"; Flags: postinstall nowait skipifsilent
