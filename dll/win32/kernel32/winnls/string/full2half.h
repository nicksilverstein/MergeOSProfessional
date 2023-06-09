/*
 * The fullwidth/halfwidth character table.
 */
/* DEFINE_FULL2HALF(full, half1, half2) */
/* NOTE: This list must be sorted. */
DEFINE_FULL2HALF(0x3000, 0x0020, 0)         /* 　 -->   */
DEFINE_FULL2HALF(0x3001, 0xFF64, 0)         /* 、 --> ､ */
DEFINE_FULL2HALF(0x3002, 0xFF61, 0)         /* 。 --> ｡ */
DEFINE_FULL2HALF(0x300C, 0xFF62, 0)         /* 「 --> ｢ */
DEFINE_FULL2HALF(0x300D, 0xFF63, 0)         /* 」 --> ｣ */
DEFINE_FULL2HALF(0x309B, 0xFF9E, 0)         /* ゛ --> ﾞ */
DEFINE_FULL2HALF(0x309C, 0xFF9F, 0)         /* ゜ --> ﾟ */
DEFINE_FULL2HALF(0x30A1, 0xFF67, 0)         /* ァ --> ｧ */
DEFINE_FULL2HALF(0x30A2, 0xFF71, 0)         /* ア --> ｱ */
DEFINE_FULL2HALF(0x30A3, 0xFF68, 0)         /* ィ --> ｨ */
DEFINE_FULL2HALF(0x30A4, 0xFF72, 0)         /* イ --> ｲ */
DEFINE_FULL2HALF(0x30A5, 0xFF69, 0)         /* ゥ --> ｩ */
DEFINE_FULL2HALF(0x30A6, 0xFF73, 0)         /* ウ --> ｳ */
DEFINE_FULL2HALF(0x30A7, 0xFF6A, 0)         /* ェ --> ｪ */
DEFINE_FULL2HALF(0x30A8, 0xFF74, 0)         /* エ --> ｴ */
DEFINE_FULL2HALF(0x30A9, 0xFF6B, 0)         /* ォ --> ｫ */
DEFINE_FULL2HALF(0x30AA, 0xFF75, 0)         /* オ --> ｵ */
DEFINE_FULL2HALF(0x30AB, 0xFF76, 0)         /* カ --> ｶ */
DEFINE_FULL2HALF(0x30AC, 0xFF76, 0xFF9E)    /* ガ --> ｶﾞ */
DEFINE_FULL2HALF(0x30AD, 0xFF77, 0)         /* キ --> ｷ */
DEFINE_FULL2HALF(0x30AE, 0xFF77, 0xFF9E)    /* ギ --> ｷﾞ */
DEFINE_FULL2HALF(0x30AF, 0xFF78, 0)         /* ク --> ｸ */
DEFINE_FULL2HALF(0x30B0, 0xFF78, 0xFF9E)    /* グ --> ｸﾞ */
DEFINE_FULL2HALF(0x30B1, 0xFF79, 0)         /* ケ --> ｹ */
DEFINE_FULL2HALF(0x30B2, 0xFF79, 0xFF9E)    /* ゲ --> ｹﾞ */
DEFINE_FULL2HALF(0x30B3, 0xFF7A, 0)         /* コ --> ｺ */
DEFINE_FULL2HALF(0x30B4, 0xFF7A, 0xFF9E)    /* ゴ --> ｺﾞ */
DEFINE_FULL2HALF(0x30B5, 0xFF7B, 0)         /* サ --> ｻ */
DEFINE_FULL2HALF(0x30B6, 0xFF7B, 0xFF9E)    /* ザ --> ｻﾞ */
DEFINE_FULL2HALF(0x30B7, 0xFF7C, 0)         /* シ --> ｼ */
DEFINE_FULL2HALF(0x30B8, 0xFF7C, 0xFF9E)    /* ジ --> ｼﾞ */
DEFINE_FULL2HALF(0x30B9, 0xFF7D, 0)         /* ス --> ｽ */
DEFINE_FULL2HALF(0x30BA, 0xFF7D, 0xFF9E)    /* ズ --> ｽﾞ */
DEFINE_FULL2HALF(0x30BB, 0xFF7E, 0)         /* セ --> ｾ */
DEFINE_FULL2HALF(0x30BC, 0xFF7E, 0xFF9E)    /* ゼ --> ｾﾞ */
DEFINE_FULL2HALF(0x30BD, 0xFF7F, 0)         /* ソ --> ｿ */
DEFINE_FULL2HALF(0x30BE, 0xFF7F, 0xFF9E)    /* ゾ --> ｿﾞ */
DEFINE_FULL2HALF(0x30BF, 0xFF80, 0)         /* タ --> ﾀ */
DEFINE_FULL2HALF(0x30C0, 0xFF80, 0xFF9E)    /* ダ --> ﾀﾞ */
DEFINE_FULL2HALF(0x30C1, 0xFF81, 0)         /* チ --> ﾁ */
DEFINE_FULL2HALF(0x30C2, 0xFF81, 0xFF9E)    /* ヂ --> ﾁﾞ */
DEFINE_FULL2HALF(0x30C3, 0xFF6F, 0)         /* ッ --> ｯ */
DEFINE_FULL2HALF(0x30C4, 0xFF82, 0)         /* ツ --> ﾂ */
DEFINE_FULL2HALF(0x30C5, 0xFF82, 0xFF9E)    /* ヅ --> ﾂﾞ */
DEFINE_FULL2HALF(0x30C6, 0xFF83, 0)         /* テ --> ﾃ */
DEFINE_FULL2HALF(0x30C7, 0xFF83, 0xFF9E)    /* デ --> ﾃﾞ */
DEFINE_FULL2HALF(0x30C8, 0xFF84, 0)         /* ト --> ﾄ */
DEFINE_FULL2HALF(0x30C9, 0xFF84, 0xFF9E)    /* ド --> ﾄﾞ */
DEFINE_FULL2HALF(0x30CA, 0xFF85, 0)         /* ナ --> ﾅ */
DEFINE_FULL2HALF(0x30CB, 0xFF86, 0)         /* ニ --> ﾆ */
DEFINE_FULL2HALF(0x30CC, 0xFF87, 0)         /* ヌ --> ﾇ */
DEFINE_FULL2HALF(0x30CD, 0xFF88, 0)         /* ネ --> ﾈ */
DEFINE_FULL2HALF(0x30CE, 0xFF89, 0)         /* ノ --> ﾉ */
DEFINE_FULL2HALF(0x30CF, 0xFF8A, 0)         /* ハ --> ﾊ */
DEFINE_FULL2HALF(0x30D0, 0xFF8A, 0xFF9E)    /* バ --> ﾊﾞ */
DEFINE_FULL2HALF(0x30D1, 0xFF8A, 0x309C)    /* パ <-- ハ゜ */
DEFINE_FULL2HALF(0x30D1, 0xFF8A, 0xFF9F)    /* パ --> ﾊﾟ */
DEFINE_FULL2HALF(0x30D2, 0xFF8B, 0)         /* ヒ --> ﾋ */
DEFINE_FULL2HALF(0x30D3, 0xFF8B, 0xFF9E)    /* ビ --> ﾋﾞ */
DEFINE_FULL2HALF(0x30D4, 0xFF8B, 0x309C)    /* ピ <-- ヒ゜ */
DEFINE_FULL2HALF(0x30D4, 0xFF8B, 0xFF9F)    /* ピ --> ﾋﾟ */
DEFINE_FULL2HALF(0x30D5, 0xFF8C, 0)         /* フ --> ﾌ */
DEFINE_FULL2HALF(0x30D6, 0xFF8C, 0xFF9E)    /* ブ --> ﾌﾞ */
DEFINE_FULL2HALF(0x30D7, 0xFF8C, 0xFF9F)    /* プ --> ﾌﾟ */
DEFINE_FULL2HALF(0x30D8, 0xFF8D, 0)         /* ヘ --> ﾍ */
DEFINE_FULL2HALF(0x30D9, 0xFF8D, 0xFF9E)    /* ベ --> ﾍﾞ */
DEFINE_FULL2HALF(0x30DA, 0xFF8D, 0xFF9F)    /* ペ --> ﾍﾟ */
DEFINE_FULL2HALF(0x30DB, 0xFF8E, 0)         /* ホ --> ﾎ */
DEFINE_FULL2HALF(0x30DC, 0xFF8E, 0xFF9E)    /* ボ --> ﾎﾞ */
DEFINE_FULL2HALF(0x30DD, 0xFF8E, 0xFF9F)    /* ポ --> ﾎﾟ */
DEFINE_FULL2HALF(0x30DE, 0xFF8F, 0)         /* マ --> ﾏ */
DEFINE_FULL2HALF(0x30DF, 0xFF90, 0)         /* ミ --> ﾐ */
DEFINE_FULL2HALF(0x30E0, 0xFF91, 0)         /* ム --> ﾑ */
DEFINE_FULL2HALF(0x30E1, 0xFF92, 0)         /* メ --> ﾒ */
DEFINE_FULL2HALF(0x30E2, 0xFF93, 0)         /* モ --> ﾓ */
DEFINE_FULL2HALF(0x30E3, 0xFF6C, 0)         /* ャ --> ｬ */
DEFINE_FULL2HALF(0x30E4, 0xFF94, 0)         /* ヤ --> ﾔ */
DEFINE_FULL2HALF(0x30E5, 0xFF6D, 0)         /* ュ --> ｭ */
DEFINE_FULL2HALF(0x30E6, 0xFF95, 0)         /* ユ --> ﾕ */
DEFINE_FULL2HALF(0x30E7, 0xFF6E, 0)         /* ョ --> ｮ */
DEFINE_FULL2HALF(0x30E8, 0xFF96, 0)         /* ヨ --> ﾖ */
DEFINE_FULL2HALF(0x30E9, 0xFF97, 0)         /* ラ --> ﾗ */
DEFINE_FULL2HALF(0x30EA, 0xFF98, 0)         /* リ --> ﾘ */
DEFINE_FULL2HALF(0x30EB, 0xFF99, 0)         /* ル --> ﾙ */
DEFINE_FULL2HALF(0x30EC, 0xFF9A, 0)         /* レ --> ﾚ */
DEFINE_FULL2HALF(0x30ED, 0xFF9B, 0)         /* ロ --> ﾛ */
DEFINE_FULL2HALF(0x30EF, 0xFF9C, 0)         /* ワ --> ﾜ */
DEFINE_FULL2HALF(0x30F2, 0xFF66, 0)         /* ヲ --> ｦ */
DEFINE_FULL2HALF(0x30F3, 0xFF9D, 0)         /* ン --> ﾝ */
DEFINE_FULL2HALF(0x30F4, 0xFF73, 0xFF9E)    /* ヴ --> ｳﾞ */
DEFINE_FULL2HALF(0x30F7, 0xFF9C, 0xFF9E)    /* ヷ --> ﾜﾞ */
DEFINE_FULL2HALF(0x30FA, 0xFF66, 0xFF9E)    /* ヺ --> ｦﾞ */
DEFINE_FULL2HALF(0x30FB, 0xFF65, 0)         /* ・ --> ･ */
DEFINE_FULL2HALF(0x30FC, 0xFF70, 0)         /* ー --> ｰ */
