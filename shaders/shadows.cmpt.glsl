#version 430
layout(local_size_x = 1, local_size_y = 1) in;
layout(rgba32f, binding = 0) uniform image2D img_output;

const float INF = 3.402823466e+38;
const float EPSILON = 0.001;
const uint light_type_ambient = 0x00000001u;
const uint light_type_point = 0x00000002u;
const uint light_type_directional = 0x00000004u;
const uint num_of_spheres = 5;

struct Light {
  uint type;
  float intensity;
  vec3 pos_or_dir;
};

struct Sphere {
  vec3 center;
  float radius;
  vec3 color;
  int specular_exponent;
};

struct Closest {
  float t;
  int sphere_index;
};

struct Viewport {
  vec2 dimensions;
  float distance_from_camera;
};

vec2 intersect_ray_sphere(vec3 camera, vec3 ray, Sphere sphere) {
  vec3 co = camera - sphere.center;

  float a_times_2 = dot(ray, ray) * 2;
  float b = 2 * dot(co, ray);
  float c = dot(co, co) - sphere.radius * sphere.radius;

  float discriminant = b * b - 2 * a_times_2 * c;

  if (discriminant < 0)
    return vec2(INF, INF);

  float sqrtd = sqrt(discriminant);
  float t1 = (-b + sqrtd) / a_times_2;
  float t2 = (-b - sqrtd) / a_times_2;

  return vec2(t1, t2);
}

Closest closest_sphere(vec3 camera, vec3 ray, float t_min, float t_max, Sphere[num_of_spheres] spheres) {
  float closest_t = INF;
  int closest_sphere_index = -1;

  for (int i = 0; i < num_of_spheres; i++) {
    vec2 intersection = intersect_ray_sphere(camera, ray, spheres[i]);

    if (intersection.x > t_min && intersection.x < t_max && intersection.x < closest_t) {
      closest_t = intersection.x;
      closest_sphere_index = i;
    }

    if (intersection.y > t_min && intersection.y < t_max && intersection.y < closest_t) {
      closest_t = intersection.y;
      closest_sphere_index = i;
    }
  }

  return Closest(closest_t, closest_sphere_index);
}

float light_intensity(vec3 pos, vec3 normal, vec3 view_vector, int specular_exponent, Light[3] lights,
                      Sphere[num_of_spheres] spheres) {
  float intensity = 0.0;

  for (int i = 0; i < 3; i++) {
    Light light = lights[i];
    if (light.type == light_type_ambient) {
      intensity += light.intensity;
    } else {
      vec3 light_direction = light.pos_or_dir;
      if (light.type == light_type_point)
        light_direction -= pos;

      // Shadow check
      Closest closest =
          closest_sphere(pos, light_direction, EPSILON, light.type == light_type_point ? 1.0 : INF, spheres);
      if (closest.sphere_index != -1)
        continue;

      // diffuse
      float normal_dot_light = dot(normal, light_direction);
      if (normal_dot_light > 0)
        intensity += light.intensity * normal_dot_light / (length(normal) * length(light_direction));

      // specular
      if (specular_exponent != -1) {
        vec3 reflection = normal * 2 * dot(normal, light_direction) - light_direction;
        float reflection_dot_view_vector = dot(reflection, view_vector);
        if (reflection_dot_view_vector > 0)
          intensity += light.intensity *
                       pow(reflection_dot_view_vector / (length(reflection) * length(view_vector)), specular_exponent);
      }
    }
  }

  return intensity;
}

vec3 canvas_to_viewport(vec2 pos, vec2 canvas_dimensions, Viewport viewport) {
  return vec3((pos.x - canvas_dimensions.x / 2) * viewport.dimensions.x / canvas_dimensions.x,
              (pos.y - canvas_dimensions.y / 2) * viewport.dimensions.y / canvas_dimensions.y,
              viewport.distance_from_camera);
}

vec3 trace_ray(vec3 camera, vec3 ray, float t_min, float t_max, vec3 background_color, Sphere[num_of_spheres] spheres,
               Light[3] lights) {
  Closest closest = closest_sphere(camera, ray, t_min, t_max, spheres);

  if (closest.sphere_index == -1)
    return background_color;

  Sphere closest_sphere = spheres[closest.sphere_index];
  vec3 contact_point = camera + ray * closest.t;
  vec3 sphere_normal_at_contact_point = normalize(contact_point - closest_sphere.center);
  return closest_sphere.color * light_intensity(contact_point, sphere_normal_at_contact_point, -ray,
                                                closest_sphere.specular_exponent, lights, spheres);
}

void main() {
  ivec2 canvas_pos = ivec2(gl_GlobalInvocationID.xy);
  vec2 canvas_dimensions = imageSize(img_output);
  Viewport viewport = Viewport(vec2(1.0, 1.0), 1.0);

  Sphere spheres[num_of_spheres] =
      Sphere[num_of_spheres](Sphere(vec3(0.0, -1.0, 3.0), 1.0, vec3(1.0, 0.0, 1.0), 500),  //
                             Sphere(vec3(2.0, 0.0, 4.0), 1.0, vec3(1.0, 1.0, 0.0), 500),   //
                             Sphere(vec3(-2.0, 0.0, 4.0), 1.0, vec3(0.0, 1.0, 1.0), 10),   //
                             Sphere(vec3(-1.0, -0.5, 11.0), 5.0, vec3(1.0, 0.5, 0.0), -1), //
                             Sphere(vec3(0.0, -5001.0, 0.0), 5000.0, vec3(0.9, 0.9, 0.9), 1000));

  Light lights[3] = Light[3](Light(light_type_ambient, 0.2, vec3(0.0, 0.0, 0.0)), //
                             Light(light_type_point, 0.6, vec3(2.0, 1.0, 0.0)),   //
                             Light(light_type_directional, 0.2, vec3(1.0, 4.0, 4.0)));

  vec3 background_color = vec3(1.0, 1.0, 1.0);

  vec3 camera = vec3(0.0, 0.0, 0.0);
  vec3 viewport_pos = canvas_to_viewport(canvas_pos, canvas_dimensions, viewport);
  vec3 ray = viewport_pos - camera;

  vec3 pixel_color_on_canvas =
      trace_ray(camera, ray, viewport.distance_from_camera, INF, background_color, spheres, lights);

  imageStore(img_output, canvas_pos, vec4(pixel_color_on_canvas, 1.0));
}
