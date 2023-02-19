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
{% set docker_url = salt['pillar.get']('default:OMV_DOCKER_APT_REPOSITORY_URL', 'https://download.docker.com/linux/debian') %}
{% set docker_key = salt['pillar.get']('default:OMV_DOCKER_KEY_URL', 'https://download.docker.com/linux/debian/gpg') %}
{% set docker_list = '/etc/apt/sources.list.d/omvdocker.list' %}

# get list of docker repos enabled in source list files
{% set pkg_repos = [] %}
{% for value in salt['pkg.list_repos']().values() %}
{% set _ = pkg_repos.extend(value) %}
{% endfor %}
{% set docker_pkg_repos = pkg_repos | rejectattr('disabled') | selectattr('uri', 'match', '^https?://download.docker.com/linux/(debian|ubuntu)$') | list %}

# create list of just repo uri
{% set repo_lines = [] %}
{% for line in docker_pkg_repos %}
{% set _ = repo_lines.append(line.line) %}
{% endfor %}

# create unique list of repo uri
{% set repo_lines2 = repo_lines | unique %}

# Delete all docker repositories from source lists
{% for docker_pkg_repo in repo_lines2 %}
remove_apt_sources_list_docker_{{ loop.index0 }}:
  module.run:
    - pkg.del_repo:
      - repo: "{{ docker_pkg_repo }}"

{% endfor %}

# enable docker repo if docker is enabled and not i386
{% if not docker | to_bool and not arch == 'i386' %}

"deb [arch={{ arch }}] {{ docker_url }} {{ oscodename }} stable":
  pkgrepo.managed:
    - file: "{{ docker_list }}"
    - gpgcheck: 1
    - key_url: {{ docker_key }}

{% endif %}
