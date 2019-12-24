/*
 * Dynastream Innovations Inc.
 * Cochrane, AB, CANADA
 *
 * Copyright � 1998-2008 Dynastream Innovations Inc.
 * All rights reserved. This software may not be reproduced by
 * any means without express written approval of Dynastream
 * Innovations Inc.
 */
#include "types.h"
#include "dsi_framer.hpp"


//////////////////////////////////////////////////////////////////////////////////
// Public Class Functions
//////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
// Default constructor.
/////////////////////////////////////////////////////////////////
DSIFramer::DSIFramer()
{
   pclSerial = (DSISerial*)NULL;
}

/////////////////////////////////////////////////////////////////
// Constructor.
/////////////////////////////////////////////////////////////////
DSIFramer::DSIFramer(DSISerial *pclSerial_)
{
   pclSerial = pclSerial_;
}

/////////////////////////////////////////////////////////////////
// Destructor.
/////////////////////////////////////////////////////////////////
DSIFramer::~DSIFramer()
{
}

#if 0
/////////////////////////////////////////////////////////////////
BOOL DSIFramer::WriteMessage(void *pvData_, USHORT usMessageSize_)
{
   return FALSE;
}
#endif
