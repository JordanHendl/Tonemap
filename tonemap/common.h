#pragma once
#include <glm/glm.hpp>
struct Camera {
  glm::mat4 view;
  glm::mat4 projection;
  glm::vec4 position;
};

struct Material {
  glm::vec4 diffuse;
  glm::vec4 specularity;
  glm::vec4 emission;
  float shininess;
};

struct Scene {
  int num_triangles;
};

struct Triangle {
  glm::vec4 a;
  glm::vec4 b;
  glm::vec4 c;
};