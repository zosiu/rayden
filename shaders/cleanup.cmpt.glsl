#version 430
layout(local_size_x = 1, local_size_y = 1) in;
layout(rgba32f, binding = 0) uniform image2D img_output;

uniform int x_rotation;
uniform int x_position;
uniform int y_rotation;
uniform int y_position;
uniform int z_rotation;
uniform int z_position;

const float POSITION_STEP = 0.1;

const float PI = 3.14159265;
const float ROTATION_STEP = PI / 36.0;

const float INF = 3.402823466e+38;
const float EPSILON = 0.001;

const uint light_type_ambient = 1;
const uint light_type_point = 2;
const uint light_type_directional = 3;

const vec3 BACKGROUND_COLOR = vec3(0.0, 0.0, 0.0);
const vec3 STARTING_CAMERA_ROTATION = vec3(0.0, 0.0, 0.0);
const vec3 STARTING_CAMERA_POSITION = vec3(0.0, 0.0, 0.0);

struct Light {
  uint type;
  float intensity;
  vec3 vector;
};

const uint NUM_OF_LIGHTS = 4;
Light LIGHTS[NUM_OF_LIGHTS] = Light[NUM_OF_LIGHTS](Light(light_type_ambient, 0.1, vec3(0.0, 0.0, 0.0)),    //
                                                   Light(light_type_point, 0.4, vec3(2.0, 1.5, 0.0)),      //
                                                   Light(light_type_point, 0.3, vec3(-1.0, 2.0, 5.0)),     //
                                                   Light(light_type_directional, 0.2, vec3(1.0, 4.0, 4.0)) //
);

struct Sphere {
  vec3 center;
  float radius;
  vec3 color;
  int specular_exponent;
  float reflective;
};

const uint NUM_OF_SPHERES = 4;
Sphere SPHERES[NUM_OF_SPHERES] = Sphere[NUM_OF_SPHERES](                 //
    Sphere(vec3(0.0, -1.0, 3.0), 1.0, vec3(1.0, 0.0, 1.0), 100, 0.2),    //
    Sphere(vec3(-2.0, 0.0, 5.0), 1.0, vec3(0.0, 1.0, 1.0), 10, 0.1),     //
    Sphere(vec3(2.0, 0.0, 4.0), 1.0, vec3(1.0, 1.0, 0.0), 500, 0.4),     //
    Sphere(vec3(0.0, -5001.0, 0.0), 5000.0, vec3(1.0, 1.0, 1.0), -1, -1) //
);

struct Closest {
  float t;
  int sphere_index;
};

struct Viewport {
  vec2 dimensions;
  float distance_from_camera;
};

mat3 rotate_x(float theta) { return mat3(1, 0, 0, 0, cos(theta), -sin(theta), 0, sin(theta), cos(theta)); }

mat3 rotate_y(float theta) { return mat3(cos(theta), 0, sin(theta), 0, 1, 0, -sin(theta), 0, cos(theta)); }

mat3 rotate_z(float theta) { return mat3(cos(theta), -sin(theta), 0, sin(theta), cos(theta), 0, 0, 0, 1); }

vec3 canvas_to_viewport(vec2 pos, vec2 canvas_dimensions, Viewport viewport) {
  return vec3((pos.x - canvas_dimensions.x / 2) * viewport.dimensions.x / canvas_dimensions.x,
              (pos.y - canvas_dimensions.y / 2) * viewport.dimensions.y / canvas_dimensions.y,
              viewport.distance_from_camera);
}

vec3 reflection(vec3 ray, vec3 normal) { return normal * 2 * dot(normal, ray) - ray; }

vec2 intersect_ray_sphere(vec3 origin, vec3 ray, Sphere sphere) {
  vec3 co = origin - sphere.center;

  float a_times_2 = dot(ray, ray) * 2;
  float b = 2 * dot(co, ray);
  float c = dot(co, co) - sphere.radius * sphere.radius;

  float discriminant = b * b - 2 * a_times_2 * c;

  if (discriminant < 0)
    return vec2(INF, INF);

  float sqrtd = sqrt(discriminant);

  return (vec2(-b, -b) + vec2(sqrtd, -sqrtd)) / a_times_2;
}

Closest closest_sphere(vec3 origin, vec3 ray, float t_min, float t_max) {
  float closest_t = INF;
  int closest_sphere_index = -1;

  for (int i = 0; i < NUM_OF_SPHERES; i++) {
    vec2 intersection = intersect_ray_sphere(origin, ray, SPHERES[i]);

    if (intersection.x < closest_t && t_min < intersection.x && intersection.x < t_max) {
      closest_t = intersection.x;
      closest_sphere_index = i;
    }

    if (intersection.y < closest_t && t_min < intersection.y && intersection.y < t_max) {
      closest_t = intersection.y;
      closest_sphere_index = i;
    }
  }

  return Closest(closest_t, closest_sphere_index);
}

float light_intensity(vec3 pos, vec3 normal, vec3 view, int specular_exponent) {
  float intensity = 0.0;

  for (int i = 0; i < NUM_OF_LIGHTS; i++) {
    Light light = LIGHTS[i];
    if (light.type == light_type_ambient) {
      intensity += light.intensity;
    } else {
      vec3 light_direction = light.vector;
      if (light.type == light_type_point)
        light_direction -= pos;

      // Shadow check
      Closest closest = closest_sphere(pos, light_direction, EPSILON, light.type == light_type_point ? 1.0 : INF);
      if (closest.sphere_index != -1)
        continue;

      // diffuse
      float n_dot_l = dot(normal, light_direction);
      if (n_dot_l > 0)
        intensity += light.intensity * n_dot_l / (length(normal) * length(light_direction));

      // specular
      if (specular_exponent != -1) {
        vec3 reflected_light = reflection(light_direction, normal);
        float r_dot_v = dot(reflected_light, view);
        if (r_dot_v > 0)
          intensity += light.intensity * pow(r_dot_v / (length(reflected_light) * length(view)), specular_exponent);
      }
    }
  }

  return intensity;
}

const uint MODE_CALL = 1;
const uint MODE_RETURN = 0;
const uint TRACE_RAY_MAX_RECURSION_LIMIT = 4;

struct TraceRayData {
  uint mode;
  vec3 origin;
  vec3 direction;
  float t_min;
  float t_max;
  uint recursion_depth;
};

struct LocalColor {
  vec3 pos;
  vec3 normal;
  vec3 color;
};

LocalColor local_color(Closest closest, vec3 origin, vec3 direction) {
  Sphere closest_sphere = SPHERES[closest.sphere_index];
  vec3 pos = origin + direction * closest.t;
  vec3 normal = normalize(pos - closest_sphere.center);
  vec3 color = closest_sphere.color * light_intensity(pos, normal, -direction, closest_sphere.specular_exponent);
  return LocalColor(pos, normal, color);
};

// vec3 trace_ray(vec3 origin, vec3 direction, float t_min, float t_max, uint recursion_depth) {
//   vec3 result = BACKGROUND_COLOR;

//   TraceRayData[TRACE_RAY_MAX_RECURSION_LIMIT + 1] stack;
//   uint stack_pointer = 0;
//   stack[stack_pointer++] = TraceRayData(MODE_CALL, origin, direction, t_min, t_max, recursion_depth);

//   while (stack_pointer != 0) {
//     TraceRayData data = stack[--stack_pointer];
//     Closest closest = closest_sphere(data.origin, data.direction, data.t_min, data.t_max);

//     if (data.mode == MODE_CALL) {
//       if (closest.sphere_index == -1) {
//         result = BACKGROUND_COLOR;
//       } else {
//         LocalColor local = local_color(closest, data.origin, data.direction);

//         if (SPHERES[closest.sphere_index].reflective <= 0 || data.recursion_depth <= 0) {
//           result = local.color;
//         } else {
//           stack[stack_pointer++] = TraceRayData( //
//               MODE_RETURN, data.origin, data.direction, data.t_min, data.t_max, data.recursion_depth);
//           stack[stack_pointer++] = TraceRayData( //
//               MODE_CALL, local.pos, reflection(-data.direction, local.normal), EPSILON, INF, data.recursion_depth -
//               1);
//         }
//       }
//     } else {
//       float reflective = SPHERES[closest.sphere_index].reflective;
//       result = result * reflective + local_color(closest, data.origin, data.direction).color * (1 - reflective);
//     }
//   }

//   return result;
// }

vec3 trace_ray_r0(vec3 origin, vec3 direction, float t_min, float t_max) {
  Closest closest = closest_sphere(origin, direction, t_min, t_max);
  return closest.sphere_index == -1 ? BACKGROUND_COLOR : local_color(closest, origin, direction).color;
}

vec3 trace_ray_r1(vec3 origin, vec3 direction, float t_min, float t_max) {
  Closest closest = closest_sphere(origin, direction, t_min, t_max);
  if (closest.sphere_index == -1)
    return BACKGROUND_COLOR;

  float reflective = SPHERES[closest.sphere_index].reflective;
  LocalColor local = local_color(closest, origin, direction);

  return reflective <= 0 ? local.color
                         : trace_ray_r0(local.pos, reflection(-direction, local.normal), EPSILON, INF) * reflective +
                               local.color * (1 - reflective);
}

vec3 trace_ray_r2(vec3 origin, vec3 direction, float t_min, float t_max) {
  Closest closest = closest_sphere(origin, direction, t_min, t_max);
  if (closest.sphere_index == -1)
    return BACKGROUND_COLOR;

  float reflective = SPHERES[closest.sphere_index].reflective;
  LocalColor local = local_color(closest, origin, direction);
  return reflective <= 0 ? local.color
                         : trace_ray_r1(local.pos, reflection(-direction, local.normal), EPSILON, INF) * reflective +
                               local.color * (1 - reflective);
}

void main() {
  vec2 canvas_dimensions = imageSize(img_output);
  ivec2 position_on_canvas = ivec2(gl_GlobalInvocationID.xy);
  Viewport viewport = Viewport(vec2(canvas_dimensions.x / canvas_dimensions.y, 1.0), 1);

  vec3 camera_rotation = STARTING_CAMERA_ROTATION +
                         vec3(x_rotation * ROTATION_STEP, y_rotation * ROTATION_STEP, z_rotation * ROTATION_STEP);
  vec3 camera_position = STARTING_CAMERA_POSITION +
                         vec3(POSITION_STEP * x_position, POSITION_STEP * y_position, POSITION_STEP * z_position);

  vec3 camera_direction = rotate_x(camera_rotation.x) * rotate_y(camera_rotation.y) * rotate_z(camera_rotation.z) *
                          canvas_to_viewport(position_on_canvas, canvas_dimensions, viewport);

  vec3 pixel_color_on_canvas = trace_ray_r2(camera_position, camera_direction, viewport.distance_from_camera, INF);
  // vec3 pixel_color_on_canvas = trace_ray(camera_position, camera_direction, viewport.distance_from_camera, INF, 3);

  imageStore(img_output, position_on_canvas, vec4(pixel_color_on_canvas, 1.0));
}
