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
{% set arch = grains['osarch'] %}
{% set oscodename = grains['oscodename'] %}
{% set docker = salt['pillar.get']('default:OMV_DISABLE_DOCKER', False) %}

{% if not docker | to_bool and not arch == 'i386' %}
{% set docker_pkg = "docker-ce" %}
{% set compose_pkg = "docker-compose-plugin" %}
{% else %}
{% set docker_pkg = "docker.io" %}
{% set compose_pkg = "docker-compose" %}
{% endif %}

{% if config.docker | to_bool  %}

docker_install_packages:
  pkg.installed:
    - pkgs:
      - "{{ docker_pkg }}"
      - "{{ compose_pkg }}"
{% if not docker | to_bool and not arch == 'i386' %}
      - containerd.io
      - docker-ce-cli
{% endif %}

{% set mounts = salt['cmd.shell']('systemctl list-units --type=mount | awk \'$5 ~ "/srv" { printf "%s ",$1 }\'') %}

/etc/systemd/system/docker.service.d/waitAllMounts.conf:
  file.managed:
    - contents: |
        [Unit]
        After=local-fs.target {{ mounts }} 
    - mode: "0644"
    - makedirs: True

systemd_daemon_reload_docker:
  cmd.run:
    - name: systemctl daemon-reload

# create daemon.json file if docker storage path is specified
{% if config.dockerStorage | length > 1 %}

/etc/docker/daemon.json:
  file.serialize:
    - dataset:
        data-root: "{{ config.dockerStorage }}"
    - serializer: json
    - user: root
    - group: root
    - mode: "0600"

{% endif %}

docker:
  service.running:
    - reload: True
    - enable: true
    - watch:
        - file: /etc/docker/daemon.json

{% else %}

docker_remove_packages:
  pkg.purged:
    - pkgs:
      - "{{ docker_pkg }}"
      - "{{ compose_pkg }}"
{% if not docker | to_bool and not arch == 'i386' %}
      - containerd.io
      - docker-ce-cli
{% endif %}

{% endif %}
