#!/bin/bash
#
# @license   http://www.gnu.org/licenses/gpl.html GPL Version 3
# @author    Volker Theile <volker.theile@openmediavault.org>
# @author    OpenMediaVault Plugin Developers <plugins@omv-extras.org>
# @copyright Copyright (c) 2009-2013 Volker Theile
# @copyright Copyright (c) 2013-2023 OpenMediaVault Plugin Developers
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

if ! omv_config_exists "/config/system/omvextras"; then
    omv_config_add_node "/config/system" "omvextras"
fi
if ! omv_config_exists "/config/system/omvextras/testing"; then
    omv_config_add_key "/config/system/omvextras" "testing" "0"
fi
if ! omv_config_exists "/config/system/omvextras/docker"; then
    if dpkg -l | grep -qE "docker-ce|docker.io"; then
        docker="1"
    else
        docker="0"
    fi
    omv_config_add_key "/config/system/omvextras" "docker" "${docker}"
fi
if ! omv_config_exists "/config/system/omvextras/dockerStorage"; then
    omv_config_add_key "/config/system/omvextras" "dockerStorage" "/var/lib/docker"
fi
if ! omv_config_exists "/config/system/omvextras/portainer"; then
    portainer="0"
    if [ "${docker}" = "1" ]; then
        if docker container ls --format "{{.Names}},{{.Image}}" | grep -wq --ignore-case "portainer"; then
            portainer="1"
        fi
    fi
    omv_config_add_key "/config/system/omvextras" "portainer" "${portainer}"
fi
if ! omv_config_exists "/config/system/omvextras/webport"; then
    omv_config_add_key "/config/system/omvextras" "webport" "9443"
fi
if ! omv_config_exists "/config/system/omvextras/agentport"; then
    omv_config_add_key "/config/system/omvextras" "agentport" "8000"
fi
if ! omv_config_exists "/config/system/omvextras/yacht"; then
    yacht="0"
    if [ "${docker}" = "1" ]; then
        if docker container ls --format "{{.Names}},{{.Image}}" | grep -wq --ignore-case "yacht"; then
            yacht="1"
        fi
    fi
    omv_config_add_key "/config/system/omvextras" "yacht" "${yacht}"
fi
if ! omv_config_exists "/config/system/omvextras/yachtport"; then
    omv_config_add_key "/config/system/omvextras" "yachtport" "8001"
fi
if ! omv_config_exists "/config/system/omvextras/ee"; then
    omv_config_add_key "/config/system/omvextras" "ee" "0"
fi
if ! omv_config_exists "/config/system/omvextras/enabletls"; then
    omv_config_add_key "/config/system/omvextras" "enabletls" "0"
fi
if ! omv_config_exists "/config/system/omvextras/tlscertificateref"; then
    omv_config_add_key "/config/system/omvextras" "tlscertificateref" ""
fi

# remove backports from sources.list
sed -i "/$(lsb_release -sc)-backports/d" /etc/apt/sources.list

exit 0
