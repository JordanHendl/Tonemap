#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_KHR_shader_subgroup_shuffle_relative : require
#extension GL_KHR_shader_subgroup_shuffle : require

#define THREADBLOCK_SIZE  1024
#define BATCH_SIZE        (THREADBLOCK_SIZE*4)

layout (local_size_x = THREADBLOCK_SIZE) in;

uint local_id = gl_LocalInvocationID.x;

layout (std430, binding=1) buffer InputData {
  uvec4 data[];
} input_data;

layout (std430, binding=2) buffer OutputData {
  uvec4 data[];
} output_data;

layout(binding = 3) uniform ConfigurationData {
  int num_bins;
  int img_width;
  int img_height;
  float min_rad;
  float max_rad;
} cfg;

shared uint s_Data[THREADBLOCK_SIZE * 2];

uint scan1Inclusive(uint idata, uint size) {
  uint pos = 2 * local_id.x - (local_id.x & (size - 1));
  s_Data[pos] = 0;
  pos += size;
  s_Data[pos] = idata;

  for (uint offset = 1; offset < size; offset <<= 1) {
    memoryBarrierShared();
    barrier();
    uint t = s_Data[pos] + s_Data[pos - offset];
    memoryBarrierShared();
    barrier();
    s_Data[pos] = t;
  }

  return s_Data[pos];
}

uint scan1Exclusive(uint idata, uint size) {
  return scan1Inclusive(idata, size) - idata;
}

uvec4 scan4Inclusive(uvec4 idata4, uint size) {
  //Level-0 inclusive scan
  idata4.y += idata4.x;
  idata4.z += idata4.y;
  idata4.w += idata4.z;

  //Level-1 exclusive scan
  uint oval = scan1Exclusive(idata4.w, size / 4);

  idata4.x += oval;
  idata4.y += oval;
  idata4.z += oval;
  idata4.w += oval;

  return idata4;
}

//Exclusive vector scan: the array to be scanned is stored
//in local thread memory scope as uint4
uvec4 scan4Exclusive(uvec4 idata4, uint size) {
  uvec4 odata4 = scan4Inclusive(idata4, size);
  odata4.x -= idata4.x;
  odata4.y -= idata4.y;
  odata4.z -= idata4.z;
  odata4.w -= idata4.w;
  return odata4;
}

void main() {
  uint idx = gl_GlobalInvocationID.x;
  uint maxidx = ((cfg.num_bins + 3) / 4);
  
  bool valid = idx < maxidx;

  //Load data
  uvec4 idata4 = valid ? input_data.data[idx] : uvec4(0);
  uvec4 odata4 = scan4Inclusive(idata4, BATCH_SIZE);
  if (valid) output_data.data[idx] = odata4;
}