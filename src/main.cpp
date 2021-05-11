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

constexpr int SCR_SIZE = 512;

using namespace rayden;

auto main() -> int {
  Logger::init();

  bool running = true;

  Window window = Window(WindowProps("rayden", SCR_SIZE, SCR_SIZE));
  window.set_event_callback([&running](const Event &event) {
    Event::dispatch<WindowResizedEvent>(
        event, [](const WindowResizedEvent &ev) { glViewport(0, 0, ev.width(), ev.height()); });

    Event::dispatch<WindowClosedEvent>(event, [&running](const WindowClosedEvent & /*ev*/) { running = false; });

    Event::dispatch<KeyPressedEvent>(event, [&running](const KeyPressedEvent &ev) {
      if (ev.key_code() == KeyCode::Escape)
        running = false;
    });
  });

  ShaderProgram quad_shader = ShaderProgram({std::make_pair(ShaderType::Vertex, "shaders/texture.vert.glsl"),
                                             std::make_pair(ShaderType::Fragment, "shaders/texture.frag.glsl")});
  ShaderProgram compute_shader = ShaderProgram({std::make_pair(ShaderType::Compute, "shaders/circle.cmpt.glsl")});
  Texture2D texture = Texture2D(SCR_SIZE, SCR_SIZE);
  FullScreenQuad quad;

  while (running) {
    {
      compute_shader.bind();
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
