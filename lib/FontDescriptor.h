//
// Created by julian on 9/24/25.
//

#ifndef X11TEST_FONTDESCRIPTOR_H
#define X11TEST_FONTDESCRIPTOR_H
#include <format>
#include <string>

namespace X11App {
    /*
    * **FOUNDRY** (`fndry`)
      The supplier of the font.
      Use this when two fonts share the same `FAMILY_NAME`. Otherwise `*` may be substituted.

    * **FAMILY\_NAME** (`fmly`)
      Typeface name defined by the foundry. Fonts with the same family name are usually visually similar.

    * **WEIGHT\_NAME** (`wght`)
      Degree of blackness of glyphs. Common values: `bold`, `medium`.

    * **SLANT** (`slant`)
      `r` = Roman/upright, `i` = italic, `o` = oblique. Usually must be specified.

    * **SETWIDTH\_NAME** (`sWdth`)
      Width set by designer. Examples: `normal`, `narrow`, `condensed`. Should not be wildcarded if multiple exist.

    * **ADD\_STYLE** (`adstyl`)
      Often empty. Safe to wildcard with `*`. In xfontsel, empty = `(nil)`.

    * **PIXEL\_SIZE** (`pxlsz`)
      Font body size for a given `POINT_SIZE` and `RESOLUTION_Y`. Changes height independent of design size.
      Scalable fonts: use `0` or `*` if `POINT_SIZE` specified.

    * **POINT\_SIZE** (`ptSz`)
      Design body height. Expressed in tenths of a point (1 pt = 1/72 inch).

    * **RESOLUTION\_X** (`resx`)
      Horizontal dpi. For scalable fonts: `0` or `*`. Bitmap fonts often `75` or `100`.

    * **RESOLUTION\_Y** (`resy`)
      Vertical dpi. For scalable fonts: `0` or `*`. For bitmaps, specify one of `resx` or `resy`, wildcard the other.

    * **SPACING** (`spc`)

      * `p` = proportional
      * `m` = monospace (equal logical width)
      * `c` = character cell (equal frame width, typewriter style).
        Some old apps render cleaner with `c` instead of `m`.

    * **AVERAGE\_WIDTH** (`avgWdth`)
      Mean glyph width. `0` for proportional fonts. Safe to wildcard with `*`.

    * **CHARSET\_REGISTRY** (`rgstry`)
      Registration authority for character encoding, e.g. `iso10646`, `koi8`. Must be paired with `CHARSET_ENCODING`. Choose registry compatible with locale.

    * **CHARSET\_ENCODING** (`encdng`)
      Identifier for character set encoding.

     */
    struct FontDescriptor {
        const char *foundry = "*"; // fndry
        const char *family_name = "helvetica"; // fmly
        const char *weight_name = "medium"; // wght
        const char *slant = "r"; // slant
        const char *setwidth_name = "*"; // sWdth
        const char *add_style = "*"; // adstyl
        int pixel_size = 0; // pxlsz
        int point_size = 120; // ptSz (in tenths of a point)
        int resolution_x = 0; // resx
        int resolution_y = 0; // resy
        char spacing = 'm'; // spc (p, m, or c)
        int average_width = 0; // avgWdth
        const char *charset_registry = "*"; // rgstry
        const char *charset_encoding = "*"; // encdng

        FontDescriptor() = default;

        FontDescriptor(const char *family, const char *weight, const int size) : family_name(family),
            weight_name(weight),
            point_size(size) {
        }

        [[nodiscard]] std::string toString() const {
            return std::format(
                "-{}-{}-{}-{}-{}-{}-{}-{}-{}-{}-{}-{}-{}-{}",
                foundry, family_name, weight_name, slant, setwidth_name, add_style, pixel_size, point_size,
                resolution_x, resolution_y, spacing, average_width, charset_registry, charset_encoding
            );
        }
    };
}
#endif //X11TEST_FONTDESCRIPTOR_H
