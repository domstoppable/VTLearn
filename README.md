# VTLearn
A training game for the Vibey Transcribey

## Settings
The following file should be included in a build (e.g.: `Saved/Config/Linux/VTSettings.ini`)
```ini
[/Script/VTLearn.SeafileClient]
DefaultRepoID=[REPO_ID]

[/Script/VTLearn.VTGameInstance]
SeafileServer=[SEAFILE_SERVER]
SeafileUsername=[SEAFILE_LOGIN]
SeafilePassword=[SEAFILE_PASSWORD]
SeafileRemotePath=[SEAFILE_REMOTE_PATH]

[/Script/VTLearn.VTSaveGame]
DefaultPID=[DEFAULT_PID]
DefaultUsername=[DEFAULT_USERNAME]
```

To use Playstation buttons, add this to `Saved/Config/Linux/Engine.ini`:
```ini
[/Psydekick/Visuals/2D/PKActionBoundMenuButton.PKActionBoundMenuButton_C]
UsePSButtonConfig=True
```