#!/usr/bin/env python3
#
# @license   http://www.gnu.org/licenses/gpl.html GPL Version 3
# @author    Volker Theile <volker.theile@openmediavault.org>
# @copyright Copyright (c) 2009-2020 Volker Theile
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.
import sys
import openmediavault
import openmediavault.firstaid
import openmediavault.subprocess

class Module(openmediavault.firstaid.IModule):
    @property
    def description(self):
        return "Clean apt"

    def execute(self):
        openmediavault.subprocess.check_call("omv-aptclean")
        return 0

if __name__ == "__main__":
    module = Module();
    sys.exit(module.execute())
