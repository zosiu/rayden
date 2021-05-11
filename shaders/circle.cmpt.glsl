#version 430
layout(local_size_x = 1, local_size_y = 1) in;
layout(rgba32f, binding = 0) uniform image2D img_output;

void main() {
  vec4 out_color = vec4(1.0, 1.0, 1.0, 1.0);

  ivec2 pos = ivec2(gl_GlobalInvocationID.xy);

  if ((pos.x - 256) * (pos.x - 256) + (pos.y - 256) * (pos.y - 256) <= 4096) {
    out_color = vec4(1.0, 0.0, 0.0, 1.0);
  }

  imageStore(img_output, pos, out_color);
}
