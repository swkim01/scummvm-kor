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

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * $URL: https://scummvm.svn.sourceforge.net/svnroot/scummvm/scummvm/branches/gsoc2010-plugins/backends/plugins/arm-loader.cpp $
 * $Id: arm-loader.cpp 52058 2010-08-13 05:58:11Z toneman1138 $
 *
 */

#if defined(DYNAMIC_MODULES) && defined(PPC_TARGET)

#include "backends/plugins/elf-loader.h"
#include "backends/plugins/ppc-loader.h"

#include "common/debug.h"

bool PPCDLObject::relocate(Common::SeekableReadStream* DLFile, unsigned long offset, unsigned long size, void *relSegment) {
	Elf32_Rela *rel = NULL;

	if (!(rel = (Elf32_Rela *)malloc(size))) {
		warning("elfloader: Out of memory.");
		return false;
	}

	if (DLFile->seek(offset, SEEK_SET) < 0 ||
			DLFile->read(rel, size) != size) {
		warning("elfloader: Relocation table load failed.");
		free(rel);
		return false;
	}

	int cnt = size / sizeof(*rel);

	debug(2, "elfloader: Loaded relocation table. %d entries. base address=%p\n", cnt, relSegment);

	uint32 *src;
	uint32 value;

	for (int i = 0; i < cnt; i++) {
		// Get the symbol this relocation entry is referring to
		Elf32_Sym *sym = (Elf32_Sym *)(_symtab) + (REL_INDEX(rel[i].r_info));

		// Get the target instruction in the code
		src = (uint32 *)((char *)relSegment + rel[i].r_offset - _segmentVMA);
		value = sym->st_value + rel[i].r_addend;

		//debug(8, "elfloader: i=%05d %p +0x%04x: (0x%08x) 0x%08x ", i, src, rel[i].r_addend, sym->st_value, *src);

		switch (REL_TYPE(rel[i].r_info)) {
		case R_PPC_ADDR32:
			*src = value;
			debug(8, "elfloader: R_PPC_ADDR32 -> 0x%08x\n", *src);
			break;
		case R_PPC_ADDR16_LO:
			*((uint16 *) src) = value;
			debug(8, "elfloader: R_PPC_ADDR16_LO -> 0x%08x\n", *src);
			break;
		case R_PPC_ADDR16_HI:
			*(uint16 *) src = value >> 16;
			debug(8, "elfloader: R_PPC_ADDR16_HA -> 0x%08x\n", *src);
			break;
		case R_PPC_ADDR16_HA:
			*(uint16 *) src = (value + 0x8000) >> 16;
			debug(8, "elfloader: R_PPC_ADDR16_HA -> 0x%08x\n", *src);
			break;
		case R_PPC_REL24:
			*src = (*src & ~0x03fffffc) | ((value - (uint32) src) & 0x03fffffc);
			debug(8, "elfloader: R_PPC_REL24 -> 0x%08x\n", *src);
			break;
		case R_PPC_REL32:
			*src = value - (uint32) src;
			debug(8, "elfloader: R_PPC_REL32 -> 0x%08x\n", *src);
			break;
		default:
			warning("elfloader: Unknown relocation type %d\n", REL_TYPE(rel[i].r_info));
			free(rel);
			return false;
		}
	}

	free(rel);
	return true;
}

bool PPCDLObject::relocateRels(Common::SeekableReadStream* DLFile, Elf32_Ehdr *ehdr, Elf32_Shdr *shdr) {
	for (int i = 0; i < ehdr->e_shnum; i++) {
		Elf32_Shdr *curShdr = &(shdr[i]);

		if ((curShdr->sh_type == SHT_REL) &&
				curShdr->sh_entsize == sizeof(Elf32_Rel) &&
				(int)curShdr->sh_link == _symtab_sect &&
				curShdr->sh_info < ehdr->e_shnum &&
				(shdr[curShdr->sh_info].sh_flags & SHF_ALLOC)) {
			warning("elfloader: REL entries not supported!\n");
			return false;
		}

		if ((curShdr->sh_type == SHT_RELA) &&
				curShdr->sh_entsize == sizeof(Elf32_Rela) &&
				(int)curShdr->sh_link == _symtab_sect &&
				curShdr->sh_info < ehdr->e_shnum &&
				(shdr[curShdr->sh_info].sh_flags & SHF_ALLOC)) {
			if (!relocate(DLFile, curShdr->sh_offset, curShdr->sh_size, _segment))
				return false;
		}
	}

	return true;
}

#endif /* defined(DYNAMIC_MODULES) && defined(PPC_TARGET) */

