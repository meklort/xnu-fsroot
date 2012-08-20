/*
 * CDDL HEADER START
 *
 * The contents of this file are subject to the terms of the
 * Common Development and Distribution License, Version 1.0 only
 * (the "License").  You may not use this file except in compliance
 * with the License.
 *
 * You can obtain a copy of the license at OPENSOLARIS.LICENSE
 * or http://www.opensolaris.org/os/licensing.
 * See the License for the specific language governing permissions
 * and limitations under the License.
 *
 * When distributing Covered Code, include this CDDL HEADER in each
 * file and include the License file at usr/src/OPENSOLARIS.LICENSE.
 * If applicable, add the following below this CDDL HEADER, with the
 * fields enclosed by brackets "[]" replaced with your own identifying
 * information: Portions Copyright [yyyy] [name of copyright owner]
 *
 * CDDL HEADER END
 */
/*
 * Copyright 2012 Evan Lojewski.  All rights reserved.
 * Use is subject to license terms.
 */

#include <IOKit/IOService.h>
#include <IOKit/IOLib.h>

class FSRoot : public IOService
{
	OSDeclareDefaultStructors(FSRoot)
    
private:
    boolean_t WaitForMatchingResource( const char * property, uint64_t timeout );
    
    static char md0disk[1024];
    
public:
	virtual bool	start(IOService *provider);
};