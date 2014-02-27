/**
 * @license   http://www.gnu.org/licenses/gpl.html GPL Version 3
 * @author    Volker Theile <volker.theile@openmediavault.org>
 * @author    Aaron Murray <aaron@omv-extras.org>
 * @copyright Copyright (c) 2009-2013 Volker Theile
 * @copyright Copyright (c) 2013-2014 Aaron Murray
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

Ext.define("OMV.module.admin.service.omvextrasorg.Repo", {
    extend   : "OMV.workspace.window.Form",
    requires : [
        "OMV.workspace.window.plugin.ConfigObject"
    ],

    rpcService   : "OmvExtrasOrg",
    rpcGetMethod : "getRepo",
    rpcSetMethod : "setRepo",
    plugins      : [{
        ptype : "configobject"
    }],

    getFormItems : function() {
        return [{
            xtype      : "textfield",
            name       : "name",
            fieldLabel : _("Name"),
            allowBlank : false
        },{
            xtype      : "textfield",
            name       : "repo",
            fieldLabel : _("Repo"),
            allowBlank : false
        },{
            xtype      : "textfield",
            name       : "comment",
            fieldLabel : _("Comment"),
            allowBlank : false
        }];
    }
});

Ext.define("OMV.module.admin.service.omvextrasorg.Custom", {
    extend   : "OMV.workspace.grid.Panel",
    requires : [
        "OMV.Rpc",
        "OMV.data.Store",
        "OMV.data.Model",
        "OMV.data.proxy.Rpc",
        "OMV.util.Format"
    ],
    uses     : [
        "OMV.module.admin.service.omvextrasorg.Repo"
    ],

    hidePagingToolbar : false,
    stateful          : true,
    stateId           : "a982a76d-6804-4632-b31b-8b48c0ea6dde",
    columns           : [{
        text      : _("Name"),
        sortable  : true,
        dataIndex : "name",
        stateId   : "name"
    },{
        text      : _("Repo"),
        sortable  : true,
        dataIndex : "repo",
        stateId   : "repo"
    },{
        text      : _("Comment"),
        sortable  : true,
        dataIndex : "comment",
        stateId   : "comment"
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
                        { name : "name", type : "string" },
                        { name : "repo", type : "string" },
                        { name : "comment", type : "string" }
                    ]
                }),
                proxy : {
                    type    : "rpc",
                    rpcData : {
                        service : "OmvExtrasOrg",
                        method  : "getRepos"
                    }
                }
            })
        });
        me.callParent(arguments);
    },

    onAddButton : function() {
        var me = this;
        Ext.create("OMV.module.admin.service.omvextrasorg.Repo", {
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
        Ext.create("OMV.module.admin.service.omvextrasorg.Repo", {
            title     : _("Edit repo"),
            uuid      : record.get("uuid"),
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
        OMV.Rpc.request({
            scope    : me,
            callback : me.onDeletion,
            rpcData  : {
                service : "OmvExtrasOrg",
                method  : "deleteRepo",
                params  : {
                    uuid : record.get("uuid")
                }
            }
        });
    }

});

OMV.WorkspaceManager.registerPanel({
    id        : "scheduledjobs",
    path      : "/service/omvextrasorg",
    text      : _("Custom"),
    position  : 30,
    className : "OMV.module.admin.service.omvextrasorg.Custom"
});
