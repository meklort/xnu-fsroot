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

#include "FSRoot.h"
#include <IOKit/IODeviceTreeSupport.h>

#include <sys/kernel.h>


#define super IOService
OSDefineMetaClassAndStructors(FSRoot, IOService);

extern "C"
{
    // found in you file system kext, initializes the vfs table.
    // NOTE: your fs ust set teh vfc_mountroot callback
    int fs_init_module();
    
    // Lets this kext know if we should setup a dummy block device (/dev/md0)
    // ZFS needs this, other shouldn't
    bool fs_needs_fake_block();
}


char FSRoot::md0disk[1024];


// our service start function, this is where we setup all the goodies
bool FSRoot::start(IOService *provider)
{
    if(fs_needs_fake_block())
    {
        IORegistryEntry *	regEntry;
        
        printf("FSRoot: prepping /dev/md0 for creation.\n");
        
        bzero(&md0disk, sizeof(md0disk));
        
        if((regEntry = IORegistryEntry::fromPath( "/chosen/memory-map", gIODTPlane )))
        {
            // XNU will automount the first RAMDisk entry it sees...
            // we need a block device to bypass the IOFindBSDRoot, so we create one here
            UInt32 ramdiskPtr[2];
            ramdiskPtr[0] = (UInt64)&md0disk;
            ramdiskPtr[1] = sizeof(md0disk);
            
            regEntry->setProperty("RAMDisk", &ramdiskPtr, sizeof(ramdiskPtr));
        }

    }

    // The IOBSD service is publiched after vfsinit is called,
    // since we cannot inject the vfs code before that (panic), we need to wait.
    // IOBSD is a good service to wait for since we can use public apis to do it
    printf("FSRoot: Waiting for IOBSD service.\n");
    while(WaitForMatchingResource("IOBSD", 10000) == FALSE);
    printf("FSRoot: IOBSD Found.\n");

    fs_init_module();
    
    return true;
}


boolean_t
FSRoot::WaitForMatchingResource( const char * property, uint64_t timeout )
{
    OSDictionary *	dict = 0;
    IOService *         match = 0;
    boolean_t		found = false;
    
    do {
        
        dict = IOService::resourceMatching( property );
        if( !dict)
            continue;
        match = IOService::waitForMatchingService( dict, timeout );
        if ( match)
            found = true;
        
    } while( false );
    
    if( dict)
        dict->release();
    if( match)
        match->release();
    
    return( found );
}
