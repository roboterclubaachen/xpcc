/**
*  Copyright (c) 2018, Marten Junga (Github.com/Maju-Ketchup)
* All Rights Reserved.
*
* The file is part of the xpcc library and is released under the 3-clause BSD
* license. See the file `LICENSE` for the full license governing this code.
*
*
* The headder contains the class implementation of the IEEE standart 802.15.4-2011 Frame
* current max size is 255 bytes but some devices are able to send 1023 bytes
* Set always control first
*
*/
#ifndef MULTILATERATION_HPP
#define MULTILATERATION_HPP

#define _USE_MATH_DEFINES
#include <cmath>
#include <math.h>
#include <stdlib.h>
#include <xpcc/math.hpp>
#include <xpcc/debug/logger.hpp>

typedef float floatunit;

namespace xpcc {

/*!
@ingroup positioning
\brief Implementation of Multilateration Algorithms

*/
struct multilateration
{
public:
	/*!
Computes numerically the position of a tag when the position (in m) and the distance (in m) to three Anchors are given
\param Vector<floatunit
\param output
\param Vector<floatunit
\param anchor0
\param Vector<floatunit
\param anchor1
\param Vector<floatunit
\param anchor2
\param distanceToAnchor0
\param distanceToAnchor1
\param distanceToAnchor2
*/
	static void trilateration(Vector<floatunit, 3> &output,
							  Vector<floatunit, 3> anchor0,
							  Vector<floatunit, 3> anchor1,
							  Vector<floatunit, 3> anchor2,
							  floatunit distanceToAnchor0,
							  floatunit distanceToAnchor1,
							  floatunit distanceToAnchor2);
	/*!
	Computes analytically the position of a tag when the position (in m) and the distance (in m) to three Anchors are given
	\param Vector<floatunit
	\param output
	\param Vector<floatunit
	\param anchor0
	\param Vector<floatunit
	\param anchor1
	\param Vector<floatunit
	\param anchor2
	\param distanceToAnchor0
	\param distanceToAnchor1
	\param distanceToAnchor2
	*/
		static void trilaterationAna(Vector<floatunit, 3> &output,
								  Vector<floatunit, 3> anchor0,
								  Vector<floatunit, 3> anchor1,
								  Vector<floatunit, 3> anchor2,
								  floatunit distanceToAnchor0,
								  floatunit distanceToAnchor1,
								  floatunit distanceToAnchor2);
	/*!
Computes the Position of a tag when the receive times (in s) and the Positions (in m) of 4 Anchors are known. All Anchors need to be synchronised!
\param Vector<floatunit
\param output
\param Vector<floatunit
\param anchor1
\param Vector<floatunit
\param anchor2
\param Vector<floatunit
\param anchor3
\param Vector<floatunit
\param anchor4
\param receiveAnchor1
\param receiveAnchor2
\param receiveAnchor3
\param receiveAnchor4
*/
	static void activemultilateration (Vector<floatunit, 3> &output,
									   const Vector<floatunit, 3> anchor1,
									   const Vector<floatunit, 3> anchor2,
									   const Vector<floatunit, 3> anchor3,
									   const Vector<floatunit, 3> anchor4,
									   const floatunit receiveAnchor1,
									   const floatunit receiveAnchor2,
									   const floatunit receiveAnchor3,
									   const floatunit receiveAnchor4);
	/*!
Computes the Position of a tag when the Sendtimes of the Tag and the receive times (in s) and the Positions (in m) of 4 Anchors are known. All Anchors need to be synchronised!

\param Vector<floatunit
\param output
\param Vector<floatunit
\param anchor1
\param Vector<floatunit
\param anchor2
\param Vector<floatunit
\param anchor3
\param Vector<floatunit
\param anchor4
\param ReceiveTimeAnchor1
\param ReceiveTimeAnchor2
\param ReceiveTimeAnchor3
\param ReceiveTimeAnchor4
\param SendtimeAnchor1
\param SendtimeAnchor2
\param SendtimeAnchor3
\param SendtimeAnchor4
*/
	static void passivemultilateration(Vector<floatunit, 3> &output,
									   const Vector<floatunit, 3> anchor1,
									   const Vector<floatunit, 3> anchor2,
									   const Vector<floatunit, 3> anchor3,
									   const Vector<floatunit, 3> anchor4,
									   const floatunit ReceiveTimeAnchor1,
									   const floatunit ReceiveTimeAnchor2,
									   const floatunit ReceiveTimeAnchor3,
									   const floatunit ReceiveTimeAnchor4,
									   const floatunit SendtimeAnchor1,
									   const floatunit SendtimeAnchor2,
									   const floatunit SendtimeAnchor3,
									   const floatunit SendtimeAnchor4);
private:
	static void rotate(floatunit angle, floatunit &x, floatunit &y);
};
}
#endif // MULTILATERATION_HPP
