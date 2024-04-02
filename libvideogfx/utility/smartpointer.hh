/********************************************************************************
  $Header$

  purpose:
    Generic smart pointer class.

    SP is a "smart pointer" class with reference counting. You can
    copy smart pointers or pass them to subroutines.

    SSP is a "simple smart pointer" which does not support copying.
    It is useful for automatic object destruction only.

  notes:

  to do:

  author(s):
   - Dirk Farin, dirk.farin@gmx.de

  modifications:
   09/May/2002 - Dirk Farin
     - first implementation
 ********************************************************************************
    LibVideoGfx - video processing library
    Copyright (C) 2002  Dirk Farin

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 ********************************************************************************/

#ifndef LIBVIDEOGFX_UTILITY_SMARTPOINTER_HH
#define LIBVIDEOGFX_UTILITY_SMARTPOINTER_HH

#include <libvideogfx/error.hh>

namespace videogfx {

  template <class T> class SP
  {
  public:
    SP()
    {
      this->counter = NULL;
      pointer = NULL;
    }

    SP(T* p)
    {
      counter = new long;
      *counter = 1;

      pointer = p;
    }

    SP(const SP<T>& s)
    {
      if (s.counter) (*s.counter)++;

      counter = s.counter;
      pointer = s.pointer;
    }

    ~SP()
    {
      Decouple();
    }

    SP<T>& operator=(const SP<T>& s)
    {
      if (&s == this) return *this;  // without this, things go wrong because Decouple() sets 'counter' AND ALSO 's.counter' to NULL

      if (s.counter) (*(s.counter))++;
      Decouple();

      counter = s.counter;
      pointer = s.pointer;

      return *this;
    }

    SP<T>&