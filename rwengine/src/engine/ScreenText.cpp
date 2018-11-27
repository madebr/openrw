#include "engine/ScreenText.hpp"

#include <rw/debug.hpp>

#include "fonts/FontMapGta3.hpp"

void ScreenText::tick(float dt) {
    auto millis = static_cast<int>(dt * 1000);

    // Remove all the immedate text
    m_textQueues[static_cast<size_t>(ScreenTextType::Immediate)].clear();

    for (auto& textQueue : m_textQueues) {
        for (unsigned int i = 0; i < textQueue.size();) {
            auto& big = textQueue[i];

            big.displayedMS += millis;
            if (big.displayedMS >= big.durationMS) {
                textQueue.erase(textQueue.begin() + i);
            } else {
                ++i;
            }
        }
    }
}

ScreenTextEntry ScreenTextEntry::makeBig(const GameStringKey& id,
                                         const GameString& str, int style,
                                         int durationMS) {
    switch (style) {
        // Color: Blue
        // Font: Pricedown
        // Style: Italic (lowercase only)
        // Horizontally: Centered
        // Vertically: Baseline at y = 252 (from top)
        // Size: 25 Pixel high letters ('S', 'l')
        case 1:
            return {str,
                    {320.f, 252.f},
                    FONT_PRICEDOWN,
                    50,
                    {2, 0, 0, 0},
                    {58, 119, 133},
                    1,
                    durationMS,
                    0,
                    600,
                    id};

        // Color: Yellow/Gold
        // Font: Pricedown
        // Style: Italic (lowercase only)
        // Horizontally: Right at x = 620 (from left)
        // Vertically: Baseline at y = 380 (from top)
        // Size: 22 Pixel high letters ('S', 'l')
        case 2:
            return {str,
                    {620.f, 380.f},
                    FONT_PRICEDOWN,
                    30,
                    {2, 3, 0, 0},
                    {214, 171, 9},
                    2,
                    durationMS,
                    0,
                    600,
                    id};

        // Color: Light brown
        // Font: Pricedown
        // Style: Italic (lowercase only)
        // Horizontally: Right at x = 620 (from left)
        // Vertically: Baseline at y = 427 (from top)
        // Size: 28 Pixel high letters ('S', 'l')
        case 3:
            return {str,
                    {320.f, 400.f},
                    FONT_PRICEDOWN,
                    50,
                    {5, 5, 0, 0},
                    {169, 123, 88},  /// @todo verify
                    1,
                    durationMS,
                    0,
                    600,
                    id};

        // Color: Blue
        // Font: Arial
        // Style: Italic
        // Horizontally: Centered
        // Vertically: Baseline at y = 176 (from top)
        // Size: 20 Pixel high letters ('S', 'l')
        case 4:
        case 5:
            return {str,
                    {320.f, 176.f},
                    FONT_ARIAL,
                    50,
                    ((style == 4) ? glm::u8vec4({2, 2, 0, 0})
                                  : glm::u8vec4({-2, -2, 0, 0})),
                    {90, 115, 150},  /// @todo verify
                    1,
                    durationMS,
                    0,
                    600,
                    id};

        // Color: Brown
        // Font: Arial
        // Style: Italic
        // Horizontally: Centered
        // Vertically: Baseline at y = 240 (from top)
        // Size: 16 Pixel high letters ('S', 'l')
        case 6:
            return {str,
                    {320.f, 240.f},
                    FONT_ARIAL,
                    50,
                    {2, 2, 0, 0},
                    {152, 89, 39},
                    1,
                    durationMS,
                    0,
                    600,
                    id};

        default:
            RW_ERROR("Unhandled text style");
            break;
    }

    return {GameStringUtil::fromString("Error, style " + std::to_string(style), FONT_PRICEDOWN),
            {320.f, 400.f},
            FONT_ARIAL,
            50,
            {20, 20, 0, 0},
            {20, 20, 200},
            1,
            durationMS,
            0,
            600,
            id};
}

ScreenTextEntry ScreenTextEntry::makeHighPriority(const GameStringKey& id,
                                                  const GameString& str,
                                                  int durationMS) {
    // Color: ?
    // Font: Arial
    // Style: Italic
    // Horizontally: Centered
    // @todo verify: Vertically: Baseline at y = 431 (from top)
    // @todo verify: Size: 15 Pixel high letters ('S', 'l')
    return {str,
            {320.f, 420.f},
            FONT_ARIAL,
            18,
            {1, 0, 0, 0},
            {255, 255, 255},
            1,
            durationMS,
            0,
            50,
            id};
}

ScreenTextEntry ScreenTextEntry::makeHelp(const GameStringKey& id,
                                          const GameString& str) {
    return {str, {20.f, 20.f}, FONT_ARIAL, 18, {0, 0, 0, 255}, {255, 255, 255}, 0, 5000,
            0,   35,           id};
}

ScreenTextEntry ScreenTextEntry::makeHiddenPackageText(const GameStringKey& id,
                                                       const GameString& str) {
    return {str,
            {318.f, 138.f},
            FONT_ARIAL,
            33,
            {2, 2, 0, 0},
            {0x59, 0x73, 0x96},
            1,
            5000,
            0,
            600,
            id};
}
