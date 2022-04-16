#version 450
#extension GL_ARB_separate_shader_objects : enable
#define WORKGROUP_SIZE 1024
layout(local_size_x=WORKGROUP_SIZE) in;

layout(binding = 0) uniform ConfigurationData {
  int num_bins;
  int img_width;
  int img_height;
  float min_rad;
  float max_rad;
} cfg;

layout(binding=1) restrict readonly buffer InputData {
  uint histogram[];
} in_data;

layout(binding=2) restrict writeonly buffer OutputData {
  float histogram[];
} out_data;

void main()
{
  uint idx = gl_LocalInvocationID.x;
  int max_bin_size = cfg.img_width * cfg.img_height;
  
  if(idx < cfg.num_bins) {
    uint bin_value = in_data.histogram[idx];
    out_data.histogram[idx] = float(bin_value) / float(max_bin_size);
  }
}