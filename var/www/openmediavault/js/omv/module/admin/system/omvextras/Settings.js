/**
 * @license   http://www.gnu.org/licenses/gpl.html GPL Version 3
 * @author    Volker Theile <volker.theile@openmediavault.org>
 * @author    OpenMediaVault Plugin Developers <plugins@omv-extras.org>
 * @copyright Copyright (c) 2009-2013 Volker Theile
 * @copyright Copyright (c) 2013-2020 OpenMediaVault Plugin Developers
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
// require("js/omv/form/plugin/LinkedFields.js")
// require("js/omv/WorkspaceManager.js")
// require("js/omv/workspace/form/Panel.js")
// require("js/omv/workspace/window/Form.js")
// require("js/omv/workspace/window/plugin/ConfigObject.js")

Ext.define("OMV.module.admin.system.omvextras.Repos", {
    extend: "OMV.workspace.form.Panel",

    rpcService: "OmvExtras",
    rpcGetMethod: "getSettings",
    rpcSetMethod: "setSettings",

    initComponent: function() {
        var me = this;
        OMV.Rpc.request({
            scope: this,
            callback: function(id, success, response) {
                var parent = me.up('tabpanel');

                if (!parent)
                    return;

                var arch = response.arch;
                var kernelPanel = parent.down('panel[title=' + _("Kernel") + ']');

                if (kernelPanel) {
                    var n = arch.indexOf("arm");
                    if (n < 0) {
                        kernelPanel.tab.show();
                    } else {
                        kernelPanel.tab.hide();
                    }
                }
            },
            relayErrors: false,
            rpcData: {
                service: "OmvExtras",
                method: "getArch"
            }
        });
        me.callParent(arguments);
    },

    getButtonItems: function() {
        var me = this;
        var items = me.callParent(arguments);
        items.push({
            id: me.getId() + "-updates",
            xtype: "button",
            text: _("Updates"),
            scope: me,
            icon: "images/refresh.png",
            menu: [{
                text: _("update"),
                icon: "images/refresh.png",
                handler: Ext.Function.bind(me.onCommandButton, me, [ "update" ])
            },{
                text: _("omv-update"),
                icon: "images/refresh.png",
                handler: Ext.Function.bind(me.onCommandButton, me, [ "omv-update" ])
            },{
                text: _("upgrade"),
                icon: "images/refresh.png",
                handler: Ext.Function.bind(me.onCommandButton, me, [ "upgrade" ])
            },{
                text: _("dist-upgrade"),
                icon: "images/refresh.png",
                handler: Ext.Function.bind(me.onCommandButton, me, [ "dist-upgrade" ])
            },{
                text: _("apt clean"),
                icon: "images/erase.png",
                handler: Ext.Function.bind(me.onCommandButton, me, [ "clean" ])
            }]
        });
        return items;
    },

    getFormItems: function () {
        var me = this;
        return [{
            xtype: "fieldset",
            title: _("General settings"),
            fieldDefaults: {
                labelSeparator: ""
            },
            items: [{
                xtype: "checkbox",
                name: "testing",
                fieldLabel: _("Testing repo"),
                checked: false
            },{
                xtype: "checkbox",
                name: "extras",
                fieldLabel: _("Extras repo"),
                checked: false
            },{
                xtype: "checkbox",
                name: "backportsStatus",
                fieldLabel: _("Backports"),
                checked: true
            }]
        }];
    },

    onCommandButton: function(command) {
        var me = this;
        me.doSubmit();
        var msg = "";
        switch (command) {
            case "update":
                msg = _("Running apt-get update ...");
                break;
            case "omv-update":
                msg = _("Running omv-update ...");
                break;
            case "upgrade":
                msg = _("Running apt-get upgrade ...");
                break;
            case "dist-upgrade":
                msg = _("Running apt-get dist-upgrade ...");
                break;
            case "clean":
                msg = _("Running omv-aptclean ...");
                break;
        }
        var wnd = Ext.create("OMV.window.Execute", {
            title: msg,
            rpcService: "OmvExtras",
            rpcMethod: "doCommand",
            rpcParams: {
                "command": command
            },
            rpcIgnoreErrors: true,
            hideStartButton: true,
            hideStopButton: true,
            listeners: {
                scope: me,
                finish: function(wnd, response) {
                    wnd.appendValue(_("Done."));
                    wnd.setButtonDisabled("close", false);
                },
                exception: function(wnd, error) {
                    OMV.MessageBox.error(null, error);
                    wnd.setButtonDisabled("close", false);
                },
                close: function() {
                    if (command == "clean") {
                        document.location.reload();
                    } else {
                        me.doReload();
                    }
                }
            }
        });
        wnd.setButtonDisabled("close", true);
        wnd.show();
        wnd.start();
    }
});

OMV.WorkspaceManager.registerPanel({
    id: "repos",
    path: "/system/omvextras",
    text: _("Settings"),
    position: 10,
    className: "OMV.module.admin.system.omvextras.Repos"
});
