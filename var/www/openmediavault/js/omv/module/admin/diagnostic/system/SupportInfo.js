/**
 * @license     http://www.gnu.org/licenses/gpl.html GPL Version 3
 * @author      Volker Theile <volker.theile@openmediavault.org>
 * @author      OpenMediaVault Plugin Developers <plugins@omv-extras.org>
 * @copyright   Copyright (c) 2009-2013 Volker Theile
 * @copyright   Copyright (c) 2013-2014 OpenMediaVault Plugin Developers
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
// require("js/omv/workspace/panel/Textarea.js")
// require("js/omv/workspace/window/Form.js")
// require("js/omv/workspace/window/plugin/ConfigObject.js")
// require("js/omv/Rpc.js")

Ext.define("OMV.module.admin.diagnostic.system.ModuleSelect", {
    extend : "OMV.workspace.window.Form",
    uses   : [
        "OMV.workspace.window.plugin.ConfigObject"
    ],

    rpcService   : "OmvExtrasOrg",
    rpcGetMethod : "getModules",
    rpcSetMethod : "setModules",

    hideResetButton : true,

    getFormItems : function() {
        var me = this;
        return [{
            xtype      : "checkbox",
            name       : "debian-version",
            fieldLabel : _("00 Debian Version"),
            checked    : true
        },{
            xtype      : "checkbox",
            name       : "omv-version",
            fieldLabel : _("00 OMV Version"),
            checked    : true
        },{
            xtype      : "checkbox",
            name       : "date",
            fieldLabel : _("10 Date"),
            checked    : true
        },{
            xtype      : "checkbox",
            name       : "hostname",
            fieldLabel : _("10 Hostname"),
            checked    : false
        },{
            xtype      : "checkbox",
            name       : "locale",
            fieldLabel : _("10 Locale"),
            checked    : true
        },{
            xtype      : "checkbox",
            name       : "uname",
            fieldLabel : _("10 uname"),
            checked    : true
        },{
            xtype      : "checkbox",
            name       : "uptime",
            fieldLabel : _("10 Uptime"),
            checked    : true
        },{
            xtype      : "checkbox",
            name       : "interfaces",
            fieldLabel : _("20 Interfaces"),
            checked    : true
        },{
            xtype      : "checkbox",
            name       : "iptables",
            fieldLabel : _("20 iptables"),
            checked    : false
        },{
            xtype      : "checkbox",
            name       : "netstat",
            fieldLabel : _("20 netstat"),
            checked    : false
        },{
            xtype      : "checkbox",
            name       : "route",
            fieldLabel : _("20 Route"),
            checked    : true
        },{
            xtype      : "checkbox",
            name       : "mdadm",
            fieldLabel : _("30 mdadm"),
            checked    : true
        },{
            xtype      : "checkbox",
            name       : "monit",
            fieldLabel : _("30 monit"),
            checked    : false
        },{
            xtype      : "checkbox",
            name       : "hpraid",
            fieldLabel : _("31 hpraid"),
            checked    : false
        },{
            xtype      : "checkbox",
            name       : "blkid",
            fieldLabel : _("40 blkid"),
            checked    : true
        },{
            xtype      : "checkbox",
            name       : "df",
            fieldLabel : _("40 df"),
            checked    : true
        },{
            xtype      : "checkbox",
            name       : "fstab",
            fieldLabel : _("40 fstab"),
            checked    : true
        },{
            xtype      : "checkbox",
            name       : "mountinfo",
            fieldLabel : _("40 Mount Info"),
            checked    : true
        },{
            xtype      : "checkbox",
            name       : "partitions",
            fieldLabel : _("40 Partitions"),
            checked    : true
        },{
            xtype      : "checkbox",
            name       : "apt-sources",
            fieldLabel : _("50 apt sources"),
            checked    : true
        },{
            xtype      : "checkbox",
            name       : "dpkg",
            fieldLabel : _("50 dpkg"),
            checked    : false
        },{
            xtype      : "checkbox",
            name       : "omv-plugins",
            fieldLabel : _("50 OMV Plugins"),
            checked    : true
        },{
            xtype      : "checkbox",
            name       : "lsmod",
            fieldLabel : _("61 lsmod"),
            checked    : true
        }];
    }
});

Ext.define("OMV.module.admin.diagnostic.system.SupportInfo", {
    extend   : "OMV.workspace.panel.Textarea",
    requires : [
        "OMV.Rpc",
        "OMV.window.MessageBox"
    ],
    uses     : [
        "OMV.module.admin.diagnostic.system.ModuleSelect"
    ],

    info : "style1",

    hideDownloadButton : false,

    rpcService : "OmvExtrasOrg",
    rpcMethod  : "getSupportInfo",
    rpcParams  : {
        info : "style1"
    },

    getTopToolbarItems: function() {
        var me = this;
        var items = me.callParent(arguments);

        items.push({
            id       : me.getId() + "-send",
            xtype    : "button",
            text     : _("Send"),
            icon     : "images/mail.png",
            iconCls  : Ext.baseCSSPrefix + "btn-icon-16x16",
            handler  : Ext.Function.bind(me.onSendButton, me, [ me ]),
            scope    : me
        },{
            id            : me.getId() + "-info",
            xtype         : "combo",
            allowBlank    : false,
            editable      : false,
            triggerAction : "all",
            displayField  : "name",
            valueField    : "cmd",
            store         : Ext.create("OMV.data.Store", {
                autoLoad : true,
                model    : OMV.data.Model.createImplicit({
                    idProperty : "name",
                    fields     : [
                        { name : "cmd", type : "string" },
                        { name : "name", type : "string" }
                    ]
                }),
                proxy : {
                    type    : "rpc",
                    rpcData : {
                        service : "OmvExtrasOrg",
                        method  : "getStyles"
                    }
                }
            }),
            listeners     : {
                scope  : me,
                change : function(combo, value) {
                    me.info = value;
                    me.showSupportInfo();
                }
            },
            value : me.info
        },{
            id       : me.getId() + "-modules",
            xtype    : "button",
            text     : _("Modules"),
            icon     : "images/grid.png",
            iconCls  : Ext.baseCSSPrefix + "btn-icon-16x16",
            handler  : Ext.Function.bind(me.onModulesButton, me, [ me ]),
            scope    : me
        });
        return items;
    },

    initComponent: function() {
        var me = this;
        me.rpcParams.info = this.info;

        me.callParent(arguments);
    },

    showSupportInfo : function() {
        var me = this;
        me.rpcParams.info = me.info;
        OMV.Rpc.request({
            scope    : this,
            callback : function(id, success, response) {
                this.setValue(response);
            },
            relayErrors : false,
            rpcData     : {
                service  : "OmvExtrasOrg",
                method   : "getSupportInfo",
                params   : {
                    info  : me.info
                }
            }
        });
    },

    onModulesButton: function() {
        var me = this;
        Ext.create("OMV.module.admin.diagnostic.system.ModuleSelect", {
            title     : _("Support Info Module Selection"),
            listeners : {
                scope  : me,
                submit : function() {
                    me.showSupportInfo();
                }
            }
        }).show();
    },

    onSendButton: function() {
        var me = this;
        Ext.create("OMV.window.Execute", {
            title      : _("Send Support Info"),
            rpcService : "OmvExtrasOrg",
            rpcMethod  : "doSend",
            listeners  : {
                scope     : me,
                exception : function(wnd, error) {
                    OMV.MessageBox.error(null, error);
                }
            }
        }).show();
    }
});

OMV.WorkspaceManager.registerPanel({
    id        : "supportinfo",
    path      : "/diagnostic/system",
    text      : _("Support Info"),
    position  : 26,
    className : "OMV.module.admin.diagnostic.system.SupportInfo"
});
