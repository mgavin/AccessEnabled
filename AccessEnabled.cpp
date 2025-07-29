/*
* Access Enabled Bakkesmod Plugin for Rocket League
*
* This plugin's purpose is to enable access to different menu items that are
* normally inaccessible to the player in certain game states.
*
* Most notably, when you're in training mode (or freeplay), you can's access the
* [Tournaments] playlist card, nor can you access [Create Private Match],
* [Create Lan Match], [Exhibition], or [Season]. This plugin removes the "Access
* Disabled" status from them.
*
* Honestly, if it does more than that, then there's an issue.
* It shouldn's. Please report it.
*
*/

#include "AccessEnabled.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

#include "HookedEvents.hpp"
#include "Logger.hpp"

namespace {
namespace log = LOGGER;
}

BAKKESMOD_PLUGIN(AccessEnabled, "AccessEnabled", "1.2.0", NULL);

/**
* @brief Things that happen when the plugin is loaded.
*/
void AccessEnabled::onLoad() {
      // initialize things
      HookedEvents::gameWrapper = gameWrapper;

      // set up logging necessities
      log::set_cvarmanager(cvarManager);
      log::set_loglevel(log::LOGLEVEL::INFO);

      CVarManager::instance().set_cvar_prefix("ae_");
      CVarManager::instance().set_cvarmanager(cvarManager);

      cvar_storage = std::make_unique<PersistentManagedCVarStorage>(this, "ae_cvars", true, true);

      init_cvars();
      init_hooked_events();
}

/**
* @brief group together the initialization of cvars
*/
void AccessEnabled::init_cvars() {
      CVarManager::instance().register_cvars();
      CVarManager::instance().get_cvar_enabled().addOnValueChanged([this](std::string oldValue, CVarWrapper newValue) {
            plugin_enabled = newValue.getBoolValue();
            plugin_enabled ? enable_plugin() : disable_plugin();
      });
#define X(name, ...) cvar_storage->AddCVar(CVarManager::instance().get_cvar_prefix() + #name);
      LIST_OF_PLUGIN_CVARS
#undef X
}

/**
* @brief Group together the initialization of hooked events
*              that will exist over the lifetime of the plugin.
*/
void AccessEnabled::init_hooked_events() {
}

/**
* @brief Things that need to be set to be activated/run when the plugin is turned "on".
*/
void AccessEnabled::enable_plugin() {
      static uint32_t c;
      HookedEvents::AddHookedEventWithCaller<ActorWrapper>(
            "Function TAGame.MenuTreeNodeEnabledChecker_TA.UpdateGFxNodeEnabled",
            [this](ActorWrapper unused, void * params, std::string eventName) {
                  struct parms {
                        uintptr_t n;
                        _ *       s;
                  } * p   = reinterpret_cast<parms *>(params);
                  c       = p->s->r;
                  p->s->r = 0;
            });

      HookedEvents::AddHookedEventWithCaller<ActorWrapper>(
            "Function TAGame.MenuTreeNodeEnabledChecker_TA.UpdateGFxNodeEnabled",
            [this](ActorWrapper unused, void * params, std::string eventName) {
                  struct parms {
                        uintptr_t n;
                        _ *       s;
                  } * p   = reinterpret_cast<parms *>(params);
                  p->s->r = c;
            },
            true);
}

/**
* @brief Things that need to be deactivated when the plugin is turned "off".
*/
void AccessEnabled::disable_plugin() {
      HookedEvents::RemoveHook("Function TAGame.MenuTreeNodeEnabledChecker_TA.UpdateGFxNodeEnabled");
      HookedEvents::RemoveHook("Function TAGame.MenuTreeNodeEnabledChecker_TA.UpdateGFxNodeEnabled", true);
}

/**
* @brief Things that happen when the plugin is unloaded.
*/
void AccessEnabled::onUnload() {
      HookedEvents::RemoveAllHooks();
}

/**
* @brief Menu code that gets called when BM renders plugin settings.
*/
void AccessEnabled::RenderSettings() {
      ImGui::TextWrapped(
            "%s",
            R"""(
*
* This plugin's purpose is to enable access to different menu items that are
* normally inaccessible to the player in certain game states.
*
* Most notably, when you're in training mode (or freeplay), you can's access the
* [Tournaments] playlist card, nor can you access [Create Private Match],
* [Create Lan Match], [Exhibition], or [Season]. This plugin removes the "Access
* Disabled" status from them.
*
* Honestly, if it does more than that, then there's an issue.
* It shouldn't. Please report it.
*

          )""");

      if (ImGui::Checkbox("Enable the plugin?", &plugin_enabled)) {
            CVarManager::instance().get_cvar_enabled().setValue(plugin_enabled);
      }
}

/**
* @brief Get the name of the plugin for bakkesmod's plugin menu.
*/
std::string AccessEnabled::GetPluginName() {
      return "Access Enabled";
}

/**
* @brief Set the current ImGui context (for bakkesmod's purpose).
*/
void AccessEnabled::SetImGuiContext(uintptr_t ctx) {
      ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext *>(ctx));
}
