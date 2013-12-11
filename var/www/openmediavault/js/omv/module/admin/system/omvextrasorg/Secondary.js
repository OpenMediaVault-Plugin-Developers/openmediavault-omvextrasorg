/**
 *
 * @license   http://www.gnu.org/licenses/gpl.html GPL Version 3
 * @author    Volker Theile <volker.theile@openmediavault.org>
 * @copyright Copyright (c) 2009-2013 Volker Theile
 *
 * OpenMediaVault is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * OpenMediaVault is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
// require("js/omv/WorkspaceManager.js")
// require("js/omv/workspace/form/Panel.js")

Ext.define("OMV.module.admin.system.omvextrasorg.Secondary", {
    extend: "OMV.workspace.form.Panel",

    rpcService: "OmvExtrasOrg",
    rpcGetMethod: "getSecondary",
    rpcSetMethod: "setSecondary",

    getFormItems: function() {
        return [{
            xtype: "fieldset",
            title: _("Additional Repositories"),
            fieldDefaults: {
                labelSeparator: ""
            },
            items: [{
                xtype: "checkbox",
                name: "vbox",
                fieldLabel: _("Virtualbox"),
                boxLabel: _("Enable OMV-Extras.org Virtualbox repository and Sun's Virtualbox repository"),
                checked: true
            },{
                xtype: "checkbox",
                name: "plex",
                fieldLabel: _("Plex"),
                boxLabel: _("Enable Plex Media Server repository"),
                checked: false
            },{
                xtype: "checkbox",
                name: "greyhole",
                fieldLabel: _("Greyhole"),
                boxLabel: _("Enable Greyhole repository"),
                checked: false
            },{
                xtype: "checkbox",
                name: "vdr",
                fieldLabel: _("VDR"),
                boxLabel: _("Enable e-tobi.net repository"),
                checked: false
            },{
                xtype: "checkbox",
                name: "miller",
                fieldLabel: _("Miller"),
                boxLabel: _("Enable Miller repository"),
                checked: false
            },{
                xtype: "checkbox",
                name: "btsync",
                fieldLabel: _("BTSync"),
                boxLabel: _("Enable BTSync repository"),
                checked: false
            }]
        }];
    }
});

OMV.WorkspaceManager.registerPanel({
    id: "secondary",
    path: "/system/omvextrasorg",
    text: _("Secondary"),
    position: 20,
    className: "OMV.module.admin.system.omvextrasorg.Secondary"
});
