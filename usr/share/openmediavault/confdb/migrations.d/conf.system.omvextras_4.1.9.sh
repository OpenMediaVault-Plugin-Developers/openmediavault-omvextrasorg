#!/bin/sh
#
# @license   http://www.gnu.org/licenses/gpl.html GPL Version 3
# @author    OpenMediaVault Plugin Developers <plugins@omv-extras.org>
# @copyright Copyright (c) 2013-2019 OpenMediaVault Plugin Developers
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

set -e

rpi="/etc/apt/preferences.d/99raspberrypiorg"

if [ -f "${rpi}" ]; then
    sed -i "s/Pin: release n=stretch, origin archive.raspberrypi.org/Pin: release o=Raspberry Pi Foundation,a=stable,n=stretch, origin archive.raspberrypi.org/g" ${rpi}
fi

exit 0
