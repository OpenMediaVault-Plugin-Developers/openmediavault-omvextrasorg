/**
 * @license   http://www.gnu.org/licenses/gpl.html GPL Version 3
 * @author    Volker Theile <volker.theile@openmediavault.org>
 * @author    OpenMediaVault Plugin Developers <plugins@omv-extras.org>
 * @copyright Copyright (c) 2009-2013 Volker Theile
 * @copyright Copyright (c) 2013-2014 OpenMediaVault Plugin Developers
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
// require("js/omv/WorkspaceManager.js")
// require("js/omv/workspace/form/Panel.js")

Ext.define("OMV.module.admin.system.omvextrasorg.Developer", {
    extend : "OMV.workspace.form.Panel",

    requires : [
        "OMV.form.plugin.LinkedFields"
    ],

    rpcService   : "OmvExtrasOrg",
    rpcGetMethod : "getDeveloper",
    rpcSetMethod : "setDeveloper",

    initComponent : function() {
        var me = this;
        me.callParent(arguments);
        me.on("submit", function() {
            OMV.MessageBox.show({
                title      : _("Confirmation"),
                msg        : _("The information about available software is out-of-date. You need to reload the information about available software."),
                buttons    : Ext.MessageBox.OKCANCEL,
                buttonText : {
                    ok      : _("Reload"),
                    cancel  : _("Close")
                },
                fn         : function(answer) {
                    if("cancel" === answer)
                        return;
                    OMV.RpcObserver.request({
                        title   : _("Downloading package information"),
                        msg     : _("The repository will be checked for new, removed or upgraded software packages."),
                        rpcData : {
                            service : "Apt",
                            method  : "update"
                        }
                    });
                },
                scope : me,
                icon  : Ext.Msg.QUESTION
            });
        }, me);
    },

    getFormItems : function() {
        var me = this;
        return [{
            xtype         : "fieldset",
            title         : _("Developer Repositories"),
            fieldDefaults : {
                labelSeparator : ""
            },
            items: [{
                xtype  : "text",
                text   : _("These repositories may break your system.  Use caution!!"),
                margin : "5 0 5 0"
            },{
                xtype      : "checkbox",
                name       : "beta",
                fieldLabel : _("Beta"),
                boxLabel   : _("Enable Beta repository"),
                checked    : false
            },{
                xtype      : "checkbox",
                name       : "dotdeb",
                fieldLabel : _("Dotdeb"),
                boxLabel   : _("Enable Dotdeb repository"),
                checked    : false
            },{
                xtype      : "checkbox",
                name       : "debmm",
                fieldLabel : _("deb-multimedia"),
                boxLabel   : _("Enable deb-multimedia repository"),
                checked    : false
            },{
                xtype      : "checkbox",
                name       : "debmmbp",
                fieldLabel : _("deb-multimedia backports"),
                boxLabel   : _("Enable deb-multimedia backports repository"),
                checked    : false
            },{
                xtype      : "checkbox",
                name       : "mariadb",
                fieldLabel : _("MariaDB"),
                boxLabel   : _("Enable MariaDB 10.0 repository"),
                checked    : false
            }]
        },{
            xtype         : "fieldset",
            title         : _("Utilities"),
            name          : "utilities",
            fieldDefaults : {
                labelSeparator : ""
            },
            items         : [{
                xtype   : "button",
                name    : "systemd",
                text    : _("Install systemd"),
                scope   : this,
                handler : Ext.Function.bind(me.onInstallSystemdButton, me, [ me ]),
                margin  : "5 0 10 0"
            }]
        }];
    },

    onInstallSystemdButton : function() {
        var me = this;
        Ext.create("OMV.window.Execute", {
            title          : _("Install systemd..."),
            rpcService     : "OmvExtrasOrg",
            rpcMethod      : "doInstallSystemd",
            hideStopButton : true,
            listeners      : {
                scope     : me,
                exception : function(wnd, error) {
                    OMV.MessageBox.error(null, error);
                }
            }
        }).show();
    }
});

OMV.WorkspaceManager.registerPanel({
    id        : "developer",
    path      : "/system/omvextrasorg",
    text      : _("Developer"),
    position  : 50,
    className : "OMV.module.admin.system.omvextrasorg.Developer"
});
