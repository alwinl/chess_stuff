/*
 * Copyright 2019 Alwin Leerling <dna.leerling@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 *
 */

#ifndef PRESENTATIONINTERFACE_H
#define PRESENTATIONINTERFACE_H

#include "pods.h"

/**-----------------------------------------------------------------------------
 * \brief This abstract class is the facade of the presentation logic.
 *
 * Implement this interface in the application.
 */
class PresentationInterface
{
public:
    PresentationInterface() {};
    virtual ~PresentationInterface() {};

	virtual void set_piece_positions( std::string FEN_string ) = 0;
	virtual void set_info( STInfo info ) = 0;
	virtual void set_thinking( bool on ) = 0;
	virtual void animate( STSquare start_square, STSquare end_square, char piece ) = 0;
};

/**-----------------------------------------------------------------------------
 * \brief A Null implementation
 *
 * Instead of testing a null pointer everywhere, use this object which does nothing
 * but does not generate access violations
 */
class NullPresentation : public PresentationInterface
{
public:
    NullPresentation() : PresentationInterface() {};
    virtual ~NullPresentation() {};

	void set_piece_positions( std::string FEN_string ) {};
	void set_info( STInfo info ) {};
	void set_thinking( bool on ) {};
	void animate( STSquare start_square, STSquare end_square, char piece ) {};
};

#endif // PRESENTATIONINTERFACE_H
