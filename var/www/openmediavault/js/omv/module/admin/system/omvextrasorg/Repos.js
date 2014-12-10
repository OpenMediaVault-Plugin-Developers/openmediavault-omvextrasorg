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
// require("js/omv/Rpc.js")
// require("js/omv/data/Store.js")
// require("js/omv/data/Model.js")
// require("js/omv/data/proxy/Rpc.js")
// require("js/omv/window/MessageBox.js")
// require("js/omv/workspace/form/Panel.js")
// require("js/omv/workspace/grid/Panel.js")
// require("js/omv/form/field/Password.js")
// require("js/omv/util/Format.js")

Ext.define("OMV.module.admin.system.omvextrasorg.Repos", {
    extend   : "OMV.workspace.grid.Panel",
    requires : [
        "OMV.data.Store",
        "OMV.data.Model",
        "OMV.data.proxy.Rpc",
        "OMV.util.Format"
    ],

    stateful : true,
    stateId  : "a01bda0e-3f21-11e2-8510-74221868ca88",
    bodyCls  : "x-grid-without-dirty-cell",
    features : [{
        ftype          : "grouping",
        groupHeaderTpl : "{name}"
    }],

    columns : [{
        text      : _("Type"),
        sortable  : true,
        groupable : true,
        hidden    : true,
        dataIndex : "type",
        stateId   : "type",
        align     : "center",
        renderer  : function(value, metaData, record, rowIndex, colIndex,
          store, view) {
            return _(value);
        }
    },{
        xtype     : "checkcolumn",
        text      : _("Enable"),
        groupable : false,
        dataIndex : "enable",
        stateId   : "enable",
        align     : "center",
        width     : 70,
        resizable : false
    },{
        text      : _("Repository"),
        sortable  : true,
        groupable : false,
        dataIndex : "repo",
        stateId   : "repo",
        width     : 200,
        resizable : false,
        renderer  : function(value, metaData, record, rowIndex, colIndex,
          store, view) {
            return _(value);
        }
    },{
        text      : _("Description"),
        sortable  : true,
        groupable : false,
        dataIndex : "description",
        stateId   : "description",
        flex      : 1,
        renderer  : function(value, metaData, record, rowIndex, colIndex,
          store, view) {
            return _(value);
        }
    }],

    hideAddButton     : true,
    hideEditButton    : true,
    hideDeleteButton  : true,
    hideApplyButton   : false,
    hideRefreshButton : false,

    initComponent: function() {
        var me = this;
        Ext.apply(me, {
            store: Ext.create("OMV.data.Store", {
                autoLoad   : true,
                groupField : "type",
                model      : OMV.data.Model.createImplicit({
                    idProperty  : "id",
                    fields      : [
                        { name : "id", type : "string" },
                        { name : "type", type : "string" },
                        { name : "enable", type : "boolean" },
                        { name : "repo", type : "string" },
                        { name : "description", type : "string" }
                    ]
                }),
                proxy : {
                    type             : "rpc",
                    appendSortParams : false,
                    rpcData          : {
                        service : "omvextrasorg",
                        method  : "getSecondaryList"
                    }
                },
                sorters : [{
                    direction : "ASC",
                    property  : "repo"
                }]
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
                service  : "omvextrasorg",
                method   : "getArch"
            }
        });

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

    getTopToolbarItems : function() {
        var me = this;
        var items = me.callParent(arguments);
        items.push({
            id       : me.getId() + "-aptclean",
            xtype    : "button",
            text     : _("Apt Clean"),
            icon     : "images/refresh.png",
            iconCls  : Ext.baseCSSPrefix + "btn-icon-16x16",
            scope    : me,
            handler  : Ext.Function.bind(me.onAptCleanButton, me, [ me ])
        });
        return items;
    },

    onApplyButton: function() {
        var me = this;
        var records = me.store.getRange();
        var params = [];
        Ext.Array.each(records, function(record) {
            params.push({
                  "id"     : record.get("id"),
                  "enable" : record.get("enable")
              });
        });
        // Execute RPC.
        OMV.Rpc.request({
              scope: me,
              callback: function(id, success, response) {
                  this.store.reload();
              },
              relayErrors: false,
              rpcData: {
                  service : "omvextrasorg",
                  method  : "set",
                  params  : params
              }
          });
    },

    onAptCleanButton: function() {
        var me = this;
        var wnd = Ext.create("OMV.window.Execute", {
            title           : _("Cleaning Apt Files and Lists..."),
            rpcService      : "OmvExtrasOrg",
            rpcMethod       : "doAptClean",
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
    }
});

OMV.WorkspaceManager.registerPanel({
    id        : "repos",
    path      : "/system/omvextrasorg",
    text      : _("Repos"),
    position  : 10,
    className : "OMV.module.admin.system.omvextrasorg.Repos"
});
