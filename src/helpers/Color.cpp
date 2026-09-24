#include "Color.hpp"

#define ALPHA(c) ((double)(((c) >> 24) & 0xff) / 255.0)
#define RED(c)   ((double)(((c) >> 16) & 0xff) / 255.0)
#define GREEN(c) ((double)(((c) >> 8) & 0xff) / 255.0)
#define BLUE(c)  ((double)(((c)) & 0xff) / 255.0)

CHyprColor::CHyprColor() = default;

CHyprColor::CHyprColor(float r_, float g_, float b_, float a_) : r(r_), g(g_), b(b_), a(a_) {
    m_okLab = Hyprgraphics::CColor(Hyprgraphics::CColor::SSRGB{r, g, b}).asOkLab();
}

CHyprColor::CHyprColor(uint64_t hex) : r(RED(hex)), g(GREEN(hex)), b(BLUE(hex)), a(ALPHA(hex)) {
    m_okLab = Hyprgraphics::CColor(Hyprgraphics::CColor::SSRGB{r, g, b}).asOkLab();
}

CHyprColor::CHyprColor(const Hyprgraphics::CColor& color, float a_) : a(a_) {
    const auto SRGB = color.asRgb();
    r               = SRGB.r;
    g               = SRGB.g;
    b               = SRGB.b;

    m_okLab = color.asOkLab();
}

uint32_t CHyprColor::getAsHex() const {
    return sc<uint32_t>(a * 255.f) * 0x1000000 + sc<uint32_t>(r * 255.f) * 0x10000 + sc<uint32_t>(g * 255.f) * 0x100 + sc<uint32_t>(b * 255.f) * 0x1;
}

Hyprgraphics::CColor::SSRGB CHyprColor::asRGB() const {
    return {r, g, b};
}

Hyprgraphics::CColor::SOkLab CHyprColor::asOkLab() const {
    return m_okLab;
}

Hyprgraphics::CColor::SHSL CHyprColor::asHSL() const {
    return Hyprgraphics::CColor(m_okLab).asHSL();
}

CHyprColor CHyprColor::stripA() const {
    return {r, g, b, 1.F};
}

CHyprColor CHyprColor::modifyA(float newa) const {
    return {r, g, b, newa};
}

static inline CHyprColor getWindowBorderColor(const PHLWINDOW window, bool active) {
    static auto PACTIVECOL              = CConfigValue<Config::IComplexConfigValue>("general:col.active_border");
    static auto PINACTIVECOL            = CConfigValue<Config::IComplexConfigValue>("general:col.inactive_border");
    static auto PNOGROUPACTIVECOL       = CConfigValue<Config::IComplexConfigValue>("general:col.nogroup_border_active");
    static auto PNOGROUPINACTIVECOL     = CConfigValue<Config::IComplexConfigValue>("general:col.nogroup_border");
    static auto PGROUPACTIVECOL         = CConfigValue<Config::IComplexConfigValue>("group:col.border_active");
    static auto PGROUPINACTIVECOL       = CConfigValue<Config::IComplexConfigValue>("group:col.border_inactive");
    static auto PGROUPACTIVELOCKEDCOL   = CConfigValue<Config::IComplexConfigValue>("group:col.border_locked_active");
    static auto PGROUPINACTIVELOCKEDCOL = CConfigValue<Config::IComplexConfigValue>("group:col.border_locked_inactive");

    auto* const ACTIVECOL              = sc<Config::CGradientValueData*>(PACTIVECOL.ptr());
    auto* const INACTIVECOL            = sc<Config::CGradientValueData*>(PINACTIVECOL.ptr());
    auto* const NOGROUPACTIVECOL       = sc<Config::CGradientValueData*>(PNOGROUPACTIVECOL.ptr());
    auto* const NOGROUPINACTIVECOL     = sc<Config::CGradientValueData*>(PNOGROUPINACTIVECOL.ptr());
    auto* const GROUPACTIVECOL         = sc<Config::CGradientValueData*>(PGROUPACTIVECOL.ptr());
    auto* const GROUPINACTIVECOL       = sc<Config::CGradientValueData*>(PGROUPINACTIVECOL.ptr());
    auto* const GROUPACTIVELOCKEDCOL   = sc<Config::CGradientValueData*>(PGROUPACTIVELOCKEDCOL.ptr());
    auto* const GROUPINACTIVELOCKEDCOL = sc<Config::CGradientValueData*>(PGROUPINACTIVELOCKEDCOL.ptr());

    const bool  GROUPLOCKED = window->grouping().group() ? window->grouping().group()->locked() || Desktop::windowState()->groupsLocked() : Desktop::windowState()->groupsLocked();

    if (active) {
        const auto* const ACTIVECOLOR = !window->grouping().group() ? (!(window->grouping().rules() & Desktop::View::GROUP_DENY) ? ACTIVECOL : NOGROUPACTIVECOL) :
                                                                      (GROUPLOCKED ? GROUPACTIVELOCKEDCOL : GROUPACTIVECOL);
        return window->m_ruleApplicator->activeBorderColor().valueOr(*ACTIVECOLOR);
    }

    const auto* const INACTIVECOLOR = !window->grouping().group() ? (!(window->grouping().rules() & Desktop::View::GROUP_DENY) ? INACTIVECOL : NOGROUPINACTIVECOL) :
                                                                    (GROUPLOCKED ? GROUPINACTIVELOCKEDCOL : GROUPINACTIVECOL);
    return window->m_ruleApplicator->inactiveBorderColor().valueOr(*INACTIVECOLOR);
}
