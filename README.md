# Gen 6 CTRPluginFramework

Gen 6 CTRPluginFramework is a 3gx plugin for Pokemon X, Y, Omega Ruby, and Alpha Sapphire on the Nintendo 3DS console. This plugin is based off of [Alolan CTRPluginFramework](https://github.com/biometrix76/alolanctrpluginframework/releases/latest), with PKHeX implementations in an updated `.3gx` file format.

![Top](https://github.com/biometrix76/Gen6CTRPluginFramework/blob/main/Top.bmp)
![Bottom](https://github.com/biometrix76/Gen6CTRPluginFramework/blob/main/Bottom.bmp)

## Installing

1. Make sure to update to the latest [Luma](https://github.com/LumaTeam/Luma3DS/releases/latest)
2. Download the latest [release](https://github.com/biometrix76/gen6ctrpluginframework/releases/latest)
3. Extract the `.zip` containing the `luma` folder to the root of the SD card and replace existing folders. Make sure `Gen6CTRPluginFramework.3gx` is the only `.3gx` file

## Building

1. Requires `devkitPro`
2. Navigate to `C:/devkitPro/msys2 and run msys2_shell.bat`
3. Copy and paste the following command and press enter
```sh
if ! grep -Fxq "[thepixellizeross-lib]" /etc/pacman.conf; then echo -e "\n[thepixellizeross-lib]\nServer = https://thepixellizeross.gitlab.io/packages/any\nSigLevel = Optional" | tee -a /etc/pacman.conf > /dev/null; fi; if ! grep -Fxq "[thepixellizeross-win]" /etc/pacman.conf; then echo -e "\n[thepixellizeross-win]\nServer = https://thepixellizeross.gitlab.io/packages/x86_64/win\nSigLevel = Optional" | tee -a /etc/pacman.conf > /dev/null; fi
```
4. Run `pacman -Sy` and verify it mentions the ThePixellizerOSS databases
5. Run `Release.bat` in the plugin directory

## Credits

Thanks to the following projects, teams, and individuals for being great resources:

- [ThePixellizerOSS](https://gitlab.com/thepixellizeross) et al. for the 3gxtool and CTRPluginFramework to build plugins with
- [PKHeX](https://github.com/kwsch/PKHeX/) et al. for the database, documentation, examples, and code
- [AnalogMan151](https://github.com/AnalogMan151) for the ultraSuMoFramework foundation of [Alolan CTRPluginFramework](https://github.com/biometrix76/alolanctrpluginframework/releases/latest)
- [dragongyre173](https://github.com/dragonfyre173) for the data viewer overlay in-game
- [JourneyOver](https://github.com/JourneyOver) et al. for providing an extensive ActionReplay code [database](https://github.com/JourneyOver/CTRPF-AR-CHEAT-CODES)
- [Alexander Hartmann](https://github.com/Hartie95) & [semaj14](https://github.com/semaj14) for providing the XY & ORAS foundation of this plugin
