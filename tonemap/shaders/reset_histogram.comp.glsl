#version 450 core
#extension GL_ARB_separate_shader_objects : enable
#extension GL_GOOGLE_include_directive    : enable

#define BLOCK_SIZE_X 32 
#define BLOCK_SIZE_Y 32 
#define BLOCK_SIZE_Z 1 
#define WORKGROUP_SIZE 1024
layout(local_size_x=WORKGROUP_SIZE) in;

layout(binding = 0) uniform ConfigurationData {
  int num_bins;
  int img_width;
  int img_height;
  float min_rad;
  float max_rad;
} cfg;

layout(binding = 1) writeonly buffer HistogramData {
  uint histogram[];
} histogram;

void main()
{
  uint global_id = gl_GlobalInvocationID.x;
  if(global_id < cfg.num_bins) {
    histogram.histogram[global_id] = 0;
  }
}
