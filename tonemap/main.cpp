#include <functional>
#include <ohm/api/ohm.h>
#include <ohm/vulkan/vulkan_impl.h>
#include "common.h"
#include <map>
#include <vector>
#include <memory>
#include <string>
#include <iostream>
#include <istream>
#include <fstream>
#include <exception>
#include <stdexcept>
#include <cmath>
#include "image.h"

using API = ohm::Vulkan;

struct Config {
  int num_bins;
  int img_width;
  int img_height;
  float min_rad;
  float max_rad;
};

constexpr auto NUM_BINS = 256;

template<typename API, typename Type>
using Array = ohm::Array<API, Type, ohm::RawAllocator<API>>;

template<typename API>
using Image = ohm::Image<API, ohm::RawAllocator<API>>;

static auto running = true;
static auto pipelines = std::map<std::string, std::shared_ptr<ohm::Pipeline<API>>>();
static auto descriptors = std::map<std::string, ohm::Descriptor<API>>();
static auto config = std::shared_ptr<Array<API, Config>>();
static auto histogram = std::shared_ptr<Array<API, unsigned>>();
static auto scanned = std::shared_ptr<Array<API, unsigned>>();
static auto normalized = std::shared_ptr<Array<API, float>>();
static auto tonecurve = std::shared_ptr<Array<API, float>>();
static auto image = std::shared_ptr<Image<API>>();
static auto intensity = std::shared_ptr<Image<API>>();
static auto output = std::shared_ptr<Image<API>>();

namespace ohm {
auto callback(const Event& event) {
  if (event.type() == Event::Type::WindowExit) {
    running = false;
  }
}

auto load_shader(std::string_view path) -> std::string {
  auto stream = std::ifstream(path.begin());

  if (stream) {
    auto tmp = std::string();
    stream.seekg(0, std::ios::end);
    tmp.reserve(stream.tellg());
    stream.seekg(0, std::ios::beg);

    tmp.assign((std::istreambuf_iterator<char>(stream)),
               std::istreambuf_iterator<char>());
    stream.close();
    return tmp;
  }
  throw std::runtime_error("Could not load file" + std::string(path));
}

auto init_pipelines() -> void {
  auto info = PipelineInfo();
  
  info = {{{"intensify.comp.glsl", load_shader("intensify.comp.glsl")}}};
  ::pipelines["intensify"] = std::make_shared<Pipeline<API>>(0, info);
  ::descriptors["intensify"] = pipelines["intensify"]->descriptor();
  
  info = {{{"reset_histogram.comp.glsl", load_shader("reset_histogram.comp.glsl")}}};
  ::pipelines["reset_histogram"] = std::make_shared<Pipeline<API>>(0, info);
  ::descriptors["reset_histogram"] = pipelines["reset_histogram"]->descriptor();
  
  info = {{{"histogram.comp.glsl", load_shader("histogram.comp.glsl")}}};
  ::pipelines["histogram"] = std::make_shared<Pipeline<API>>(0, info);
  ::descriptors["histogram"] = pipelines["histogram"]->descriptor();
  
  info = {{{"inclusive_scan.comp.glsl", load_shader("inclusive_scan.comp.glsl")}}};
  ::pipelines["inclusive_scan"] = std::make_shared<Pipeline<API>>(0, info);
  ::descriptors["inclusive_scan"] = pipelines["inclusive_scan"]->descriptor();
  
  info = {{{"normalize_histogram.comp.glsl", load_shader("normalize_histogram.comp.glsl")}}};
  ::pipelines["normalize_histogram"] = std::make_shared<Pipeline<API>>(0, info);
  ::descriptors["normalize_histogram"] = pipelines["normalize_histogram"]->descriptor();
  
  info = {{{"generate_tonemap.comp.glsl", load_shader("generate_tonemap.comp.glsl")}}};
  ::pipelines["generate_tonemap"] = std::make_shared<Pipeline<API>>(0, info);
  ::descriptors["generate_tonemap"] = pipelines["generate_tonemap"]->descriptor();
  
  info = {{{"apply_tonemap.comp.glsl", load_shader("apply_tonemap.comp.glsl")}}};
  ::pipelines["apply_tonemap"] = std::make_shared<Pipeline<API>>(0, info);
  ::descriptors["apply_tonemap"] = pipelines["apply_tonemap"]->descriptor();
}

auto init_data(std::string_view filename) {
  // initialize images.
  common::ImageInfo<float> loaded = common::loadRGBA32F(filename.begin());
  image = std::make_shared<::Image<API>>(0, loaded.info());
  intensity = std::make_shared<::Image<API>>(0, loaded.info());
  output = std::make_shared<::Image<API>>(0, loaded.info());
  auto staging = ::Array<API, float>(0, loaded.pixels.size(), ohm::HeapType::HostVisible);
  auto cmds = ohm::Commands<API>(0);
  
  //initialize buffers.
  config = std::make_shared<::Array<API, Config>>(0, 1, ohm::HeapType::HostVisible);
  histogram = std::make_shared<::Array<API, unsigned>>(0, NUM_BINS);
  scanned = std::make_shared<::Array<API, unsigned>>(0, NUM_BINS);
  normalized = std::make_shared<::Array<API, float>>(0, NUM_BINS);
  tonecurve = std::make_shared<::Array<API, float>>(0, NUM_BINS);
  
  //fill out buffers.
  auto cfg = Config();
  cfg.num_bins = NUM_BINS;
  cfg.img_width = image->width();
  cfg.img_height = image->height();
  cfg.min_rad = 0.0f;
  cfg.max_rad = 1.0f;
  
  cmds.begin();
  cmds.copy(&cfg, *config);
  cmds.copy(loaded.pixels.data(), staging);
  cmds.copy(staging, *image);
  cmds.submit();
  cmds.synchronize();
  
  //now we bind all the descriptors with data they need.
  descriptors["intensify"].bind("input_tex", *image);
  descriptors["intensify"].bind("output_tex", *intensity);
  
  descriptors["reset_histogram"].bind("cfg", *config);
  descriptors["reset_histogram"].bind("histogram", *histogram);
  
  descriptors["histogram"].bind("cfg", *config);
  descriptors["histogram"].bind("input_tex", *intensity);
  descriptors["histogram"].bind("histogram", *histogram);
  
  descriptors["inclusive_scan"].bind("cfg", *config);
  descriptors["inclusive_scan"].bind("input_data", *histogram);
  descriptors["inclusive_scan"].bind("output_data", *scanned);
  
  descriptors["normalize_histogram"].bind("cfg", *config);
  descriptors["normalize_histogram"].bind("in_data", *scanned);
  descriptors["normalize_histogram"].bind("out_data", *normalized);
  
  descriptors["generate_tonemap"].bind("cfg", *config);
  descriptors["generate_tonemap"].bind("in_data", *normalized);
  descriptors["generate_tonemap"].bind("out_data", *tonecurve);
  
  descriptors["apply_tonemap"].bind("cfg", *config);
  descriptors["apply_tonemap"].bind("in_data", *tonecurve);
  descriptors["apply_tonemap"].bind("radiance_tex", *image);
  descriptors["apply_tonemap"].bind("input_tex", *intensity);
  descriptors["apply_tonemap"].bind("output_tex", *output);
} 

auto run(int argc, const char* argv[]) -> bool {
  auto window = Window<API>(0, {"Histogram Equilization", 1280, 1024, true});
  auto cmd = Commands<API>(0);
  auto cb = EventRegister<API>();
  auto x = image->width() / 32;
  auto y = image->height() / 32;
  auto flat = std::max(NUM_BINS / 1024, 1);
  
  auto rerecord = [&](){
    cmd.begin();
    cmd.bind(descriptors["intensify"]);
    cmd.dispatch(x, y);
    cmd.bind(descriptors["reset_histogram"]);
    cmd.dispatch(flat, 1);
    cmd.bind(descriptors["histogram"]);
    cmd.dispatch(x, y);
    cmd.bind(descriptors["inclusive_scan"]);
    cmd.dispatch(flat, 1, 1);
    cmd.bind(descriptors["normalize_histogram"]);
    cmd.dispatch(flat, 1, 1);
    cmd.bind(descriptors["generate_tonemap"]);
    cmd.dispatch(flat, 1, 1);
    cmd.bind(descriptors["apply_tonemap"]);
    cmd.dispatch(x, y);
    cmd.blit(*output, window);
  };
  
  rerecord();
  window.wait(cmd);
  cb.add(&callback);
  while (running) {
    poll_events<API>();
    if (!window.present()) {
      rerecord();
    }
  }
  return true;
}
}  // namespace ohm

auto main(int argc, const char* argv[]) -> int {
  if(argc != 2) {
    std::cout << "Usage: " << argv[0] << " <image>" << std::endl;
    return 0;
  }
  ohm::System<API>::setDebugParameter("VK_LAYER_KHRONOS_validation");
  ohm::System<API>::setDebugParameter("VK_LAYER_LUNARG_standard_validation");
  ohm::System<API>::initialize();
  ohm::init_pipelines();
  ohm::init_data(argv[1]);
  ohm::run(argc, argv);
  descriptors.clear();
  pipelines.clear();
  config = nullptr;
  histogram = nullptr;
  scanned = nullptr;
  normalized = nullptr;
  tonecurve = nullptr;
  image = nullptr;
  intensity = nullptr;
  output = nullptr;
  ohm::System<API>::shutdown();
  return 0;
}