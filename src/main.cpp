#include <glad/glad.h>

#include <rayden/FullScreenQuad.hpp>
#include <rayden/KeyCodes.hpp>
#include <rayden/KeyPressedEvent.hpp>
#include <rayden/Logger.hpp>
#include <rayden/ShaderProgram.hpp>
#include <rayden/Texture2D.hpp>
#include <rayden/Window.hpp>
#include <rayden/WindowClosedEvent.hpp>
#include <rayden/WindowResizedEvent.hpp>

constexpr int SCR_SIZE_W = 800;
constexpr int SCR_SIZE_H = 600;

using namespace rayden;

auto main() -> int {
  Logger::init();

  bool running = true;

  int rotation_x = 0;
  int position_x = 0;
  int rotation_y = 0;
  int position_y = 0;
  int rotation_z = 0;
  int position_z = 0;

  Window window = Window(WindowProps("rayden", SCR_SIZE_W, SCR_SIZE_H));
  window.set_event_callback([&](const Event &event) {
    Event::dispatch<WindowResizedEvent>(
        event, [](const WindowResizedEvent &ev) { glViewport(0, 0, ev.width(), ev.height()); });

    Event::dispatch<WindowClosedEvent>(event, [&](const WindowClosedEvent & /*ev*/) { running = false; });

    Event::dispatch<KeyPressedEvent>(event, [&](const KeyPressedEvent &ev) {
      if (ev.key_code() == KeyCode::Escape)
        running = false;

      if (ev.key_code() == KeyCode::R) {
        rotation_x = 0;
        position_x = 0;
        rotation_y = 0;
        position_y = 0;
        rotation_z = 0;
        position_z = 0;
      }

      if (ev.key_code() == KeyCode::X)
        switch (ev.key_mod()) {
        case KeyMod::Shift:
          rotation_x--;
          break;
        case KeyMod::Alt:
          position_x++;
          break;
        case KeyMod::Control:
          position_x--;
          break;
        default:
          rotation_x++;
        }

      if (ev.key_code() == KeyCode::Y)
        switch (ev.key_mod()) {
        case KeyMod::Shift:
          rotation_y--;
          break;
        case KeyMod::Alt:
          position_y++;
          break;
        case KeyMod::Control:
          position_y--;
          break;
        default:
          rotation_y++;
        }

      if (ev.key_code() == KeyCode::Z)
        switch (ev.key_mod()) {
        case KeyMod::Shift:
          rotation_z--;
          break;
        case KeyMod::Alt:
          position_z++;
          break;
        case KeyMod::Control:
          position_z--;
          break;
        default:
          rotation_z++;
        }
    });
  });

  ShaderProgram quad_shader = ShaderProgram({std::make_pair(ShaderType::Vertex, "shaders/texture.vert.glsl"),
                                             std::make_pair(ShaderType::Fragment, "shaders/texture.frag.glsl")});
  ShaderProgram compute_shader = ShaderProgram({std::make_pair(ShaderType::Compute, "shaders/cleanup.cmpt.glsl")});
  Texture2D texture = Texture2D(SCR_SIZE_W, SCR_SIZE_H);
  FullScreenQuad quad;

  while (running) {
    {
      compute_shader.bind();
      compute_shader.upload_int_unifom("x_position", position_x);
      compute_shader.upload_int_unifom("x_rotation", rotation_x);
      compute_shader.upload_int_unifom("y_position", position_y);
      compute_shader.upload_int_unifom("y_rotation", rotation_y);
      compute_shader.upload_int_unifom("z_position", position_z);
      compute_shader.upload_int_unifom("z_rotation", rotation_z);

      glDispatchCompute(texture.width(), texture.height(), 1);
    }

    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    {
      quad_shader.bind();
      quad.draw();
    }

    window.update();
  }

  return 0;
}
