#pragma once
#include "ohm/vulkan/vulkan_impl.h"
#include <vector>

namespace common {
using ohm::ImageFormat;

/** Template structure containing information and data about an image.
 */
template <typename Type>
struct ImageInfo {
  std::vector<Type> pixels;  ///< The raw pixel data of the image.
  int width;                 ///< The width of the image.
  int height;                ///< The height of the image.
  int channels;  ///< The amount of channels ( i.e. RGBA=4 vs RGB=3 ) of each
                 ///< pixel.

  inline auto info() -> ohm::ImageInfo;
};

/** Function to load the image at the specified path.
 * @param path Path to image to load.
 * @return A Structure containing information about this image.
 */
auto loadRGBA32F(const char* path) -> ImageInfo<float>;

/** Function to load the image at the specified path.
 * @param path Path to image to load.
 * @return A Structure containing information about this image.
 */
auto loadRGB32F(const char* path) -> ImageInfo<float>;

/** Function to load the image at the specified path.
 * @param path Path to image to load.
 * @return A Structure containing information about this image.
 */
auto loadRG32F(const char* path) -> ImageInfo<float>;

/** Function to load the image at the specified path.
 * @param path Path to image to load.
 * @return A Structure containing information about this image.
 */
auto loadR32F(const char* path) -> ImageInfo<float>;

/** Function to load the image at the specified path.
 * @param path Path to image to load.
 * @return A Structure containing information about this image.
 */
auto loadRGBA8(const char* path) -> ImageInfo<unsigned char>;

/** Function to load the image at the specified path.
 * @param path Path to image to load.
 * @return A Structure containing information about this image.
 */
auto loadRGB8(const char* path) -> ImageInfo<unsigned char>;

/** Function to load the image at the specified path.
 * @param path Path to image to load.
 * @return A Structure containing information about this image.
 */
auto loadRG8(const char* path) -> ImageInfo<unsigned char>;

/** Function to load the image at the specified path.
 * @param path Path to image to load.
 * @return A Structure containing information about this image.
 */
auto loadR8(const char* path) -> ImageInfo<unsigned char>;

/** Function to load the image at the specified path.
 * @param path Path to image to load.
 * @return A Structure containing information about this image.
 */
auto loadRGBA32F(unsigned num_bytes, const unsigned char* bytes)
    -> ImageInfo<float>;

/** Function to load the image at the specified path.
 * @param path Path to image to load.
 * @return A Structure containing information about this image.
 */
auto loadRGB32F(unsigned num_bytes, const unsigned char* bytes)
    -> ImageInfo<float>;

/** Function to load the image at the specified path.
 * @param path Path to image to load.
 * @return A Structure containing information about this image.
 */
auto loadRG32F(unsigned num_bytes, const unsigned char* bytes)
    -> ImageInfo<float>;

/** Function to load the image at the specified path.
 * @param path Path to image to load.
 * @return A Structure containing information about this image.
 */
auto loadR32F(unsigned num_bytes, const unsigned char* bytes)
    -> ImageInfo<float>;

/** Function to load the image at the specified path.
 * @param path Path to image to load.
 * @return A Structure containing information about this image.
 */
auto loadRGBA8(unsigned num_bytes, const unsigned char* bytes)
    -> ImageInfo<unsigned char>;

/** Function to load the image at the specified path.
 * @param path Path to image to load.
 * @return A Structure containing information about this image.
 */
auto loadRGB8(unsigned num_bytes, const unsigned char* bytes)
    -> ImageInfo<unsigned char>;

/** Function to load the image at the specified path.
 * @param path Path to image to load.
 * @return A Structure containing information about this image.
 */
auto loadRG8(unsigned num_bytes, const unsigned char* bytes)
    -> ImageInfo<unsigned char>;

/** Function to load the image at the specified path.
 * @param path Path to image to load.
 * @return A Structure containing information about this image.
 */
auto loadR8(unsigned num_bytes, const unsigned char* bytes)
    -> ImageInfo<unsigned char>;

/** Function to save an image to the filesystem.
 * @param info The structure containing information about the image to save.
 * @param path The path to the desired output image.
 */
auto savePNG(const ImageInfo<float>& info, const char* path) -> void;

/** Function to save an image to the filesystem.
 * @param info The structure containing information about the image to save.
 * @param path The path to the desired output image.
 */
auto savePNG(const ImageInfo<unsigned char>& info, const char* path) -> void;

template <typename Type>
inline auto ImageInfo<Type>::info() -> ohm::ImageInfo {
  auto info = ohm::ImageInfo();
  info.width = this->width;
  info.height = this->height;
  return info;
}

template <>
inline auto ImageInfo<float>::info() -> ohm::ImageInfo {
  auto info = ohm::ImageInfo();
  info.width = this->width;
  info.height = this->height;

  auto format = ImageFormat::RGBA32F;
  switch (this->channels) {
    case 1:
      format = ImageFormat::R32F;
      break;
    case 2:
      format = ImageFormat::RG32F;
      break;
    case 3:
      format = ImageFormat::RGB32F;
      break;
    case 4:
      format = ImageFormat::RGBA32F;
      break;
    default:
      break;
  }

  info.format = format;
  return info;
}

template <>
inline auto ImageInfo<unsigned char>::info() -> ohm::ImageInfo {
  auto info = ohm::ImageInfo();
  info.width = this->width;
  info.height = this->height;

  auto format = ImageFormat::RGBA8;
  switch (this->channels) {
    case 1:
      format = ImageFormat::R8;
      break;
      //      case 2  : format = ImageFormat::RG8   ; break ;
    case 3:
      format = ImageFormat::RGB8;
      break;
    case 4:
      format = ImageFormat::RGBA8;
      break;
    default:
      break;
  }

  info.format = format;
  return info;
}
}  // namespace swspp
