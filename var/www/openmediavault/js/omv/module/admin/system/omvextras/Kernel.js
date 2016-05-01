/**
 * @license   http://www.gnu.org/licenses/gpl.html GPL Version 3
 * @author    Volker Theile <volker.theile@openmediavault.org>
 * @author    OpenMediaVault Plugin Developers <plugins@omv-extras.org>
 * @copyright Copyright (c) 2009-2013 Volker Theile
 * @copyright Copyright (c) 2013-2016 OpenMediaVault Plugin Developers
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
// require("js/omv/Rpc.js")
// require("js/omv/data/Store.js")
// require("js/omv/data/Model.js")
// require("js/omv/data/proxy/Rpc.js")
// require("js/omv/window/MessageBox.js")

Ext.define("OMV.module.admin.system.omvextras.Kernel", {
    extend : "OMV.workspace.form.Panel",

    rpcService   : "OmvExtras",
    rpcGetMethod : "getKernel",

    hideTopToolbar: true,

    getFormItems : function() {
        var me = this;
        return [{
            xtype         : "fieldset",
            title         : _("Kernels"),
            fieldDefaults : {
                labelSeparator : ""
            },
            items         : [{
                name          : "kernels",
                xtype         : "combo",
                fieldLabel    : _("Installed Kernels"),
                allowBlank    : false,
                editable      : false,
                triggerAction : "all",
                displayField  : "name",
                valueField    : "key",
                store         : Ext.create("OMV.data.Store", {
                    autoLoad : true,
                    model    : OMV.data.Model.createImplicit({
                        idProperty : "key",
                        fields     : [
                            { name : "key", type : "integer" },
                            { name : "name", type : "string" }
                        ]
                    }),
                    proxy : {
                        type    : "rpc",
                        rpcData : {
                            service : "OmvExtras",
                            method  : "getKernelList"
                        }
                    }
                })
            },{
                xtype   : "button",
                name    : "setboot",
                text    : _("Set as default boot kernel"),
                scope   : this,
                handler : Ext.Function.bind(me.onSetBootButton, me, [ me ]),
                margin  : "5 0 8 0"
            },{
                xtype   : "button",
                name    : "backports",
                text    : _("Install Backports kernel"),
                scope   : this,
                handler : Ext.Function.bind(me.onCommandButton, me, [ me ]),
                margin  : "5 0 8 10"
            }]
        },{
            xtype         : "fieldset",
            title         : _("Notes"),
            fieldDefaults : {
                labelSeparator : ""
            },
            items         : [{
                border : false,
                html   : "<ul>" +
                           "<li>" + _("Setting the wrong default boot kernel may cause the system to be inaccessible.  The boot menu will still be available to select a different kernel.") + "</li>" +
                           "<li>" + _("Installing the backports kernel with not uninstall the standard kernel.") + "</li>" +
                           "<li>" + _("If the system does not boot using the backports kernel, the boot menu will still have the option to boot the standard kernel.") + "</li>" +
                         "</ul>"
            }]
        }];
    },

    onCommandButton : function() {
        var me = this;
        Ext.create("OMV.window.Execute", {
            title          : _("Install Backports kernel ..."),
            rpcService     : "OmvExtras",
            rpcMethod      : "doCommand",
            rpcParams      : {
                "command" : "installbackports"
            },
            hideStopButton : true,
            listeners      : {
                scope     : me,
                exception : function(wnd, error) {
                    OMV.MessageBox.error(null, error);
                }
            }
        }).show();
    },

    onSetBootButton : function() {
        var me = this;
        var key = me.findField("kernels").getValue();
        OMV.MessageBox.wait(null, _("Setting boot kernel ..."));
        OMV.Rpc.request({
            scope       : me,
            relayErrors : false,
            rpcData     : {
                service  : "OmvExtras",
                method   : "setBootKernel",
                params   : {
                    key : key
                }
            },
            success : function(id, success, response) {
                me.doReload();
                OMV.MessageBox.hide();
            }
        });
    }
});

OMV.WorkspaceManager.registerPanel({
    id        : "kernel",
    path      : "/system/omvextras",
    text      : _("Kernel"),
    position  : 20,
    className : "OMV.module.admin.system.omvextras.Kernel"
});
