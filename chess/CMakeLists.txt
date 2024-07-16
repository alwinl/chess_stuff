#
# CMakeLists.txt Copyright 2024 Alwin Leerling dna.leerling@gmail.com
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
# MA 02110-1301, USA.
#

add_executable( chess
	chessmain.cc
	guisrc/chessboard.cc
	guisrc/chesscontroller.cc
	guisrc/dialogs.cc
	guisrc/resources.c
	logicsrc/chessengine.cc
	logicsrc/chessgame.cc
	logicsrc/gamestate.cc
	logicsrc/piece.cc
	logicsrc/piece_square_tables.cc
	logicsrc/ply.cc
)

		# <Unit filename="chessmain.cc" />
		# <Unit filename="guisrc/chessboard.cc">
		# <Unit filename="guisrc/chesscontroller.cc">
		# <Unit filename="guisrc/dialogs.cc">
		# <Unit filename="guisrc/resources.c">
		# <Unit filename="logicsrc/chessengine.cc" />
		# <Unit filename="logicsrc/chessgame.cc" />
		# <Unit filename="logicsrc/gamestate.cc" />
		# <Unit filename="logicsrc/piece.cc" />
		# <Unit filename="logicsrc/piece_square_tables.cc" />
		# <Unit filename="logicsrc/ply.cc" />
		# <Unit filename="logictest/testapplication.cc" />
		# <Unit filename="logictest/testgameobject.cc">
