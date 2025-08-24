#pragma once

#include <iostream>
#include <png.h>

struct RGB {
    int r, g, b;
    bool operator==(const RGB& rhs) const {
        return r == rhs.r && g == rhs.g && b == rhs.b;
    }
};

class Image {
public:
    Image(int width, int height) {
        height_ = height;
        width_ = width;

        image_ = static_cast<png_bytep*>(malloc(sizeof(png_bytep) * height_));
        for (int row = 0; row < height_; row++) {
            image_[row] = static_cast<png_bytep>(malloc(sizeof(png_bytep) * width_ * 4));
            for (int col = 0; col < width_; col++) {
                // RGB (RGBA) format - black non-transparent
                image_[row][col * 4] = image_[row][col * 4 + 1] = image_[row][col * 4 + 2] = 0;
                image_[row][col * 4 + 3] = 255; // for RBGA
            }
        }
    }

    explicit Image(const std::string& filename) {
        if (filename.find(".png") != std::string::npos) {
            ReadImage(filename);
        } else {
            throw std::runtime_error("Can't process non-png file format");
        }
    }

    void ReadImage(const std::string& filename) {
        FILE* fp = fopen(filename.c_str(), "rb");
        if (!fp) {
            throw std::runtime_error("Can't open png file for reading");
        }

        png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
        if (!png) {
            throw std::runtime_error("Can't create png read struct");
        }

        png_infop info = png_create_info_struct(png);
        if (!info) {
            throw std::runtime_error("Can't create png info struct");
        }

        png_init_io(png, fp);
        png_read_info(png, info);

        width_ = png_get_image_width(png, info);
        height_ = png_get_image_height(png, info);
        png_byte color_type = png_get_color_type(png, info);
        png_byte bit_depth = png_get_bit_depth(png, info);

        // Read any color_type into 8bit depth, RGBA format.

        if (bit_depth == 16) {
            png_set_strip_16(png);
        }

        if (color_type == PNG_COLOR_TYPE_PALETTE) {
            png_set_palette_to_rgb(png);
        }

        // PNG_COLOR_TYPE_GRAY_ALPHA is always 8 or 16bit depth.
        if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) {
            png_set_expand_gray_1_2_4_to_8(png);
        }

        if (png_get_valid(png, info, PNG_INFO_tRNS)) {
            png_set_tRNS_to_alpha(png);
        }

        // These color_type don't have an alpha channel then fill it with 0xff.
        if (color_type == PNG_COLOR_TYPE_RGB || color_type == PNG_COLOR_TYPE_GRAY ||
            color_type == PNG_COLOR_TYPE_PALETTE) {
            png_set_filler(png, 0xFF, PNG_FILLER_AFTER);
        }

        if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
            png_set_gray_to_rgb(png);
        }

        png_read_update_info(png, info);

        image_ = static_cast<png_bytep*>(malloc(sizeof(png_bytep) * height_));
        for (int row = 0; row < height_; row++) {
            image_[row] = static_cast<png_byte*>(malloc(png_get_rowbytes(png, info)));
        }

        png_read_image(png, image_);
        png_destroy_read_struct(&png, &info, nullptr);
        fclose(fp);
    }

    void Write(const std::string& filename) {
        FILE* fp = fopen(filename.c_str(), "wb");
        if (!fp) {
            throw std::runtime_error("Can't open png file for writing");
        }

        png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
        if (!png) {
            throw std::runtime_error("Can't create png write struct");
        }

        png_infop info = png_create_info_struct(png);
        if (!info) {
            throw std::runtime_error("Can't create png info struct");
        }

        png_init_io(png, fp);

        // Output is 8bit depth, RGBA format.
        png_set_IHDR(png, info, width_, height_, 8, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE,
                     PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
        png_write_info(png, info);

        // To remove the alpha channel for PNG_COLOR_TYPE_RGB format,
        // Use png_set_filler().
        // png_set_filler(png, 0, PNG_FILLER_AFTER);

        png_write_image(png, image_);
        png_write_end(png, nullptr);

        fclose(fp);
        png_destroy_write_struct(&png, &info);
    }

    RGB GetPixel(int row, int col) const {
        return RGB{image_[row][col * 4], image_[row][col * 4 + 1], image_[row][col * 4 + 2]};
    }

    void SetPixel(int row, int col, const RGB& px) {
        image_[row][col * 4] = px.r;
        image_[row][col * 4 + 1] = px.g;
        image_[row][col * 4 + 2] = px.b;
    }

    int Height() const {
        return height_;
    }

    int Width() const {
        return width_;
    }

    ~Image() {
        for (int row = 0; row < height_; row++) {
            free(image_[row]);
        }
        free(image_);
    }

private:
    int height_, width_;
    png_bytep* image_;
};