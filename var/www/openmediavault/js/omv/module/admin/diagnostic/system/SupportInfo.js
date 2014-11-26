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
        "OMV.form.CompositeField",
        "OMV.workspace.window.plugin.ConfigObject"
    ],

    rpcService   : "OmvExtrasOrg",
    rpcGetMethod : "getModules",
    rpcSetMethod : "setModules",

    hideResetButton : true,

    height : 400,
    width  : 350,

    getFormItems : function() {
        var me = this;
        return [{
            xtype: "compositefield",
            items: [{
                xtype      : "checkbox",
                name       : "debian-version",
                boxLabel   : _("00 Debian Version"),
                checked    : true,
                width      : 160
            },{
                xtype      : "checkbox",
                name       : "monit",
                boxLabel   : _("30 monit"),
                checked    : false
            }]
        },{
            xtype: "compositefield",
            items: [{
                xtype      : "checkbox",
                name       : "omv-version",
                boxLabel   : _("00 OMV Version"),
                checked    : true,
                width      : 160
            },{
                xtype      : "checkbox",
                name       : "hpraid",
                boxLabel   : _("31 hpraid"),
                checked    : false
            }]
        },{
            xtype: "compositefield",
            items: [{
                xtype      : "checkbox",
                name       : "date",
                boxLabel   : _("10 Date"),
                checked    : true,
                width      : 160
            },{
                xtype      : "checkbox",
                name       : "blkid",
                boxLabel   : _("40 blkid"),
                checked    : true
            }]
        },{
            xtype: "compositefield",
            items: [{
                xtype      : "checkbox",
                name       : "hostname",
                boxLabel   : _("10 Hostname"),
                checked    : false,
                width      : 160
            },{
                xtype      : "checkbox",
                name       : "df",
                boxLabel   : _("40 df"),
                checked    : true
            }]
        },{
            xtype: "compositefield",
            items: [{
                xtype      : "checkbox",
                name       : "locale",
                boxLabel   : _("10 Locale"),
                checked    : true,
                width      : 160
            },{
                xtype      : "checkbox",
                name       : "fstab",
                boxLabel   : _("40 fstab"),
                checked    : true
            }]
        },{
            xtype: "compositefield",
            items: [{
                xtype      : "checkbox",
                name       : "uname",
                boxLabel   : _("10 uname"),
                checked    : true,
                width      : 160
            },{
                xtype      : "checkbox",
                name       : "mountinfo",
                boxLabel   : _("40 Mount Info"),
                checked    : true
            }]
        },{
            xtype: "compositefield",
            items: [{
                xtype      : "checkbox",
                name       : "uptime",
                boxLabel   : _("10 Uptime"),
                checked    : true,
                width      : 160
            },{
                xtype      : "checkbox",
                name       : "partitions",
                boxLabel   : _("40 Partitions"),
                checked    : true
            }]
        },{
            xtype: "compositefield",
            items: [{
                xtype      : "checkbox",
                name       : "interfaces",
                boxLabel   : _("20 Interfaces"),
                checked    : true,
                width      : 160
            },{
                xtype      : "checkbox",
                name       : "apt-sources",
                boxLabel   : _("50 apt sources"),
                checked    : true
            }]
        },{
            xtype: "compositefield",
            items: [{
                xtype      : "checkbox",
                name       : "iptables",
                boxLabel   : _("20 iptables"),
                checked    : false,
                width      : 160
            },{
                xtype      : "checkbox",
                name       : "dpkg",
                boxLabel   : _("50 dpkg"),
                checked    : false
            }]
        },{
            xtype: "compositefield",
            items: [{
                xtype      : "checkbox",
                name       : "netstat",
                boxLabel   : _("20 netstat"),
                checked    : false,
                width      : 160
            },{
                xtype      : "checkbox",
                name       : "omv-plugins",
                boxLabel   : _("50 OMV Plugins"),
                checked    : true
            }]
        },{
            xtype: "compositefield",
            items: [{
                xtype      : "checkbox",
                name       : "route",
                boxLabel   : _("20 Route"),
                checked    : true,
                width      : 160
            },{
                xtype      : "checkbox",
                name       : "lsmod",
                boxLabel   : _("61 lsmod"),
                checked    : true
            }]
        },{
            xtype: "compositefield",
            items: [{
                xtype      : "checkbox",
                name       : "mdadm",
                boxLabel   : _("30 mdadm"),
                checked    : true,
                width      : 160
            }]
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
