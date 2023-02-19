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
{% set use_kernel_backports = salt['pillar.get']('default:OMV_APT_USE_KERNEL_BACKPORTS', True) -%}
{% set arch = grains['osarch'] %}
{% set oscodename = grains['oscodename'] %}
{% set dist = pillar['productinfo']['distribution'] %}
{% set repo_url = salt['pillar.get']('default:OMV_EXTRAS_APT_REPOSITORY_URL', 'https://openmediavault-plugin-developers.github.io/packages/debian') -%}
{% set key_dir = '/usr/share/keyrings' %}
{% set omvextras_key = key_dir ~ '/omvextras2026.asc' %}
{% set list = '/etc/apt/sources.list.d/omvextras.list' %}
{% set pref = '/etc/apt/preferences.d/omvextras.pref' %}

remove_apt_list_omvextras:
  file.absent:
    - name: "{{ list }}"

download_omvextras_key:
  file.managed:
    - name: "{{ omvextras_key }}"
    - source: {{ repo_url }}/omvextras2026.asc
    - source_hash: 108ce460d5ebf0377dbe4f8067e9b990

deb [signed-by={{ omvextras_key }} arch={{ arch }}] {{ repo_url }} {{ dist }} main:
  pkgrepo.managed:
    - file: "{{ list }}"
    - keyserver: keyserver.ubuntu.com
    - keyid: 326A835E697B890A
    - aptkey: False

{%- if config.testing | to_bool %}

"deb {{ repo_url }} {{ dist }}-testing main":
  pkgrepo.managed:
    - file: "{{ list }}"

{% else %}

"deb {{ repo_url }} {{ dist }}-testing main":
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
    - mode: "0644"

{% endif %}
