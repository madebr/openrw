#include "names.hpp"

#include <rw.h>

#include <iterator>
#include <sstream>

FlagStringBiMap::FlagStringBiMap(const std::initializer_list<MapType>& list, std::uint32_t mask, bool exclusive)
    : m_map(list.begin(), list.end())
    , m_mask(mask)
    , m_exclusive(exclusive) {
}

#include <iostream>
std::string FlagStringBiMap::toString(std::uint32_t flag) const {
    flag &= m_mask;
    if (m_exclusive) {
        for (const auto& mapping : m_map) {
            if (mapping.second == flag) {
                return mapping.first;
            }
        }
        return std::to_string(flag);
    } else {
        size_t nb = 0;
        std::vector<std::string> flags;
        for (const auto& mapping : m_map) {
            if ((mapping.second & flag) == mapping.second) {
                flags.push_back(mapping.first);
                ++nb;
                std::cout << "added " << mapping.first << "\n";
            }
            if (nb == 0) {
                flags.push_back(std::to_string(flag));
            }
        }
        std::ostringstream oss;
        std::ostream_iterator<std::string> oit(oss, ", ");
        std::copy(flags.begin(), flags.end(), oit);
        return oss.str();
    }
}

std::uint32_t FlagStringBiMap::toFlag(const std::string& s) const {
    for (const auto& mapping : m_map) {
        if (mapping.first == s) {
            return mapping.second;
        }
    }
    return std::stoi(s);
}

const FlagStringBiMap BIMAP_PLATFORM_STR(
{
    {"null",    rw::Platform::PLATFORM_NULL, },
    {"gl",      rw::Platform::PLATFORM_GL,   },
    {"ps2",     rw::Platform::PLATFORM_PS2,  },
    {"xbox",    rw::Platform::PLATFORM_XBOX, },
    {"d3d8",    rw::Platform::PLATFORM_D3D8, },
    {"d3d9",    rw::Platform::PLATFORM_D3D9, },
    {"wdgl",    rw::Platform::PLATFORM_WDGL, },
    {"gl3",     rw::Platform::PLATFORM_GL3,  },
});

const FlagStringBiMap BIMAP_TEXTURE_FILTERMODE_STR(
{
    {"none",                      0,                                            },
    {"nearest",                   rw::Texture::FilterMode::NEAREST,             },
    {"linear",                    rw::Texture::FilterMode::LINEAR,              },
    {"mip nearest",               rw::Texture::FilterMode::MIPNEAREST,          },
    {"mip linear",                rw::Texture::FilterMode::MIPLINEAR,           },
    {"linear mip nearest",        rw::Texture::FilterMode::LINEARMIPNEAREST,    },
    {"linear mip linear",         rw::Texture::FilterMode::LINEARMIPLINEAR,     },
});

const FlagStringBiMap BIMAP_TEXTURE_ADDRESSING_STR(
{
    {"none",    0,                                  },
    {"wrap",    rw::Texture::Addressing::WRAP,      },
    {"mirror",  rw::Texture::Addressing::MIRROR,    },
    {"clamp",   rw::Texture::Addressing::CLAMP,     },
    {"border",  rw::Texture::Addressing::BORDER,    },
});

const FlagStringBiMap BIMAP_RASTER_FORMAT_STR(
{
    {"default",     rw::Raster::Format::DEFAULT,    },
    {"C1555",       rw::Raster::Format::C1555,      },
    {"C565",        rw::Raster::Format::C565,       },
    {"C4444",       rw::Raster::Format::C4444,      },
    {"LUM8",        rw::Raster::Format::LUM8,       },
    {"C8888",       rw::Raster::Format::C8888,      },
    {"C888",        rw::Raster::Format::C888,       },
    {"D16",         rw::Raster::Format::D16,        },
    {"D24",         rw::Raster::Format::D24,        },
    {"D32",         rw::Raster::Format::D32,        },
    {"C555",        rw::Raster::Format::C555,       },
}, 0x0fff);

const FlagStringBiMap BIMAP_RASTER_MIPMAP_STR(
{
    {"default",     rw::Raster::Format::DEFAULT,    },
    {"automipmap",  rw::Raster::Format::AUTOMIPMAP, },
    {"mipmap",      rw::Raster::Format::MIPMAP,     },
}, 0x9000);

const FlagStringBiMap BIMAP_RASTER_PALETTE_STR(
{
    {"default",     rw::Raster::Format::DEFAULT,    },
    {"pal8",        rw::Raster::Format::PAL8,       },
    {"pal4",        rw::Raster::Format::PAL4,       },
}, 0x6000);

const FlagStringBiMap BIMAP_RASTER_TYPE_STR(
{
    {"normal",               rw::Raster::Type::NORMAL,          },
    {"zbuffer",              rw::Raster::Type::ZBUFFER,         },
    {"camera",               rw::Raster::Type::CAMERA,          },
    {"texture",              rw::Raster::Type::TEXTURE,         },
    {"cameratexture",        rw::Raster::Type::CAMERATEXTURE,   },
    {"dontallocate",         rw::Raster::Type::DONTALLOCATE,    },
});

//const FlagStringBiMap2 BIMAP_PALETTEFORMAT_STR(
//{
//    {"none",  RWBSTextureNative::PaletteFormat::PALETTE_NONE, },
//    {"4bit",  RWBSTextureNative::PaletteFormat::PALETTE_4BIT, },
//    {"8bit",  RWBSTextureNative::PaletteFormat::PALETTE_8BIT, },
//}, true);

//const FlagStringBiMap BIMAP_MIPMAPFORMAT_STR(
//{
//    {"none",          RWBSTextureNative::MipMapFormat::MIPMAP_NONE,       },
//    {"included",      RWBSTextureNative::MipMapFormat::MIPMAP_INCLUDED,       },
//    {"autogenerate",  RWBSTextureNative::MipMapFormat::MIPMAP_AUTOGENERATE,   },
//}, true);
