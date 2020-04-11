//*****************************************************************************
//   +--+
//   | ++----+
//   +-++    |
//     |     |
//   +-+--+  |
//   | +--+--+
//   +----+    Copyright (c) 2012 Code Red Technologies Ltd.
//
// Wrapper for LPC407x_8x_177x_8x.h header
//
// Software License Agreement
//
// The software is owned by Code Red Technologies and/or its suppliers, and is
// protected under applicable copyright laws.  All rights are reserved.  Any
// use in violation of the foregoing restrictions may subject the user to criminal
// sanctions under applicable laws, as well as to civil liability for the breach
// of the terms and conditions of this license.
//
// THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
// OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
// USE OF THIS SOFTWARE FOR COMMERCIAL DEVELOPMENT AND/OR EDUCATION IS SUBJECT
// TO A CURRENT END USER LICENSE AGREEMENT (COMMERCIAL OR EDUCATIONAL) WITH
// CODE RED TECHNOLOGIES LTD.
//
//*****************************************************************************
#ifndef LPC407X_8X_H_
#define LPC407X_8X_H_

// NXP now provide a shared code base for the LPC177x_8x and LPC407x_8x
// families of MCUs.
// This header is provided to act as a wrapper such that code that
// accidentally uses a LPC407x_8x specific header filename will redirect
// automatically to the real header. A warning message will also be emitted.

#warning ** Using wrapper header - change your code to include LPC407x_8x_177x_8x.h directly **
#include "LPC407x_8x_177x_8x.h"

#endif /* LPC407X_8X_H_ */
