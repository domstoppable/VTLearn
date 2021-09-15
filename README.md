# VTLearn
A training game for the Vibey Transcribey

## Settings
The following file should be included in a build (e.g.: `Saved/Config/Linux/VTSettings.ini`)
```ini
[/Script/VTLearn.VTGameInstance]
DefaultPID=[DEFAULT_PID]
DefaultUsername=[DEFAULT_USERNAME]
SeafileServer=[SEAFILE_SERVER]
SeafileUsername=[SEAFILE_LOGIN]
SeafilePassword=[SEAFILE_PASSWORD]
SeafileRepoID=[SEAFILE_REPO_ID]
SeafileRemotePath=[SEAFILE_REMOTE_PATH]
ContactInfo="Need help? Call, text, or email anytime, day or night\n    [PHONE]\n    [EMAIL]"
```

To use Playstation buttons, add this to `Saved/Config/Linux/Engine.ini`:
```ini
[/Psydekick/Visuals/2D/PKActionBoundMenuButton.PKActionBoundMenuButton_C]
UsePSButtonConfig=True
```