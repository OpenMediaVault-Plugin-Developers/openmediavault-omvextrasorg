/**
 * @license   http://www.gnu.org/licenses/gpl.html GPL Version 3
 * @author    Volker Theile <volker.theile@openmediavault.org>
 * @author    OpenMediaVault Plugin Developers <plugins@omv-extras.org>
 * @copyright Copyright (c) 2009-2013 Volker Theile
 * @copyright Copyright (c) 2013-2017 OpenMediaVault Plugin Developers
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
// require("js/omv/workspace/grid/Panel.js")
// require("js/omv/workspace/window/Form.js")
// require("js/omv/workspace/window/plugin/ConfigObject.js")
// require("js/omv/util/Format.js")
// require("js/omv/Rpc.js")
// require("js/omv/data/Store.js")
// require("js/omv/data/Model.js")
// require("js/omv/data/proxy/Rpc.js")

Ext.define("OMV.module.admin.system.omvextras.Repo", {
    extend   : "OMV.workspace.window.Form",
    requires : [
        "OMV.workspace.window.plugin.ConfigObject"
    ],

    rpcService   : "OmvExtras",
    rpcGetMethod : "getRepo",
    rpcSetMethod : "setRepo",

    plugins      : [{
        ptype : "configobject"
    }],

    height : 300,

    getFormItems : function() {
        return [{
            xtype      : "checkbox",
            name       : "enable",
            fieldLabel : _("Enable"),
            checked    : true
        },{
            xtype      : "textfield",
            name       : "name",
            fieldLabel : _("Name"),
            allowBlank : false,
            readOnly   : this.permanent
        },{
            xtype      : "textfield",
            name       : "comment",
            fieldLabel : _("Comment"),
            allowBlank : false,
            readOnly   : this.permanent
        },{
            xtype: "fieldset",
            title: _("Repo #1"),
            fieldDefaults: {
                labelSeparator: ""
            },
            items: [{
                xtype      : "textfield",
                name       : "repo1",
                fieldLabel : _("Repo"),
                allowBlank : false,
                readOnly   : this.permanent
            },{
                xtype      : "textfield",
                name       : "key1",
                fieldLabel : _("Key"),
                allowBlank : true,
                readOnly   : this.permanent
            },{
                xtype      : "textfield",
                name       : "package1",
                fieldLabel : _("Package"),
                allowBlank : true,
                readOnly   : this.permanent
            },{
                xtype      : "textfield",
                name       : "pin1",
                fieldLabel : _("Pin"),
                allowBlank : true,
                readOnly   : this.permanent
            },{
                xtype      : "textfield",
                name       : "priority1",
                fieldLabel : _("Priority"),
                allowBlank : true,
                readOnly   : this.permanent
            }]
        },{
            xtype: "fieldset",
            title: _("Repo #2"),
            fieldDefaults: {
                labelSeparator: ""
            },
            items: [{
                xtype      : "textfield",
                name       : "repo2",
                fieldLabel : _("Repo"),
                allowBlank : true,
                readOnly   : this.permanent
            },{
                xtype      : "textfield",
                name       : "key2",
                fieldLabel : _("Key"),
                allowBlank : true,
                readOnly   : this.permanent
            },{
                xtype      : "textfield",
                name       : "package2",
                fieldLabel : _("Package"),
                allowBlank : true,
                readOnly   : this.permanent
            },{
                xtype      : "textfield",
                name       : "pin2",
                fieldLabel : _("Pin"),
                allowBlank : true,
                readOnly   : this.permanent
            },{
                xtype      : "textfield",
                name       : "priority2",
                fieldLabel : _("Priority"),
                allowBlank : true,
                readOnly   : this.permanent
            }]
        },{
            xtype: "fieldset",
            title: _("Repo #3"),
            fieldDefaults: {
                labelSeparator: ""
            },
            items: [{
                xtype      : "textfield",
                name       : "repo3",
                fieldLabel : _("Repo"),
                allowBlank : true,
                readOnly   : this.permanent
            },{
                xtype      : "textfield",
                name       : "key3",
                fieldLabel : _("Key"),
                allowBlank : true,
                readOnly   : this.permanent
            },{
                xtype      : "textfield",
                name       : "package3",
                fieldLabel : _("Package"),
                allowBlank : true,
                readOnly   : this.permanent
            },{
                xtype      : "textfield",
                name       : "pin3",
                fieldLabel : _("Pin"),
                allowBlank : true,
                readOnly   : this.permanent
            },{
                xtype      : "textfield",
                name       : "priority3",
                fieldLabel : _("Priority"),
                allowBlank : true,
                readOnly   : this.permanent
            }]
        },{
            xtype      : "checkbox",
            name       : "permanent",
            fieldLabel : _("Permanent"),
            checked    : false,
            readOnly   : true
        }];
    }
});

Ext.define("OMV.module.admin.system.omvextras.Repos", {
    extend   : "OMV.workspace.grid.Panel",
    requires : [
        "OMV.Rpc",
        "OMV.data.Store",
        "OMV.data.Model",
        "OMV.data.proxy.Rpc",
        "OMV.util.Format"
    ],
    uses     : [
        "OMV.module.admin.system.omvextras.Repo"
    ],

    hidePagingToolbar : false,
    stateful          : true,
    stateId           : "a982b26d-6804-4632-b31b-1b48c0ea6dde",
    columns           : [{
        xtype     : "booleaniconcolumn",
        text      : _("Enabled"),
        sortable  : true,
        dataIndex : "enable",
        stateId   : "enable",
        align     : "center",
        width     : 80,
        resizable : false,
        iconCls   :  Ext.baseCSSPrefix + "grid-cell-booleaniconcolumn-switch"
    },{
        text      : _("Name"),
        sortable  : true,
        dataIndex : "name",
        stateId   : "name"
    },{
        text      : _("Comment"),
        sortable  : true,
        dataIndex : "comment",
        stateId   : "comment",
        flex      : 1
    }],

    initComponent : function() {
        var me = this;
        Ext.apply(me, {
            store : Ext.create("OMV.data.Store", {
                autoLoad : true,
                model    : OMV.data.Model.createImplicit({
                    idProperty  : "uuid",
                    fields      : [
                        { name : "uuid", type : "string" },
                        { name : "enable", type : "boolean" },
                        { name : "name", type : "string" },
                        { name : "repo1", type : "string" },
                        { name : "repo2", type : "string" },
                        { name : "repo3", type : "string" },
                        { name : "key1", type : "string" },
                        { name : "key2", type : "string" },
                        { name : "key3", type : "string" },
                        { name : "comment", type : "string" },
                        { name : "package1", type : "string" },
                        { name : "pin1", type : "string" },
                        { name : "priority1", type : "string" },
                        { name : "package2", type : "string" },
                        { name : "pin2", type : "string" },
                        { name : "priority2", type : "string" },
                        { name : "package3", type : "string" },
                        { name : "pin3", type : "string" },
                        { name : "priority3", type : "string" },
                        { name : "permanent", type : "boolean" }
                    ]
                }),
                proxy : {
                    type    : "rpc",
                    rpcData : {
                        service : "OmvExtras",
                        method  : "getRepoList"
                    }
                }
            })
        });

        OMV.Rpc.request({
            scope    : this,
            callback : function(id, success, response) {
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
            relayErrors : false,
            rpcData     : {
                service  : "OmvExtras",
                method   : "getArch"
            }
        });

        me.callParent(arguments);
    },

    getTopToolbarItems : function() {
        var me = this;
        var items = me.callParent(arguments);
        Ext.Array.push(items, {
            id      : me.getId() + "-check",
            xtype   : "button",
            text    : _("Update"),
            icon    : "images/refresh.png",
            iconCls : Ext.baseCSSPrefix + "btn-icon-16x16",
            handler : Ext.Function.bind(me.onCheckButton, me, [ me ]),
            scope   : me
        },{
            id      : me.getId() + "-aptclean",
            xtype   : "button",
            text    : _("Apt Clean"),
            icon    : "images/erase.png",
            iconCls : Ext.baseCSSPrefix + "btn-icon-16x16",
            handler : Ext.Function.bind(me.onAptCleanButton, me, [ me ]),
            scope   : me
        });
        return items;
    },

    onAddButton : function() {
        var me = this;
        Ext.create("OMV.module.admin.system.omvextras.Repo", {
            title     : _("Add repo"),
            uuid      : OMV.UUID_UNDEFINED,
            listeners : {
                scope  : me,
                submit : function() {
                    this.doReload();
                }
            }
        }).show();
    },

    onEditButton : function() {
        var me = this;
        var record = me.getSelected();
        //Check to see if permanent
        Ext.create("OMV.module.admin.system.omvextras.Repo", {
            title     : _("Edit repo"),
            uuid      : record.get("uuid"),
            permanent : record.get("permanent"),
            listeners : {
                scope  : me,
                submit : function() {
                    this.doReload();
                }
            }
        }).show();
    },

    doDeletion : function(record) {
        var me = this;
        var record = me.getSelected();
        //Check to see if permanent
        OMV.Rpc.request({
            scope    : me,
            callback : me.onDeletion,
            rpcData  : {
                service : "OmvExtras",
                method  : "deleteRepo",
                params  : {
                    uuid : record.get("uuid")
                }
            }
        });
    },

    onCheckButton : function() {
        var me = this;
        var wnd = Ext.create("OMV.window.Execute", {
            title           : _("Checking for new updates ..."),
            rpcService      : "omvextras",
            rpcMethod       : "doUpdate",
            rpcIgnoreErrors : true,
            hideStartButton : true,
            hideStopButton  : true,
            listeners       : {
                scope     : me,
                finish    : function(wnd, response) {
                    wnd.appendValue(_("Done..."));
                    wnd.setButtonDisabled("close", false);
                },
                exception : function(wnd, error) {
                    OMV.MessageBox.error(null, error);
                    wnd.setButtonDisabled("close", false);
                }
            }
        });
        wnd.setButtonDisabled("close", true);
        wnd.show();
        wnd.start();
    },

    onAptCleanButton: function() {
        var me = this;
        var wnd = Ext.create("OMV.window.Execute", {
            title           : _("Cleaning Apt Files and Lists..."),
            rpcService      : "OmvExtras",
            rpcMethod       : "doCommand",
            rpcParams      : {
                "command" : "aptclean"
            },
            rpcIgnoreErrors : true,
            hideStartButton : true,
            hideStopButton  : true,
            listeners       : {
                scope     : me,
                finish    : function(wnd, response) {
                    wnd.appendValue(_("Done..."));
                    wnd.setButtonDisabled("close", false);
                },
                exception : function(wnd, error) {
                    OMV.MessageBox.error(null, error);
                    wnd.setButtonDisabled("close", false);
                },
                close     : function() {
                    document.location.reload();
                }
            }
        });
        wnd.setButtonDisabled("close", true);
        wnd.show();
        wnd.start();
    }
});

OMV.WorkspaceManager.registerPanel({
    id        : "repos",
    path      : "/system/omvextras",
    text      : _("Repos"),
    position  : 10,
    className : "OMV.module.admin.system.omvextras.Repos"
});
