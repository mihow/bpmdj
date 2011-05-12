/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001-2004 Werner Van Belle
 See 'BeatMixing.ps' for more information

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
****/

#define accessors(type,var,init) \
  private: type _##var; \
  public: inline type get_##var() const { return _##var; }; \
  inline void set_##var(type a) { _##var = a; }; \
  inline void init_##var() { _##var = init; };

#define container_accessors(type,var,accesstype,access) \
  private: type _##var; \
  public: inline type get_##var() const { return _##var; }; \
  inline void set_##var(type a) { _##var = a; }; \
  inline void init_##var() { _##var = init; };

#define singleton_accessors(type,var)  \
  private: static type _##var; \
  public: static inline type get_##var() { return _##var; }; \
  void static inline set_##var(type a) { _##var = a; };

#define singleton_array_accessors(type,type2,var)  \
  private: static type _##var; \
  public: static inline type get_##var() { return _##var; }; \
  void static inline set_##var(type a) { _##var = a; }; \
  static inline type2 get_##var(int idx) { return _##var[idx];}; \
  static inline void set_##var(int idx, type2 v) {_##var[idx]=v;};

#define init_singleton_var(klasse, var, type, init) type klasse::_##var = init


// Step 1. write accessors for all variables.
// 
