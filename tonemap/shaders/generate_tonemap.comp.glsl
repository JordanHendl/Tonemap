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
  float histogram[];
} in_data;

layout(binding=2) restrict writeonly buffer OutputData {
  float tonecurve[];
} out_data;

float linear(int bin) {
  int idx = int(gl_LocalInvocationID.x);
  float bin_value = in_data.histogram[idx];
  float linear_interpolation = (float(bin)/float(cfg.num_bins));
  return linear_interpolation * bin_value;
}

void main()
{
  int idx = int(gl_LocalInvocationID.x);
  float val ;
  if(idx < cfg.num_bins) {
    val = linear(idx);
    out_data.tonecurve[idx] = val;
  }
}