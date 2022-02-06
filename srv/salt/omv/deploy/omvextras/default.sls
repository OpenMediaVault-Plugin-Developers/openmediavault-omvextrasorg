# @license   http://www.gnu.org/licenses/gpl.html GPL Version 3
# @author    Volker Theile <volker.theile@openmediavault.org>
# @copyright Copyright (c) 2019-2022 OpenMediaVault Plugin Developers
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
{% set use_kernel_backports = salt['pillar.get']('default:OMV_APT_USE_KERNEL_BACKPORTS', True) -%}
{% set arch = grains['osarch'] %}
{% set oscodename = grains['oscodename'] %}
{% set docker = salt['pillar.get']('default:OMV_DISABLE_DOCKER', False) %}
{% set dist = pillar['productinfo']['distribution'] %}
{% set repo_url = salt['pillar.get']('default:OMV_EXTRAS_APT_REPOSITORY_URL', 'https://openmediavault-plugin-developers.github.io/packages/debian') -%}
{% set docker_url = salt['pillar.get']('default:OMV_DOCKER_APT_REPOSITORY_URL', 'https://download.docker.com/linux/debian') -%}
{% set docker_key = salt['pillar.get']('default:OMV_DOCKER_KEY_URL', 'https://download.docker.com/linux/ubuntu/gpg') -%}
{% set list = '/etc/apt/sources.list.d/omvextras.list' %}
{% set pref = '/etc/apt/preferences.d/omvextras.pref' %}

remove_apt_list_omvextras:
  file.absent:
    - name: "{{ list }}"

omvextrasbaserepo:
  pkgrepo.managed:
    - humanname: omv-extras.org {{ dist }}
    - name: "deb {{ repo_url }} {{ dist }} main"
    - file: "{{ list }}"
    - gpgcheck: 1
    - key_url: {{ repo_url }}/omvextras2026.asc

{%- if config.testing | to_bool %}

"deb {{ repo_url }} {{ dist }}-testing main":
  pkgrepo.managed:
    - file: "{{ list }}"

{% else %}

"deb {{ repo_url }} {{ dist }}-testing main":
  pkgrepo.absent

{% endif %}

{% if not docker | to_bool and not arch == 'i386' %}

"deb [arch={{ arch }}] {{ docker_url }} {{ oscodename }} stable":
  pkgrepo.managed:
    - file: "{{ list }}"
    - gpgcheck: 1
    - key_url: {{ docker_key }}

{% else %}

"deb [arch={{ arch }}] {{ docker_url }} {{ oscodename }} stable":
  pkgrepo.absent

{% endif %}

{% if not use_kernel_backports | to_bool %}

remove_apt_pref_omvextras:
  file.absent:
    - name: "{{ pref }}"

{% else %}

configure_apt_pref_omvextras:
  file.managed:
    - name: "{{ pref }}"
    - source:
      - salt://{{ tpldir }}/files/etc-apt-preferences_d-omvextras_pref.j2
    - template: jinja
    - user: root
    - group: root
    - mode: 644

{% endif %}

refresh_database_apt:
  module.run:
    - pkg.refresh_db:
