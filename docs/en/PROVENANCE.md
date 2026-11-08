# Provenance and verification

The included hardware-demo YAML is based on a tested 48-kHz board configuration, with neutral device and entity names. The existing C++ helper identifiers were retained for compatibility. On-device logs established display, touch, codec, I²S, PSRAM, SD and download. No CI or on-device run of the **neutralized** demo is claimed.

- LCDWiki product documentation and quick-start: https://www.lcdwiki.com/3.5inch_ESP32-S3_Display
- Independent hardware documentation: https://github.com/jlmeredith/ES3C35P
- Xiaozhi board source: https://github.com/78/xiaozhi-esp32/tree/main/main/boards/lcdwiki-es3c35p

## ST77922 initialization: documented provenance

The ST77922 register sequence appears in the **LCDWiki ES3C35P examples**. The **[jlmeredith/ES3C35P](https://github.com/jlmeredith/ES3C35P)** project also documents the same table in the shipped device firmware and provides an ESPHome-compatible representation. The sequence in our YAML follows that documented register order, including the distinctive `0x80`, `0xB9`, `0x2A`, `0x2B` and `0x11` commands. The precise historical path by which the sequence entered our earlier working configuration cannot be established conclusively.

References:
- [LCDWiki ES3C35P product documentation and starter materials](https://www.lcdwiki.com/3.5inch_ESP32-S3_Display)
- [LCDWiki ES3C35P download directory](https://www.lcdwiki.com/res/ES3C35P/)
- [jlmeredith/ES3C35P: hardware research, sequence and license notes](https://github.com/jlmeredith/ES3C35P)
- [xiaozhi-esp32 ES3C35P board implementation](https://github.com/78/xiaozhi-esp32/tree/main/main/boards/lcdwiki-es3c35p)

The register table is provided as **hardware interoperability data**. jlmeredith's own repository code is MIT-licensed, and its documentation does not claim original copyright in the vendor-derived register table. This **does not automatically grant an MIT license to all LCDWiki materials**. Preserve applicable third-party notices when incorporating separately licensed code.

**Not redistributed:** factory firmware, complete flash images, or the vendor starter package. No blanket permission for all vendor assets is asserted.
