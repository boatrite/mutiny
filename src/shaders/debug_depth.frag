#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D depthMap;
uniform float near_plane;
uniform float far_plane;

// required when using a perspective projection matrix
float LinearizeDepth(float depth) {
  float z = depth * 2.0 - 1.0; // Back to NDC
  return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));
}

void main() {
  float depthValue = texture(depthMap, TexCoords).r;
  if (TexCoords.x < 0.01 || TexCoords.y < 0.01 || TexCoords.x > 0.99 || TexCoords.y > 0.99) {
    FragColor = vec4(0, 1, 1, 1);
  } else if ((TexCoords.x >= 0.49 && TexCoords.x <= 0.51) || TexCoords.y == 0.5) {
    FragColor = vec4(1, 0, 0, 1);
  } else {
    // FragColor = vec4(vec3(LinearizeDepth(depthValue) / far_plane), 1.0); // perspective
    FragColor = vec4(vec3(depthValue), 1.0); // orthographic
  }
}

