#include <3ds.h>
#include <algorithm>
#include <cctype>
#include <cmath>
#include <cfloat>
#include <cstdarg>
#include <ctime>
#include <cstring>
#include <cstdio>
#include <limits.h>
#include <locale>
#include <malloc.h>
#include <map>
#include <queue>
#include <random>
#include <sstream>
#include <stack>
#include <stdlib.h>
#include <string.h>
#include <sys/iosupport.h>
#include <sys/lock.h>
#include <sys/reent.h>
#include <unordered_map>
#include <vector>

#include "csvc.h"
#include "ctrulibExtension.h"
#include "font6x10Linux.h"
#include "math.h"
#include "malloc.h"
#include "memory.h"
#include "plgldr.h"
#include "types.h"
#include "Unicode.h"

#include "CTRPluginFrameworkImpl/Preferences.hpp"
#include "CTRPluginFramework/Utils/LineReader.hpp"
#include "CTRPluginFramework/Utils/LineWriter.hpp"
#include "CTRPluginFramework/Utils/StringExtensions.hpp"
#include "CTRPluginFramework/Utils/Utils.hpp"
#include "CTRPluginFramework/Menu/Keyboard.hpp"
#include "CTRPluginFramework/Menu/KeyboardHandler.hpp"
#include "CTRPluginFramework/Menu/MessageBox.hpp"
#include "CTRPluginFramework/Menu/PluginMenu.hpp"
#include "CTRPluginFramework/Graphics/Color.hpp"
#include "CTRPluginFramework/Graphics/OSD.hpp"
#include "CTRPluginFramework/Graphics/Render.hpp"
#include "CTRPluginFramework/System/Controller.hpp"
#include "CTRPluginFramework/System/Directory.hpp"
#include "CTRPluginFramework/System/FwkSettings.hpp"
#include "CTRPluginFramework/System/Lock.hpp"
#include "CTRPluginFramework/System/MemoryManager.hpp"
#include "CTRPluginFramework/System/Mutex.hpp"
#include "CTRPluginFramework/System/Nibble.hpp"
#include "CTRPluginFramework/System/Hook.hpp"
#include "CTRPluginFramework/System/Process.hpp"
#include "CTRPluginFramework/System/Sleep.hpp"
#include "CTRPluginFramework/System/Sudo.hpp"
#include "CTRPluginFramework/System/System.hpp"
#include "CTRPluginFramework/System/Task.hpp"
#include "CTRPluginFramework/System/Time.hpp"
#include "CTRPluginFramework/System/Touch.hpp"
#include "CTRPluginFramework/System/Vector.hpp"
#include "CTRPluginFrameworkImpl/Disassembler/arm_disasm.h"
#include "CTRPluginFrameworkImpl/Menu.hpp"
#include "CTRPluginFrameworkImpl/Menu/GuideReader.hpp"
#include "CTRPluginFrameworkImpl/Menu/HexEditor.hpp"
#include "CTRPluginFrameworkImpl/Menu/HotkeysModifier.hpp"
#include "CTRPluginFrameworkImpl/Menu/MenuEntryTools.hpp"
#include "CTRPluginFrameworkImpl/Menu/PluginMenuActionReplay.hpp"
#include "CTRPluginFrameworkImpl/Menu/PluginMenuExecuteLoop.hpp"
#include "CTRPluginFrameworkImpl/Menu/PluginMenuHome.hpp"
#include "CTRPluginFrameworkImpl/Menu/PluginMenuTools.hpp"
#include "CTRPluginFrameworkImpl/Menu/SearchMenu.hpp"
#include "CTRPluginFrameworkImpl/Menu/SubMenu.hpp"
#include "CTRPluginFrameworkImpl/Menu/WatchPointManager.hpp"
#include "CTRPluginFrameworkImpl/ActionReplay/ARCode.hpp"
#include "CTRPluginFrameworkImpl/ActionReplay/ARCodeEditor.hpp"
#include "CTRPluginFrameworkImpl/ActionReplay/ARHandler.hpp"
#include "CTRPluginFrameworkImpl/ActionReplay/MenuEntryActionReplay.hpp"
#include "CTRPluginFrameworkImpl/Graphics/BMPImage.hpp"
#include "CTRPluginFrameworkImpl/Graphics/Button.hpp"
#include "CTRPluginFrameworkImpl/Graphics/ComboBox.hpp"
#include "CTRPluginFrameworkImpl/Graphics/FloatingButton.hpp"
#include "CTRPluginFrameworkImpl/Graphics/Font.hpp"
#include "CTRPluginFrameworkImpl/Graphics/Icon.hpp"
#include "CTRPluginFrameworkImpl/Graphics/MessageBoxImpl.hpp"
#include "CTRPluginFrameworkImpl/Graphics/OSDImpl.hpp"
#include "CTRPluginFrameworkImpl/Graphics/PrivColor.hpp"
#include "CTRPluginFrameworkImpl/Graphics/Renderer.hpp"
#include "CTRPluginFrameworkImpl/Graphics/TouchKey.hpp"
#include "CTRPluginFrameworkImpl/Graphics/TouchKeyString.hpp"
#include "CTRPluginFrameworkImpl/Graphics/UIContainer.hpp"
#include "CTRPluginFrameworkImpl/System/Services/Gsp.hpp"
#include "CTRPluginFrameworkImpl/System/Events.hpp"
#include "CTRPluginFrameworkImpl/System/FSPath.hpp"
#include "CTRPluginFrameworkImpl/System/Heap.hpp"
#include "CTRPluginFrameworkImpl/System/HookManager.hpp"
#include "CTRPluginFrameworkImpl/System/IAllocator.hpp"
#include "CTRPluginFrameworkImpl/System/MMU.hpp"
#include "CTRPluginFrameworkImpl/System/ProcessImpl.hpp"
#include "CTRPluginFrameworkImpl/Search/SearchBase.hpp"
#include "CTRPluginFrameworkImpl/System/Scheduler.hpp"
#include "CTRPluginFrameworkImpl/System/Screen.hpp"
#include "CTRPluginFrameworkImpl/System/Screenshot.hpp"
#include "CTRPluginFrameworkImpl/System/SystemImpl.hpp"
#include "CTRPluginFrameworkImpl/Search/Search32.hpp"

using namespace std;