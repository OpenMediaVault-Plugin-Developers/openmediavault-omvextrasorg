#!/bin/bash
#
# @license   http://www.gnu.org/licenses/gpl.html GPL Version 3
# @author    OpenMediaVault Plugin Developers <plugins@omv-extras.org>
# @copyright Copyright (c) 2013-2024 OpenMediaVault Plugin Developers
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

. /usr/share/openmediavault/scripts/helper-functions

if omv_config_exists "/config/system/omvextras"; then
  omv_config_delete "/config/system/omvextras"
fi

if ! omv_config_exists "/config/system/omvextras"; then

  omv_config_add_node "/config/system" "omvextras"

  # docker
  docker=$(grep -l docker /etc/apt/sources.list.d/* | wc -l)
  if [ ${docker} -gt 0 ]; then
    docker=1
  fi
  omv_config_add_key "/config/system/omvextras" "docker" "${docker}"

fi

exit 0
