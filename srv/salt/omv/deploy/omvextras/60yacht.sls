# @license   http://www.gnu.org/licenses/gpl.html GPL Version 3
# @copyright Copyright (c) 2019-2023 OpenMediaVault Plugin Developers
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

{% set config = salt['omv_conf.get']('conf.system.omvextras') %}

{% if config.docker | to_bool %}
{% if config.yacht | to_bool %}

yacht:
  docker_container.running:
    - image: selfhostedpro/yacht:latest
    - restart_policy: unless-stopped
    - port_bindings:
      - {{ config.yachtport }}:8000
    - binds:
      - /var/run/docker.sock:/var/run/docker.sock
      - yacht_data:/data

{% else %}

yacht:
  docker_container.absent:
  - force: True

{% endif %}
{% endif %}
