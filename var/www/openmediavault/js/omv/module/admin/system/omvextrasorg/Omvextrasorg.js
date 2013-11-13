/**
 * This file is part of OpenMediaVault.
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
 * along with OpenMediaVault. If not, see <http://www.gnu.org/licenses/>.
 */
// require("js/omv/WorkspaceManager.js")
// require("js/omv/workspace/form/Panel.js")

Ext.define("OMV.module.admin.system.omvextrasorg.Info", {
    extend: "OMV.workspace.form.Panel",

    rpcService: "OmvExtrasOrg",
    rpcGetMethod: "getSettings",
    rpcSetMethod: "setSettings",

    getFormItems: function() {
        var me = this;
        return [{
            xtype: "fieldset",
            title: _("Repositories"),
            fieldDefaults: {
                labelSeparator: ""
            },
            items: [{
                xtype: "checkbox",
                name: "enable",
                fieldLabel: _("OMV-Extras.org"),
                boxLabel: _("Enable OMV-Extras.org repository"),
                checked: true
            },{
                xtype: "checkbox",
                name: "testing",
                fieldLabel: _("Testing"),
                boxLabel: _("Enable OMV-Extras.org testing repository  (Use at your own risk!)"),
                checked: false
            },{
                xtype: "checkbox",
                name: "vbox",
                fieldLabel: _("Virtualbox"),
                boxLabel: _("Enable OMV-Extras.org Virtualbox repository and Sun's Virtualbox repository  (disable if using armel/armhf)"),
                checked: true
            },{
                xtype: "checkbox",
                name: "plex",
                fieldLabel: _("Plex"),
                boxLabel: _("Enable Plex Media Server repository  (disable if using armel/armhf)"),
                checked: false
            }]
        },{
            xtype: "fieldset",
            title: _("Developer Features (Experimental)"),
            fieldDefaults: {
                labelSeparator: ""
            },
            items: [{
                xtype: "checkbox",
                name: "dotdeb",
                fieldLabel: _("Dotdeb"),
                boxLabel: _("Enable Dotdeb repository"),
                checked: false
            },{
                xtype: "checkbox",
                name: "debmm",
                fieldLabel: _("deb-multimedia"),
                boxLabel: _("Enable deb-multimedia repository"),
                checked: false
            },{
                xtype: "checkbox",
                name: "debmmbp",
                fieldLabel: _("deb-multimedia backports"),
                boxLabel: _("Enable deb-multimedia backports repository"),
                checked: false
            },{
                border : false,
                html   : "<br />"
            },{
                xtype   : "button",
                name    : "backports",
                text    : _("Install Backports 3.2 Kernel"),
                scope   : this,
                handler : Ext.Function.bind(me.onBackportsButton, me, [ me ])
            },{
                border : false,
                html   : "<ul>" +
                           "<li>" + _("Do not install the Backports Kernel if you use iSCSI!") + "</li>" +
                           "<li>" + _("This will not uninstall the 2.6 kernel.") + "</li>" +
                           "<li>" + _("If the system does not boot using the 3.2 kernel, the boot menu will still have the option to boot the 2.6 kernel.") + "</li>" +
                           "<li>" + _("If you are currently using Virtualbox, you will need to run the following command as root:  /etc/init.d/vboxdrv setup") + "</li>" +
                         "</ul>"
            }]
        }];
    },

    onBackportsButton: function() {
        var me = this;
        Ext.create("OMV.window.Execute", {
            title: _("Install Backports 3.2 kernel ..."),
            rpcService: "OmvExtrasOrg",
            rpcMethod: "installBackports",
            hideStopButton: true,
            listeners: {
                scope: me,
                exception: function(wnd, error) {
                    OMV.MessageBox.error(null, error);
                }
            }
        }).show();
    }
});

OMV.WorkspaceManager.registerNode({
    id: "omvextrasorg",
    path: "/system",
    text: _("OMV-Extras.org"),
    icon16: "images/plug.png",
    iconSvg: "images/plug.svg",
    position: 95
});

OMV.WorkspaceManager.registerPanel({
    id: "omvextrasorg",
    path: "/system/omvextrasorg",
    text: _("OMV-Extras.org"),
    position: 10,
    className: "OMV.module.admin.system.omvextrasorg.Info"
});
