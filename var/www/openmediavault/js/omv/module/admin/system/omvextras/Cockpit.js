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

Ext.define("OMV.module.admin.system.omvextras.Cockpit", {
    extend: "OMV.workspace.form.Panel",

    rpcService: "OmvExtras",
    rpcGetMethod: "getCockpit",

    hideOkButton: true,
    hideResetButton: true,

    getButtonItems: function() {
        var me = this;
        var items = me.callParent(arguments);
        items.push({
            id: me.getId() + "-install",
            xtype: "button",
            text: _("Install"),
            icon: "images/add.png",
            iconCls: Ext.baseCSSPrefix + "btn-icon-16x16",
            scope: me,
            handler: Ext.Function.bind(me.onCommandButton, me, [ "cockpit_install" ])
        },{
            id: me.getId() + "-remove",
            xtype: "button",
            text: _("Remove"),
            icon: "images/minus.png",
            iconCls: Ext.baseCSSPrefix + "btn-icon-16x16",
            scope: me,
            handler: Ext.Function.bind(me.onCommandButton, me, [ "cockpit_remove" ])
        },{
            id: me.getId() + "-open",
            xtype: "button",
            text: _("Open web"),
            icon: "images/arrow-up.png",
            iconCls: Ext.baseCSSPrefix + "btn-icon-16x16",
            scope: me,
            handler: function() {
                window.open("http://" + location.hostname + ":9090", "_blank");
            }
        });
        return items;
    },

    getFormItems: function () {
        var me = this;
        return [{
            xtype: "fieldset",
            title: _("Cockpit"),
            fieldDefaults: {
                labelSeparator: ""
            },
            items: [{
                xtype: "textfield",
                name: "cockpitStatus",
                fieldLabel: _("Status"),
                submitValue: false
            },{
                xtype: "textfield",
                name: "cockpitVersion",
                fieldLabel: _("Version"),
                submitValue: false
            },{
                border: false,
                html: "<ul>" +
                        "<li>" + _("Cockpit listens on port 9090 for the web interface.") + "</li>" +
                      "</ul>"
            }]
        }];
    },

    onCommandButton: function(command) {
        var me = this;
        var msg = "";
        var str = command.split("_");
        if (str[1] == "remove") {
            action = _("Removing");
        } else {
            action = _("Installing");
        }
        msg = action + " " + str[0] + " ...";
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
                    me.doReload();
                }
            }
        });
        wnd.setButtonDisabled("close", true);
        wnd.show();
        wnd.start();
    }
});

OMV.WorkspaceManager.registerPanel({
    id: "cockpit",
    path: "/system/omvextras",
    text: "Cockpit",
    position: 30,
    className: "OMV.module.admin.system.omvextras.Cockpit"
});
