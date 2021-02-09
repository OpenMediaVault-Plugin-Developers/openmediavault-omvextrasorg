# @license   http://www.gnu.org/licenses/gpl.html GPL Version 3
# @author    Volker Theile <volker.theile@openmediavault.org>
# @copyright Copyright (c) 2019-2021 OpenMediaVault Plugin Developers
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
{% set teamviewer = salt['pillar.get']('default:OMV_DISABLE_TEAMVIEWER', False) %}
{% set dist = pillar['productinfo']['distribution'] %}

omvextrasbaserepo:
  pkgrepo.managed:
    - humanname: omv-extras.org {{ dist }}
    - name: "deb https://openmediavault-plugin-developers.github.io/packages/main/{{ dist }} ./"
    - file: /etc/apt/sources.list.d/omvextras.list
    - gpgcheck: 1
    - key_url: https://openmediavault-plugin-developers.github.io/packages/omvextras2026.asc

{%- if config.testing | to_bool %}

"deb https://openmediavault-plugin-developers.github.io/packages/main/{{ dist }}-testing ./":
  pkgrepo.managed:
    - file: /etc/apt/sources.list.d/omvextras.list

{% else %}

"deb https://openmediavault-plugin-developers.github.io/packages/main/{{ dist }}-testing ./":
  pkgrepo.absent

{% endif %}

{%- if config.extras | to_bool %}

"deb https://openmediavault-plugin-developers.github.io/packages/main/{{ dist }}-extras ./":
  pkgrepo.managed:
    - file: /etc/apt/sources.list.d/omvextras.list

{% else %}

"deb https://openmediavault-plugin-developers.github.io/packages/main/{{ dist }}-extras ./":
  pkgrepo.absent

{% endif %}

{% if not docker | to_bool and not arch == 'i386' %}

"deb [arch={{ arch }}] https://download.docker.com/linux/debian {{ oscodename }} stable":
  pkgrepo.managed:
    - file: /etc/apt/sources.list.d/omvextras.list
    - gpgcheck: 1
    - key_url: https://download.docker.com/linux/ubuntu/gpg

{% else %}

"deb [arch={{ arch }}] https://download.docker.com/linux/debian {{ oscodename }} stable":
  pkgrepo.absent

{% endif %}

{% if (arch == 'amd64' or arch == 'i386') and teamviewer | to_bool %}

"deb http://linux.teamviewer.com/deb stable main":
  pkgrepo.managed:
    - file: /etc/apt/sources.list.d/omvextras.list

{% else %}

"deb http://linux.teamviewer.com/deb stable main":
  pkgrepo.absent

{% endif %}

{% if not use_kernel_backports | to_bool %}

remove_apt_pref_omvextras:
  file.absent:
    - name: "/etc/apt/preferences.d/omvextras.pref"

{% else %}

configure_apt_pref_omvextras:
  file.managed:
    - name: "/etc/apt/preferences.d/omvextras.pref"
    - source:
      - salt://{{ tpldir }}/files/etc-apt-preferences_d-omvextras_pref.j2
    - template: jinja
    - user: root
    - group: root
    - mode: 644

{% endif %}

refresh_database_apt:
  module.run:
    - name: pkg.refresh_db
