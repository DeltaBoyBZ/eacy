#pragma once

/**
 * Copyright 2023 Matthew Peter Smith
 *
 * This code is provided under an MIT License. 
 * See LICENSE.txt at the root of this Git repository. 
 */

#ifdef __cplusplus
#define EAC_DEC extern "C" 
#else
#define EAC_DEC
#endif

EAC_DEC unsigned EACLoad(const char*);
EAC_DEC void* EACGet(unsigned); 
EAC_DEC void EACReload(unsigned);
EAC_DEC void EACRevise(); 
EAC_DEC unsigned EACGetRevision(); 
