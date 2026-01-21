#include "imgui/imgui.h"
#include <cstdio>

// Color4 structure to hold RGBA values
struct Color4 {
    float r, g, b, a;
    
    Color4() : r(1.0f), g(1.0f), b(1.0f), a(1.0f) {}
    Color4(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {}
    
    // Convert to ImVec4 for ImGui
    ImVec4 toImVec4() const {
        return ImVec4(r, g, b, a);
    }
    
    // Create from ImVec4
    static Color4 fromImVec4(const ImVec4& v) {
        return Color4(v.x, v.y, v.z, v.w);
    }
};

class Color4Editor {
private:
    Color4 color;
    Color4 savedColors[8];
    char hexInput[16];
    
    void updateHexInput() {
        snprintf(hexInput, sizeof(hexInput), "%02X%02X%02X%02X",
                (int)(color.r * 255), (int)(color.g * 255),
                (int)(color.b * 255), (int)(color.a * 255));
    }
    
    void parseHexInput() {
        unsigned int r, g, b, a = 255;
        int len = strlen(hexInput);
        
        if (len == 6) {
            if (sscanf(hexInput, "%02x%02x%02x", &r, &g, &b) == 3) {
                color.r = r / 255.0f;
                color.g = g / 255.0f;
                color.b = b / 255.0f;
            }
        } else if (len == 8) {
            if (sscanf(hexInput, "%02x%02x%02x%02x", &r, &g, &b, &a) == 4) {
                color.r = r / 255.0f;
                color.g = g / 255.0f;
                color.b = b / 255.0f;
                color.a = a / 255.0f;
            }
        }
    }
    
public:
    Color4Editor() : color(1.0f, 0.5f, 0.2f, 1.0f) {
        updateHexInput();
        
        // Initialize saved colors with some defaults
        savedColors[0] = Color4(1.0f, 0.0f, 0.0f, 1.0f);
        savedColors[1] = Color4(0.0f, 1.0f, 0.0f, 1.0f);
        savedColors[2] = Color4(0.0f, 0.0f, 1.0f, 1.0f);
        savedColors[3] = Color4(1.0f, 1.0f, 0.0f, 1.0f);
        savedColors[4] = Color4(1.0f, 0.0f, 1.0f, 1.0f);
        savedColors[5] = Color4(0.0f, 1.0f, 1.0f, 1.0f);
        savedColors[6] = Color4(1.0f, 1.0f, 1.0f, 1.0f);
        savedColors[7] = Color4(0.0f, 0.0f, 0.0f, 1.0f);
    }
    
    void render() {
        ImGui::Begin("Color4 Editor", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
        
        // Main color picker
        ImVec4 imColor = color.toImVec4();
        if (ImGui::ColorPicker4("##picker", (float*)&imColor, 
            ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_DisplayRGB | 
            ImGuiColorEditFlags_DisplayHSV | ImGuiColorEditFlags_DisplayHex)) {
            color = Color4::fromImVec4(imColor);
            updateHexInput();
        }
        
        ImGui::Separator();
        
        // RGBA sliders
        ImGui::Text("RGBA Values:");
        bool changed = false;
        changed |= ImGui::SliderFloat("R", &color.r, 0.0f, 1.0f);
        changed |= ImGui::SliderFloat("G", &color.g, 0.0f, 1.0f);
        changed |= ImGui::SliderFloat("B", &color.b, 0.0f, 1.0f);
        changed |= ImGui::SliderFloat("A", &color.a, 0.0f, 1.0f);
        
        if (changed) {
            updateHexInput();
        }
        
        ImGui::Separator();
        
        // Hex input
        ImGui::Text("Hex (RRGGBB or RRGGBBAA):");
        ImGui::SetNextItemWidth(150);
        if (ImGui::InputText("##hex", hexInput, sizeof(hexInput), 
            ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase)) {
            parseHexInput();
        }
        
        ImGui::Separator();
        
        // Color preview
        ImGui::Text("Preview:");
        ImGui::ColorButton("##preview", color.toImVec4(), 
            ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_AlphaPreview, 
            ImVec2(200, 50));
        
        ImGui::Separator();
        
        // Saved colors palette
        ImGui::Text("Saved Colors:");
        for (int i = 0; i < 8; i++) {
            ImGui::PushID(i);
            if (ImGui::ColorButton("##saved", savedColors[i].toImVec4(), 
                ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_AlphaPreview, 
                ImVec2(40, 40))) {
                color = savedColors[i];
                updateHexInput();
            }
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("Click to load\nRight-click to save");
            }
            if (ImGui::BeginPopupContextItem("context")) {
                if (ImGui::MenuItem("Save current color here")) {
                    savedColors[i] = color;
                }
                ImGui::EndPopup();
            }
            if ((i + 1) % 4 != 0) ImGui::SameLine();
            ImGui::PopID();
        }
        
        ImGui::Separator();
        
        // Output values
        ImGui::Text("Output:");
        ImGui::Text("float[4]: %.3f, %.3f, %.3f, %.3f", color.r, color.g, color.b, color.a);
        ImGui::Text("uint8[4]: %d, %d, %d, %d", 
            (int)(color.r * 255), (int)(color.g * 255), 
            (int)(color.b * 255), (int)(color.a * 255));
        
        ImGui::End();
    }
    
    const Color4& getColor() const { return color; }
    void setColor(const Color4& c) { 
        color = c; 
        updateHexInput();
    }
};

// Example usage in your main loop:
// Color4Editor editor;
// 
// // In your render loop:
// editor.render();
// 
// // Get the current color:
// Color4 currentColor = editor.getColor();