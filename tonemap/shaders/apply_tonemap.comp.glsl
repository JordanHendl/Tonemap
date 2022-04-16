#version 450
#extension GL_ARB_separate_shader_objects : enable
#define BLOCK_SIZE_X 32 
#define BLOCK_SIZE_Y 32 
#define BLOCK_SIZE_Z 1 
layout( local_size_x = BLOCK_SIZE_X, local_size_y = BLOCK_SIZE_Y, local_size_z = BLOCK_SIZE_Z ) in ; 
layout(binding = 0) uniform ConfigurationData {
  int num_bins;
  int img_width;
  int img_height;
  float min_rad;
  float max_rad;
} cfg;

layout(binding=1) restrict readonly buffer InputData {
  float tonemap[];
} in_data;

layout( binding = 2, rgba32f ) coherent restrict readonly uniform image2D radiance_tex;
layout( binding = 3, rgba32f ) coherent restrict readonly uniform image2D input_tex;
layout( binding = 4, rgba32f ) coherent restrict writeonly uniform image2D output_tex;

void main()
{
  const ivec2 coords = ivec2(gl_GlobalInvocationID.xy);
  const ivec2 dim = imageSize(input_tex);
  const vec4 lum = imageLoad( input_tex, coords);
  vec4 color = imageLoad(radiance_tex, coords);
  float pix = lum.r;
  const int bin = clamp(int((pix - cfg.min_rad)/(cfg.max_rad-cfg.min_rad) * cfg.num_bins), 0, cfg.num_bins-1);
  pix = lum.r * in_data.tonemap[bin];
  
  color = color * (pix / lum.r);
  imageStore(output_tex, coords, vec4(color.rgb, 1.0f));
}