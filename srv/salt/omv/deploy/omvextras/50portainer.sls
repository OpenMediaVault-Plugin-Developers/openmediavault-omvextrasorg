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
{% if config.portainer | to_bool %}

{% if config.enabletls | to_bool %}
{% set certificate_prefix = salt['pillar.get']('default:OMV_SSL_CERTIFICATE_PREFIX', 'openmediavault-') %}
{% set crt = '/etc/ssl/certs/' ~ certificate_prefix ~ config.tlscertificateref ~ '.crt' %}
{% set key = '/etc/ssl/private/' ~ certificate_prefix ~ config.tlscertificateref ~ '.key' %}
{% endif %}

{% set image = 'ce' %}
{% if config.ee | to_bool %}
{% set image = 'ee' %}
{% endif %}

portainer:
  docker_container.running:
    - restart: unless-stopped
    - image: portainer/portainer-{{ image }}:latest
    - port_bindings:
      - {{ config.agentport }}:8000
{% if config.enabletls | to_bool %}
      - {{ config.webport }}:9443
{% else %}
      - {{ config.webport }}:9000
{% endif %}
    - binds:
      - /var/run/docker.sock:/var/run/docker.sock
      - portainer_data:/data
{% if config.enabletls | to_bool %}
      - {{ crt }}:/certs/openmediavault.crt:ro
      - {{ key }}:/certs/openmediavault.key:ro
    - environment:
      - sslcert: /certs/openmediavault.crt
      - sslkey: /certs/openmediavault.key
{% endif %}

{% else %}

portainer:
  docker_container.absent:
  - force: True

{% endif %}
{% endif %}
