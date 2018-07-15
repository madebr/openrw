#ifndef _RWTOOLS_RWTEXTURE_NAMES_HPP_
#define _RWTOOLS_RWTEXTURE_NAMES_HPP_

#include <cstdint>
#include <string>
#include <vector>

class FlagStringBiMap {
public:
    typedef std::pair<std::string, std::uint32_t> MapType;
    FlagStringBiMap(const std::initializer_list<MapType>& list, std::uint32_t mask=-1, bool exclusive=true);
    std::string toString(std::uint32_t flag) const;
    std::uint32_t toFlag(const std::string& s) const;
private:
    std::vector<MapType> m_map;
    std::uint32_t m_mask;
    bool m_exclusive;
};

extern const FlagStringBiMap BIMAP_PLATFORM_STR;
extern const FlagStringBiMap BIMAP_TEXTURE_FILTERMODE_STR;
extern const FlagStringBiMap BIMAP_TEXTURE_ADDRESSING_STR;
extern const FlagStringBiMap BIMAP_RASTER_FORMAT_STR;
extern const FlagStringBiMap BIMAP_RASTER_MIPMAP_STR;
extern const FlagStringBiMap BIMAP_RASTER_PALETTE_STR;
extern const FlagStringBiMap BIMAP_RASTER_TYPE_STR;

#endif
