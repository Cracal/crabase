/**
 * @file cra_macro.h
 * @author Cracal
 * @brief macro utils
 * @version 0.1
 * @date 2026-07-26
 *
 * @copyright Copyright (c) 2026
 *
 */
#ifndef __CRABASE_CRA_MACRO_H__
#define __CRABASE_CRA_MACRO_H__
#include "cra_assert.h"

#define CRA_TUPLE(...)        (__VA_ARGS__)
#define CRA_UNTUPLE(...)      __VA_ARGS__
#define CRA_TUPLE_M(M, ...)   M(__VA_ARGS__)
#define CRA_UNTUPLE_M(M, ...) M __VA_ARGS__

#define CRA_128_NUM                                                                                                    \
    128, 127, 126, 125, 124, 123, 122, 121, 120, 119, 118, 117, 116, 115, 114, 113, 112, 111, 110, 109, 108, 107, 106, \
      105, 104, 103, 102, 101, 100, 99, 98, 97, 96, 95, 94, 93, 92, 91, 90, 89, 88, 87, 86, 85, 84, 83, 82, 81, 80,    \
      79, 78, 77, 76, 75, 74, 73, 72, 71, 70, 69, 68, 67, 66, 65, 64, 63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52,  \
      51, 50, 49, 48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24,  \
      23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1

#define CRA_CNT_GET(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21,    \
                    _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40,     \
                    _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59,     \
                    _60, _61, _62, _63, _64, _65, _66, _67, _68, _69, _70, _71, _72, _73, _74, _75, _76, _77, _78,     \
                    _79, _80, _81, _82, _83, _84, _85, _86, _87, _88, _89, _90, _91, _92, _93, _94, _95, _96, _97,     \
                    _98, _99, _100, _101, _102, _103, _104, _105, _106, _107, _108, _109, _110, _111, _112, _113,      \
                    _114, _115, _116, _117, _118, _119, _120, _121, _122, _123, _124, _125, _126, _127, _128, _N, ...) \
    _N

// count arguments
#define CRA_CNT(...) CRA_APPLY(CRA_CNT_GET, (__VA_ARGS__, CRA_128_NUM, 0))

// get i-th argument
#define CRA_GET_I(i, ...) CRA_CAT(CRA_GET_I, i)(__VA_ARGS__)

// get n arguments
#define CRA_GET_N(n, ...) CRA_CAT(CRA_GET_N, n)(__VA_ARGS__)

// increment
#define CRA_INC(n) CRA_CAT(CRA_INC, n)
// decrement
#define CRA_DEC(n) CRA_CAT(CRA_DEC, n)

// repeat:
//     CRA_REPEAT(MACRO, param, 3) => MACRO(0, param) MACRO(1, param) MACRO(2, param)
#define CRA_REPEAT(M, p, n) CRA_CAT(CRA_REPEAT, n)(M, p)

// foreach:
//     CRA_ITER(MACRO, param, xx1, xx2, ...xxn) => MACRO(param, xx1) MACRO(param, xx2) ...MACRO(param, xxn)
#define CRA_ITER(M, p, ...) CRA_CAT(CRA_ITER, CRA_CNT(__VA_ARGS__))(M, p, __VA_ARGS__)

// cond ? t : f
#define CRA_IIF(cond) CRA_CAT(CRA_IIF, cond)

// auto recursive
#define CRA_AUTO_REC(p, n) CRA_CAT(CRA_AUTO_REC, n)(p)

// apply(a, b) => a b
#define CRA_APPLY CRA_CAT(CRA_APPLY, CRA_AUTO_REC(CRA_APPLY_P, 4))

#if 1 // GET_I

#define CRA_GET_I00(x, ...)  x
#define CRA_GET_I0(...)      CRA_GET_I00(__VA_ARGS__, YOU_WILL_NEVER_GET_THIS)
#define CRA_GET_I1(x, ...)   CRA_GET_I0(__VA_ARGS__)
#define CRA_GET_I2(x, ...)   CRA_GET_I1(__VA_ARGS__)
#define CRA_GET_I3(x, ...)   CRA_GET_I2(__VA_ARGS__)
#define CRA_GET_I4(x, ...)   CRA_GET_I3(__VA_ARGS__)
#define CRA_GET_I5(x, ...)   CRA_GET_I4(__VA_ARGS__)
#define CRA_GET_I6(x, ...)   CRA_GET_I5(__VA_ARGS__)
#define CRA_GET_I7(x, ...)   CRA_GET_I6(__VA_ARGS__)
#define CRA_GET_I8(x, ...)   CRA_GET_I7(__VA_ARGS__)
#define CRA_GET_I9(x, ...)   CRA_GET_I8(__VA_ARGS__)
#define CRA_GET_I10(x, ...)  CRA_GET_I9(__VA_ARGS__)
#define CRA_GET_I11(x, ...)  CRA_GET_I10(__VA_ARGS__)
#define CRA_GET_I12(x, ...)  CRA_GET_I11(__VA_ARGS__)
#define CRA_GET_I13(x, ...)  CRA_GET_I12(__VA_ARGS__)
#define CRA_GET_I14(x, ...)  CRA_GET_I13(__VA_ARGS__)
#define CRA_GET_I15(x, ...)  CRA_GET_I14(__VA_ARGS__)
#define CRA_GET_I16(x, ...)  CRA_GET_I15(__VA_ARGS__)
#define CRA_GET_I17(x, ...)  CRA_GET_I16(__VA_ARGS__)
#define CRA_GET_I18(x, ...)  CRA_GET_I17(__VA_ARGS__)
#define CRA_GET_I19(x, ...)  CRA_GET_I18(__VA_ARGS__)
#define CRA_GET_I20(x, ...)  CRA_GET_I19(__VA_ARGS__)
#define CRA_GET_I21(x, ...)  CRA_GET_I20(__VA_ARGS__)
#define CRA_GET_I22(x, ...)  CRA_GET_I21(__VA_ARGS__)
#define CRA_GET_I23(x, ...)  CRA_GET_I22(__VA_ARGS__)
#define CRA_GET_I24(x, ...)  CRA_GET_I23(__VA_ARGS__)
#define CRA_GET_I25(x, ...)  CRA_GET_I24(__VA_ARGS__)
#define CRA_GET_I26(x, ...)  CRA_GET_I25(__VA_ARGS__)
#define CRA_GET_I27(x, ...)  CRA_GET_I26(__VA_ARGS__)
#define CRA_GET_I28(x, ...)  CRA_GET_I27(__VA_ARGS__)
#define CRA_GET_I29(x, ...)  CRA_GET_I28(__VA_ARGS__)
#define CRA_GET_I30(x, ...)  CRA_GET_I29(__VA_ARGS__)
#define CRA_GET_I31(x, ...)  CRA_GET_I30(__VA_ARGS__)
#define CRA_GET_I32(x, ...)  CRA_GET_I31(__VA_ARGS__)
#define CRA_GET_I33(x, ...)  CRA_GET_I32(__VA_ARGS__)
#define CRA_GET_I34(x, ...)  CRA_GET_I33(__VA_ARGS__)
#define CRA_GET_I35(x, ...)  CRA_GET_I34(__VA_ARGS__)
#define CRA_GET_I36(x, ...)  CRA_GET_I35(__VA_ARGS__)
#define CRA_GET_I37(x, ...)  CRA_GET_I36(__VA_ARGS__)
#define CRA_GET_I38(x, ...)  CRA_GET_I37(__VA_ARGS__)
#define CRA_GET_I39(x, ...)  CRA_GET_I38(__VA_ARGS__)
#define CRA_GET_I40(x, ...)  CRA_GET_I39(__VA_ARGS__)
#define CRA_GET_I41(x, ...)  CRA_GET_I40(__VA_ARGS__)
#define CRA_GET_I42(x, ...)  CRA_GET_I41(__VA_ARGS__)
#define CRA_GET_I43(x, ...)  CRA_GET_I42(__VA_ARGS__)
#define CRA_GET_I44(x, ...)  CRA_GET_I43(__VA_ARGS__)
#define CRA_GET_I45(x, ...)  CRA_GET_I44(__VA_ARGS__)
#define CRA_GET_I46(x, ...)  CRA_GET_I45(__VA_ARGS__)
#define CRA_GET_I47(x, ...)  CRA_GET_I46(__VA_ARGS__)
#define CRA_GET_I48(x, ...)  CRA_GET_I47(__VA_ARGS__)
#define CRA_GET_I49(x, ...)  CRA_GET_I48(__VA_ARGS__)
#define CRA_GET_I50(x, ...)  CRA_GET_I49(__VA_ARGS__)
#define CRA_GET_I51(x, ...)  CRA_GET_I50(__VA_ARGS__)
#define CRA_GET_I52(x, ...)  CRA_GET_I51(__VA_ARGS__)
#define CRA_GET_I53(x, ...)  CRA_GET_I52(__VA_ARGS__)
#define CRA_GET_I54(x, ...)  CRA_GET_I53(__VA_ARGS__)
#define CRA_GET_I55(x, ...)  CRA_GET_I54(__VA_ARGS__)
#define CRA_GET_I56(x, ...)  CRA_GET_I55(__VA_ARGS__)
#define CRA_GET_I57(x, ...)  CRA_GET_I56(__VA_ARGS__)
#define CRA_GET_I58(x, ...)  CRA_GET_I57(__VA_ARGS__)
#define CRA_GET_I59(x, ...)  CRA_GET_I58(__VA_ARGS__)
#define CRA_GET_I60(x, ...)  CRA_GET_I59(__VA_ARGS__)
#define CRA_GET_I61(x, ...)  CRA_GET_I60(__VA_ARGS__)
#define CRA_GET_I62(x, ...)  CRA_GET_I61(__VA_ARGS__)
#define CRA_GET_I63(x, ...)  CRA_GET_I62(__VA_ARGS__)
#define CRA_GET_I64(x, ...)  CRA_GET_I63(__VA_ARGS__)
#define CRA_GET_I65(x, ...)  CRA_GET_I64(__VA_ARGS__)
#define CRA_GET_I66(x, ...)  CRA_GET_I65(__VA_ARGS__)
#define CRA_GET_I67(x, ...)  CRA_GET_I66(__VA_ARGS__)
#define CRA_GET_I68(x, ...)  CRA_GET_I67(__VA_ARGS__)
#define CRA_GET_I69(x, ...)  CRA_GET_I68(__VA_ARGS__)
#define CRA_GET_I70(x, ...)  CRA_GET_I69(__VA_ARGS__)
#define CRA_GET_I71(x, ...)  CRA_GET_I70(__VA_ARGS__)
#define CRA_GET_I72(x, ...)  CRA_GET_I71(__VA_ARGS__)
#define CRA_GET_I73(x, ...)  CRA_GET_I72(__VA_ARGS__)
#define CRA_GET_I74(x, ...)  CRA_GET_I73(__VA_ARGS__)
#define CRA_GET_I75(x, ...)  CRA_GET_I74(__VA_ARGS__)
#define CRA_GET_I76(x, ...)  CRA_GET_I75(__VA_ARGS__)
#define CRA_GET_I77(x, ...)  CRA_GET_I76(__VA_ARGS__)
#define CRA_GET_I78(x, ...)  CRA_GET_I77(__VA_ARGS__)
#define CRA_GET_I79(x, ...)  CRA_GET_I78(__VA_ARGS__)
#define CRA_GET_I80(x, ...)  CRA_GET_I79(__VA_ARGS__)
#define CRA_GET_I81(x, ...)  CRA_GET_I80(__VA_ARGS__)
#define CRA_GET_I82(x, ...)  CRA_GET_I81(__VA_ARGS__)
#define CRA_GET_I83(x, ...)  CRA_GET_I82(__VA_ARGS__)
#define CRA_GET_I84(x, ...)  CRA_GET_I83(__VA_ARGS__)
#define CRA_GET_I85(x, ...)  CRA_GET_I84(__VA_ARGS__)
#define CRA_GET_I86(x, ...)  CRA_GET_I85(__VA_ARGS__)
#define CRA_GET_I87(x, ...)  CRA_GET_I86(__VA_ARGS__)
#define CRA_GET_I88(x, ...)  CRA_GET_I87(__VA_ARGS__)
#define CRA_GET_I89(x, ...)  CRA_GET_I88(__VA_ARGS__)
#define CRA_GET_I90(x, ...)  CRA_GET_I89(__VA_ARGS__)
#define CRA_GET_I91(x, ...)  CRA_GET_I90(__VA_ARGS__)
#define CRA_GET_I92(x, ...)  CRA_GET_I91(__VA_ARGS__)
#define CRA_GET_I93(x, ...)  CRA_GET_I92(__VA_ARGS__)
#define CRA_GET_I94(x, ...)  CRA_GET_I93(__VA_ARGS__)
#define CRA_GET_I95(x, ...)  CRA_GET_I94(__VA_ARGS__)
#define CRA_GET_I96(x, ...)  CRA_GET_I95(__VA_ARGS__)
#define CRA_GET_I97(x, ...)  CRA_GET_I96(__VA_ARGS__)
#define CRA_GET_I98(x, ...)  CRA_GET_I97(__VA_ARGS__)
#define CRA_GET_I99(x, ...)  CRA_GET_I98(__VA_ARGS__)
#define CRA_GET_I100(x, ...) CRA_GET_I99(__VA_ARGS__)
#define CRA_GET_I101(x, ...) CRA_GET_I100(__VA_ARGS__)
#define CRA_GET_I102(x, ...) CRA_GET_I101(__VA_ARGS__)
#define CRA_GET_I103(x, ...) CRA_GET_I102(__VA_ARGS__)
#define CRA_GET_I104(x, ...) CRA_GET_I103(__VA_ARGS__)
#define CRA_GET_I105(x, ...) CRA_GET_I104(__VA_ARGS__)
#define CRA_GET_I106(x, ...) CRA_GET_I105(__VA_ARGS__)
#define CRA_GET_I107(x, ...) CRA_GET_I106(__VA_ARGS__)
#define CRA_GET_I108(x, ...) CRA_GET_I107(__VA_ARGS__)
#define CRA_GET_I109(x, ...) CRA_GET_I108(__VA_ARGS__)
#define CRA_GET_I110(x, ...) CRA_GET_I109(__VA_ARGS__)
#define CRA_GET_I111(x, ...) CRA_GET_I110(__VA_ARGS__)
#define CRA_GET_I112(x, ...) CRA_GET_I111(__VA_ARGS__)
#define CRA_GET_I113(x, ...) CRA_GET_I112(__VA_ARGS__)
#define CRA_GET_I114(x, ...) CRA_GET_I113(__VA_ARGS__)
#define CRA_GET_I115(x, ...) CRA_GET_I114(__VA_ARGS__)
#define CRA_GET_I116(x, ...) CRA_GET_I115(__VA_ARGS__)
#define CRA_GET_I117(x, ...) CRA_GET_I116(__VA_ARGS__)
#define CRA_GET_I118(x, ...) CRA_GET_I117(__VA_ARGS__)
#define CRA_GET_I119(x, ...) CRA_GET_I118(__VA_ARGS__)
#define CRA_GET_I120(x, ...) CRA_GET_I119(__VA_ARGS__)
#define CRA_GET_I121(x, ...) CRA_GET_I120(__VA_ARGS__)
#define CRA_GET_I122(x, ...) CRA_GET_I121(__VA_ARGS__)
#define CRA_GET_I123(x, ...) CRA_GET_I122(__VA_ARGS__)
#define CRA_GET_I124(x, ...) CRA_GET_I123(__VA_ARGS__)
#define CRA_GET_I125(x, ...) CRA_GET_I124(__VA_ARGS__)
#define CRA_GET_I126(x, ...) CRA_GET_I125(__VA_ARGS__)
#define CRA_GET_I127(x, ...) CRA_GET_I126(__VA_ARGS__)
#define CRA_GET_I128(x, ...) CRA_GET_I127(__VA_ARGS__)

#endif // end GET_I

#if 1 // GET_N

#define CRA_GET_N1           CRA_GET_I0
#define CRA_GET_N2(x, ...)   x, CRA_GET_N1(__VA_ARGS__)
#define CRA_GET_N3(x, ...)   x, CRA_GET_N2(__VA_ARGS__)
#define CRA_GET_N4(x, ...)   x, CRA_GET_N3(__VA_ARGS__)
#define CRA_GET_N5(x, ...)   x, CRA_GET_N4(__VA_ARGS__)
#define CRA_GET_N6(x, ...)   x, CRA_GET_N5(__VA_ARGS__)
#define CRA_GET_N7(x, ...)   x, CRA_GET_N6(__VA_ARGS__)
#define CRA_GET_N8(x, ...)   x, CRA_GET_N7(__VA_ARGS__)
#define CRA_GET_N9(x, ...)   x, CRA_GET_N8(__VA_ARGS__)
#define CRA_GET_N10(x, ...)  x, CRA_GET_N9(__VA_ARGS__)
#define CRA_GET_N11(x, ...)  x, CRA_GET_N10(__VA_ARGS__)
#define CRA_GET_N12(x, ...)  x, CRA_GET_N11(__VA_ARGS__)
#define CRA_GET_N13(x, ...)  x, CRA_GET_N12(__VA_ARGS__)
#define CRA_GET_N14(x, ...)  x, CRA_GET_N13(__VA_ARGS__)
#define CRA_GET_N15(x, ...)  x, CRA_GET_N14(__VA_ARGS__)
#define CRA_GET_N16(x, ...)  x, CRA_GET_N15(__VA_ARGS__)
#define CRA_GET_N17(x, ...)  x, CRA_GET_N16(__VA_ARGS__)
#define CRA_GET_N18(x, ...)  x, CRA_GET_N17(__VA_ARGS__)
#define CRA_GET_N19(x, ...)  x, CRA_GET_N18(__VA_ARGS__)
#define CRA_GET_N20(x, ...)  x, CRA_GET_N19(__VA_ARGS__)
#define CRA_GET_N21(x, ...)  x, CRA_GET_N20(__VA_ARGS__)
#define CRA_GET_N22(x, ...)  x, CRA_GET_N21(__VA_ARGS__)
#define CRA_GET_N23(x, ...)  x, CRA_GET_N22(__VA_ARGS__)
#define CRA_GET_N24(x, ...)  x, CRA_GET_N23(__VA_ARGS__)
#define CRA_GET_N25(x, ...)  x, CRA_GET_N24(__VA_ARGS__)
#define CRA_GET_N26(x, ...)  x, CRA_GET_N25(__VA_ARGS__)
#define CRA_GET_N27(x, ...)  x, CRA_GET_N26(__VA_ARGS__)
#define CRA_GET_N28(x, ...)  x, CRA_GET_N27(__VA_ARGS__)
#define CRA_GET_N29(x, ...)  x, CRA_GET_N28(__VA_ARGS__)
#define CRA_GET_N30(x, ...)  x, CRA_GET_N29(__VA_ARGS__)
#define CRA_GET_N31(x, ...)  x, CRA_GET_N30(__VA_ARGS__)
#define CRA_GET_N32(x, ...)  x, CRA_GET_N31(__VA_ARGS__)
#define CRA_GET_N33(x, ...)  x, CRA_GET_N32(__VA_ARGS__)
#define CRA_GET_N34(x, ...)  x, CRA_GET_N33(__VA_ARGS__)
#define CRA_GET_N35(x, ...)  x, CRA_GET_N34(__VA_ARGS__)
#define CRA_GET_N36(x, ...)  x, CRA_GET_N35(__VA_ARGS__)
#define CRA_GET_N37(x, ...)  x, CRA_GET_N36(__VA_ARGS__)
#define CRA_GET_N38(x, ...)  x, CRA_GET_N37(__VA_ARGS__)
#define CRA_GET_N39(x, ...)  x, CRA_GET_N38(__VA_ARGS__)
#define CRA_GET_N40(x, ...)  x, CRA_GET_N39(__VA_ARGS__)
#define CRA_GET_N41(x, ...)  x, CRA_GET_N40(__VA_ARGS__)
#define CRA_GET_N42(x, ...)  x, CRA_GET_N41(__VA_ARGS__)
#define CRA_GET_N43(x, ...)  x, CRA_GET_N42(__VA_ARGS__)
#define CRA_GET_N44(x, ...)  x, CRA_GET_N43(__VA_ARGS__)
#define CRA_GET_N45(x, ...)  x, CRA_GET_N44(__VA_ARGS__)
#define CRA_GET_N46(x, ...)  x, CRA_GET_N45(__VA_ARGS__)
#define CRA_GET_N47(x, ...)  x, CRA_GET_N46(__VA_ARGS__)
#define CRA_GET_N48(x, ...)  x, CRA_GET_N47(__VA_ARGS__)
#define CRA_GET_N49(x, ...)  x, CRA_GET_N48(__VA_ARGS__)
#define CRA_GET_N50(x, ...)  x, CRA_GET_N49(__VA_ARGS__)
#define CRA_GET_N51(x, ...)  x, CRA_GET_N50(__VA_ARGS__)
#define CRA_GET_N52(x, ...)  x, CRA_GET_N51(__VA_ARGS__)
#define CRA_GET_N53(x, ...)  x, CRA_GET_N52(__VA_ARGS__)
#define CRA_GET_N54(x, ...)  x, CRA_GET_N53(__VA_ARGS__)
#define CRA_GET_N55(x, ...)  x, CRA_GET_N54(__VA_ARGS__)
#define CRA_GET_N56(x, ...)  x, CRA_GET_N55(__VA_ARGS__)
#define CRA_GET_N57(x, ...)  x, CRA_GET_N56(__VA_ARGS__)
#define CRA_GET_N58(x, ...)  x, CRA_GET_N57(__VA_ARGS__)
#define CRA_GET_N59(x, ...)  x, CRA_GET_N58(__VA_ARGS__)
#define CRA_GET_N60(x, ...)  x, CRA_GET_N59(__VA_ARGS__)
#define CRA_GET_N61(x, ...)  x, CRA_GET_N60(__VA_ARGS__)
#define CRA_GET_N62(x, ...)  x, CRA_GET_N61(__VA_ARGS__)
#define CRA_GET_N63(x, ...)  x, CRA_GET_N62(__VA_ARGS__)
#define CRA_GET_N64(x, ...)  x, CRA_GET_N63(__VA_ARGS__)
#define CRA_GET_N65(x, ...)  x, CRA_GET_N64(__VA_ARGS__)
#define CRA_GET_N66(x, ...)  x, CRA_GET_N65(__VA_ARGS__)
#define CRA_GET_N67(x, ...)  x, CRA_GET_N66(__VA_ARGS__)
#define CRA_GET_N68(x, ...)  x, CRA_GET_N67(__VA_ARGS__)
#define CRA_GET_N69(x, ...)  x, CRA_GET_N68(__VA_ARGS__)
#define CRA_GET_N70(x, ...)  x, CRA_GET_N69(__VA_ARGS__)
#define CRA_GET_N71(x, ...)  x, CRA_GET_N70(__VA_ARGS__)
#define CRA_GET_N72(x, ...)  x, CRA_GET_N71(__VA_ARGS__)
#define CRA_GET_N73(x, ...)  x, CRA_GET_N72(__VA_ARGS__)
#define CRA_GET_N74(x, ...)  x, CRA_GET_N73(__VA_ARGS__)
#define CRA_GET_N75(x, ...)  x, CRA_GET_N74(__VA_ARGS__)
#define CRA_GET_N76(x, ...)  x, CRA_GET_N75(__VA_ARGS__)
#define CRA_GET_N77(x, ...)  x, CRA_GET_N76(__VA_ARGS__)
#define CRA_GET_N78(x, ...)  x, CRA_GET_N77(__VA_ARGS__)
#define CRA_GET_N79(x, ...)  x, CRA_GET_N78(__VA_ARGS__)
#define CRA_GET_N80(x, ...)  x, CRA_GET_N79(__VA_ARGS__)
#define CRA_GET_N81(x, ...)  x, CRA_GET_N80(__VA_ARGS__)
#define CRA_GET_N82(x, ...)  x, CRA_GET_N81(__VA_ARGS__)
#define CRA_GET_N83(x, ...)  x, CRA_GET_N82(__VA_ARGS__)
#define CRA_GET_N84(x, ...)  x, CRA_GET_N83(__VA_ARGS__)
#define CRA_GET_N85(x, ...)  x, CRA_GET_N84(__VA_ARGS__)
#define CRA_GET_N86(x, ...)  x, CRA_GET_N85(__VA_ARGS__)
#define CRA_GET_N87(x, ...)  x, CRA_GET_N86(__VA_ARGS__)
#define CRA_GET_N88(x, ...)  x, CRA_GET_N87(__VA_ARGS__)
#define CRA_GET_N89(x, ...)  x, CRA_GET_N88(__VA_ARGS__)
#define CRA_GET_N90(x, ...)  x, CRA_GET_N89(__VA_ARGS__)
#define CRA_GET_N91(x, ...)  x, CRA_GET_N90(__VA_ARGS__)
#define CRA_GET_N92(x, ...)  x, CRA_GET_N91(__VA_ARGS__)
#define CRA_GET_N93(x, ...)  x, CRA_GET_N92(__VA_ARGS__)
#define CRA_GET_N94(x, ...)  x, CRA_GET_N93(__VA_ARGS__)
#define CRA_GET_N95(x, ...)  x, CRA_GET_N94(__VA_ARGS__)
#define CRA_GET_N96(x, ...)  x, CRA_GET_N95(__VA_ARGS__)
#define CRA_GET_N97(x, ...)  x, CRA_GET_N96(__VA_ARGS__)
#define CRA_GET_N98(x, ...)  x, CRA_GET_N97(__VA_ARGS__)
#define CRA_GET_N99(x, ...)  x, CRA_GET_N98(__VA_ARGS__)
#define CRA_GET_N100(x, ...) x, CRA_GET_N99(__VA_ARGS__)
#define CRA_GET_N101(x, ...) x, CRA_GET_N100(__VA_ARGS__)
#define CRA_GET_N102(x, ...) x, CRA_GET_N101(__VA_ARGS__)
#define CRA_GET_N103(x, ...) x, CRA_GET_N102(__VA_ARGS__)
#define CRA_GET_N104(x, ...) x, CRA_GET_N103(__VA_ARGS__)
#define CRA_GET_N105(x, ...) x, CRA_GET_N104(__VA_ARGS__)
#define CRA_GET_N106(x, ...) x, CRA_GET_N105(__VA_ARGS__)
#define CRA_GET_N107(x, ...) x, CRA_GET_N106(__VA_ARGS__)
#define CRA_GET_N108(x, ...) x, CRA_GET_N107(__VA_ARGS__)
#define CRA_GET_N109(x, ...) x, CRA_GET_N108(__VA_ARGS__)
#define CRA_GET_N110(x, ...) x, CRA_GET_N109(__VA_ARGS__)
#define CRA_GET_N111(x, ...) x, CRA_GET_N110(__VA_ARGS__)
#define CRA_GET_N112(x, ...) x, CRA_GET_N111(__VA_ARGS__)
#define CRA_GET_N113(x, ...) x, CRA_GET_N112(__VA_ARGS__)
#define CRA_GET_N114(x, ...) x, CRA_GET_N113(__VA_ARGS__)
#define CRA_GET_N115(x, ...) x, CRA_GET_N114(__VA_ARGS__)
#define CRA_GET_N116(x, ...) x, CRA_GET_N115(__VA_ARGS__)
#define CRA_GET_N117(x, ...) x, CRA_GET_N116(__VA_ARGS__)
#define CRA_GET_N118(x, ...) x, CRA_GET_N117(__VA_ARGS__)
#define CRA_GET_N119(x, ...) x, CRA_GET_N118(__VA_ARGS__)
#define CRA_GET_N120(x, ...) x, CRA_GET_N119(__VA_ARGS__)
#define CRA_GET_N121(x, ...) x, CRA_GET_N120(__VA_ARGS__)
#define CRA_GET_N122(x, ...) x, CRA_GET_N121(__VA_ARGS__)
#define CRA_GET_N123(x, ...) x, CRA_GET_N122(__VA_ARGS__)
#define CRA_GET_N124(x, ...) x, CRA_GET_N123(__VA_ARGS__)
#define CRA_GET_N125(x, ...) x, CRA_GET_N124(__VA_ARGS__)
#define CRA_GET_N126(x, ...) x, CRA_GET_N125(__VA_ARGS__)
#define CRA_GET_N127(x, ...) x, CRA_GET_N126(__VA_ARGS__)
#define CRA_GET_N128(x, ...) x, CRA_GET_N127(__VA_ARGS__)

#endif // end GET_N

#if 1 // INC & DEC

#define CRA_INC0   1
#define CRA_INC1   2
#define CRA_INC2   3
#define CRA_INC3   4
#define CRA_INC4   5
#define CRA_INC5   6
#define CRA_INC6   7
#define CRA_INC7   8
#define CRA_INC8   9
#define CRA_INC9   10
#define CRA_INC10  11
#define CRA_INC11  12
#define CRA_INC12  13
#define CRA_INC13  14
#define CRA_INC14  15
#define CRA_INC15  16
#define CRA_INC16  17
#define CRA_INC17  18
#define CRA_INC18  19
#define CRA_INC19  20
#define CRA_INC20  21
#define CRA_INC21  22
#define CRA_INC22  23
#define CRA_INC23  24
#define CRA_INC24  25
#define CRA_INC25  26
#define CRA_INC26  27
#define CRA_INC27  28
#define CRA_INC28  29
#define CRA_INC29  30
#define CRA_INC30  31
#define CRA_INC31  32
#define CRA_INC32  33
#define CRA_INC33  34
#define CRA_INC34  35
#define CRA_INC35  36
#define CRA_INC36  37
#define CRA_INC37  38
#define CRA_INC38  39
#define CRA_INC39  40
#define CRA_INC40  41
#define CRA_INC41  42
#define CRA_INC42  43
#define CRA_INC43  44
#define CRA_INC44  45
#define CRA_INC45  46
#define CRA_INC46  47
#define CRA_INC47  48
#define CRA_INC48  49
#define CRA_INC49  50
#define CRA_INC50  51
#define CRA_INC51  52
#define CRA_INC52  53
#define CRA_INC53  54
#define CRA_INC54  55
#define CRA_INC55  56
#define CRA_INC56  57
#define CRA_INC57  58
#define CRA_INC58  59
#define CRA_INC59  60
#define CRA_INC60  61
#define CRA_INC61  62
#define CRA_INC62  63
#define CRA_INC63  64
#define CRA_INC64  65
#define CRA_INC65  66
#define CRA_INC66  67
#define CRA_INC67  68
#define CRA_INC68  69
#define CRA_INC69  70
#define CRA_INC70  71
#define CRA_INC71  72
#define CRA_INC72  73
#define CRA_INC73  74
#define CRA_INC74  75
#define CRA_INC75  76
#define CRA_INC76  77
#define CRA_INC77  78
#define CRA_INC78  79
#define CRA_INC79  80
#define CRA_INC80  81
#define CRA_INC81  82
#define CRA_INC82  83
#define CRA_INC83  84
#define CRA_INC84  85
#define CRA_INC85  86
#define CRA_INC86  87
#define CRA_INC87  88
#define CRA_INC88  89
#define CRA_INC89  90
#define CRA_INC90  91
#define CRA_INC91  92
#define CRA_INC92  93
#define CRA_INC93  94
#define CRA_INC94  95
#define CRA_INC95  96
#define CRA_INC96  97
#define CRA_INC97  98
#define CRA_INC98  99
#define CRA_INC99  100
#define CRA_INC100 101
#define CRA_INC101 102
#define CRA_INC102 103
#define CRA_INC103 104
#define CRA_INC104 105
#define CRA_INC105 106
#define CRA_INC106 107
#define CRA_INC107 108
#define CRA_INC108 109
#define CRA_INC109 110
#define CRA_INC110 111
#define CRA_INC111 112
#define CRA_INC112 113
#define CRA_INC113 114
#define CRA_INC114 115
#define CRA_INC115 116
#define CRA_INC116 117
#define CRA_INC117 118
#define CRA_INC118 119
#define CRA_INC119 120
#define CRA_INC120 121
#define CRA_INC121 122
#define CRA_INC122 123
#define CRA_INC123 124
#define CRA_INC124 125
#define CRA_INC125 126
#define CRA_INC126 127
#define CRA_INC127 128

#define CRA_DEC128 127
#define CRA_DEC127 126
#define CRA_DEC126 125
#define CRA_DEC125 124
#define CRA_DEC124 123
#define CRA_DEC123 122
#define CRA_DEC122 121
#define CRA_DEC121 120
#define CRA_DEC120 119
#define CRA_DEC119 118
#define CRA_DEC118 117
#define CRA_DEC117 116
#define CRA_DEC116 115
#define CRA_DEC115 114
#define CRA_DEC114 113
#define CRA_DEC113 112
#define CRA_DEC112 111
#define CRA_DEC111 110
#define CRA_DEC110 109
#define CRA_DEC109 108
#define CRA_DEC108 107
#define CRA_DEC107 106
#define CRA_DEC106 105
#define CRA_DEC105 104
#define CRA_DEC104 103
#define CRA_DEC103 102
#define CRA_DEC102 101
#define CRA_DEC101 100
#define CRA_DEC100 99
#define CRA_DEC99  98
#define CRA_DEC98  97
#define CRA_DEC97  96
#define CRA_DEC96  95
#define CRA_DEC95  94
#define CRA_DEC94  93
#define CRA_DEC93  92
#define CRA_DEC92  91
#define CRA_DEC91  90
#define CRA_DEC90  89
#define CRA_DEC89  88
#define CRA_DEC88  87
#define CRA_DEC87  86
#define CRA_DEC86  85
#define CRA_DEC85  84
#define CRA_DEC84  83
#define CRA_DEC83  82
#define CRA_DEC82  81
#define CRA_DEC81  80
#define CRA_DEC80  79
#define CRA_DEC79  78
#define CRA_DEC78  77
#define CRA_DEC77  76
#define CRA_DEC76  75
#define CRA_DEC75  74
#define CRA_DEC74  73
#define CRA_DEC73  72
#define CRA_DEC72  71
#define CRA_DEC71  70
#define CRA_DEC70  69
#define CRA_DEC69  68
#define CRA_DEC68  67
#define CRA_DEC67  66
#define CRA_DEC66  65
#define CRA_DEC65  64
#define CRA_DEC64  63
#define CRA_DEC63  62
#define CRA_DEC62  61
#define CRA_DEC61  60
#define CRA_DEC60  59
#define CRA_DEC59  58
#define CRA_DEC58  57
#define CRA_DEC57  56
#define CRA_DEC56  55
#define CRA_DEC55  54
#define CRA_DEC54  53
#define CRA_DEC53  52
#define CRA_DEC52  51
#define CRA_DEC51  50
#define CRA_DEC50  49
#define CRA_DEC49  48
#define CRA_DEC48  47
#define CRA_DEC47  46
#define CRA_DEC46  45
#define CRA_DEC45  44
#define CRA_DEC44  43
#define CRA_DEC43  42
#define CRA_DEC42  41
#define CRA_DEC41  40
#define CRA_DEC40  39
#define CRA_DEC39  38
#define CRA_DEC38  37
#define CRA_DEC37  36
#define CRA_DEC36  35
#define CRA_DEC35  34
#define CRA_DEC34  33
#define CRA_DEC33  32
#define CRA_DEC32  31
#define CRA_DEC31  30
#define CRA_DEC30  29
#define CRA_DEC29  28
#define CRA_DEC28  27
#define CRA_DEC27  26
#define CRA_DEC26  25
#define CRA_DEC25  24
#define CRA_DEC24  23
#define CRA_DEC23  22
#define CRA_DEC22  21
#define CRA_DEC21  20
#define CRA_DEC20  19
#define CRA_DEC19  18
#define CRA_DEC18  17
#define CRA_DEC17  16
#define CRA_DEC16  15
#define CRA_DEC15  14
#define CRA_DEC14  13
#define CRA_DEC13  12
#define CRA_DEC12  11
#define CRA_DEC11  10
#define CRA_DEC10  9
#define CRA_DEC9   8
#define CRA_DEC8   7
#define CRA_DEC7   6
#define CRA_DEC6   5
#define CRA_DEC5   4
#define CRA_DEC4   3
#define CRA_DEC3   2
#define CRA_DEC2   1
#define CRA_DEC1   0

#endif // end INC & DEC

#if 1 // REPEAT

#define CRA_REPEAT1(M, p)   M(0, p)
#define CRA_REPEAT2(M, p)   CRA_REPEAT1(M, p) M(1, p)
#define CRA_REPEAT3(M, p)   CRA_REPEAT2(M, p) M(2, p)
#define CRA_REPEAT4(M, p)   CRA_REPEAT3(M, p) M(3, p)
#define CRA_REPEAT5(M, p)   CRA_REPEAT4(M, p) M(4, p)
#define CRA_REPEAT6(M, p)   CRA_REPEAT5(M, p) M(5, p)
#define CRA_REPEAT7(M, p)   CRA_REPEAT6(M, p) M(6, p)
#define CRA_REPEAT8(M, p)   CRA_REPEAT7(M, p) M(7, p)
#define CRA_REPEAT9(M, p)   CRA_REPEAT8(M, p) M(8, p)
#define CRA_REPEAT10(M, p)  CRA_REPEAT9(M, p) M(9, p)
#define CRA_REPEAT11(M, p)  CRA_REPEAT10(M, p) M(10, p)
#define CRA_REPEAT12(M, p)  CRA_REPEAT11(M, p) M(11, p)
#define CRA_REPEAT13(M, p)  CRA_REPEAT12(M, p) M(12, p)
#define CRA_REPEAT14(M, p)  CRA_REPEAT13(M, p) M(13, p)
#define CRA_REPEAT15(M, p)  CRA_REPEAT14(M, p) M(14, p)
#define CRA_REPEAT16(M, p)  CRA_REPEAT15(M, p) M(15, p)
#define CRA_REPEAT17(M, p)  CRA_REPEAT16(M, p) M(16, p)
#define CRA_REPEAT18(M, p)  CRA_REPEAT17(M, p) M(17, p)
#define CRA_REPEAT19(M, p)  CRA_REPEAT18(M, p) M(18, p)
#define CRA_REPEAT20(M, p)  CRA_REPEAT19(M, p) M(19, p)
#define CRA_REPEAT21(M, p)  CRA_REPEAT20(M, p) M(20, p)
#define CRA_REPEAT22(M, p)  CRA_REPEAT21(M, p) M(21, p)
#define CRA_REPEAT23(M, p)  CRA_REPEAT22(M, p) M(22, p)
#define CRA_REPEAT24(M, p)  CRA_REPEAT23(M, p) M(23, p)
#define CRA_REPEAT25(M, p)  CRA_REPEAT24(M, p) M(24, p)
#define CRA_REPEAT26(M, p)  CRA_REPEAT25(M, p) M(25, p)
#define CRA_REPEAT27(M, p)  CRA_REPEAT26(M, p) M(26, p)
#define CRA_REPEAT28(M, p)  CRA_REPEAT27(M, p) M(27, p)
#define CRA_REPEAT29(M, p)  CRA_REPEAT28(M, p) M(28, p)
#define CRA_REPEAT30(M, p)  CRA_REPEAT29(M, p) M(29, p)
#define CRA_REPEAT31(M, p)  CRA_REPEAT30(M, p) M(30, p)
#define CRA_REPEAT32(M, p)  CRA_REPEAT31(M, p) M(31, p)
#define CRA_REPEAT33(M, p)  CRA_REPEAT32(M, p) M(32, p)
#define CRA_REPEAT34(M, p)  CRA_REPEAT33(M, p) M(33, p)
#define CRA_REPEAT35(M, p)  CRA_REPEAT34(M, p) M(34, p)
#define CRA_REPEAT36(M, p)  CRA_REPEAT35(M, p) M(35, p)
#define CRA_REPEAT37(M, p)  CRA_REPEAT36(M, p) M(36, p)
#define CRA_REPEAT38(M, p)  CRA_REPEAT37(M, p) M(37, p)
#define CRA_REPEAT39(M, p)  CRA_REPEAT38(M, p) M(38, p)
#define CRA_REPEAT40(M, p)  CRA_REPEAT39(M, p) M(39, p)
#define CRA_REPEAT41(M, p)  CRA_REPEAT40(M, p) M(40, p)
#define CRA_REPEAT42(M, p)  CRA_REPEAT41(M, p) M(41, p)
#define CRA_REPEAT43(M, p)  CRA_REPEAT42(M, p) M(42, p)
#define CRA_REPEAT44(M, p)  CRA_REPEAT43(M, p) M(43, p)
#define CRA_REPEAT45(M, p)  CRA_REPEAT44(M, p) M(44, p)
#define CRA_REPEAT46(M, p)  CRA_REPEAT45(M, p) M(45, p)
#define CRA_REPEAT47(M, p)  CRA_REPEAT46(M, p) M(46, p)
#define CRA_REPEAT48(M, p)  CRA_REPEAT47(M, p) M(47, p)
#define CRA_REPEAT49(M, p)  CRA_REPEAT48(M, p) M(48, p)
#define CRA_REPEAT50(M, p)  CRA_REPEAT49(M, p) M(49, p)
#define CRA_REPEAT51(M, p)  CRA_REPEAT50(M, p) M(50, p)
#define CRA_REPEAT52(M, p)  CRA_REPEAT51(M, p) M(51, p)
#define CRA_REPEAT53(M, p)  CRA_REPEAT52(M, p) M(52, p)
#define CRA_REPEAT54(M, p)  CRA_REPEAT53(M, p) M(53, p)
#define CRA_REPEAT55(M, p)  CRA_REPEAT54(M, p) M(54, p)
#define CRA_REPEAT56(M, p)  CRA_REPEAT55(M, p) M(55, p)
#define CRA_REPEAT57(M, p)  CRA_REPEAT56(M, p) M(56, p)
#define CRA_REPEAT58(M, p)  CRA_REPEAT57(M, p) M(57, p)
#define CRA_REPEAT59(M, p)  CRA_REPEAT58(M, p) M(58, p)
#define CRA_REPEAT60(M, p)  CRA_REPEAT59(M, p) M(59, p)
#define CRA_REPEAT61(M, p)  CRA_REPEAT60(M, p) M(60, p)
#define CRA_REPEAT62(M, p)  CRA_REPEAT61(M, p) M(61, p)
#define CRA_REPEAT63(M, p)  CRA_REPEAT62(M, p) M(62, p)
#define CRA_REPEAT64(M, p)  CRA_REPEAT63(M, p) M(63, p)
#define CRA_REPEAT65(M, p)  CRA_REPEAT64(M, p) M(64, p)
#define CRA_REPEAT66(M, p)  CRA_REPEAT65(M, p) M(65, p)
#define CRA_REPEAT67(M, p)  CRA_REPEAT66(M, p) M(66, p)
#define CRA_REPEAT68(M, p)  CRA_REPEAT67(M, p) M(67, p)
#define CRA_REPEAT69(M, p)  CRA_REPEAT68(M, p) M(68, p)
#define CRA_REPEAT70(M, p)  CRA_REPEAT69(M, p) M(69, p)
#define CRA_REPEAT71(M, p)  CRA_REPEAT70(M, p) M(70, p)
#define CRA_REPEAT72(M, p)  CRA_REPEAT71(M, p) M(71, p)
#define CRA_REPEAT73(M, p)  CRA_REPEAT72(M, p) M(72, p)
#define CRA_REPEAT74(M, p)  CRA_REPEAT73(M, p) M(73, p)
#define CRA_REPEAT75(M, p)  CRA_REPEAT74(M, p) M(74, p)
#define CRA_REPEAT76(M, p)  CRA_REPEAT75(M, p) M(75, p)
#define CRA_REPEAT77(M, p)  CRA_REPEAT76(M, p) M(76, p)
#define CRA_REPEAT78(M, p)  CRA_REPEAT77(M, p) M(77, p)
#define CRA_REPEAT79(M, p)  CRA_REPEAT78(M, p) M(78, p)
#define CRA_REPEAT80(M, p)  CRA_REPEAT79(M, p) M(79, p)
#define CRA_REPEAT81(M, p)  CRA_REPEAT80(M, p) M(80, p)
#define CRA_REPEAT82(M, p)  CRA_REPEAT81(M, p) M(81, p)
#define CRA_REPEAT83(M, p)  CRA_REPEAT82(M, p) M(82, p)
#define CRA_REPEAT84(M, p)  CRA_REPEAT83(M, p) M(83, p)
#define CRA_REPEAT85(M, p)  CRA_REPEAT84(M, p) M(84, p)
#define CRA_REPEAT86(M, p)  CRA_REPEAT85(M, p) M(85, p)
#define CRA_REPEAT87(M, p)  CRA_REPEAT86(M, p) M(86, p)
#define CRA_REPEAT88(M, p)  CRA_REPEAT87(M, p) M(87, p)
#define CRA_REPEAT89(M, p)  CRA_REPEAT88(M, p) M(88, p)
#define CRA_REPEAT90(M, p)  CRA_REPEAT89(M, p) M(89, p)
#define CRA_REPEAT91(M, p)  CRA_REPEAT90(M, p) M(90, p)
#define CRA_REPEAT92(M, p)  CRA_REPEAT91(M, p) M(91, p)
#define CRA_REPEAT93(M, p)  CRA_REPEAT92(M, p) M(92, p)
#define CRA_REPEAT94(M, p)  CRA_REPEAT93(M, p) M(93, p)
#define CRA_REPEAT95(M, p)  CRA_REPEAT94(M, p) M(94, p)
#define CRA_REPEAT96(M, p)  CRA_REPEAT95(M, p) M(95, p)
#define CRA_REPEAT97(M, p)  CRA_REPEAT96(M, p) M(96, p)
#define CRA_REPEAT98(M, p)  CRA_REPEAT97(M, p) M(97, p)
#define CRA_REPEAT99(M, p)  CRA_REPEAT98(M, p) M(98, p)
#define CRA_REPEAT100(M, p) CRA_REPEAT99(M, p) M(99, p)
#define CRA_REPEAT101(M, p) CRA_REPEAT100(M, p) M(100, p)
#define CRA_REPEAT102(M, p) CRA_REPEAT101(M, p) M(101, p)
#define CRA_REPEAT103(M, p) CRA_REPEAT102(M, p) M(102, p)
#define CRA_REPEAT104(M, p) CRA_REPEAT103(M, p) M(103, p)
#define CRA_REPEAT105(M, p) CRA_REPEAT104(M, p) M(104, p)
#define CRA_REPEAT106(M, p) CRA_REPEAT105(M, p) M(105, p)
#define CRA_REPEAT107(M, p) CRA_REPEAT106(M, p) M(106, p)
#define CRA_REPEAT108(M, p) CRA_REPEAT107(M, p) M(107, p)
#define CRA_REPEAT109(M, p) CRA_REPEAT108(M, p) M(108, p)
#define CRA_REPEAT110(M, p) CRA_REPEAT109(M, p) M(109, p)
#define CRA_REPEAT111(M, p) CRA_REPEAT110(M, p) M(110, p)
#define CRA_REPEAT112(M, p) CRA_REPEAT111(M, p) M(111, p)
#define CRA_REPEAT113(M, p) CRA_REPEAT112(M, p) M(112, p)
#define CRA_REPEAT114(M, p) CRA_REPEAT113(M, p) M(113, p)
#define CRA_REPEAT115(M, p) CRA_REPEAT114(M, p) M(114, p)
#define CRA_REPEAT116(M, p) CRA_REPEAT115(M, p) M(115, p)
#define CRA_REPEAT117(M, p) CRA_REPEAT116(M, p) M(116, p)
#define CRA_REPEAT118(M, p) CRA_REPEAT117(M, p) M(117, p)
#define CRA_REPEAT119(M, p) CRA_REPEAT118(M, p) M(118, p)
#define CRA_REPEAT120(M, p) CRA_REPEAT119(M, p) M(119, p)
#define CRA_REPEAT121(M, p) CRA_REPEAT120(M, p) M(120, p)
#define CRA_REPEAT122(M, p) CRA_REPEAT121(M, p) M(121, p)
#define CRA_REPEAT123(M, p) CRA_REPEAT122(M, p) M(122, p)
#define CRA_REPEAT124(M, p) CRA_REPEAT123(M, p) M(123, p)
#define CRA_REPEAT125(M, p) CRA_REPEAT124(M, p) M(124, p)
#define CRA_REPEAT126(M, p) CRA_REPEAT125(M, p) M(125, p)
#define CRA_REPEAT127(M, p) CRA_REPEAT126(M, p) M(126, p)
#define CRA_REPEAT128(M, p) CRA_REPEAT127(M, p) M(127, p)

#endif // end REPEAT

#if 1 // ITERATE(FOREACH)

#define CRA_ITER1(M, p, ...)      M(p, __VA_ARGS__)
#define CRA_ITER2(M, p, x, ...)   M(p, x) CRA_ITER1(M, p, __VA_ARGS__)
#define CRA_ITER3(M, p, x, ...)   M(p, x) CRA_ITER2(M, p, __VA_ARGS__)
#define CRA_ITER4(M, p, x, ...)   M(p, x) CRA_ITER3(M, p, __VA_ARGS__)
#define CRA_ITER5(M, p, x, ...)   M(p, x) CRA_ITER4(M, p, __VA_ARGS__)
#define CRA_ITER6(M, p, x, ...)   M(p, x) CRA_ITER5(M, p, __VA_ARGS__)
#define CRA_ITER7(M, p, x, ...)   M(p, x) CRA_ITER6(M, p, __VA_ARGS__)
#define CRA_ITER8(M, p, x, ...)   M(p, x) CRA_ITER7(M, p, __VA_ARGS__)
#define CRA_ITER9(M, p, x, ...)   M(p, x) CRA_ITER8(M, p, __VA_ARGS__)
#define CRA_ITER10(M, p, x, ...)  M(p, x) CRA_ITER9(M, p, __VA_ARGS__)
#define CRA_ITER11(M, p, x, ...)  M(p, x) CRA_ITER10(M, p, __VA_ARGS__)
#define CRA_ITER12(M, p, x, ...)  M(p, x) CRA_ITER11(M, p, __VA_ARGS__)
#define CRA_ITER13(M, p, x, ...)  M(p, x) CRA_ITER12(M, p, __VA_ARGS__)
#define CRA_ITER14(M, p, x, ...)  M(p, x) CRA_ITER13(M, p, __VA_ARGS__)
#define CRA_ITER15(M, p, x, ...)  M(p, x) CRA_ITER14(M, p, __VA_ARGS__)
#define CRA_ITER16(M, p, x, ...)  M(p, x) CRA_ITER15(M, p, __VA_ARGS__)
#define CRA_ITER17(M, p, x, ...)  M(p, x) CRA_ITER16(M, p, __VA_ARGS__)
#define CRA_ITER18(M, p, x, ...)  M(p, x) CRA_ITER17(M, p, __VA_ARGS__)
#define CRA_ITER19(M, p, x, ...)  M(p, x) CRA_ITER18(M, p, __VA_ARGS__)
#define CRA_ITER20(M, p, x, ...)  M(p, x) CRA_ITER19(M, p, __VA_ARGS__)
#define CRA_ITER21(M, p, x, ...)  M(p, x) CRA_ITER20(M, p, __VA_ARGS__)
#define CRA_ITER22(M, p, x, ...)  M(p, x) CRA_ITER21(M, p, __VA_ARGS__)
#define CRA_ITER23(M, p, x, ...)  M(p, x) CRA_ITER22(M, p, __VA_ARGS__)
#define CRA_ITER24(M, p, x, ...)  M(p, x) CRA_ITER23(M, p, __VA_ARGS__)
#define CRA_ITER25(M, p, x, ...)  M(p, x) CRA_ITER24(M, p, __VA_ARGS__)
#define CRA_ITER26(M, p, x, ...)  M(p, x) CRA_ITER25(M, p, __VA_ARGS__)
#define CRA_ITER27(M, p, x, ...)  M(p, x) CRA_ITER26(M, p, __VA_ARGS__)
#define CRA_ITER28(M, p, x, ...)  M(p, x) CRA_ITER27(M, p, __VA_ARGS__)
#define CRA_ITER29(M, p, x, ...)  M(p, x) CRA_ITER28(M, p, __VA_ARGS__)
#define CRA_ITER30(M, p, x, ...)  M(p, x) CRA_ITER29(M, p, __VA_ARGS__)
#define CRA_ITER31(M, p, x, ...)  M(p, x) CRA_ITER30(M, p, __VA_ARGS__)
#define CRA_ITER32(M, p, x, ...)  M(p, x) CRA_ITER31(M, p, __VA_ARGS__)
#define CRA_ITER33(M, p, x, ...)  M(p, x) CRA_ITER32(M, p, __VA_ARGS__)
#define CRA_ITER34(M, p, x, ...)  M(p, x) CRA_ITER33(M, p, __VA_ARGS__)
#define CRA_ITER35(M, p, x, ...)  M(p, x) CRA_ITER34(M, p, __VA_ARGS__)
#define CRA_ITER36(M, p, x, ...)  M(p, x) CRA_ITER35(M, p, __VA_ARGS__)
#define CRA_ITER37(M, p, x, ...)  M(p, x) CRA_ITER36(M, p, __VA_ARGS__)
#define CRA_ITER38(M, p, x, ...)  M(p, x) CRA_ITER37(M, p, __VA_ARGS__)
#define CRA_ITER39(M, p, x, ...)  M(p, x) CRA_ITER38(M, p, __VA_ARGS__)
#define CRA_ITER40(M, p, x, ...)  M(p, x) CRA_ITER39(M, p, __VA_ARGS__)
#define CRA_ITER41(M, p, x, ...)  M(p, x) CRA_ITER40(M, p, __VA_ARGS__)
#define CRA_ITER42(M, p, x, ...)  M(p, x) CRA_ITER41(M, p, __VA_ARGS__)
#define CRA_ITER43(M, p, x, ...)  M(p, x) CRA_ITER42(M, p, __VA_ARGS__)
#define CRA_ITER44(M, p, x, ...)  M(p, x) CRA_ITER43(M, p, __VA_ARGS__)
#define CRA_ITER45(M, p, x, ...)  M(p, x) CRA_ITER44(M, p, __VA_ARGS__)
#define CRA_ITER46(M, p, x, ...)  M(p, x) CRA_ITER45(M, p, __VA_ARGS__)
#define CRA_ITER47(M, p, x, ...)  M(p, x) CRA_ITER46(M, p, __VA_ARGS__)
#define CRA_ITER48(M, p, x, ...)  M(p, x) CRA_ITER47(M, p, __VA_ARGS__)
#define CRA_ITER49(M, p, x, ...)  M(p, x) CRA_ITER48(M, p, __VA_ARGS__)
#define CRA_ITER50(M, p, x, ...)  M(p, x) CRA_ITER49(M, p, __VA_ARGS__)
#define CRA_ITER51(M, p, x, ...)  M(p, x) CRA_ITER50(M, p, __VA_ARGS__)
#define CRA_ITER52(M, p, x, ...)  M(p, x) CRA_ITER51(M, p, __VA_ARGS__)
#define CRA_ITER53(M, p, x, ...)  M(p, x) CRA_ITER52(M, p, __VA_ARGS__)
#define CRA_ITER54(M, p, x, ...)  M(p, x) CRA_ITER53(M, p, __VA_ARGS__)
#define CRA_ITER55(M, p, x, ...)  M(p, x) CRA_ITER54(M, p, __VA_ARGS__)
#define CRA_ITER56(M, p, x, ...)  M(p, x) CRA_ITER55(M, p, __VA_ARGS__)
#define CRA_ITER57(M, p, x, ...)  M(p, x) CRA_ITER56(M, p, __VA_ARGS__)
#define CRA_ITER58(M, p, x, ...)  M(p, x) CRA_ITER57(M, p, __VA_ARGS__)
#define CRA_ITER59(M, p, x, ...)  M(p, x) CRA_ITER58(M, p, __VA_ARGS__)
#define CRA_ITER60(M, p, x, ...)  M(p, x) CRA_ITER59(M, p, __VA_ARGS__)
#define CRA_ITER61(M, p, x, ...)  M(p, x) CRA_ITER60(M, p, __VA_ARGS__)
#define CRA_ITER62(M, p, x, ...)  M(p, x) CRA_ITER61(M, p, __VA_ARGS__)
#define CRA_ITER63(M, p, x, ...)  M(p, x) CRA_ITER62(M, p, __VA_ARGS__)
#define CRA_ITER64(M, p, x, ...)  M(p, x) CRA_ITER63(M, p, __VA_ARGS__)
#define CRA_ITER65(M, p, x, ...)  M(p, x) CRA_ITER64(M, p, __VA_ARGS__)
#define CRA_ITER66(M, p, x, ...)  M(p, x) CRA_ITER65(M, p, __VA_ARGS__)
#define CRA_ITER67(M, p, x, ...)  M(p, x) CRA_ITER66(M, p, __VA_ARGS__)
#define CRA_ITER68(M, p, x, ...)  M(p, x) CRA_ITER67(M, p, __VA_ARGS__)
#define CRA_ITER69(M, p, x, ...)  M(p, x) CRA_ITER68(M, p, __VA_ARGS__)
#define CRA_ITER70(M, p, x, ...)  M(p, x) CRA_ITER69(M, p, __VA_ARGS__)
#define CRA_ITER71(M, p, x, ...)  M(p, x) CRA_ITER70(M, p, __VA_ARGS__)
#define CRA_ITER72(M, p, x, ...)  M(p, x) CRA_ITER71(M, p, __VA_ARGS__)
#define CRA_ITER73(M, p, x, ...)  M(p, x) CRA_ITER72(M, p, __VA_ARGS__)
#define CRA_ITER74(M, p, x, ...)  M(p, x) CRA_ITER73(M, p, __VA_ARGS__)
#define CRA_ITER75(M, p, x, ...)  M(p, x) CRA_ITER74(M, p, __VA_ARGS__)
#define CRA_ITER76(M, p, x, ...)  M(p, x) CRA_ITER75(M, p, __VA_ARGS__)
#define CRA_ITER77(M, p, x, ...)  M(p, x) CRA_ITER76(M, p, __VA_ARGS__)
#define CRA_ITER78(M, p, x, ...)  M(p, x) CRA_ITER77(M, p, __VA_ARGS__)
#define CRA_ITER79(M, p, x, ...)  M(p, x) CRA_ITER78(M, p, __VA_ARGS__)
#define CRA_ITER80(M, p, x, ...)  M(p, x) CRA_ITER79(M, p, __VA_ARGS__)
#define CRA_ITER81(M, p, x, ...)  M(p, x) CRA_ITER80(M, p, __VA_ARGS__)
#define CRA_ITER82(M, p, x, ...)  M(p, x) CRA_ITER81(M, p, __VA_ARGS__)
#define CRA_ITER83(M, p, x, ...)  M(p, x) CRA_ITER82(M, p, __VA_ARGS__)
#define CRA_ITER84(M, p, x, ...)  M(p, x) CRA_ITER83(M, p, __VA_ARGS__)
#define CRA_ITER85(M, p, x, ...)  M(p, x) CRA_ITER84(M, p, __VA_ARGS__)
#define CRA_ITER86(M, p, x, ...)  M(p, x) CRA_ITER85(M, p, __VA_ARGS__)
#define CRA_ITER87(M, p, x, ...)  M(p, x) CRA_ITER86(M, p, __VA_ARGS__)
#define CRA_ITER88(M, p, x, ...)  M(p, x) CRA_ITER87(M, p, __VA_ARGS__)
#define CRA_ITER89(M, p, x, ...)  M(p, x) CRA_ITER88(M, p, __VA_ARGS__)
#define CRA_ITER90(M, p, x, ...)  M(p, x) CRA_ITER89(M, p, __VA_ARGS__)
#define CRA_ITER91(M, p, x, ...)  M(p, x) CRA_ITER90(M, p, __VA_ARGS__)
#define CRA_ITER92(M, p, x, ...)  M(p, x) CRA_ITER91(M, p, __VA_ARGS__)
#define CRA_ITER93(M, p, x, ...)  M(p, x) CRA_ITER92(M, p, __VA_ARGS__)
#define CRA_ITER94(M, p, x, ...)  M(p, x) CRA_ITER93(M, p, __VA_ARGS__)
#define CRA_ITER95(M, p, x, ...)  M(p, x) CRA_ITER94(M, p, __VA_ARGS__)
#define CRA_ITER96(M, p, x, ...)  M(p, x) CRA_ITER95(M, p, __VA_ARGS__)
#define CRA_ITER97(M, p, x, ...)  M(p, x) CRA_ITER96(M, p, __VA_ARGS__)
#define CRA_ITER98(M, p, x, ...)  M(p, x) CRA_ITER97(M, p, __VA_ARGS__)
#define CRA_ITER99(M, p, x, ...)  M(p, x) CRA_ITER98(M, p, __VA_ARGS__)
#define CRA_ITER100(M, p, x, ...) M(p, x) CRA_ITER99(M, p, __VA_ARGS__)
#define CRA_ITER101(M, p, x, ...) M(p, x) CRA_ITER100(M, p, __VA_ARGS__)
#define CRA_ITER102(M, p, x, ...) M(p, x) CRA_ITER101(M, p, __VA_ARGS__)
#define CRA_ITER103(M, p, x, ...) M(p, x) CRA_ITER102(M, p, __VA_ARGS__)
#define CRA_ITER104(M, p, x, ...) M(p, x) CRA_ITER103(M, p, __VA_ARGS__)
#define CRA_ITER105(M, p, x, ...) M(p, x) CRA_ITER104(M, p, __VA_ARGS__)
#define CRA_ITER106(M, p, x, ...) M(p, x) CRA_ITER105(M, p, __VA_ARGS__)
#define CRA_ITER107(M, p, x, ...) M(p, x) CRA_ITER106(M, p, __VA_ARGS__)
#define CRA_ITER108(M, p, x, ...) M(p, x) CRA_ITER107(M, p, __VA_ARGS__)
#define CRA_ITER109(M, p, x, ...) M(p, x) CRA_ITER108(M, p, __VA_ARGS__)
#define CRA_ITER110(M, p, x, ...) M(p, x) CRA_ITER109(M, p, __VA_ARGS__)
#define CRA_ITER111(M, p, x, ...) M(p, x) CRA_ITER110(M, p, __VA_ARGS__)
#define CRA_ITER112(M, p, x, ...) M(p, x) CRA_ITER111(M, p, __VA_ARGS__)
#define CRA_ITER113(M, p, x, ...) M(p, x) CRA_ITER112(M, p, __VA_ARGS__)
#define CRA_ITER114(M, p, x, ...) M(p, x) CRA_ITER113(M, p, __VA_ARGS__)
#define CRA_ITER115(M, p, x, ...) M(p, x) CRA_ITER114(M, p, __VA_ARGS__)
#define CRA_ITER116(M, p, x, ...) M(p, x) CRA_ITER115(M, p, __VA_ARGS__)
#define CRA_ITER117(M, p, x, ...) M(p, x) CRA_ITER116(M, p, __VA_ARGS__)
#define CRA_ITER118(M, p, x, ...) M(p, x) CRA_ITER117(M, p, __VA_ARGS__)
#define CRA_ITER119(M, p, x, ...) M(p, x) CRA_ITER118(M, p, __VA_ARGS__)
#define CRA_ITER120(M, p, x, ...) M(p, x) CRA_ITER119(M, p, __VA_ARGS__)
#define CRA_ITER121(M, p, x, ...) M(p, x) CRA_ITER120(M, p, __VA_ARGS__)
#define CRA_ITER122(M, p, x, ...) M(p, x) CRA_ITER121(M, p, __VA_ARGS__)
#define CRA_ITER123(M, p, x, ...) M(p, x) CRA_ITER122(M, p, __VA_ARGS__)
#define CRA_ITER124(M, p, x, ...) M(p, x) CRA_ITER123(M, p, __VA_ARGS__)
#define CRA_ITER125(M, p, x, ...) M(p, x) CRA_ITER124(M, p, __VA_ARGS__)
#define CRA_ITER126(M, p, x, ...) M(p, x) CRA_ITER125(M, p, __VA_ARGS__)
#define CRA_ITER127(M, p, x, ...) M(p, x) CRA_ITER126(M, p, __VA_ARGS__)
#define CRA_ITER128(M, p, x, ...) M(p, x) CRA_ITER127(M, p, __VA_ARGS__)

#endif // end ITERATE(FOREACH)

#if 1 // CRA_IIF

#define CRA_IIF0(t, f) f
#define CRA_IIF1(t, f) t

#endif // end CRA_IIF

#if 1 // CRA_AUTO_REC

#define CRA_AUTO_REC1(p) 1
#define CRA_AUTO_REC2(p) CRA_IIF(p(1))(1, 2)
#define CRA_AUTO_REC4(p) CRA_IIF(p(2))(CRA_IIF(p(1))(1, 2), CRA_IIF(p(3))(3, 4))

#endif // end CRA_AUTO_REC

#if 1 // CRA_APPLY

#define CRA_APPLY1(a, b) a b
#define CRA_APPLY2(a, b) a b
#define CRA_APPLY3(a, b) a b
#define CRA_APPLY4(a, b) a b

#define CRA_APPLY_P_MARK            1
#define CRA_APPLY_P_CRA_APPLY1(...) 0
#define CRA_APPLY_P_CRA_APPLY2(...) 0
#define CRA_APPLY_P_CRA_APPLY3(...) 0
#define CRA_APPLY_P_CRA_APPLY4(...) 0

#define CRA_APPLY_P(n) CRA_CAT(CRA_APPLY_P_, CRA_APPLY##n(MARK, ))

#endif // end CRA_APPLY

// static_assert(CRA_CNT() == 0, "CRA_CNT() failed for 0 arguments"); // not supported
static_assert(CRA_CNT(a) == 1, "CRA_CNT() failed for 1 argument");
static_assert(CRA_CNT(a, b) == 2, "CRA_CNT() failed for 2 arguments");
static_assert(CRA_CNT(CRA_128_NUM) == 128, "CRA_CNT() failed for 128 arguments");

#endif
