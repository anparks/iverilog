/*
 * Copyright (c) 2001 Stephen Williams (steve@icarus.com)
 *
 *    This source code is free software; you can redistribute it
 *    and/or modify it in source code form under the terms of the GNU
 *    General Public License as published by the Free Software
 *    Foundation; either version 2 of the License, or (at your option)
 *    any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */
#if !defined(WINNT)
#ident "$Id: codes.cc,v 1.4 2001/03/22 05:08:00 steve Exp $"
#endif

# include  "codes.h"
# include  <string.h>
# include  <assert.h>


const unsigned code_index0_size = 2 << 9;
const unsigned code_index1_size = 2 << 11;
const unsigned code_index2_size = 2 << 10;

struct code_index0 {
      struct vvp_code_s table[code_index0_size];
};

struct code_index1 {
      struct code_index0* table[code_index1_size];
};

static vvp_cpoint_t code_count = 0;
static struct code_index1*code_table[code_index2_size] = { 0 };


void codespace_init(void)
{
      code_table[0] = 0;
      code_count = 1;
}

vvp_cpoint_t codespace_allocate(void)
{
      vvp_cpoint_t idx = code_count;

      idx /= code_index0_size;

      unsigned index1 = idx % code_index1_size;
      idx /= code_index1_size;

      assert(idx < code_index2_size);

      if (code_table[idx] == 0) {
	    code_table[idx] = new struct code_index1;
	    memset(code_table[idx], 0, sizeof code_table[idx]);
      }

      if (code_table[idx]->table[index1] == 0) {
	    code_table[idx]->table[index1] = new struct code_index0;
	    memset(code_table[idx]->table[index1],
		   0, sizeof(struct code_index0));
      }

      vvp_cpoint_t res = code_count;
      code_count += 1;
      return res;
}

vvp_code_t codespace_index(vvp_cpoint_t point)
{
      assert(point < code_count);

      unsigned index0 = point % code_index0_size;
      point /= code_index0_size;

      unsigned index1 = point % code_index1_size;
      point /= code_index1_size;

      return code_table[point]->table[index1]->table + index0;
}

void codespace_dump(FILE*fd)
{
      for (unsigned idx = 0 ;  idx < code_count ;  idx += 1) {
	    fprintf(fd, "  %8x: ", idx);
	    vvp_code_t cop = codespace_index(idx);

	    if (cop->opcode == &of_ASSIGN) {
		  fprintf(fd, "%%assign 0x%u, %u, %u\n",
			  cop->iptr, cop->bit_idx1, cop->bit_idx2);

	    } else if (cop->opcode == &of_DELAY) {
		  fprintf(fd, "%%delay %lu\n", (unsigned long)cop->number);

	    } else if (cop->opcode == &of_END) {
		  fprintf(fd, "%%end\n");

	    } else if (cop->opcode == &of_JMP) {
		  fprintf(fd, "%%jmp 0x%u\n", cop->cptr);

	    } else if (cop->opcode == &of_SET) {
		  fprintf(fd, "%%set 0x%u, %u\n", cop->iptr, cop->bit_idx1);

	    } else {
		  fprintf(fd, "opcode %p\n", cop->opcode);
	    }
      }
}


/*
 * $Log: codes.cc,v $
 * Revision 1.4  2001/03/22 05:08:00  steve
 *  implement %load, %inv, %jum/0 and %cmp/u
 *
 * Revision 1.3  2001/03/20 06:16:23  steve
 *  Add support for variable vectors.
 *
 * Revision 1.2  2001/03/11 23:06:49  steve
 *  Compact the vvp_code_s structure.
 *
 * Revision 1.1  2001/03/11 00:29:38  steve
 *  Add the vvp engine to cvs.
 *
 */

