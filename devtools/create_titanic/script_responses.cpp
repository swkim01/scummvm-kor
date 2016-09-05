/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

 // Disable symbol overrides so that we can use system headers.
#define FORBIDDEN_SYMBOL_ALLOW_ALL

// HACK to allow building with the SDL backend on MinGW
// see bug #1800764 "TOOLS: MinGW tools building broken"
#ifdef main
#undef main
#endif // main

#include "file.h"
#include "script_responses.h"

static const int DESKBOT_RESPONSES[128][5] = {
	{ MKTAG('P', 'K', 'U', 'P'), 240573, 0, 0, 0 },
	{ MKTAG('S', 'E', 'X', '1'), 240573, 0, 0, 0 },
	{ MKTAG('D', 'E', 'S', '1'), 240354, 0, 0, 0 },
	{ MKTAG('D', 'E', 'S', '2'), 240547, 0, 0, 0 },
	{ MKTAG('S', 'W', 'E', 'R'), 240578, 0, 0, 0 },
	{ MKTAG('B', 'Y', 'Z', 'A'), 241173, 0, 0, 0 },
	{ MKTAG('S', 'A', 'S', 'S'), 240986, 0, 0, 0 },
	{ MKTAG('S', 'U', 'M', 'S'), 240453, 0, 0, 0 },
	{ MKTAG('F', 'O', 'O', 'D'), 240849, 0, 0, 0 },
	{ MKTAG('J', 'F', 'O', 'D'), 240849, 0, 0, 0 },
	{ MKTAG('C', 'H', 'S', 'E'), 240849, 0, 0, 0 },
	{ MKTAG('A', 'C', 'T', 'R'), 240654, 0, 0, 0 },
	{ MKTAG('A', 'C', 'T', 'S'), 240655, 0, 0, 0 },
	{ MKTAG('M', 'U', 'S', 'I'), 240681, 241621, 0, 0 },
	{ MKTAG('S', 'O', 'N', 'G'), 240681, 241621, 0, 0 },
	{ MKTAG('S', 'O', 'A', 'P'), 240681, 0, 0, 0 },
	{ MKTAG('T', 'V', 'S', 'H'), 240681, 0, 0, 0 },
	{ MKTAG('A', 'R', 'T', 'I'), 240657, 0, 0, 0 },
	{ MKTAG('A', 'U', 'T', 'H'), 240657, 0, 0, 0 },
	{ MKTAG('C', 'O', 'M', 'D'), 240657, 240785, 0, 0 },
	{ MKTAG('C', 'O', 'O', 'K'), 240657, 0, 0, 0 },
	{ MKTAG('C', 'O', 'P', 'S'), 240657, 0, 0, 0 },
	{ MKTAG('H', 'E', 'R', 'O'), 240657, 0, 0, 0 },
	{ MKTAG('H', 'O', 'S', 'T'), 240657, 0, 0, 0 },
	{ MKTAG('P', 'T', 'I', 'C'), 240657, 0, 0, 0 },
	{ MKTAG('S', 'C', 'I', 'T'), 240657, 0, 0, 0 },
	{ MKTAG('E', 'A', 'R', 'T'), 240728, 0, 0, 0 },
	{ MKTAG('P', 'L', 'A', 'N'), 240728, 0, 0, 0 },
	{ MKTAG('F', 'I', 'L', 'M'), 240939, 0, 0, 0 },
	{ MKTAG('F', 'I', 'S', 'H'), 240437, 0, 0, 0 },
	{ MKTAG('H', 'H', 'G', 'Q'), 241065, 240453, 0, 0 },
	{ MKTAG('L', 'I', 'Q', 'D'), 241167, 0, 0, 0 },
	{ MKTAG('P', 'H', 'I', 'L'), 240607, 0, 0, 0 },
	{ MKTAG('S', 'I', 'C', 'K'), 241170, 0, 0, 0 },
	{ MKTAG('T', 'W', 'A', 'T'), 240975, 0, 0, 0 },
	{ MKTAG('H', 'A', 'H', 'A'), 240785, 0, 0, 0 },
	{ MKTAG('S', 'C', 'I', 'T'), 240968, 241617, 0, 0 },
	{ MKTAG('S', 'C', 'I', 'E'), 240967, 241616, 0, 0 },
	{ MKTAG('S', 'L', 'O', 'W'), 241614, 0, 0, 0 },
	{ MKTAG('T', 'H', 'R', 'T'), 240760, 241615, 0, 0 },
	{ MKTAG('T', 'D', 'V', 'P'), 241161, 241618, 0, 0 },
	{ MKTAG('T', 'I', 'T', 'A'), 241619, 0, 0, 0 },
	{ MKTAG('C', 'S', 'P', 'Y'), 241620, 0, 0, 0 },
	{ MKTAG('M', 'I', 'N', 'S'), 241621, 0, 0, 0 },
	{ MKTAG('M', 'C', 'P', 'Y'), 241622, 0, 0, 0 },
	{ MKTAG('D', 'N', 'C', 'E'), 241623, 0, 0, 0 },
	{ MKTAG('N', 'A', 'U', 'T'), 241624, 0, 0, 0 },
	{ MKTAG('A', 'D', 'V', 'T'), 240939, 241622, 0, 0 },
	{ MKTAG('A', 'N', 'S', 'W'), 240453, 0, 0, 0 },
	{ MKTAG('A', 'R', 'T', 'Y'), 240658, 0, 0, 0 },
	{ MKTAG('B', 'A', 'R', '1'), 240491, 0, 0, 0 },
	{ MKTAG('B', 'A', 'R', '3'), 240610, 0, 0, 0 },
	{ MKTAG('B', 'A', 'R', 'K'), 240768, 0, 0, 0 },
	{ MKTAG('B', 'A', 'R', 'U'), 240768, 0, 0, 0 },
	{ MKTAG('B', 'E', 'L', '1'), 240940, 0, 0, 0 },
	{ MKTAG('B', 'E', 'L', '2'), 240591, 0, 0, 0 },
	{ MKTAG('B', 'E', 'L', '3'), 240775, 0, 0, 0 },
	{ MKTAG('B', 'E', 'L', '4'), 240558, 0, 0, 0 },
	{ MKTAG('B', 'E', 'L', '5'), 240336, 0, 0, 0 },
	{ MKTAG('B', 'E', 'L', '6'), 240759, 240760, 0, 0 },
	{ MKTAG('B', 'E', 'L', '7'), 240726, 0, 0, 0 },
	{ MKTAG('B', 'L', 'F', '1'), 241652, 0, 0, 0 },
	{ MKTAG('B', 'L', 'F', '2'), 240939, 0, 0, 0 },
	{ MKTAG('B', 'L', 'P', '1'), 240654, 0, 0, 0 },
	{ MKTAG('B', 'L', 'P', '2'), 240654, 0, 0, 0 },
	{ MKTAG('B', 'L', 'P', '3'), 240654, 0, 0, 0 },
	{ MKTAG('B', 'L', 'P', '4'), 240655, 0, 0, 0 },
	{ MKTAG('B', 'L', 'R', '1'), 240654, 0, 0, 0 },
	{ MKTAG('B', 'L', 'R', '2'), 240655, 0, 0, 0 },
	{ MKTAG('B', 'L', 'T', '1'), 240718, 0, 0, 0 },
	{ MKTAG('B', 'L', 'T', '2'), 240681, 0, 0, 0 },
	{ MKTAG('B', 'L', 'T', '3'), 240655, 0, 0, 0 },
	{ MKTAG('B', 'L', 'T', '4'), 240664, 0, 0, 0 },
	{ MKTAG('B', 'L', 'T', '5'), 240681, 0, 0, 0 },
	{ MKTAG('B', 'O', 'D', 'Y'), 240596, 0, 0, 0 },
	{ MKTAG('B', 'O', 'Y', 'S'), 240654, 0, 0, 0 },
	{ MKTAG('B', 'R', 'N', 'D'), 240939, 241622, 0, 0 },
	{ MKTAG('C', 'L', 'U', 'B'), 241675, 240681, 241623, 0 },
	{ MKTAG('C', 'M', 'N', 'T'), 240849, 0, 0, 0 },
	{ MKTAG('C', 'R', 'I', 'M'), 241096, 240725, 240729, 0 },
	{ MKTAG('D', 'C', 'T', 'R'), 240681, 0, 0, 0 },
	{ MKTAG('D', 'O', 'R', '2'), 241405, 241404, 241403, 241402 },
	{ MKTAG('D', 'R', 'U', 'G'), 240922, 240931, 0, 0 },
	{ MKTAG('F', 'A', 'M', 'E'), 240726, 0, 0, 0 },
	{ MKTAG('F', 'A', 'S', 'H'), 241172, 0, 0, 0 },
	{ MKTAG('F', 'A', 'U', 'N'), 240939, 0, 0, 0 },
	{ MKTAG('F', 'L', 'O', 'R'), 240825, 0, 0, 0 },
	{ MKTAG('F', 'U', 'L', 'N'), 240864, 241072, 0, 0 },
	{ MKTAG('G', 'I', 'R', 'L'), 240655, 0, 0, 0 },
	{ MKTAG('H', 'B', 'B', 'Y'), 241144, 0, 0, 0 },
	{ MKTAG('H', 'H', 'L', 'D'), 241144, 0, 0, 0 },
	{ MKTAG('H', 'O', 'M', 'E'), 240844, 240626, 0, 0 },
	{ MKTAG('I', 'S', 'H', 'E'), 240731, 0, 0, 0 },
	{ MKTAG('J', 'N', 'A', 'M'), 240785, 0, 0, 0 },
	{ MKTAG('J', 'O', 'K', 'E'), 240785, 0, 0, 0 },
	{ MKTAG('K', 'N', 'O', 'B'), 240657, 0, 0, 0 },
	{ MKTAG('K', 'P', 'L', 'C'), 240844, 240626, 0, 0 },
	{ MKTAG('L', 'I', 'F', '3'), 240722, 0, 0, 0 },
	{ MKTAG('L', 'I', 'T', 'E'), 240785, 0, 0, 0 },
	{ MKTAG('L', 'I', 'T', 'R'), 241404, 241405, 241403, 241406 },
	{ MKTAG('M', 'A', 'D', '1'), 241124, 240971, 241615, 0 },
	{ MKTAG('M', 'A', 'D', '4'), 241341, 0, 0, 0 },
	{ MKTAG('M', 'A', 'D', '6'), 240860, 241114, 0, 0 },
	{ MKTAG('M', 'A', 'G', 'S'), 241404, 241405, 241403, 241407 },
	{ MKTAG('M', 'L', 'T', 'Y'), 240718, 240719, 0, 0 },
	{ MKTAG('N', 'I', 'K', 'E'), 241622, 0, 0, 0 },
	{ MKTAG('N', 'I', 'K', 'N'), 240785, 0, 0, 0 },
	{ MKTAG('N', 'P', 'L', 'C'), 240844, 240626, 0, 0 },
	{ MKTAG('O', 'R', 'D', '1'), 240695, 0, 0, 0 },
	{ MKTAG('O', 'R', 'D', '2'), 240744, 240650, 0, 0 },
	{ MKTAG('O', 'R', 'D', '3'), 240647, 0, 0, 0 },
	{ MKTAG('O', 'R', 'D', '4'), 240647, 0, 0, 0 },
	{ MKTAG('O', 'R', 'D', '5'), 241191, 0, 0, 0 },
	{ MKTAG('P', 'G', 'R', 'P'), 240681, 0, 0, 0 },
	{ MKTAG('P', 'L', 'A', 'C'), 240728, 0, 0, 0 },
	{ MKTAG('R', 'C', 'K', 'T'), 241070, 241161, 0, 0 },
	{ MKTAG('S', 'F', 'S', 'F'), 241172, 0, 0, 0 },
	{ MKTAG('S', 'P', 'R', 'T'), 241172, 0, 0, 0 },
	{ MKTAG('S', 'U', 'C', '1'), 240467, 0, 0, 0 },
	{ MKTAG('T', 'E', 'A', 'M'), 241172, 0, 0, 0 },
	{ MKTAG('T', 'L', 'A', ' '), 240727, 240658, 0, 0 },
	{ MKTAG('T', 'O', 'Y', 'S'), 240607, 240606, 0, 0 },
	{ MKTAG('T', 'R', 'A', '2'), 240611, 0, 0, 0 },
	{ MKTAG('T', 'R', 'A', '3'), 240611, 0, 0, 0 },
	{ MKTAG('W', 'E', 'A', 'P'), 240939, 0, 0, 0 },
	{ MKTAG('W', 'E', 'A', 'T'), 241093, 241094, 241095, 0 },
	{ MKTAG('W', 'T', 'H', 'R'), 241093, 241094, 241095, 0 },
	{ MKTAG('W', 'W', 'E', 'B'), 241172, 0, 0, 0 }
};

static const int BELLBOT_RESPONSES[130][5] = {
	{ MKTAG('A', 'C', 'T', 'R'), 200505, 0, 0, 0 },
	{ MKTAG('A', 'C', 'T', 'S'), 200505, 0, 0, 0 },
	{ MKTAG('F', 'A', 'M', 'E'), 200532, 200585, 0, 0 },
	{ MKTAG('A', 'D', 'V', 'T'), 200506, 0, 0, 0 },
	{ MKTAG('L', 'I', 'Q', 'D'), 200507, 200527, 0, 0 },
	{ MKTAG('F', 'A', 'U', 'N'), 200511, 0, 0, 0 },
	{ MKTAG('H', 'B', 'B', 'Y'), 200514, 0, 0, 0 },
	{ MKTAG('N', 'I', 'K', 'E'), 200514, 0, 0, 0 },
	{ MKTAG('B', 'R', 'N', 'D'), 200514, 0, 0, 0 },
	{ MKTAG('P', 'G', 'R', 'P'), 200514, 0, 0, 0 },
	{ MKTAG('R', 'C', 'K', 'T'), 200514, 0, 0, 0 },
	{ MKTAG('D', 'R', 'U', 'G'), 200528, 0, 0, 0 },
	{ MKTAG('F', 'A', 'S', 'H'), 200533, 0, 0, 0 },
	{ MKTAG('T', 'O', 'Y', 'S'), 200533, 0, 0, 0 },
	{ MKTAG('F', 'O', 'O', 'D'), 202270, 0, 0, 0 },
	{ MKTAG('J', 'F', 'O', 'D'), 202270, 0, 0, 0 },
	{ MKTAG('A', 'R', 'T', 'I'), 200538, 0, 0, 0 },
	{ MKTAG('A', 'R', 'T', 'Y'), 200538, 0, 0, 0 },
	{ MKTAG('L', 'I', 'T', 'R'), 200538, 0, 0, 0 },
	{ MKTAG('C', 'R', 'I', 'M'), 200538, 0, 0, 0 },
	{ MKTAG('C', 'S', 'P', 'Y'), 200538, 0, 0, 0 },
	{ MKTAG('W', 'E', 'A', 'T'), 200538, 0, 0, 0 },
	{ MKTAG('M', 'U', 'S', 'I'), 200539, 0, 0, 0 },
	{ MKTAG('S', 'O', 'N', 'G'), 200539, 0, 0, 0 },
	{ MKTAG('F', 'I', 'L', 'M'), 200534, 0, 0, 0 },
	{ MKTAG('B', 'L', 'F', '1'), 200535, 0, 0, 0 },
	{ MKTAG('B', 'L', 'F', '2'), 200536, 0, 0, 0 },
	{ MKTAG('M', 'A', 'G', 'S'), 200542, 0, 0, 0 },
	{ MKTAG('P', 'H', 'I', 'L'), 200557, 0, 0, 0 },
	{ MKTAG('P', 'L', 'A', 'N'), 200562, 0, 0, 0 },
	{ MKTAG('E', 'A', 'R', 'T'), 200562, 202252, 0, 0 },
	{ MKTAG('P', 'L', 'A', 'C'), 200562, 202252, 0, 0 },
	{ MKTAG('F', 'L', 'O', 'R'), 200570, 0, 0, 0 },
	{ MKTAG('P', 'T', 'I', 'C'), 200571, 0, 0, 0 },
	{ MKTAG('B', 'L', 'P', '1'), 200577, 0, 0, 0 },
	{ MKTAG('B', 'L', 'P', '2'), 200575, 0, 0, 0 },
	{ MKTAG('B', 'L', 'P', '3'), 200576, 0, 0, 0 },
	{ MKTAG('B', 'L', 'P', '4'), 200578, 0, 0, 0 },
	{ MKTAG('K', 'N', 'O', 'B'), 200579, 0, 0, 0 },
	{ MKTAG('B', 'L', 'R', '1'), 200580, 0, 0, 0 },
	{ MKTAG('B', 'L', 'R', '2'), 200581, 0, 0, 0 },
	{ MKTAG('S', 'E', 'X', '1'), 200582, 0, 0, 0 },
	{ MKTAG('S', 'P', 'R', 'T'), 200584, 0, 0, 0 },
	{ MKTAG('T', 'E', 'A', 'M'), 200584, 0, 0, 0 },
	{ MKTAG('H', 'E', 'R', 'O'), 200585, 0, 0, 0 },
	{ MKTAG('T', 'W', 'A', 'T'), 200588, 0, 0, 0 },
	{ MKTAG('S', 'W', 'E', 'R'), 200590, 200336, 0, 0 },
	{ MKTAG('T', 'R', 'A', '2'), 200594, 0, 0, 0 },
	{ MKTAG('T', 'R', 'A', '3'), 200594, 0, 0, 0 },
	{ MKTAG('T', 'V', 'S', 'H'), 200595, 0, 0, 0 },
	{ MKTAG('S', 'O', 'A', 'P'), 200595, 0, 0, 0 },
	{ MKTAG('C', 'O', 'M', 'D'), 200595, 0, 0, 0 },
	{ MKTAG('C', 'O', 'O', 'K'), 200595, 0, 0, 0 },
	{ MKTAG('C', 'O', 'P', 'S'), 200595, 0, 0, 0 },
	{ MKTAG('D', 'C', 'T', 'R'), 200595, 0, 0, 0 },
	{ MKTAG('S', 'F', 'S', 'F'), 200595, 0, 0, 0 },
	{ MKTAG('H', 'O', 'S', 'T'), 200595, 0, 0, 0 },
	{ MKTAG('B', 'L', 'T', '1'), 200596, 0, 0, 0 },
	{ MKTAG('B', 'L', 'T', '2'), 200597, 0, 0, 0 },
	{ MKTAG('B', 'L', 'T', '3'), 200598, 0, 0, 0 },
	{ MKTAG('B', 'L', 'T', '4'), 200599, 0, 0, 0 },
	{ MKTAG('B', 'L', 'T', '5'), 200600, 0, 0, 0 },
	{ MKTAG('W', 'E', 'A', 'P'), 200601, 0, 0, 0 },
	{ MKTAG('A', 'U', 'T', 'H'), 200605, 0, 0, 0 },
	{ MKTAG('H', 'H', 'L', 'D'), 200536, 0, 0, 0 },
	{ MKTAG('W', 'W', 'E', 'B'), 200608, 0, 0, 0 },
	{ MKTAG('M', 'L', 'T', 'Y'), 200608, 0, 0, 0 },
	{ MKTAG('P', 'K', 'U', 'P'), 200067, 0, 0, 0 },
	{ MKTAG('S', 'U', 'C', '1'), 200067, 0, 0, 0 },
	{ MKTAG('B', 'E', 'L', '1'), 200684, 0, 0, 0 },
	{ MKTAG('B', 'E', 'L', '2'), 200887, 0, 0, 0 },
	{ MKTAG('B', 'E', 'L', '3'), 200610, 0, 0, 0 },
	{ MKTAG('B', 'E', 'L', '4'), 200015, 0, 0, 0 },
	{ MKTAG('B', 'E', 'L', '5'), 200043, 0, 0, 0 },
	{ MKTAG('B', 'E', 'L', '6'), 200333, 0, 0, 0 },
	{ MKTAG('B', 'E', 'L', '7'), 200884, 0, 0, 0 },
	{ MKTAG('H', 'H', 'G', 'Q'), 200516, 0, 0, 0 },
	{ MKTAG('C', 'H', 'A', 'N'), 200961, 0, 0, 0 },
	{ MKTAG('B', 'Y', 'Z', 'A'), 201271, 0, 0, 0 },
	{ MKTAG('F', 'I', 'S', 'H'), 201242, 0, 0, 0 },
	{ MKTAG('S', 'A', 'S', 'S'), 201256, 0, 0, 0 },
	{ MKTAG('S', 'I', 'C', 'K'), 201704, 0, 0, 0 },
	{ MKTAG('N', 'H', 'R', 'O'), 201704, 0, 0, 0 },
	{ MKTAG('S', 'C', 'I', 'E'), 201675, 0, 0, 0 },
	{ MKTAG('S', 'C', 'I', 'T'), 201676, 0, 0, 0 },
	{ MKTAG('T', 'D', 'V', 'P'), 200490, 0, 0, 0 },
	{ MKTAG('H', 'A', 'H', 'A'), 200950, 0, 0, 0 },
	{ MKTAG('S', 'U', 'C', '1'), 200582, 0, 0, 0 },
	{ MKTAG('T', 'L', 'A', ' '), 201243, 0, 0, 0 },
	{ MKTAG('J', 'O', 'K', 'E'), 201244, 0, 0, 0 },
	{ MKTAG('C', 'H', 'S', 'E'), 202270, 0, 0, 0 },
	{ MKTAG('C', 'L', 'U', 'B'), 201654, 0, 0, 0 },
	{ MKTAG('S', 'L', 'O', 'W'), 201877, 0, 0, 0 },
	{ MKTAG('T', 'H', 'R', 'T'), 201238, 201269, 201982, 200336 },
	{ MKTAG('A', 'N', 'S', 'W'), 200139, 0, 0, 0 },
	{ MKTAG('B', 'A', 'R', '1'), 202051, 200473, 200229, 0 },
	{ MKTAG('B', 'A', 'R', 'K'), 200510, 201326, 0, 0 },
	{ MKTAG('B', 'A', 'R', 'U'), 200510, 201326, 0, 0 },
	{ MKTAG('B', 'O', 'D', 'Y'), 201704, 0, 0, 0 },
	{ MKTAG('C', 'M', 'N', 'T'), 202270, 0, 0, 0 },
	{ MKTAG('D', 'E', 'S', '2'), 201529, 0, 0, 0 },
	{ MKTAG('D', 'N', 'C', 'E'), 200952, 200953, 200960, 0 },
	{ MKTAG('D', 'O', 'R', '2'), 200372, 0, 0, 0 },
	{ MKTAG('F', 'U', 'L', 'N'), 202035, 202044, 0, 0 },
	{ MKTAG('H', 'O', 'M', 'E'), 202252, 0, 0, 0 },
	{ MKTAG('I', 'S', 'H', 'E'), 201609, 0, 0, 0 },
	{ MKTAG('J', 'N', 'A', 'M'), 202035, 0, 0, 0 },
	{ MKTAG('L', 'I', 'F', '1'), 201704, 0, 0, 0 },
	{ MKTAG('L', 'I', 'F', '2'), 201704, 0, 0, 0 },
	{ MKTAG('L', 'I', 'F', '3'), 201704, 0, 0, 0 },
	{ MKTAG('M', 'A', 'D', '1'), 201238, 0, 0, 0 },
	{ MKTAG('M', 'A', 'D', '4'), 200292, 0, 0, 0 },
	{ MKTAG('M', 'A', 'D', '5'), 200140, 0, 0, 0 },
	{ MKTAG('M', 'A', 'D', '6'), 200968, 0, 0, 0 },
	{ MKTAG('M', 'C', 'P', 'Y'), 200514, 0, 0, 0 },
	{ MKTAG('M', 'I', 'N', 'S'), 200541, 0, 0, 0 },
	{ MKTAG('N', 'A', 'U', 'T'), 200529, 0, 0, 0 },
	{ MKTAG('N', 'O', 'N', 'O'), 200127, 0, 0, 0 },
	{ MKTAG('N', 'P', 'L', 'C'), 200234, 201625, 0, 0 },
	{ MKTAG('O', 'R', 'D', '1'), 200473, 0, 0, 0 },
	{ MKTAG('O', 'R', 'D', '2'), 200473, 0, 0, 0 },
	{ MKTAG('O', 'R', 'D', '3'), 200473, 0, 0, 0 },
	{ MKTAG('O', 'R', 'D', '4'), 200473, 0, 0, 0 },
	{ MKTAG('O', 'R', 'D', '5'), 200473, 0, 0, 0 },
	{ MKTAG('S', 'U', 'C', '2'), 200024, 0, 0, 0 },
	{ MKTAG('S', 'U', 'M', 'S'), 200139, 0, 0, 0 },
	{ MKTAG('T', 'I', 'T', 'A'), 200606, 0, 0, 0 },
	{ MKTAG('W', 'T', 'H', 'R'), 201094, 201097, 201092, 0 },
	{ MKTAG('Y', 'E', 'S', 'S'), 201525, 201529, 0, 0 },
};

const int BARBOT_RESPONSES[93][2] = {
	{ MKTAG('S', 'L', 'O', 'W'), 0x3D09E },
	{ MKTAG('T', 'H', 'R', 'T'), 0x3D0B1 },
	{ MKTAG('A', 'C', 'T', 'R'), 0x3D77F },
	{ MKTAG('A', 'C', 'T', 'S'), 0x3D780 },
	{ MKTAG('A', 'D', 'V', 'T'), 0x3D781 },
	{ MKTAG('A', 'R', 'T', 'I'), 0x3D782 },
	{ MKTAG('A', 'R', 'T', 'Y'), 0x3D783 },
	{ MKTAG('A', 'U', 'T', 'H'), 0x3D784 },
	{ MKTAG('B', 'A', 'R', 'K'), 0x3D786 },
	{ MKTAG('B', 'A', 'R', 'U'), 0x3D786 },
	{ MKTAG('B', 'O', 'Y', 'S'), 0x3D239 },
	{ MKTAG('G', 'I', 'R', 'L'), 0x3D23A },
	{ MKTAG('B', 'R', 'N', 'D'), 0x3D787 },
	{ MKTAG('C', 'H', 'S', 'E'), 0x3D788 },
	{ MKTAG('C', 'O', 'M', 'D'), 0x3D789 },
	{ MKTAG('C', 'O', 'O', 'K'), 0x3D78A },
	{ MKTAG('C', 'O', 'P', 'S'), 0x3D78B },
	{ MKTAG('C', 'R', 'I', 'M'), 0x3D78C },
	{ MKTAG('C', 'S', 'P', 'Y'), 0x3D78D },
	{ MKTAG('D', 'C', 'T', 'R'), 0x3D78E },
	{ MKTAG('D', 'N', 'C', 'E'), 0x3D78F },
	{ MKTAG('D', 'R', 'U', 'G'), 0x3D790 },
	{ MKTAG('E', 'A', 'R', 'T'), 0x3D791 },
	{ MKTAG('E', 'M', 'O', 'T'), 0x3D792 },
	{ MKTAG('F', 'A', 'M', 'E'), 0x3D793 },
	{ MKTAG('A', 'S', 'H', '1'), 0x3D794 },
	{ MKTAG('A', 'S', 'H', '2'), 0x3D795 },
	{ MKTAG('F', 'A', 'U', 'N'), 0x3D796 },
	{ MKTAG('F', 'I', 'L', 'M'), 0x3D797 },
	{ MKTAG('F', 'I', 'S', 'H'), 0x3D798 },
	{ MKTAG('F', 'L', 'O', 'R'), 0x3D799 },
	{ MKTAG('F', 'O', 'O', 'D'), 0x3D79A },
	{ MKTAG('J', 'F', 'O', 'D'), 0x3D79A },
	{ MKTAG('H', 'B', 'B', 'Y'), 0x3D79B },
	{ MKTAG('H', 'E', 'R', 'O'), 0x3D79C },
	{ MKTAG('H', 'H', 'L', 'D'), 0x3D79D },
	{ MKTAG('H', 'O', 'M', 'E'), 0x3D79E },
	{ MKTAG('H', 'O', 'S', 'T'), 0x3D79F },
	{ MKTAG('K', 'N', 'O', 'B'), 0x3D7A0 },
	{ MKTAG('L', 'I', 'Q', 'D'), 0x3D7A1 },
	{ MKTAG('L', 'I', 'T', 'R'), 0x3D7A2 },
	{ MKTAG('M', 'A', 'G', 'S'), 0x3D7A3 },
	{ MKTAG('M', 'C', 'P', 'Y'), 0x3D7A4 },
	{ MKTAG('M', 'I', 'N', 'S'), 0x3D7A5 },
	{ MKTAG('M', 'L', 'T', 'Y'), 0x3D7A6 },
	{ MKTAG('M', 'U', 'S', 'I'), 0x3D7A7 },
	{ MKTAG('N', 'A', 'U', 'T'), 0x3D7A8 },
	{ MKTAG('P', 'G', 'R', 'P'), 0x3D7A9 },
	{ MKTAG('P', 'H', 'I', 'L'), 0x3D7AA },
	{ MKTAG('P', 'K', 'U', 'P'), 0x3D7AB },
	{ MKTAG('P', 'L', 'A', 'N'), 0x3D7AC },
	{ MKTAG('B', 'L', 'P', '1'), 0x3D7AD },
	{ MKTAG('B', 'L', 'P', '2'), 0x3D7AD },
	{ MKTAG('P', 'T', 'I', 'C'), 0x3D7AD },
	{ MKTAG('R', 'C', 'K', 'T'), 0x3D7AE },
	{ MKTAG('S', 'C', 'I', 'E'), 0x3D7AF },
	{ MKTAG('S', 'C', 'I', 'T'), 0x3D7B0 },
	{ MKTAG('S', 'E', 'X', '1'), 0x3D7B1 },
	{ MKTAG('S', 'F', 'S', 'F'), 0x3D7B2 },
	{ MKTAG('S', 'O', 'A', 'P'), 0x3D7B4 },
	{ MKTAG('S', 'O', 'N', 'G'), 0x3D7B5 },
	{ MKTAG('S', 'P', 'R', 'T'), 0x3D7B6 },
	{ MKTAG('S', 'W', 'E', 'R'), 0x3D7B7 },
	{ MKTAG('T', 'D', 'V', 'P'), 0x3D7BC },
	{ MKTAG('T', 'E', 'A', 'M'), 0x3D7BD },
	{ MKTAG('T', 'I', 'T', 'A'), 0x3D7BF },
	{ MKTAG('T', 'L', 'A', ' '), 0x3D7C0 },
	{ MKTAG('T', 'O', 'Y', 'S'), 0x3D7C1 },
	{ MKTAG('T', 'R', 'A', '2'), 0x3D7C2 },
	{ MKTAG('T', 'R', 'A', '3'), 0x3D7C3 },
	{ MKTAG('T', 'V', 'S', 'H'), 0x3D7C4 },
	{ MKTAG('W', 'E', 'A', 'P'), 0x3D7C5 },
	{ MKTAG('W', 'E', 'A', 'T'), 0x3D7C6 },
	{ MKTAG('W', 'W', 'E', 'B'), 0x3D7C7 },
	{ MKTAG('I', 'S', 'H', 'E'), 0x3D221 },
	{ MKTAG('L', 'I', 'T', 'E'), 0x3D31B },
	{ MKTAG('B', 'Y', 'Z', 'A'), 0x3D448 },
	{ MKTAG('W', 'T', 'H', 'R'), 0x3D6E5 },
	{ MKTAG('N', 'P', 'L', 'C'), 0x3D231 },
	{ MKTAG('B', 'A', 'R', '1'), 0x3D095 },
	{ MKTAG('B', 'A', 'R', '2'), 0x3D107 },
	{ MKTAG('B', 'A', 'R', '3'), 0x3D09D },
	{ MKTAG('M', 'P', 'O', 'P'), 0x3D0D3 },
	{ MKTAG('J', 'O', 'K', 'E'), 0x3D5A9 },
	{ MKTAG('J', 'N', 'A', 'M'), 0x3D5A9 },
	{ MKTAG('N', 'I', 'B', '1'), 0x3D128 },
	{ MKTAG('N', 'I', 'B', '2'), 0x3D0DC },
	{ MKTAG('N', 'I', 'B', '3'), 0x3D345 },
	{ MKTAG('N', 'I', 'B', '4'), 0x3D125 },
	{ MKTAG('N', 'I', 'B', '5'), 0x3D369 },
	{ MKTAG('N', 'I', 'B', '6'), 0x3D444 },
	{ MKTAG('B', 'A', 'R', '4'), 0x3D0DF },
	{ MKTAG('F', 'U', 'L', 'N'), 0x3D32C }
};

const int DOORBOT_RESPONSES[101][2] = {
	{ MKTAG('W', 'E', 'A', 'T'), 0x2E29 },
	{ MKTAG('T', 'W', 'A', 'T'), 0x2E29 },
	{ MKTAG('B', 'A', 'R', 'M'), 0x2E29 },
	{ MKTAG('B', 'A', 'R', 'U'), 0x2E29 },
	{ MKTAG('B', 'A', 'R', 'K'), 0x2E29 },
	{ MKTAG('B', 'Y', 'Z', 'A'), 0x274E },
	{ MKTAG('S', 'I', 'C', 'K'), 0x28AC },
	{ MKTAG('B', 'O', 'D', 'Y'), 0x28AC },
	{ MKTAG('N', 'H', 'R', 'O'), 0x28A8 },
	{ MKTAG('N', 'P', 'L', 'C'), 0x28A7 },
	{ MKTAG('H', 'O', 'M', 'E'), 0x28A7 },
	{ MKTAG('S', 'C', 'I', 'E'), 0x28A9 },
	{ MKTAG('P', 'T', 'I', 'C'), 0x2E42 },
	{ MKTAG('P', 'G', 'R', 'P'), 0x2E42 },
	{ MKTAG('B', 'L', 'P', '1'), 0x2E42 },
	{ MKTAG('B', 'L', 'P', '2'), 0x2E42 },
	{ MKTAG('B', 'L', 'P', '3'), 0x2E42 },
	{ MKTAG('B', 'L', 'P', '4'), 0x2E42 },
	{ MKTAG('B', 'L', 'F', '1'), 0x2E3C },
	{ MKTAG('B', 'L', 'F', '2'), 0x2E3C },
	{ MKTAG('B', 'L', 'R', '1'), 0x2E42 },
	{ MKTAG('B', 'L', 'T', '1'), 0x2E56 },
	{ MKTAG('B', 'L', 'T', '2'), 0x2E48 },
	{ MKTAG('B', 'L', 'T', '3'), 0x2E55 },
	{ MKTAG('B', 'L', 'T', '4'), 0x2E56 },
	{ MKTAG('B', 'L', 'T', '5'), 0x2E56 },
	{ MKTAG('S', 'W', 'E', 'R'), 0x2E4E },
	{ MKTAG('S', 'O', 'N', 'G'), 0x2E49 },
	{ MKTAG('L', 'I', 'T', 'R'), 0x2E40 },
	{ MKTAG('A', 'R', 'T', 'I'), 0x2E1F },
	{ MKTAG('N', 'I', 'K', 'E'), 0x2E22 },
	{ MKTAG('E', 'M', 'O', 'T'), 0x2E38 },
	{ MKTAG('D', 'R', 'U', 'G'), 0x2E36 },
	{ MKTAG('E', 'A', 'R', 'T'), 0x2E37 },
	{ MKTAG('F', 'A', 'M', 'E'), 0x2E39 },
	{ MKTAG('F', 'A', 'U', 'N'), 0x2E3B },
	{ MKTAG('F', 'I', 'S', 'H'), 0x2E3B },
	{ MKTAG('F', 'L', 'O', 'R'), 0x2E3D },
	{ MKTAG('F', 'O', 'O', 'D'), 0x2E3E },
	{ MKTAG('J', 'F', 'O', 'D'), 0x2E3E },
	{ MKTAG('H', 'B', 'B', 'Y'), 0x2E3F },
	{ MKTAG('H', 'E', 'R', 'O'), 0x2E4D },
	{ MKTAG('H', 'O', 'S', 'T'), 0x2E55 },
	{ MKTAG('L', 'I', 'Q', 'D'), 0x2E35 },
	{ MKTAG('M', 'A', 'G', 'S'), 0x2E40 },
	{ MKTAG('P', 'H', 'I', 'L'), 0x2E44 },
	{ MKTAG('P', 'K', 'U', 'P'), 0x2E24 },
	{ MKTAG('P', 'L', 'A', 'N'), 0x2E45 },
	{ MKTAG('R', 'C', 'K', 'T'), 0x2E4A },
	{ MKTAG('S', 'E', 'X', '1'), 0x2E47 },
	{ MKTAG('S', 'U', 'C', '1'), 0x2E47 },
	{ MKTAG('S', 'O', 'A', 'P'), 0x2E48 },
	{ MKTAG('T', 'O', 'Y', 'S'), 0x2E50 },
	{ MKTAG('H', 'H', 'L', 'D'), 0x2E50 },
	{ MKTAG('A', 'C', 'T', 'R'), 0x2E1B },
	{ MKTAG('A', 'C', 'T', 'S'), 0x2E1C },
	{ MKTAG('A', 'D', 'V', 'T'), 0x2E1D },
	{ MKTAG('B', 'R', 'N', 'D'), 0x2E21 },
	{ MKTAG('B', 'R', 'N', '2'), 0x2E22 },
	{ MKTAG('B', 'R', 'N', '3'), 0x2E23 },
	{ MKTAG('F', 'A', 'S', 'H'), 0x2E3A },
	{ MKTAG('F', 'I', 'L', 'M'), 0x2E3C },
	{ MKTAG('K', 'N', 'O', 'B'), 0x2E42 },
	{ MKTAG('M', 'U', 'S', 'I'), 0x2E43 },
	{ MKTAG('S', 'F', 'S', 'F'), 0x2E46 },
	{ MKTAG('S', 'P', 'R', 'T'), 0x2E4B },
	{ MKTAG('T', 'E', 'A', 'M'), 0x2E4C },
	{ MKTAG('T', 'R', 'A', 'V'), 0x2E52 },
	{ MKTAG('T', 'V', 'S', 'H'), 0x2E56 },
	{ MKTAG('W', 'E', 'A', 'P'), 0x2E57 },
	{ MKTAG('W', 'W', 'E', 'B'), 0x2E58 },
	{ MKTAG('A', 'R', 'T', 'Y'), 0x2E1E },
	{ MKTAG('C', 'O', 'M', 'D'), 0x2E25 },
	{ MKTAG('C', 'O', 'O', 'K'), 0x2E26 },
	{ MKTAG('C', 'O', 'P', 'S'), 0x2E27 },
	{ MKTAG('C', 'R', 'I', 'M'), 0x2E28 },
	{ MKTAG('D', 'C', 'T', 'R'), 0x2E29 },
	{ MKTAG('A', 'U', 'T', 'H'), 0x2E29 },
	{ MKTAG('M', 'L', 'T', 'Y'), 0x2E41 },
	{ MKTAG('S', 'A', 'S', 'S'), 0x28C3 },
	{ MKTAG('B', 'O', 'Y', 'S'), 0x2768 },
	{ MKTAG('G', 'I', 'R', 'L'), 0x2769 },
	{ MKTAG('T', 'D', 'V', 'P'), 0x277D },
	{ MKTAG('I', 'S', 'H', 'E'), 0x27B6 },
	{ MKTAG('J', 'O', 'K', 'E'), 0x29FF },
	{ MKTAG('J', 'N', 'A', 'M'), 0x29FF },
	{ MKTAG('S', 'L', 'O', 'W'), 0x2823 },
	{ MKTAG('T', 'H', 'R', 'T'), 0x2823 },
	{ MKTAG('D', 'O', 'R', '2'), 0x2BC0 },
	{ MKTAG('M', 'P', 'O', 'P'), 0x2BC0 },
	{ MKTAG('C', 'L', 'U', 'B'), 0x2E39 },
	{ MKTAG('C', 'O', 'L', 'R'), 0x2776 },
	{ MKTAG('D', 'N', 'C', 'E'), 0x2931 },
	{ MKTAG('M', 'C', 'P', 'Y'), 0x276B },
	{ MKTAG('M', 'I', 'N', 'S'), 0x2E43 },
	{ MKTAG('P', 'L', 'A', 'C'), 0x2E37 },
	{ MKTAG('T', 'I', 'T', 'A'), 0x2E40 },
	{ MKTAG('T', 'L', 'A', ' '), 0x277D },
	{ MKTAG('H', 'A', 'H', 'A'), 0x27A8 },
	{ MKTAG('F', 'U', 'L', 'N'), 0x2B15 },
	{ MKTAG('B', 'A', 'R', '1'), 0x2E35 }
};

const int LIFTBOT_RESPONSES[34][2] = {
	{ MKTAG('L', 'I', 'F', '1'), 0x33453 },
	{ MKTAG('L', 'I', 'F', '2'), 0x3345D },
	{ MKTAG('L', 'I', 'F', '3'), 0x3354E },
	{ MKTAG('L', 'I', 'F', '4'), 0x3374B },
	{ MKTAG('I', 'S', 'H', 'E'), 0x335F7 },
	{ MKTAG('J', 'O', 'K', 'E'), 0x3374B },
	{ MKTAG('J', 'N', 'A', 'M'), 0x3374B },
	{ MKTAG('S', 'L', 'O', 'W'), 0x337B5 },
	{ MKTAG('T', 'H', 'R', 'T'), 0x337BB },
	{ MKTAG('S', 'C', 'I', 'E'), 0x337B2 },
	{ MKTAG('S', 'C', 'I', 'T'), 0x337B4 },
	{ MKTAG('T', 'D', 'V', 'P'), 0x337BA },
	{ MKTAG('T', 'I', 'T', 'A'), 0x337C0 },
	{ MKTAG('C', 'S', 'P', 'Y'), 0x337A5 },
	{ MKTAG('M', 'U', 'S', 'I'), 0x337AA },
	{ MKTAG('M', 'C', 'P', 'Y'), 0x337A9 },
	{ MKTAG('D', 'N', 'C', 'E'), 0x337A7 },
	{ MKTAG('N', 'A', 'U', 'T'), 0x337AC },
	{ MKTAG('T', 'L', 'A', ' '), 0x337C1 },
	{ MKTAG('S', 'U', 'M', 'S'), 0x33598 },
	{ MKTAG('O', 'R', 'D', '1'), 0x33776 },
	{ MKTAG('O', 'R', 'D', '2'), 0x33779 },
	{ MKTAG('O', 'R', 'D', '3'), 0x3377A },
	{ MKTAG('O', 'R', 'D', '4'), 0x3377B },
	{ MKTAG('O', 'R', 'D', '5'), 0x3377C },
	{ MKTAG('O', 'R', 'D', '6'), 0x3377D },
	{ MKTAG('O', 'R', 'D', '7'), 0x3377E },
	{ MKTAG('L', 'I', 'Q', 'D'), 0x337FC },
	{ MKTAG('F', 'O', 'O', 'D'), 0x337FD },
	{ MKTAG('J', 'F', 'O', 'D'), 0x337FD },
	{ MKTAG('W', 'E', 'A', 'T'), 0x337E3 },
	{ MKTAG('S', 'I', 'C', 'K'), 0x336A1 },
	{ MKTAG('B', 'O', 'D', 'Y'), 0x33624 },
	{ MKTAG('B', 'Y', 'Z', 'A'), 0x33617 }
};

const int MAITRED_RESPONSES[74][2] = {
	{ MKTAG('M', 'A', 'D', '1'), 0x3F7E2 },
	{ MKTAG('M', 'A', 'D', '2'), 0x3F916 },
	{ MKTAG('M', 'A', 'D', '3'), 0x3F931 },
	{ MKTAG('M', 'A', 'D', '4'), 0x3F936 },
	{ MKTAG('M', 'A', 'D', '5'), 0x3F938 },
	{ MKTAG('M', 'A', 'D', '6'), 0x3F943 },
	{ MKTAG('M', 'A', 'D', '7'), 0x3F947 },
	{ MKTAG('M', 'A', 'D', '8'), 0x3F945 },
	{ MKTAG('M', 'A', 'D', '9'), 0x3F946 },
	{ MKTAG('M', 'D', '1', '0'), 0x3F9F5 },
	{ MKTAG('M', 'D', '1', '1'), 0x3F982 },
	{ MKTAG('J', 'F', 'O', 'D'), 0x3F930 },
	{ MKTAG('C', 'M', 'N', 'T'), 0x3F937 },
	{ MKTAG('H', 'A', 'H', 'A'), 0x3FA47 },
	{ MKTAG('S', 'U', 'M', 'S'), 0x3F9FE },
	{ MKTAG('A', 'N', 'S', 'W'), 0x3F90B },
	{ MKTAG('M', 'U', 'S', 'I'), 0x3FA35 },
	{ MKTAG('S', 'O', 'N', 'G'), 0x3FA35 },
	{ MKTAG('A', 'C', 'T', 'R'), 0x3F9B6 },
	{ MKTAG('A', 'C', 'T', 'S'), 0x3F9B7 },
	{ MKTAG('A', 'R', 'T', 'I'), 0x3F9CE },
	{ MKTAG('A', 'R', 'T', 'Y'), 0x3F9CE },
	{ MKTAG('A', 'U', 'T', 'H'), 0x3FAB6 },
	{ MKTAG('C', 'O', 'M', 'D'), 0x3F963 },
	{ MKTAG('C', 'O', 'P', 'S'), 0x3F9F8 },
	{ MKTAG('C', 'R', 'I', 'M'), 0x3F9F8 },
	{ MKTAG('C', 'S', 'P', 'Y'), 0x3F965 },
	{ MKTAG('D', 'C', 'T', 'R'), 0x3F9B6 },
	{ MKTAG('D', 'R', 'U', 'G'), 0x3F96F },
	{ MKTAG('E', 'A', 'R', 'T'), 0x3F9E3 },
	{ MKTAG('E', 'M', 'O', 'T'), 0x3FA29 },
	{ MKTAG('F', 'A', 'M', 'E'), 0x3FAB6 },
	{ MKTAG('F', 'A', 'S', 'H'), 0x3F8EA },
	{ MKTAG('F', 'A', 'U', 'N'), 0x3F969 },
	{ MKTAG('F', 'I', 'L', 'M'), 0x3F9CE },
	{ MKTAG('F', 'L', 'O', 'R'), 0x3F9A3 },
	{ MKTAG('H', 'B', 'B', 'Y'), 0x3F987 },
	{ MKTAG('H', 'E', 'R', 'O'), 0x3F8DA },
	{ MKTAG('H', 'H', 'G', 'Q'), 0x3F96B },
	{ MKTAG('H', 'H', 'L', 'D'), 0x3FA2B },
	{ MKTAG('H', 'O', 'S', 'T'), 0x3F9E1 },
	{ MKTAG('K', 'N', 'O', 'B'), 0x3F9E1 },
	{ MKTAG('L', 'I', 'Q', 'D'), 0x3F91C },
	{ MKTAG('L', 'I', 'T', 'R'), 0x3F9CE },
	{ MKTAG('M', 'A', 'G', 'S'), 0x3F912 },
	{ MKTAG('M', 'L', 'T', 'Y'), 0x3F9F7 },
	{ MKTAG('P', 'G', 'R', 'P'), 0x3F8D0 },
	{ MKTAG('P', 'H', 'I', 'L'), 0x3F8E9 },
	{ MKTAG('P', 'K', 'U', 'P'), 0x3F9FB },
	{ MKTAG('P', 'L', 'A', 'C'), 0x3FA22 },
	{ MKTAG('P', 'T', 'I', 'C'), 0x3F8D0 },
	{ MKTAG('R', 'C', 'K', 'T'), 0x3F9A3 },
	{ MKTAG('S', 'C', 'I', 'E'), 0x3F968 },
	{ MKTAG('S', 'C', 'I', 'T'), 0x3F9E2 },
	{ MKTAG('S', 'E', 'X', '1'), 0x3F9C9 },
	{ MKTAG('S', 'F', 'S', 'F'), 0x3F988 },
	{ MKTAG('S', 'O', 'A', 'P'), 0x3FA3C },
	{ MKTAG('S', 'P', 'R', 'T'), 0x3FAB6 },
	{ MKTAG('S', 'W', 'E', 'R'), 0x3F96B },
	{ MKTAG('T', 'E', 'A', 'M'), 0x3F9B8 },
	{ MKTAG('T', 'O', 'Y', 'S'), 0x3F96D },
	{ MKTAG('T', 'V', 'S', 'H'), 0x3F9CE },
	{ MKTAG('W', 'E', 'A', 'P'), 0x3F9D8 },
	{ MKTAG('W', 'W', 'E', 'B'), 0x3F987 },
	{ MKTAG('B', 'Y', 'Z', 'A'), 0x3F9A5 },
	{ MKTAG('T', 'W', 'A', 'T'), 0x3F96F },
	{ MKTAG('M', 'C', 'P', 'Y'), 0x3F9B9 },
	{ MKTAG('T', 'H', 'R', 'T'), 0x3FB15 },
	{ MKTAG('T', 'D', 'V', 'P'), 0x3FB17 },
	{ MKTAG('T', 'I', 'T', 'A'), 0x3FB18 },
	{ MKTAG('M', 'I', 'N', 'S'), 0x3FB19 },
	{ MKTAG('D', 'N', 'C', 'E'), 0x3FB1A },
	{ MKTAG('N', 'A', 'U', 'T'), 0x3FB1B },
	{ MKTAG('T', 'L', 'A', ' '), 0x3FB1C }
};

void writeScriptResponses(const char *name, const int *tags, uint count, int valuesPerTag) {
	outputFile.seek(dataOffset);

	for (uint idx = 0; idx < count * (valuesPerTag + 1); ++idx, ++tags)
		outputFile.writeLong(*tags);

	writeEntryHeader(name, dataOffset, count * (valuesPerTag + 1) * 4);
	dataOffset += count * (valuesPerTag + 1) * 4;
}

void writeAllScriptResponses() {
	writeScriptResponses("Responses/Barbot", &BARBOT_RESPONSES[0][0], 93, 1);
	writeScriptResponses("Responses/Bellbot", &BELLBOT_RESPONSES[0][0], 130, 4);
	writeScriptResponses("Responses/Deskbot", &DESKBOT_RESPONSES[0][0], 128, 4);
	writeScriptResponses("Responses/Doorbot", &DOORBOT_RESPONSES[0][0], 101, 1);
	writeScriptResponses("Responses/Liftbot", &LIFTBOT_RESPONSES[0][0], 34, 1);
	writeScriptResponses("Responses/MaitreD", &MAITRED_RESPONSES[0][0], 74, 1);
}
