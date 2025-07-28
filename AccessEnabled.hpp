#ifndef __ACCESS_ENABLED_HPP__
#define __ACCESS_ENABLED_HPP__

#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/PluginSettingsWindow.h"

#include "PersistentManagedCVarStorage.hpp"

// registerCvar([req] name,[req] default_value,[req] description, searchable, has_min,
// min, has_max, max, save_to_cfg)
#define LIST_OF_PLUGIN_CVARS X(enabled, "1", "Governs whether the AccessEnabled BakkesMod plugin is enabled.", true)

#include "CVarManager.hpp"

class AccessEnabled : public BakkesMod::Plugin::BakkesModPlugin, public BakkesMod::Plugin::PluginSettingsWindow {
private:
      std::unique_ptr<PersistentManagedCVarStorage> cvar_storage;
      bool                                          plugin_enabled = true;

      void init_cvars();
      void init_hooked_events();

      void enable_plugin();
      void disable_plugin();

      std::map<uintptr_t, uint32_t> restore;

      struct _ {
            unsigned char q[0x128];
            uint32_t      r;
            unsigned char s[0x5A];
      };

public:
      // bakkesmod plugin loading/unloading
      void onLoad();
      void onUnload();

      // bakkesmod plugin window settings
      void        RenderSettings();
      std::string GetPluginName();
      void        SetImGuiContext(uintptr_t ctx);
};

#endif
