// icons.h

#include <string>
#include <vector>
#include "esphome/core/log.h"          // Ensure the correct logging functionality is included
#include "esphome/components/api/api_server.h"
#include "text.h"

namespace nspanel_ha_blueprint {

    // Structure representing an icon on a Nextion page
    struct PageIcon {
        char page[15];           // The page this icon belongs to
        char component[15];      // Component identifier for the icon
        char icon[5];            // Icon codepoint (e.g., "\uE6E8" for mdi:lightbulb-on-outline)
        uint16_t icon_color;     // RGB565 color value for the icon
        uint8_t icon_font;       // Font identifier for the icon
        bool visible;            // Visibility flag for the icon

        PageIcon(const char* page, const char* component, const char* icon_code, uint16_t color, uint8_t font, bool visibility)
            : icon_color(color), icon_font(font), visible(visibility) {
            // Copy the page, component, and icon code safely
            strncpy(this->page, page, sizeof(this->page) - 1);
            this->page[sizeof(this->page) - 1] = '\0';
            strncpy(this->component, component, sizeof(this->component) - 1);
            this->component[sizeof(this->component) - 1] = '\0';
            strncpy(icon, icon_code, sizeof(icon) - 1);
            icon[sizeof(icon) - 1] = '\0';
        }
    };

    // Vector for managing pointers to icons, allocated dynamically in PSRAM
    std::vector<PageIcon*>* icons;

    // Function to set up the icons management structure
    void setup_icons() {
        // Allocate memory for the icons vector in PSRAM
        esphome::ESP_LOGD("nspanel_ha_blueprint.icons", "Allocating memory for icons vector");
        esphome::ExternalRAMAllocator<std::vector<PageIcon*>>
                                        allocator(esphome::ExternalRAMAllocator<std::vector<PageIcon*>>::ALLOW_FAILURE);
        std::vector<PageIcon*>* icons = allocator.allocate(sizeof(std::vector<PageIcon*>));
        if (!icons or icons == nullptr) {
            esphome::ESP_LOGE("nspanel_ha_blueprint.icons",
                                "Failed to allocate memory for icons vector.");
            return;  // Memory allocation failed, do not proceed
        }
        esphome::ESP_LOGD("nspanel_ha_blueprint.icons", "Memory allocated");
    }

    // Function to find an icon based on page and component ID
    PageIcon* find_icon(const char* page, const char* component) {
        if (icons != nullptr) {
            for (auto* icon : *icons) {
                if (strncmp(icon->page, page, sizeof(icon->page)) == 0 &&
                    strncmp(icon->component, component, sizeof(icon->component)) == 0) {
                    return icon;
                }
            }
        }
        return nullptr; // Icon not found
    }

    // Function to add a new icon or update an existing one, and return a pointer to the icon
    PageIcon* add_icon(const char* page, const char* component, const char* icon_code, uint16_t color, uint8_t font, bool visibility) {
        PageIcon* existing_icon = find_icon(page, component);
        if (existing_icon != nullptr) {
            // Update the existing icon
            strncpy(existing_icon->icon, icon_code, sizeof(existing_icon->icon) - 1);
            existing_icon->icon[sizeof(existing_icon->icon) - 1] = '\0';
            existing_icon->icon_color = color;
            existing_icon->icon_font = font;
            existing_icon->visible = visibility;
            return existing_icon;
        } else {
            // Allocate memory for the new icon in PSRAM
            esphome::ESP_LOGD("nspanel_ha_blueprint.icons", "Allocating memory for new icon");
            esphome::ExternalRAMAllocator<PageIcon> allocator(esphome::ExternalRAMAllocator<PageIcon>::ALLOW_FAILURE);
            PageIcon* new_icon = allocator.allocate(sizeof(PageIcon));
            if (!new_icon or new_icon == nullptr) {
                esphome::ESP_LOGE("nspanel_ha_blueprint.icons",
                                    "Failed to allocate memory for new icon vector.");
                return nullptr;  // Memory allocation failed
            }
            esphome::ESP_LOGD("nspanel_ha_blueprint.icons", "Memory allocated");
            // Use placement new to construct the icon in allocated memory
            new (new_icon) PageIcon(page, component, icon_code, color, font, visibility);
            icons->push_back(new_icon);
            return new_icon; // Return a pointer to the newly added icon
        }
    }

    // Function to find an icon or add it if it doesn't exist, with default values for the new icon
    PageIcon* get_icon(const char* page, const char* component) {
        PageIcon* icon = find_icon(page, component);
        if (icon == nullptr) {
            // Add a new icon with default values
            icon = add_icon(page, component, "\uFFFF", 0, 8, false);
        }
        return icon;
    }

    // Example function to set the visibility of a specific icon
    void set_icon_visibility(const char* page, const char* component, bool visibility) {
        PageIcon* icon = find_icon(page, component);
        if (icon != nullptr) {
            icon->visible = visibility;
        } else {
            esphome::ESP_LOGW("set_icon_visibility", "Icon not found for page %s and component %s", page, component);
        }
    }

    // Destructor for cleanup, if needed
    void cleanup_icons() {
        if (icons != nullptr) {
            // Free each icon
            for (auto* icon : *icons) {
                icon->~PageIcon(); // Explicitly call the destructor
                free(icon);        // Free the allocated memory for each icon
            }
            icons->~vector(); // Explicitly call the destructor for the vector
            free(icons);      // Free the allocated memory for the vector
            icons = nullptr;
        }
    }

}  // namespace nspanel_ha_blueprint