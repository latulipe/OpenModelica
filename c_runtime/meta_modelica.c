/*
 * This file is part of OpenModelica.
 *
 * Copyright (c) 1998-CurrentYear, Link�ping University,
 * Department of Computer and Information Science,
 * SE-58183 Link�ping, Sweden.
 *
 * All rights reserved.
 *
 * THIS PROGRAM IS PROVIDED UNDER THE TERMS OF GPL VERSION 3 
 * AND THIS OSMC PUBLIC LICENSE (OSMC-PL). 
 * ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS PROGRAM CONSTITUTES RECIPIENT'S  
 * ACCEPTANCE OF THE OSMC PUBLIC LICENSE.
 *
 * The OpenModelica software and the Open Source Modelica
 * Consortium (OSMC) Public License (OSMC-PL) are obtained
 * from Link�ping University, either from the above address,
 * from the URLs: http://www.ida.liu.se/projects/OpenModelica or  
 * http://www.openmodelica.org, and in the OpenModelica distribution. 
 * GNU version 3 is obtained from: http://www.gnu.org/copyleft/gpl.html.
 *
 * This program is distributed WITHOUT ANY WARRANTY; without
 * even the implied warranty of  MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE, EXCEPT AS EXPRESSLY SET FORTH
 * IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE CONDITIONS
 * OF OSMC-PL.
 *
 * See the full OSMC Public License conditions for more details.
 *
 */


#include "modelica.h"

jmp_buf *mmc_jumper;

void *mmc_alloc_bytes(unsigned nbytes)
{
  static char *mmc_cur_malloc_buf = NULL;
  static long mmc_cur_malloc_buf_ix=0;
  /* Until we have GC, we simply allocate in 256MB chunks... */
  const long mmc_cur_malloc_buf_sz=256*1024*1024; /* 256MB chunks */
  void *p=0;
  /* fprintf(stderr, "1 mmc_alloc_bytes(%ld): %ld,%ld\n", nbytes, mmc_cur_malloc_buf, mmc_cur_malloc_buf_ix); */
  if (mmc_cur_malloc_buf == NULL || nbytes>(mmc_cur_malloc_buf_sz-mmc_cur_malloc_buf_ix)) {
    if ( (mmc_cur_malloc_buf = malloc(mmc_cur_malloc_buf_sz)) == 0 ) {
      fflush(NULL);
      fprintf(stderr, "malloc(%u) failed: %s\n", nbytes, strerror(errno));
      assert(p != 0);
    }
    mmc_cur_malloc_buf_ix = 0;
    assert(nbytes <= mmc_cur_malloc_buf_sz);
  }
  p = mmc_cur_malloc_buf + mmc_cur_malloc_buf_ix;

  /* Force 16-byte alignment, like malloc... TODO: Check if this is needed :) */
  mmc_cur_malloc_buf_ix += nbytes; /* + ((nbytes%16) ? 16-(nbytes%16): 0); */

  /* fprintf(stderr, "2 mmc_alloc_bytes(%ld): %ld,%ld => %ld\n", nbytes, mmc_cur_malloc_buf, mmc_cur_malloc_buf_ix, p); */
  return p;
}

void* mmc_mk_rcon(double d)
{
    struct mmc_real *p = mmc_alloc_words(sizeof(struct mmc_real)/MMC_SIZE_INT);
    p->header = MMC_REALHDR;
    p->data = d;
    return MMC_TAGPTR(p);
}

void *mmc_mk_box_arr(int slots, unsigned ctor, const void** args)
{
    int i;
    struct mmc_struct *p = mmc_alloc_words(slots+1);
    p->header = MMC_STRUCTHDR(slots, ctor);
    for (i=0; i<slots; i++) {
      p->data[i] = (void*) args[i];
    }
    return MMC_TAGPTR(p);
}

void *mmc_mk_box_no_assign(int slots, unsigned ctor)
{
    struct mmc_struct *p = mmc_alloc_words(slots+1);
    p->header = MMC_STRUCTHDR(slots, ctor);
    return MMC_TAGPTR(p);
}

valueEq_rettype valueEq(modelica_metatype lhs, modelica_metatype rhs)
{
  mmc_uint_t h_lhs;
  mmc_uint_t h_rhs;
  int numslots;
  unsigned ctor;
  int i;
  void *lhs_data, *rhs_data;
  struct record_description *lhs_desc,*rhs_desc;

  if (lhs == rhs) {
    return 1;
  }

  if ((0 == ((mmc_sint_t)lhs & 1)) && (0 == ((mmc_sint_t)rhs & 1))) {
    return lhs == rhs;
  }
  
  h_lhs = MMC_GETHDR(lhs);
  h_rhs = MMC_GETHDR(rhs);

  if (h_lhs != h_rhs)
    return 0;

  if (h_lhs == MMC_NILHDR) {
    return 1;
  }

  if (h_lhs == MMC_REALHDR) {
    double d1,d2;
    d1 = MMC_REALDATA(lhs);
    d2 = MMC_REALDATA(rhs);
    return d1 == d2;
  }
  if (MMC_HDRISSTRING(h_lhs)) {
    return 0 == strcmp(MMC_STRINGDATA(lhs),MMC_STRINGDATA(rhs));
  }

  numslots = MMC_HDRSLOTS(h_lhs);
  ctor = 255 & (h_lhs >> 2);
  
  if (numslots>0 && ctor > 1) { /* RECORD */
    lhs_desc = MMC_FETCH(MMC_OFFSET(MMC_UNTAGPTR(lhs),1));
    rhs_desc = MMC_FETCH(MMC_OFFSET(MMC_UNTAGPTR(rhs),1));
    /* Slow; not needed
    if (0 != strcmp(lhs_desc->name,rhs_desc->name))
      return 0;
    */
    for (i=2; i<=numslots; i++) {
      lhs_data = MMC_FETCH(MMC_OFFSET(MMC_UNTAGPTR(lhs),i));
      rhs_data = MMC_FETCH(MMC_OFFSET(MMC_UNTAGPTR(rhs),i));
      if (0 == valueEq(lhs_data,rhs_data))
        return 0;
    }
    return 1;
  }

  if (numslots>0 && ctor == 0) { /* TUPLE */
    for (i=0; i<numslots; i++) {
      void *tlhs, *trhs;
      tlhs = MMC_FETCH(MMC_OFFSET(MMC_UNTAGPTR(lhs),i+1));
      trhs = MMC_FETCH(MMC_OFFSET(MMC_UNTAGPTR(rhs),i+1));
      if (0 == valueEq(tlhs,trhs)) {
        return 0;
      }
    }
    return 1;
  }

  if (numslots==0 && ctor==1) /* NONE() */ {
    return 1;
  }

  if (numslots==1 && ctor==1) /* SOME(x) */ {
    return valueEq(MMC_FETCH(MMC_OFFSET(MMC_UNTAGPTR(lhs),1)),MMC_FETCH(MMC_OFFSET(MMC_UNTAGPTR(rhs),1)));
  }

  if (numslots==2 && ctor==1) { /* CONS-PAIR */
    while (!MMC_NILTEST(lhs) && !MMC_NILTEST(rhs)) {
      if (!valueEq(MMC_CAR(lhs),MMC_CAR(rhs)))
        return 0;
      lhs = MMC_CDR(lhs);
      rhs = MMC_CDR(rhs);
    }
    return MMC_NILTEST(lhs) == MMC_NILTEST(rhs);
  }

  fprintf(stderr, "%s:%d: %d slots; ctor %d - FAILED to detect the type\n", __FILE__, __LINE__, numslots, ctor);
  EXIT(1);
}

/*
mmc__uniontype__metarecord__typedef__equal_rettype
mmc__uniontype__metarecord__typedef__equal(void* ut,int ex_ctor,int fieldNums)
{
  mmc_uint_t hdr;
  int numslots;
  unsigned ctor;
  struct record_description* desc;

  hdr = MMC_GETHDR(ut);
  numslots = MMC_HDRSLOTS(hdr);
  ctor = 255 & (hdr >> 2);

  if (numslots == fieldNums+1 && ctor == ex_ctor+3) { // RECORD
    desc = MMC_FETCH(MMC_OFFSET(MMC_UNTAGPTR(ut),1));
    return 1;
  } else {
    return 0;
  }
}
*/

void debug__print(void* prefix, void* any)
{
  fprintf(stderr, "%s%s", MMC_STRINGDATA(prefix), anyString(any));
}

static char *anyStringBuf = 0;
int anyStringBufSize = 0;

inline static void checkAnyStringBufSize(int ix, int szNewObject)
{
  if (anyStringBufSize-ix < szNewObject+1) {
    anyStringBuf = realloc(anyStringBuf, anyStringBufSize*2);
    anyStringBufSize *= 2;
  }
}

inline static int anyStringWork(void* any, int ix)
{
  mmc_uint_t hdr;
  int numslots;
  unsigned ctor;
  int i;
  void *data;
  struct record_description *desc;

  if ((0 == ((mmc_sint_t)any & 1))) {
    checkAnyStringBufSize(ix,40);
    ix += sprintf(anyStringBuf+ix, "%ld", (long) ((mmc_sint_t)any)>>1);
    return ix;
  }
  
  hdr = MMC_GETHDR(any);

  if (hdr == MMC_NILHDR) {
    checkAnyStringBufSize(ix,2);
    ix += sprintf(anyStringBuf+ix, "{}");
    return ix;
  }

  if (hdr == MMC_REALHDR) {
    checkAnyStringBufSize(ix,40);
    ix += sprintf(anyStringBuf+ix, "%.7g", (double) MMC_REALDATA(any));
    return ix;
  }
  if (MMC_HDRISSTRING(hdr)) {
    MMC_CHECK_STRING(any);
    checkAnyStringBufSize(ix,strlen(MMC_STRINGDATA(any)));
    ix += sprintf(anyStringBuf+ix, "\"%s\"", MMC_STRINGDATA(any));
    return ix;
  }

  numslots = MMC_HDRSLOTS(hdr);
  ctor = 255 & (hdr >> 2);
  
  if (numslots>0 && ctor == MMC_ARRAY_TAG) { /* MetaModelica-style array */
    checkAnyStringBufSize(ix,40);
    ix += sprintf(anyStringBuf+ix, "MetaArray(");
    for (i=1; i<=numslots; i++) {
      data = MMC_FETCH(MMC_OFFSET(MMC_UNTAGPTR(any),i));
      ix = anyStringWork(data, ix);
      if (i!=numslots) {
        checkAnyStringBufSize(ix,2);
        ix += sprintf(anyStringBuf+ix, ", ");
      }
    }
    checkAnyStringBufSize(ix,2);
    ix += sprintf(anyStringBuf+ix, ")");
    return ix;
  }
  if (numslots>0 && ctor > 1) { /* RECORD */
    desc = MMC_FETCH(MMC_OFFSET(MMC_UNTAGPTR(any),1));
    checkAnyStringBufSize(ix,strlen(desc->name)+2);
    ix += sprintf(anyStringBuf+ix, "%s(", desc->name);
    for (i=2; i<=numslots; i++) {
      data = MMC_FETCH(MMC_OFFSET(MMC_UNTAGPTR(any),i));
      checkAnyStringBufSize(ix,strlen(desc->fieldNames[i-2])+3);
      ix += sprintf(anyStringBuf+ix, "%s = ", desc->fieldNames[i-2]);
      ix = anyStringWork(data,ix);
      if (i!=numslots) {
        checkAnyStringBufSize(ix,2);
        ix += sprintf(anyStringBuf+ix, ", ");
      }
    }
    checkAnyStringBufSize(ix,1);
    ix += sprintf(anyStringBuf+ix, ")");
    return ix;
  }

  if (numslots>0 && ctor == 0) { /* TUPLE */
    checkAnyStringBufSize(ix,1);
    ix += sprintf(anyStringBuf+ix, "(");
    for (i=0; i<numslots; i++) {
      ix = anyStringWork(MMC_FETCH(MMC_OFFSET(MMC_UNTAGPTR(any),i+1)),ix);
      if (i!=numslots-1) {
        checkAnyStringBufSize(ix,2);
        ix += sprintf(anyStringBuf+ix, ", ");
      }
    }
    checkAnyStringBufSize(ix,1);
    ix += sprintf(anyStringBuf+ix, ")");
    return ix;
  }

  if (numslots==0 && ctor==1) /* NONE() */ {
    checkAnyStringBufSize(ix,6);
    ix += sprintf(anyStringBuf+ix, "NONE()");
    return ix;
  }

  if (numslots==1 && ctor==1) /* SOME(x) */ {
    checkAnyStringBufSize(ix,5);
    ix += sprintf(anyStringBuf+ix, "SOME(");
    ix = anyStringWork(MMC_FETCH(MMC_OFFSET(MMC_UNTAGPTR(any),1)),ix);
    checkAnyStringBufSize(ix,1);
    ix += sprintf(anyStringBuf+ix, ")");
    return ix;
  }

  if (numslots==2 && ctor==1) { /* CONS-PAIR */
    checkAnyStringBufSize(ix,1);
    ix += sprintf(anyStringBuf+ix, "{");
    ix = anyStringWork(MMC_CAR(any),ix);
    any = MMC_CDR(any);
    while (!MMC_NILTEST(any)) {
      checkAnyStringBufSize(ix,2);
      ix += sprintf(anyStringBuf+ix, ", ");
      ix = anyStringWork(MMC_CAR(any),ix);
      any = MMC_CDR(any);
    }
    checkAnyStringBufSize(ix,1);
    ix += sprintf(anyStringBuf+ix, "}");
    return ix;
  }

  fprintf(stderr, "%s:%d: %d slots; ctor %d - FAILED to detect the type\n", __FILE__, __LINE__, numslots, ctor);
  EXIT(1);  
}

char* anyString(void* any)
{
  if (anyStringBufSize == 0) {
    anyStringBuf = malloc(8192);
    anyStringBufSize = 8192;
  }
  *anyStringBuf = '\0';
  anyStringWork(any,0);
  return strdup(anyStringBuf);
}

void* mmc_anyString(void* any)
{
  if (anyStringBufSize == 0) {
    anyStringBuf = malloc(8192);
    anyStringBufSize = 8192;
  }
  *anyStringBuf = '\0';
  anyStringWork(any,0);
  return mmc_mk_scon(anyStringBuf);
}

void printAny(void* any)
{
  if (anyStringBufSize == 0) {
    anyStringBuf = malloc(8192);
    anyStringBufSize = 8192;
  }
  *anyStringBuf = '\0';
  anyStringWork(any,0);
  fputs(anyStringBuf, stderr);
}

void printTypeOfAny(void* any) /* for debugging */
{
  mmc_uint_t hdr;
  int numslots;
  unsigned ctor;
  int i;
  void *data;
  struct record_description *desc;

  if ((0 == ((mmc_sint_t)any & 1))) {
    fprintf(stderr, "Integer");
    return;
  }
  
  hdr = MMC_GETHDR(any);

  if (hdr == MMC_NILHDR) {
    fprintf(stderr, "list<Any>");
    return;
  }

  if (hdr == MMC_REALHDR) {
    fprintf(stderr, "Real");
    return;
  }

  if (MMC_HDRISSTRING(hdr)) {
    fprintf(stderr, "String");
    return;
  }

  numslots = MMC_HDRSLOTS(hdr);
  ctor = 255 & (hdr >> 2);
  
  if (numslots>0 && ctor == MMC_ARRAY_TAG) { /* MetaModelica-style array */
    fprintf(stderr, "meta_array<");
    data = MMC_FETCH(MMC_OFFSET(MMC_UNTAGPTR(any),1));
    printTypeOfAny(data);
    fprintf(stderr, ">");
    return;
  }
  if (numslots>0 && ctor > 1) { /* RECORD */
    desc = MMC_FETCH(MMC_OFFSET(MMC_UNTAGPTR(any),1));
    fprintf(stderr, "%s(", desc->name);
    for (i=2; i<=numslots; i++) {
      data = MMC_FETCH(MMC_OFFSET(MMC_UNTAGPTR(any),i));
      fprintf(stderr, "%s = ", desc->fieldNames[i-2]);
      printTypeOfAny(data);
      if (i!=numslots)
        fprintf(stderr, ", ");
    }
    fprintf(stderr, ")");
    return;
  }

  if (numslots>0 && ctor == 0) { /* TUPLE */
    fprintf(stderr, "tuple<");
    printTypeOfAny(MMC_FETCH(MMC_OFFSET(MMC_UNTAGPTR(any),1)));
    fprintf(stderr, ">");
    return;
  }

  if (numslots==0 && ctor==1) /* NONE() */ {
    fprintf(stderr, "Option<Any>");
    return;
  }

  if (numslots==1 && ctor==1) /* SOME(x) */ {
    fprintf(stderr, "Option<");
    printTypeOfAny(MMC_FETCH(MMC_OFFSET(MMC_UNTAGPTR(any),1)));
    fprintf(stderr, ">");
    return;
  }

  if (numslots==2 && ctor==1) { /* CONS-PAIR */
    fprintf(stderr, "list<");
    printTypeOfAny(MMC_CAR(any));
    fprintf(stderr, ">");
    return;
  }

  fprintf(stderr, "%s:%d: %d slots; ctor %d - FAILED to detect the type\n", __FILE__, __LINE__, numslots, ctor);
  EXIT(1);
}

unsigned long mmc_prim_hash(void *p)
{
  unsigned long hash = 0;
  void **pp = NULL;
  mmc_uint_t phdr = 0;
  mmc_uint_t slots = 0;
  
mmc_prim_hash_tail_recur:
  if ((0 == ((mmc_sint_t)p & 1)))
  {
    return hash + (unsigned long)MMC_UNTAGFIXNUM(p);
  } 
  
  phdr = MMC_GETHDR(p);
  hash += (unsigned long)phdr;

  if( phdr == MMC_REALHDR ) 
  {
    return hash + (unsigned long)mmc_unbox_real(p);
  } 
  
  if( MMC_HDRISSTRING(phdr) ) 
  {
    return hash + (unsigned long)stringHashDjb2(p);
  }
  
  if( MMC_HDRISSTRUCT(phdr) ) 
  {
    slots = MMC_HDRSLOTS(phdr);
    pp = MMC_STRUCTDATA(p);
    hash += MMC_HDRCTOR(phdr);
    if (slots == 0) 
      return hash;

    while ( --slots > 0)
    {
       hash += mmc_prim_hash(*pp++);
    }
    p = *pp;
    goto mmc_prim_hash_tail_recur;
  }
  return hash;
}

modelica_integer valueHashMod(void *p, modelica_integer mod)
{
  modelica_integer res = mmc_prim_hash(p) % (unsigned long) mod;
  return res;
}

void* boxptr_valueHashMod(void *p, void *mod)
{
  return mmc_mk_icon(mmc_prim_hash(p) % (unsigned long) mmc_unbox_integer(mod));
}
