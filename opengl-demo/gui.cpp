#include "gui.hpp"

#include <algorithm>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

GUI::GUI(GLFWwindow *window) {
  ImGui::CreateContext();
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init();
}

void GUI::draw() {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  ImGui::SetNextWindowPos({0, 0});
  ImGui::Begin("OpenGL Demo", nullptr,
               ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize |
                   ImGuiWindowFlags_NoSavedSettings);

  auto io = ImGui::GetIO();

  auto n = ImGui::GetFrameCount() % hist.size();
  hist[n] = io.Framerate;
  auto [fpsMin, fpsMax] = std::minmax_element(hist.begin(), hist.end());

  ImGui::Text("%.2f ms/frame [%.2f, %.2f]", io.DeltaTime * 1000, 1000 / *fpsMin,
              1000 / *fpsMax);
  ImGui::Text("%.1f FPS [%.1f, %.1f]", io.Framerate, *fpsMin, *fpsMax);

  ImGui::PlotHistogram("\r", hist.data(), hist.size(), n, NULL, 0, *fpsMax,
                       {0, 60});

  ImGui::Text("%d vertices, %d triangles", io.MetricsRenderVertices,
              io.MetricsRenderIndices / 3);

  ImGui::Separator();
  if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen)) {
    ImGui::Text("FOV: %.1f", data.fov);
    ImGui::Checkbox("Wireframe mode", &data.wireframeMode);
  }

  if (ImGui::CollapsingHeader("Materials", ImGuiTreeNodeFlags_DefaultOpen)) {
    for (auto &[name, one] : data.materials) {
      ImGui::PushID(name.c_str());

      if (ImGui::CollapsingHeader(name.c_str())) {        
        ImGui::Image(ImTextureID(one->diffuse), {64, 64});
        ImGui::SameLine();
        ImGui::Image(ImTextureID(one->specular), {64, 64});
        ImGui::DragFloat("Shininess", &one->shininess, 1.0, 1.0, 250.0);
      }
      ImGui::PopID();
    }
  }

  if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_DefaultOpen)) {
    ImGui::ColorEdit3("Ambient##light", data.lightAmbient,
                      ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_Float |
                          ImGuiColorEditFlags_PickerHueWheel);
    ImGui::ColorEdit3("Diffuse##light", data.lightDiffuse,
                      ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_Float |
                          ImGuiColorEditFlags_PickerHueWheel);
    ImGui::ColorEdit3("Specular##light", data.lightSpecular,
                      ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_Float |
                          ImGuiColorEditFlags_PickerHueWheel);
  }
  ImGui::End();

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
